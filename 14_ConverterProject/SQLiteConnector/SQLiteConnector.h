#pragma once
#ifndef SQLITE_CONNECTOR_H
#define SQLITE_CONNECTOR_H

#include <format>
#include <string>
#include <iostream>
#include <unordered_set>
#include <variant>
#include <utility>
#include <memory>
#include <vector>
////////
#include <sqlite3.h>
////////
#include "../ConditionChecker/ConditionChecker.h"


///////////////////////////////


class SQLite_Statement;

class SQLite_ConnectionSentinel
{
    SQLite_ConnectionSentinel(const SQLite_ConnectionSentinel&) = delete;
    SQLite_ConnectionSentinel& operator=(const SQLite_ConnectionSentinel&) = delete;
    SQLite_ConnectionSentinel(SQLite_ConnectionSentinel&&) = delete;
    SQLite_ConnectionSentinel& operator=(SQLite_ConnectionSentinel&&) = delete;

private:
    mutable std::mutex Mutex;
    std::unordered_set<SQLite_Statement*> ActiveStatements;

public:
    SQLite_ConnectionSentinel() : Mutex(), ActiveStatements() {}
    ~SQLite_ConnectionSentinel();

    void Register(SQLite_Statement* Statement);
    void Unregister(SQLite_Statement* Statement);

    void InvalidateAll();
};


///////////////////////////////


struct StatementCreationKit
{
    friend class SQLite_DbConnection;
    friend class SQLite_ConnectionSentinel_T01_RegisterAddsPointer_Test;
    friend class SQLite_ConnectionSentinel_T02_UnregisterRemovesPointer_Test;
    friend class SQLite_ConnectionSentinel_T03_DoubleRegisterHasSingleEntry_Test;
    friend class SQLite_ConnectionSentinel_T04_DoubleUnregisterSafe_Test;
    friend class SQLite_ConnectionSentinel_T05_InvalidateAllClearsSet_Test;
    friend class SQLite_ConnectionSentinel_T06_RegisterAfterInvalidateWorks_Test;
    friend class SQLite_ConnectionSentinel_T07_RegisterAndDestroyUnregistersAutomatically_Test;
    friend class SQLite_ConnectionSentinel_T08_DestructorCallsInvalidateAll_Test;
    friend class SQLite_ConnectionSentinel_T09_ThreadSafetyParallelRegisterUnregister_Test;
    friend class SQLite_ConnectionSentinel_T13_NullptrSentinelSafe_Test;
    friend class SQLite_ConnectionSentinel_T15_ThreadSafetyConcurrentAccess_Test;
    
private:
    StatementCreationKit(std::weak_ptr<SQLite_ConnectionSentinel> Sentinel, sqlite3* DbHandle) : Sentinel(Sentinel), DbHandle(DbHandle) {}

public:
    std::weak_ptr<SQLite_ConnectionSentinel> Sentinel;
    sqlite3* DbHandle = nullptr;

public:
    StatementCreationKit(const StatementCreationKit&) = default;
    StatementCreationKit& operator=(const StatementCreationKit&) = default;
    StatementCreationKit(StatementCreationKit&&) noexcept = default;
    StatementCreationKit& operator=(StatementCreationKit&&) noexcept = default;
};


///////////////////////////////


class SQLite_DbConnection
{
    friend class SQLite_DbConnection_T12_CorruptGetShouldCrash_Test;

    SQLite_DbConnection() = delete;
    SQLite_DbConnection(const SQLite_DbConnection&) = delete;
    SQLite_DbConnection& operator=(const SQLite_DbConnection&) = delete;

    SQLite_DbConnection(SQLite_DbConnection&&) = delete;
    SQLite_DbConnection& operator=(SQLite_DbConnection&&) = delete;

private:
    sqlite3* DbHandle = nullptr;
    std::shared_ptr<SQLite_ConnectionSentinel> Sentinel;

private:
    void SetupConnection(const char* WayToDb, const bool IsReadOnly, const bool EnableSentinel);

    void CheckOpenResult(const int OpenResult);

    void CloseConnection();

public:
    SQLite_DbConnection(const char* WayToDb, const bool IsReadOnly, const bool EnableSentinel = true);
    ~SQLite_DbConnection();

    sqlite3* Get() const;
    StatementCreationKit MakeStatementKit() const;
};


///////////////////////////////


class SQLite_Statement
{
    friend class SQLite_ConnectionSentinel;
    friend class SQLite_Statement_T01_UnusedTailNullOrEmpty_NoWarning_Test;
    friend class SQLite_Statement_T02_UnusedTailPresent_ShouldWarn_Test;
    friend class SQLite_ConnectionSentinel_T14_FinalizeFromSentinelIdempotent_Test;

    SQLite_Statement(const SQLite_Statement&) = delete;
    SQLite_Statement& operator=(const SQLite_Statement&) = delete;

    SQLite_Statement(SQLite_Statement&& Other) noexcept = delete;
    SQLite_Statement& operator=(SQLite_Statement&& Other) noexcept = delete;

private:
    sqlite3* DbHandle = nullptr;
    sqlite3_stmt* StatementPtr = nullptr;
    std::weak_ptr<SQLite_ConnectionSentinel> Sentinel;

private:
    static void WarnOnUnusedTail(const char* UnusedTail);
    void PrepareStatement(sqlite3* DbHandleParam, const char* SqlQuery);
    void RegisterInSentinel(std::weak_ptr<SQLite_ConnectionSentinel> Sentinel);
    void UnRegisterFromSentinel();

public:
    SQLite_Statement(sqlite3* DbHandleParam, const char* SqlQuery);
    SQLite_Statement(const StatementCreationKit& Kit, const char* SqlQuery);

    ~SQLite_Statement();

    sqlite3_stmt* Get() const;
    bool IsValid() const;
    bool Step() const;
    void Reset() const;
    void Finalize();
};


///////////////////


struct SQLite_ParamVisitor
{
    SQLite_ParamVisitor() = delete;

private:
    sqlite3_stmt* Statement;
    int ParamIndex;

public:
    SQLite_ParamVisitor(sqlite3_stmt* Statement, const int ParamIndex);

    int operator()(const int64_t Value) const;
    int operator()(const int Value) const;
    int operator()(const double Value) const;
    int operator()(const std::string& Value) const;
    int operator()(std::nullptr_t) const;
};


///////////////////


using BindableValue = std::variant<int64_t, double, std::string, std::nullptr_t>;
using BindablePair = std::pair<std::string, std::variant<int64_t, double, std::string, std::nullptr_t>>;

class SQLite_ParamValidator
{
    friend class SQLite_NamedParamBinder;
private:
    static std::string NormalizeParamName(const std::string& Name);
    static std::unordered_set<std::string> GetParamNames(const std::vector<BindablePair>& NamedValues);
    static void CheckIfParamsInStatement(sqlite3_stmt* Statement, const std::unordered_set<std::string>& ParamNames);
private:
    static void BindParamsByName(sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues);
};

class SQLite_NamedParamBinder
{
    friend class SQLite_NamedParamBinderTest_T08_InvalidParamIndexShouldThrow_Test;
private:
    static void PrepareStatementForBinding(sqlite3_stmt* Statement);
    static std::string NormalizeParamName(const std::string &Name);
    static int GetParamIndexForProvidedName(sqlite3_stmt *Statement, const std::string &Name);
    static int BindOneParam(sqlite3_stmt *Statement, int ParamIndex, const BindableValue &Value);
    static void ApplyBindablePairsToStatement(sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues);

public:
    static void BindParamsByName(sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues);
};


///////////////////


#endif // SQLITE_CONNECTOR_H
