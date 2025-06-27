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
#include "../SQLGenerals/SQLGenerals.h"

///////////////////////////////


class SQLite_Statement;

using BindablePair = std::pair<std::string, FieldValue>;


///////////////////////////////


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
    #include "SQLite_TestFriends.inl"
    
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
    #include "SQLite_TestFriends.inl"

    SQLite_DbConnection() = delete;
    SQLite_DbConnection(const SQLite_DbConnection&) = delete;
    SQLite_DbConnection& operator=(const SQLite_DbConnection&) = delete;

    SQLite_DbConnection(SQLite_DbConnection&&) = delete;
    SQLite_DbConnection& operator=(SQLite_DbConnection&&) = delete;

private:
    sqlite3* DbHandle = nullptr;
    std::shared_ptr<SQLite_ConnectionSentinel> Sentinel;

private:
    void SetupConnection(const std::string& WayToDb, const bool IsReadOnly, const bool EnableSentinel);

    void CheckOpenResult(const int OpenResult);

    void CloseConnection();

public:
    SQLite_DbConnection(const std::string& WayToDb, const bool IsReadOnly, const bool EnableSentinel = true);
    ~SQLite_DbConnection();

    sqlite3* Get() const;
    StatementCreationKit MakeStatementKit() const;
};


///////////////////////////////


class SQLite_Statement
{
    friend class SQLite_ConnectionSentinel;
    #include "SQLite_TestFriends.inl"

    SQLite_Statement(const SQLite_Statement&) = delete;
    SQLite_Statement& operator=(const SQLite_Statement&) = delete;

private:
    sqlite3* DbHandle = nullptr;
    sqlite3_stmt* StatementPtr = nullptr;
    std::weak_ptr<SQLite_ConnectionSentinel> Sentinel;

private:
    static void WarnOnUnusedTail(const std::string& UnusedTail);
    void PrepareStatement(sqlite3* DbHandleParam, const std::string& SqlQuery);
    void RegisterInSentinel(std::weak_ptr<SQLite_ConnectionSentinel> Sentinel);
    void UnRegisterFromSentinel();

public:
    SQLite_Statement(sqlite3* DbHandleParam, const std::string& SqlQuery);
    SQLite_Statement(const StatementCreationKit& Kit, const std::string& SqlQuery);

    SQLite_Statement(SQLite_Statement&& Other) noexcept;
    SQLite_Statement& operator=(SQLite_Statement&& Other) noexcept;

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
    int operator()(const std::vector<uint8_t>& Value) const;
    int operator()(std::nullptr_t) const;
};


///////////////////


class SQLite_ParamValidator
{
    friend class SQLite_NamedParamBinder;
    SQLite_ParamValidator() = delete;
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
    SQLite_NamedParamBinder() = delete;
private:
    static void PrepareStatementForBinding(sqlite3_stmt* Statement);
    static std::string NormalizeParamName(const std::string &Name);
    static int GetParamIndexForProvidedName(sqlite3_stmt *Statement, const std::string &ProvidedName);
    static int BindOneParam(sqlite3_stmt *Statement, int ParamIndex, const FieldValue &Value);
    static void ApplyBindablePairsToStatement(sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues);

public:
    static void BindParamsByName(sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues);
};


///////////////////


class SQLite_FieldReader
{
    SQLite_FieldReader() = delete;

private:

    static int64_t ExtractInteger(sqlite3_stmt* Statement, int ColumnIndex);
    static double ExtractDouble(sqlite3_stmt* Statement, int ColumnIndex);
    static std::string ExtractText(sqlite3_stmt* Statement, int ColumnIndex);
    static std::vector<uint8_t> ExtractBlob(sqlite3_stmt* Statement, int ColumnIndex);

    static void ValidateStatementReadiness(sqlite3_stmt* Statement);
    static void ValidateColumnIndex(sqlite3_stmt* Statement, int ColumnIndex);
    static void ValidateParams(sqlite3_stmt* Statement, int ColumnIndex);

public:
    static FieldValue GetFieldValue(sqlite3_stmt* Statement, int ColumnIndex);
};


///////////////////


class SQLite_Executor
{
    SQLite_Executor() = delete;

private:

    static TableValues1D GetOneRow(sqlite3_stmt* RawStmt, const int ColumnCount)
    {
        TableValues1D Row;

        for (int ColumnIndex = 0; ColumnIndex < ColumnCount; ++ColumnIndex)
        {
            Row.emplace_back(SQLite_FieldReader::GetFieldValue(RawStmt, ColumnIndex));
        }

        return Row;
    }

    static SQL_TableValues2D GetQueryResult(const SQLite_Statement& Statement)
    {
        CONFIRM(Statement.IsValid());

        sqlite3_stmt* RawStmt = Statement.Get();
        CONFIRMS(RawStmt, "RawStmt is not valid.");

        TableValues2D ResultValues2D;

        const int ColumnCount = sqlite3_column_count(RawStmt);

        while (Statement.Step())
        {
            ResultValues2D.emplace_back(GetOneRow(RawStmt, ColumnCount));
        }

        return ResultValues2D;
    }

public:

    static SQL_TableValues2D ExecuteQuery(const std::string& DbPath, const std::string& Query)
    {
        CONFIRM(!DbPath.empty());
        CONFIRM(!Query.empty());

        SQLite_DbConnection Connection(DbPath.c_str(), false, true);
        StatementCreationKit Kit = Connection.MakeStatementKit();
        SQLite_Statement Statement(Kit, Query.c_str());
        CONFIRM(Statement.IsValid());

        return GetQueryResult(Statement);
    }
};


#endif // SQLITE_CONNECTOR_H
