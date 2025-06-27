
#include <mutex>
#include <cstring>
////////
#include <sqlite3.h>
#include <gtest/gtest.h>
////////
#include "SQLiteConnector.h"


////////////////////


void SQLite_DbConnection::SetupConnection(const std::string& WayToDb, const bool IsReadOnly, const bool EnableSentinel)
{
    CONFIRM(!WayToDb.empty());
    const int OpenFlags = IsReadOnly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE;
    const int AllFlags = OpenFlags | SQLITE_OPEN_EXRESCODE | SQLITE_OPEN_FULLMUTEX;
    const int OpenResult = sqlite3_open_v2(WayToDb.c_str(), &DbHandle, AllFlags, nullptr);

    CheckOpenResult(OpenResult);

    if (!EnableSentinel) return;
    Sentinel = std::make_shared<SQLite_ConnectionSentinel>();
}

void SQLite_DbConnection::CheckOpenResult(const int OpenResult)
{
    if (OpenResult == SQLITE_OK) return;
    CloseConnection();
    CONFIRMS(!OpenResult,
        "Error {}: [{}]", OpenResult, sqlite3_errmsg(DbHandle));
}

SQLite_DbConnection::SQLite_DbConnection(const std::string& WayToDb, const bool IsReadOnly, const bool EnableSentinel)
    : DbHandle(nullptr), Sentinel(nullptr)
{
    SetupConnection(WayToDb, IsReadOnly, EnableSentinel);
}

void SQLite_DbConnection::CloseConnection()
{
    if (Sentinel) Sentinel->InvalidateAll();
    const int CloseResult = sqlite3_close(DbHandle);
    
    CONFIRMS(!CloseResult,
        "Error {}: [{}]", CloseResult, sqlite3_errmsg(DbHandle));
}

SQLite_DbConnection::~SQLite_DbConnection()
{
    CloseConnection();
}

sqlite3* SQLite_DbConnection::Get() const
{
    CONFIRM(DbHandle);
    return DbHandle;
}


////////////////////


StatementCreationKit SQLite_DbConnection::MakeStatementKit() const
{
    return StatementCreationKit{ Sentinel, DbHandle };
}


////////////////////


void SQLite_Statement::WarnOnUnusedTail(const std::string& UnusedTail)
{
    const bool NoUnusedTail = UnusedTail.empty() || UnusedTail[0] == '\0';
    if (NoUnusedTail) return;
    std::cerr << std::format("\n Unused tail [{}]: {}\n", UnusedTail.size(), UnusedTail);
}

void SQLite_Statement::PrepareStatement(sqlite3* DbHandleParam, const std::string& SqlQuery)
{
    DbHandle = DbHandleParam;
    const char* UnusedTail = nullptr;
    const int PrepareResult = sqlite3_prepare_v2(DbHandleParam, SqlQuery.c_str(), -1, &StatementPtr, &UnusedTail);
    
    CONFIRMS(!PrepareResult,
        "Error [{}]: [{}]", PrepareResult, sqlite3_errmsg(DbHandleParam));

    WarnOnUnusedTail(UnusedTail);
}

void SQLite_Statement::RegisterInSentinel(std::weak_ptr<SQLite_ConnectionSentinel> Sentinel)
{
    std::shared_ptr<SQLite_ConnectionSentinel> SharedSentinel = Sentinel.lock();
    if (!SharedSentinel) return;
    SharedSentinel->Register(this);
}

void SQLite_Statement::UnRegisterFromSentinel()
{
    std::shared_ptr<SQLite_ConnectionSentinel> SharedSentinel = Sentinel.lock();
    if (!SharedSentinel) return;
    SharedSentinel->Unregister(this);
}

SQLite_Statement::SQLite_Statement(sqlite3* DbHandleParam, const std::string& SqlQuery)
    : DbHandle(nullptr), StatementPtr(nullptr), Sentinel()
{
    CONFIRMS(DbHandleParam, "DbHandleParam must not be null");
    CONFIRM(!SqlQuery.empty());
    PrepareStatement(DbHandleParam, SqlQuery);
}

SQLite_Statement::SQLite_Statement(const StatementCreationKit& Kit, const std::string& SqlQuery)
    : SQLite_Statement(Kit.DbHandle, SqlQuery)
{
    Sentinel = Kit.Sentinel;
    RegisterInSentinel(Sentinel);
}

SQLite_Statement::SQLite_Statement(SQLite_Statement&& Other) noexcept
    : DbHandle(Other.DbHandle),
      StatementPtr(Other.StatementPtr),
      Sentinel(std::move(Other.Sentinel))
{
    // Transfer ownership and nullify the source
    Other.DbHandle = nullptr;
    Other.StatementPtr = nullptr;

    // Re-register this object in the sentinel (if valid)
    RegisterInSentinel(Sentinel);
}

SQLite_Statement& SQLite_Statement::operator=(SQLite_Statement&& Other) noexcept
{
    if (this == &Other) return *this;

    Finalize(); // Ensure this object releases any held resources

    DbHandle = Other.DbHandle;
    StatementPtr = Other.StatementPtr;
    Sentinel = std::move(Other.Sentinel);

    Other.DbHandle = nullptr;
    Other.StatementPtr = nullptr;

    RegisterInSentinel(Sentinel);

    return *this;
}

void SQLite_Statement::Finalize()
{
    UnRegisterFromSentinel();
    if (StatementPtr == nullptr) return;
    const int FinalizeResult = sqlite3_finalize(StatementPtr);

    CONFIRMS(!FinalizeResult,
        "Error [{}]: [{}]", FinalizeResult, sqlite3_errmsg(DbHandle));

    StatementPtr = nullptr;
    DbHandle = nullptr;
}

SQLite_Statement::~SQLite_Statement()
{
    Finalize();
}

sqlite3_stmt* SQLite_Statement::Get() const
{
    if (!IsValid()) return nullptr;
    CONFIRM(StatementPtr);
    return StatementPtr;
}

bool SQLite_Statement::IsValid() const
{
    return StatementPtr != nullptr && DbHandle != nullptr;
}

bool SQLite_Statement::Step() const
{
    if (!IsValid()) return false;
    const int StepResult = sqlite3_step(StatementPtr);
    switch (StepResult)
    {
        case SQLITE_ROW:
            return true;
        case SQLITE_DONE:
            return false;
        default:
            CONFIRMS(!StepResult,
                "Error [{}]: [{}]", StepResult, sqlite3_errmsg(DbHandle));
            return false;
    }
}

void SQLite_Statement::Reset() const
{
    if (!IsValid()) return;
    const int ResetResult = sqlite3_reset(StatementPtr);
    CONFIRMS(!ResetResult,
        "Error [{}]: [{}]", ResetResult, sqlite3_errmsg(DbHandle));
}


////////////////////


void SQLite_ConnectionSentinel::Register(SQLite_Statement* Statement)
{
    std::scoped_lock Lock(Mutex);
    ActiveStatements.insert(Statement);
}

void SQLite_ConnectionSentinel::Unregister(SQLite_Statement* Statement)
{
    std::scoped_lock Lock(Mutex);
    ActiveStatements.erase(Statement);
}

void SQLite_ConnectionSentinel::InvalidateAll()
{
    std::unordered_set<SQLite_Statement*> Snapshot;

    {
        std::scoped_lock Lock(Mutex);
        Snapshot = ActiveStatements;
        ActiveStatements.clear();
    }

    for (SQLite_Statement* Statement : Snapshot)
    {
        if (Statement == nullptr) continue;
        Statement->Finalize();
    }
}

SQLite_ConnectionSentinel::~SQLite_ConnectionSentinel()
{
    InvalidateAll();
}


////////////////////


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

std::unordered_set<std::string> SQLite_ParamValidator::GetParamNames(const std::vector<BindablePair> &NamedValues)
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

void SQLite_NamedParamBinder::ApplyBindablePairsToStatement(sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues)
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



////////////////////



FieldValue SQLite_FieldReader::GetFieldValue(sqlite3_stmt* Statement, int ColumnIndex)
{
    ValidateParams(Statement, ColumnIndex);

    const int Type = sqlite3_column_type(Statement, ColumnIndex);

    switch (Type)
    {
        case SQLITE_INTEGER:
            return ExtractInteger(Statement, ColumnIndex);
        case SQLITE_FLOAT:
            return ExtractDouble(Statement, ColumnIndex);
        case SQLITE_TEXT:
            return ExtractText(Statement, ColumnIndex);
        case SQLITE_BLOB:
            return ExtractBlob(Statement, ColumnIndex);
        case SQLITE_NULL:
            return nullptr;
        default:
            CONFIRMS(false, "Unknown SQLite column type: {}", Type);
    }
}

//\/\/\/\/\/\/\/\/\/

int64_t SQLite_FieldReader::ExtractInteger(sqlite3_stmt* Statement, int ColumnIndex)
{
    return static_cast<int64_t>(sqlite3_column_int64(Statement, ColumnIndex));
}

//=================

double SQLite_FieldReader::ExtractDouble(sqlite3_stmt* Statement, int ColumnIndex)
{
    return sqlite3_column_double(Statement, ColumnIndex);
}

//=================

std::string SQLite_FieldReader::ExtractText(sqlite3_stmt* Statement, int ColumnIndex)
{
    const unsigned char* TextPtr = sqlite3_column_text(Statement, ColumnIndex);
    const int Bytes = sqlite3_column_bytes(Statement, ColumnIndex);
    return std::string(reinterpret_cast<const char*>(TextPtr), Bytes);
}

//=================

std::vector<uint8_t> SQLite_FieldReader::ExtractBlob(sqlite3_stmt* Statement, int ColumnIndex)
{
    const void* BlobPtr = sqlite3_column_blob(Statement, ColumnIndex);
    const int Size = sqlite3_column_bytes(Statement, ColumnIndex);
    const uint8_t* ByteData = static_cast<const uint8_t*>(BlobPtr);
    return std::vector<uint8_t>(ByteData, ByteData + Size);
}

//=================

void SQLite_FieldReader::ValidateParams(sqlite3_stmt* Statement, int ColumnIndex)
{
    ValidateStatementReadiness(Statement);
    ValidateColumnIndex(Statement, ColumnIndex);
}

//\/\/\/\/\/\/\/\/\/

void SQLite_FieldReader::ValidateColumnIndex(sqlite3_stmt* Statement, int ColumnIndex)
{
    const int ColumnCount = sqlite3_column_count(Statement);
    CONFIRMS(ColumnIndex >= 0 && ColumnIndex < ColumnCount,
            "Invalid ColumnIndex: {}, statement has {} columns", ColumnIndex, ColumnCount);
}

//=================

void SQLite_FieldReader::ValidateStatementReadiness(sqlite3_stmt* Statement)
{
    CONFIRMS(Statement,
        "sqlite3_stmt* Statement is not valid.");

    const int RowCount = sqlite3_data_count(Statement);
    CONFIRMS(RowCount > 0,
        "No active row: statement was either not stepped or has been finalized.");
}



////////////////////


