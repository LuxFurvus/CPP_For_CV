#pragma once
#ifndef SQLITE_STATEMENT_H
#define SQLITE_STATEMENT_H


#include <mutex>
#include <memory>
#include <unordered_set>
////////
#include "SQLite_Statement.h"



struct sqlite3;
struct sqlite3_stmt;
class SQLite_Statement;



///////////////////////////////


class SQLite_StatementSentinel
{
    SQLite_StatementSentinel(const SQLite_StatementSentinel&) = delete;
    SQLite_StatementSentinel& operator=(const SQLite_StatementSentinel&) = delete;
    SQLite_StatementSentinel(SQLite_StatementSentinel&&) = delete;
    SQLite_StatementSentinel& operator=(SQLite_StatementSentinel&&) = delete;

private:
    mutable std::mutex Mutex;
    std::unordered_set<SQLite_Statement*> ActiveStatements;

public:
    SQLite_StatementSentinel() : Mutex(), ActiveStatements() {}
    ~SQLite_StatementSentinel();

    void Register(SQLite_Statement* Statement);
    void Unregister(SQLite_Statement* Statement);

    void InvalidateAll() noexcept;
};


///////////////////////////////


struct SQLite_StatementCreationKit
{
    friend class SQLite_DbConnection;
    #include "SQLite_TestFriends.inl"
    
private:
    SQLite_StatementCreationKit(std::weak_ptr<SQLite_StatementSentinel> Sentinel, sqlite3* DbHandle) : Sentinel(Sentinel), DbHandle(DbHandle) {}

public:
    std::weak_ptr<SQLite_StatementSentinel> Sentinel;
    sqlite3* DbHandle = nullptr;

public:
    SQLite_StatementCreationKit(const SQLite_StatementCreationKit&) = default;
    SQLite_StatementCreationKit& operator=(const SQLite_StatementCreationKit&) = default;
    SQLite_StatementCreationKit(SQLite_StatementCreationKit&&) noexcept = default;
    SQLite_StatementCreationKit& operator=(SQLite_StatementCreationKit&&) noexcept = default;
};


///////////////////////////////

///////////////////////////////


class SQLite_Statement
{
    friend class SQLite_StatementSentinel;
    #include "SQLite_TestFriends.inl"

    SQLite_Statement(const SQLite_Statement&) = delete;
    SQLite_Statement& operator=(const SQLite_Statement&) = delete;

private:
    sqlite3* DbHandle = nullptr;
    sqlite3_stmt* StatementPtr = nullptr;
    std::weak_ptr<SQLite_StatementSentinel> Sentinel;

private:
    static void WarnOnUnusedTail(const std::string& UnusedTail);
    void PrepareStatement(sqlite3* DbHandleParam, const std::string& SqlQuery);
    void RegisterInSentinel(std::weak_ptr<SQLite_StatementSentinel> Sentinel);
    void UnRegisterFromSentinel();

public:
    SQLite_Statement(sqlite3* DbHandleParam, const std::string& SqlQuery);
    SQLite_Statement(const SQLite_StatementCreationKit& Kit, const std::string& SqlQuery);

    SQLite_Statement(SQLite_Statement&& Other) noexcept;
    SQLite_Statement& operator=(SQLite_Statement&& Other) noexcept;

    ~SQLite_Statement() noexcept;
    void Finalize() noexcept;

    sqlite3_stmt* Get() const;
    bool IsValid() const;

    void CrashIfNotValid();
    void Reset();
    bool Step();
};



#endif // SQLITE_STATEMENT_H
