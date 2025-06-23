
#include <mutex>
#include <cstring>
////////
#include "SQLiteConnector.h"

////////////////////

void SQLite_DbChecker::CheckResult(const int ResultCode, const char* OperatonType)
{
    if (ResultCode == SQLITE_OK) return;
    CONFIRMS(false, "ErrorCode: {}, Operation: [{}]", ResultCode, OperatonType);
}

void SQLite_DbChecker::CheckResult(sqlite3* DbHandle, const int ResultCode, const char* OperatonType)
{
    if (ResultCode == SQLITE_OK) return;
    CONFIRMF(OperatonType, "OperatonType message is null");

    const int ErrorCode = sqlite3_extended_errcode(DbHandle);
    CONFIRMS(false,
        "ErrorCode: {}, Operation: [{}]: {}", ErrorCode, OperatonType, sqlite3_errmsg(DbHandle));
}

void SQLite_DbChecker::WarnIfFalse(const bool Condition, const char* WarningMsg)
{
    if (Condition) return;
    CONFIRM(WarningMsg);
    std::print(std::cerr, "Warning: {}\n", WarningMsg);
}

////////////////////


void SQLite_DbConnection::SetupConnection(const char* WayToDb, const bool IsReadOnly, const bool EnableSentinel)
{
    CONFIRM(WayToDb);
    const int OpenFlags = IsReadOnly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE;
    const int AllFlags = OpenFlags | SQLITE_OPEN_EXRESCODE | SQLITE_OPEN_FULLMUTEX;
    const int OpenResult = sqlite3_open_v2(WayToDb, &DbHandle, AllFlags, nullptr);

    CONFIRMS(!OpenResult,
        "Error {}: [{}]", OpenResult, sqlite3_errmsg(DbHandle));

    if (!EnableSentinel) return;
    Sentinel = std::make_shared<ConnectionSentinel>();
}

SQLite_DbConnection::SQLite_DbConnection(const char* WayToDb, const bool IsReadOnly, const bool EnableSentinel)
    : DbHandle(nullptr), Sentinel(nullptr)
{
    SetupConnection(WayToDb, IsReadOnly, EnableSentinel);
}

SQLite_DbConnection::~SQLite_DbConnection()
{
    if (Sentinel) Sentinel->InvalidateAll();
    const int CloseResult = sqlite3_close(DbHandle);
    
    CONFIRMS(!CloseResult,
        "Error {}: [{}]", CloseResult, sqlite3_errmsg(DbHandle));
}

sqlite3* SQLite_DbConnection::Get() const
{
    CONFIRM(DbHandle);
    return DbHandle;
}

StatementCreationKit SQLite_DbConnection::MakeStatementKit() const
{
    return StatementCreationKit{ Sentinel, DbHandle };
}


////////////////////


void SQLite_Statement::WarnOnUnusedTail(const char* UnusedTail)
{
    const bool NoUnusedTail = UnusedTail == nullptr || strlen(UnusedTail) == 0;
    if (NoUnusedTail) return;
    const std::string UnusedTailMsg = std::format("Unused tail [{}]: {}", strlen(UnusedTail), UnusedTail);
    SQLite_DbChecker::WarnIfFalse(NoUnusedTail, UnusedTailMsg.c_str());
}

void SQLite_Statement::FinalizeFromSentinel()
{
    if (!IsValid()) return;
    sqlite3_finalize(StatementPtr);
    StatementPtr = nullptr;
    DbHandle = nullptr;
}

void SQLite_Statement::PrepareStatement(sqlite3* DbHandleParam, const char* SqlQuery)
{
    DbHandle = DbHandleParam;
    const char* UnusedTail = nullptr;
    const int PrepareResult = sqlite3_prepare_v2(DbHandleParam, SqlQuery, -1, &StatementPtr, &UnusedTail);
    SQLite_DbChecker::CheckResult(DbHandle, PrepareResult, "prepare");
    WarnOnUnusedTail(UnusedTail);
}

void SQLite_Statement::RegisterInSentinel(std::weak_ptr<ConnectionSentinel> Sentinel)
{
    std::shared_ptr<ConnectionSentinel> SharedSentinel = Sentinel.lock();
    if (!SharedSentinel) return;
    SharedSentinel->Register(this);
}

void SQLite_Statement::UnRegisterFromSentinel()
{
    std::shared_ptr<ConnectionSentinel> SharedSentinel = Sentinel.lock();
    if (!SharedSentinel) return;
    SharedSentinel->Unregister(this);
}

SQLite_Statement::SQLite_Statement(sqlite3* DbHandleParam, const char* SqlQuery)
    : DbHandle(nullptr), StatementPtr(nullptr), Sentinel()
{
    CONFIRM(DbHandleParam);
    CONFIRM(SqlQuery);
    PrepareStatement(DbHandleParam, SqlQuery);
}

SQLite_Statement::SQLite_Statement(const StatementCreationKit& Kit, const char* SqlQuery)
    : SQLite_Statement(Kit.DbHandle, SqlQuery)
{
    Sentinel = Kit.Sentinel;
    RegisterInSentinel(Sentinel);
}

SQLite_Statement::~SQLite_Statement()
{
    UnRegisterFromSentinel();
    if (StatementPtr == nullptr) return;
    const int FinalizeResult = sqlite3_finalize(StatementPtr);
    SQLite_DbChecker::CheckResult(DbHandle, FinalizeResult, "finalize");
    StatementPtr = nullptr;
    DbHandle = nullptr;
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
            SQLite_DbChecker::CheckResult(DbHandle, StepResult, "step");
            return false;
    }
}

void SQLite_Statement::Reset() const
{
    if (!IsValid()) return;
    const int ResetResult = sqlite3_reset(StatementPtr);
    SQLite_DbChecker::CheckResult(DbHandle, ResetResult, "reset");
}


////////////////////


void ConnectionSentinel::Register(SQLite_Statement* Statement)
{
    std::scoped_lock Lock(Mutex);
    ActiveStatements.insert(Statement);
}

void ConnectionSentinel::Unregister(SQLite_Statement* Statement)
{
    std::scoped_lock Lock(Mutex);
    ActiveStatements.erase(Statement);
}

void ConnectionSentinel::InvalidateAll()
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
        Statement->FinalizeFromSentinel();
    }
}

ConnectionSentinel::~ConnectionSentinel()
{
    InvalidateAll();
}


////////////////////


SQLiteParamVisitor::SQLiteParamVisitor(sqlite3_stmt* Statement, const int ParamIndex)
    : Statement(Statement), ParamIndex(ParamIndex)
{
    CONFIRM(Statement);
    CONFIRM(ParamIndex > 0);
}

int SQLiteParamVisitor::operator()(const int64_t Value) const
{
    return sqlite3_bind_int64(Statement, ParamIndex, Value);
}

int SQLiteParamVisitor::operator()(const int Value) const
{
    return (*this)(static_cast<int64_t>(Value));
}

int SQLiteParamVisitor::operator()(const double Value) const
{
    return sqlite3_bind_double(Statement, ParamIndex, Value);
}

int SQLiteParamVisitor::operator()(const std::string& Value) const
{
    return sqlite3_bind_text(Statement, ParamIndex, Value.c_str(), -1, SQLITE_TRANSIENT);
}

int SQLiteParamVisitor::operator()(std::nullptr_t) const
{
    return sqlite3_bind_null(Statement, ParamIndex);
}


////////////////////

std::string SQLite_ParamValidator::NormalizeParamName(const std::string &Name)
{
    if (Name.empty()) return Name;
    const char Prefix = Name[0];
    if (Prefix == ':' || Prefix == '@' || Prefix == '$')
    {
        return Name.substr(1);
    }
    return Name;
}

std::unordered_set<std::string> SQLite_ParamValidator::GetParamNames(const std::vector<BindablePair> &NamedValues)
{
    std::unordered_set<std::string> Names;
    for (const auto& [Name, _] : NamedValues)
    {
        Names.emplace(NormalizeParamName(Name));
    }
    return Names;
}

void SQLite_ParamValidator::CheckForParamNamePresence(sqlite3_stmt* Statement, const std::unordered_set<std::string>& ParamNames)
{
    const int ParamCount = sqlite3_bind_parameter_count(Statement);
    for (int i = 1; i <= ParamCount; ++i)
    {
        const char* ParamName = sqlite3_bind_parameter_name(Statement, i);
        if (ParamName == nullptr || ParamName[0] == '\0') continue;

        const std::string NormalizedParamName = NormalizeParamName(ParamName);

        CONFIRMF(ParamNames.contains(NormalizedParamName),
            std::format("Param name {} is not provided", ParamName));
    }
}

void SQLite_ParamValidator::ValidateParamNames(sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues)
{
    const std::unordered_set<std::string> ProvidedNames = GetParamNames(NamedValues);
    CheckForParamNamePresence(Statement, ProvidedNames);
}

int SQLite_NamedParamBinder::BindOneParam(sqlite3_stmt* Statement, const int ParamIndex, const BindableValue& Value)
{
    return std::visit(SQLiteParamVisitor(Statement, ParamIndex), Value);
}

void SQLite_NamedParamBinder::PrepareStatementForBinding(sqlite3_stmt* Statement)
{
    const int ResetResult = sqlite3_reset(Statement);
    CONFIRMF(!ResetResult,
        std::format("Error [{}]: Failed to reset statement", ResetResult));

    const int ClearResult = sqlite3_clear_bindings(Statement);
    CONFIRMF(!ClearResult,
        std::format("Error [{}]: Failed to clear bindings", ClearResult));
}

void SQLite_NamedParamBinder::ApplyBindablePairsToStatement(sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues)
{
    for (const auto& [Name, Value] : NamedValues)
    {
        const int ParamIndex = sqlite3_bind_parameter_index(Statement, Name.c_str());

        CONFIRMF(ParamIndex != 0,
            std::format("Param name {} is not provided", Name));

        const int BindResult = BindOneParam(Statement, ParamIndex, Value);

        CONFIRMF(!BindResult,
            std::format("Error [{}]: Failed to bind param {}", BindResult, Name));
    }
}

void SQLite_NamedParamBinder::BindParamsByName(sqlite3_stmt* Statement, const std::vector<BindablePair>& NamedValues)
{
    CONFIRM(Statement);
    SQLite_ParamValidator::ValidateParamNames(Statement, NamedValues);
    PrepareStatementForBinding(Statement);
    ApplyBindablePairsToStatement(Statement, NamedValues);
}


////////////////////

