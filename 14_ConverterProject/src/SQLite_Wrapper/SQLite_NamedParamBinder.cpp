
#include "SQLite_NamedParamBinder.h"
////////
#include <sqlite3.h>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <variant>
#include <vector>
////////
#include "../ConditionChecker/ConditionChecker.h"

////////////////////

std::unordered_set<std::string> SQLite_ParamValidator::GetParamNames(
    const std::vector<BindablePair> &NamedValues)
{
    std::unordered_set<std::string> Names;
    for (const auto& [Name, _] : NamedValues)
    {
        Names.emplace(Name); // Keep full name including prefix
    }
    return Names;
}

void SQLite_ParamValidator::CheckIfParamsInStatement(
    sqlite3_stmt* Statement,
    const std::unordered_set<std::string>& ParamNames)
{
    const int ParamCount = sqlite3_bind_parameter_count(Statement);
    for (int i = 1; i <= ParamCount; ++i)
    {
        const char* ParamName = sqlite3_bind_parameter_name(Statement, i);
        if (ParamName == nullptr || ParamName[0] == '\0') continue;

        CONFIRMS(ParamNames.contains(ParamName),
            "Param name {} is not provided", ParamName);
    }
}

void SQLite_ParamValidator::BindParamsByName(sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues)
{
    CONFIRM(Statement);
    CONFIRM(!NamedValues.empty());

    const int ParamCount = sqlite3_bind_parameter_count(Statement);
    CONFIRM(ParamCount > 0);

    const std::unordered_set<std::string> ProvidedNames = GetParamNames(NamedValues);
    CheckIfParamsInStatement(Statement, ProvidedNames);
}

int SQLite_NamedParamBinder::BindOneParam(sqlite3_stmt* Statement, const int ParamIndex, const FieldValue& Value)
{
    return std::visit(SQLite_ParamVisitor(Statement, ParamIndex), Value);
}


///////////////////


SQLite_ParamVisitor::SQLite_ParamVisitor(sqlite3_stmt* Statement, const int ParamIndex)
    : Statement(Statement), ParamIndex(ParamIndex)
{
    CONFIRM(Statement);
    CONFIRM(ParamIndex > 0);
}

int SQLite_ParamVisitor::operator()(const int64_t Value) const
{
    return sqlite3_bind_int64(Statement, ParamIndex, Value);
}

int SQLite_ParamVisitor::operator()(const int Value) const
{
    return (*this)(static_cast<int64_t>(Value));
}

int SQLite_ParamVisitor::operator()(const double Value) const
{
    return sqlite3_bind_double(Statement, ParamIndex, Value);
}

int SQLite_ParamVisitor::operator()(const std::string& Value) const
{
    return sqlite3_bind_text(Statement, ParamIndex, Value.c_str(), -1, SQLITE_TRANSIENT);
}

int SQLite_ParamVisitor::operator()(const std::vector<uint8_t>& Value) const
{
    return sqlite3_bind_blob(Statement, ParamIndex, Value.data(), static_cast<int>(Value.size()), SQLITE_TRANSIENT);
}

int SQLite_ParamVisitor::operator()(std::nullptr_t) const
{
    return sqlite3_bind_null(Statement, ParamIndex);
}


////////////////////


void SQLite_NamedParamBinder::PrepareStatementForBinding(sqlite3_stmt* Statement)
{
    const int ResetResult = sqlite3_reset(Statement);
    CONFIRMS(!ResetResult,
        "Error [{}]: Failed to reset statement", ResetResult);

    const int ClearResult = sqlite3_clear_bindings(Statement);
    CONFIRMS(!ClearResult,
        "Error [{}]: Failed to clear bindings", ClearResult);
}

std::string SQLite_NamedParamBinder::NormalizeParamName(const std::string &Name)
{
    if (Name.empty()) return Name;
    const char Prefix = Name[0];
    if (Prefix == ':' || Prefix == '@' || Prefix == '$')
    {
        return Name.substr(1);
    }
    return Name;
}

int SQLite_NamedParamBinder::GetParamIndexForProvidedName(sqlite3_stmt* Statement, const std::string& ProvidedName)
{
    const std::string Normalized = NormalizeParamName(ProvidedName);

    int ParamIndex = 0;

    // Try all 3 possible prefixes
    for (const char Prefix : {':', '@', '$'})
    {
        const std::string Prefixed = std::string(1, Prefix) + Normalized;
        ParamIndex = sqlite3_bind_parameter_index(Statement, Prefixed.c_str());
        if (ParamIndex != 0) break;
    }

    CONFIRMS(ParamIndex != 0,
            "Param name {} is not provided", ProvidedName);

    return ParamIndex;    
}

void SQLite_NamedParamBinder::ApplyBindablePairsToStatement(
    sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues)
{
    for (const auto& [ProvidedName, Value] : NamedValues)
    {
        const int ParamIndex = GetParamIndexForProvidedName(Statement, ProvidedName);      

        const int BindResult = BindOneParam(Statement, ParamIndex, Value);
        CONFIRMS(!BindResult,
            "Error [{}]: Failed to bind param {}", BindResult, ProvidedName);
    }
}

void SQLite_NamedParamBinder::BindParamsByName(sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues)
{
    SQLite_ParamValidator::BindParamsByName(Statement, NamedValues);
    PrepareStatementForBinding(Statement);
    ApplyBindablePairsToStatement(Statement, NamedValues);
}


