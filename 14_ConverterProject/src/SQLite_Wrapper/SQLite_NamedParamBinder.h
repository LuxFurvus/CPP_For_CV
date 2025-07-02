#pragma once
#ifndef SQLITE_NAMED_PARAM_BINDER_H
#define SQLITE_NAMED_PARAM_BINDER_H

#include <string>
#include <unordered_set>
#include <variant>
#include <cstdint>
#include <vector>
////////

struct sqlite3_stmt;

///////////////////


using FieldValue = std::variant<int64_t, double, std::string, std::vector<uint8_t>, std::nullptr_t>;

using BindablePair = std::pair<std::string, FieldValue>;


///////////////////


class SQLite_NamedParamBinderValidator
{
    friend class SQLite_NamedParamBinder;
    SQLite_NamedParamBinderValidator() = delete;

private:
    static std::string NormalizeParamName(const std::string& Name);

    static std::unordered_set<std::string> GetParamNames(
            const std::vector<BindablePair>& NamedValues);
    
    static void CheckIfParamsInStatement(
        sqlite3_stmt* Statement, const std::unordered_set<std::string>& ParamNames);

private:
    static void Validate(sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues);
};


///////////////////


struct SQLite_ParamVisitor
{
    SQLite_ParamVisitor() = delete;

private:
    sqlite3_stmt* Statement;
    int ParamIndex;

public:
    SQLite_ParamVisitor(sqlite3_stmt* InStatement, const int InParamIndex);

    int operator()(const int64_t Value) const;
    int operator()(const int Value) const;
    int operator()(const double Value) const;
    int operator()(const std::string& Value) const;
    int operator()(const std::vector<uint8_t>& Value) const;
    int operator()(std::nullptr_t) const;
};


///////////////////


class SQLite_NamedParamBinder
{
    friend class SQLite_NamedParamBinderTest_T08_InvalidParamIndexShouldThrow_Test;
    SQLite_NamedParamBinder() = delete;

private:
    static void PrepareStatementForBinding(sqlite3_stmt* Statement);
    static std::string NormalizeParamName(const std::string &Name);
    static int BindOneParam(sqlite3_stmt *Statement, int ParamIndex, const FieldValue &Value);
    static int GetParamIndexForProvidedName(
        sqlite3_stmt *Statement, const std::string &ProvidedName);
    static void ApplyBindablePairsToStatement(
        sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues);

public:
    static void BindParamsByName(
        sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues);
};

///////////////////

#endif // SQLITE_NAMED_PARAM_BINDER_H