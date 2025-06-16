#pragma once
#ifndef SQLITE_CONNECTOR_H
#define SQLITE_CONNECTOR_H

#include <format>
#include <string>
#include <iostream>
#include <unordered_set>
////////
#include <sqlite3.h>
////////
#include "../ConditionChecker/ConditionChecker.h"

class DbChecker
{
    friend class SQLiteConnector_DbChecker_Test;
private:
    DbChecker() = delete;
public:
    static void CheckResult(sqlite3* DbHandle, const int ResultCode, const char* OperatonType);
    static void WarnIfFalse(const bool Condition, const char* WarningMsg);
};

///////////////////////////////

class SQLiteStatement;

class ConnectionSentinel
{
private:
    mutable std::mutex Mutex;
    std::unordered_set<SQLiteStatement*> ActiveStatements;

public:
    ConnectionSentinel() = default;
    ~ConnectionSentinel();

    ConnectionSentinel(const ConnectionSentinel&) = delete;
    ConnectionSentinel& operator=(const ConnectionSentinel&) = delete;

    void Register(SQLiteStatement* Statement);
    void Unregister(SQLiteStatement* Statement);

    void InvalidateAll();
};

///////////////////////////////

struct StatementCreationKit
{
    friend class DbConnection;
    friend class SQLiteConnector_ConnectionSentinel_Cases_Test;
private:
    StatementCreationKit(std::weak_ptr<ConnectionSentinel> Sentinel, sqlite3* DbHandle) : Sentinel(Sentinel), DbHandle(DbHandle) {}
public:
    std::weak_ptr<ConnectionSentinel> Sentinel;
    sqlite3* DbHandle = nullptr;
};

///////////////////////////////

class DbConnection
{
    friend class SQLiteConnector_DbConnection_Test;
private:
    sqlite3* DbHandle = nullptr;
    std::shared_ptr<ConnectionSentinel> Sentinel;

public:

    DbConnection(const char* WayToDb, const bool IsReadOnly, bool EnableSentinel = true)
    {
        CONFIRM(WayToDb);
        const int OpenFlags = IsReadOnly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE;
        const int AllFlags = OpenFlags | SQLITE_OPEN_EXRESCODE | SQLITE_OPEN_FULLMUTEX;
        const int OpenResult = sqlite3_open_v2(WayToDb, &DbHandle, AllFlags, nullptr);
        DbChecker::CheckResult(DbHandle, OpenResult, "open database");

        if (!EnableSentinel) return;
        Sentinel = std::make_shared<ConnectionSentinel>();
    }
    ~DbConnection()
    {
        if (Sentinel) Sentinel->InvalidateAll();
        const int CloseResult = sqlite3_close(DbHandle);
        DbChecker::CheckResult(DbHandle, CloseResult, "close database");
    }
    sqlite3* Get() const
    {
        CONFIRM(DbHandle);
        return DbHandle;
    }
    StatementCreationKit MakeStatementKit() const
    {
        return StatementCreationKit{ Sentinel, DbHandle };
    }
};

///////////////////////////////

class SQLiteStatement
{
    friend class ConnectionSentinel;
    friend class SQLiteConnector_SQLiteStatement_Test;
    friend class SQLiteConnector_ConnectionSentinel_Cases_Test;
private:
    sqlite3* DbHandle = nullptr;
    sqlite3_stmt* StatementPtr = nullptr;
    std::weak_ptr<ConnectionSentinel> Sentinel;

private:

    static void WarnOnUnusedTail(const char* UnusedTail)
    {
        const bool NoUnusedTail = UnusedTail == nullptr || strlen(UnusedTail) == 0;
        if (NoUnusedTail) return;
        const std::string UnusedTailMsg = std::format("Unused tail [{}]: {}", strlen(UnusedTail), UnusedTail);
        DbChecker::WarnIfFalse(NoUnusedTail, UnusedTailMsg.c_str());
    }

    void FinalizeFromSentinel()
    {
        if (!IsValid()) return;
        sqlite3_finalize(StatementPtr);
        StatementPtr = nullptr;
        DbHandle = nullptr;
    }

    void PrepareStatement(sqlite3* DbHandleParam, const char* SqlQuery)
    {
        DbHandle = DbHandleParam;
        const char* UnusedTail = nullptr;
        const int PrepareResult = sqlite3_prepare_v2(DbHandleParam, SqlQuery, -1, &StatementPtr, &UnusedTail);
        DbChecker::CheckResult(DbHandle, PrepareResult, "prepare");
        WarnOnUnusedTail(UnusedTail);
    }

    void RegisterInSentinel(std::weak_ptr<ConnectionSentinel> Sentinel)
    {
        std::shared_ptr<ConnectionSentinel> SharedSentinel = Sentinel.lock();
        if (!SharedSentinel) return;
        SharedSentinel->Register(this);
    }

    void UnRegisterFromSentinel()
    {
        std::shared_ptr<ConnectionSentinel> SharedSentinel = Sentinel.lock();
        if (!SharedSentinel) return;
        SharedSentinel->Unregister(this);        
    }

public:
    SQLiteStatement(sqlite3* DbHandleParam, const char* SqlQuery)
    {
        CONFIRM(DbHandleParam);
        CONFIRM(SqlQuery);
        PrepareStatement(DbHandleParam, SqlQuery);
    }

    SQLiteStatement(const StatementCreationKit& Kit, const char* SqlQuery)
        : SQLiteStatement(Kit.DbHandle, SqlQuery)
    {
        Sentinel = Kit.Sentinel;
        RegisterInSentinel(Sentinel);
    }
    
    ~SQLiteStatement()
    {
        UnRegisterFromSentinel();
        if (StatementPtr == nullptr) return;
        const int FinalizeResult = sqlite3_finalize(StatementPtr);
        DbChecker::CheckResult(DbHandle, FinalizeResult, "finalize");
        StatementPtr = nullptr;
        DbHandle = nullptr;
    }

    bool IsValid() const
    {
        return StatementPtr != nullptr && DbHandle != nullptr;
    }

    sqlite3_stmt* Get() const
    {
        if (!IsValid()) return nullptr;
        CONFIRM(StatementPtr);
        return StatementPtr;
    }

    bool Step() const
    {
        if (!IsValid()) return false;
        const int StepResult = sqlite3_step(StatementPtr);
        switch(StepResult)
        {
            case SQLITE_ROW:
                return true;
            case SQLITE_DONE:
                return false;
            default:
                DbChecker::CheckResult(DbHandle, StepResult, "step");
                return false;
        }
    }
    void Reset() const
    {
        if (!IsValid()) return;
        const int ResetResult = sqlite3_reset(StatementPtr);
        DbChecker::CheckResult(DbHandle, ResetResult, "reset");
    }
};

#endif // SQLITE_CONNECTOR_H