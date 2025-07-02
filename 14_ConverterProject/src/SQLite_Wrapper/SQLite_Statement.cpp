
#include "SQLite_Statement.h"
////////
#include <sqlite3.h>
////////
#include "../ConditionChecker/ConditionChecker.h"


////////////////////


SQLite_Statement::SQLite_Statement(const SQLite_StatementCreationKit& Kit, const std::string& SqlQuery)
    : SQLite_Statement(Kit.DbHandle, SqlQuery)
{
    Sentinel = Kit.Sentinel;
    RegisterInSentinel(Sentinel);
}

//\/\/\/\/\/\/\/\/\/

void SQLite_Statement::RegisterInSentinel(std::weak_ptr<SQLite_StatementSentinel> InSentinel)
{
    std::shared_ptr<SQLite_StatementSentinel> SharedSentinel = InSentinel.lock();
    if (!SharedSentinel) return;
    SharedSentinel->Register(this);
}

//=================

SQLite_Statement::SQLite_Statement(sqlite3* DbHandleParam, const std::string& SqlQuery)
    : DbHandle(nullptr), StatementPtr(nullptr), Sentinel()
{
    CONFIRMS(DbHandleParam, "DbHandleParam must not be null");
    CONFIRM(!SqlQuery.empty());
    PrepareStatement(DbHandleParam, SqlQuery);
}

//\/\/\/\/\/\/\/\/\/

void SQLite_Statement::PrepareStatement(sqlite3* DbHandleParam, const std::string& SqlQuery)
{
    DbHandle = DbHandleParam;
    const char* UnusedTail = nullptr;
    const int PrepareResult = sqlite3_prepare_v2(DbHandleParam, SqlQuery.c_str(), -1, &StatementPtr, &UnusedTail);
    
    CONFIRMS(!PrepareResult,
        "Error [{}]: [{}]", PrepareResult, sqlite3_errmsg(DbHandleParam));

    WarnOnUnusedTail(UnusedTail);
}

//\/\/\/\/\/\/\/\/\/

void SQLite_Statement::WarnOnUnusedTail(const std::string& UnusedTail)
{
    WARNIFFALSE(UnusedTail.empty(),
        "Unused tail [{}]: {}", UnusedTail.size(), UnusedTail);
}

////////////////////

SQLite_Statement::SQLite_Statement(SQLite_Statement&& Other) noexcept
    : DbHandle(Other.DbHandle),
      StatementPtr(Other.StatementPtr),
      Sentinel(std::move(Other.Sentinel))
{
    Other.DbHandle = nullptr;
    Other.StatementPtr = nullptr;
    RegisterInSentinel(Sentinel);
}

////////////////////

SQLite_Statement& SQLite_Statement::operator=(SQLite_Statement&& Other) noexcept
{
    if (this == &Other) return *this;

    Finalize();

    DbHandle = Other.DbHandle;
    StatementPtr = Other.StatementPtr;
    Sentinel = std::move(Other.Sentinel);

    Other.DbHandle = nullptr;
    Other.StatementPtr = nullptr;

    RegisterInSentinel(Sentinel);

    return *this;
}

SQLite_Statement::~SQLite_Statement() noexcept
{
    Finalize();
}

//\/\/\/\/\/\/\/\/\/

void SQLite_Statement::Finalize() noexcept
{
    UnRegisterFromSentinel();

    const int FinalizeResult = sqlite3_finalize(StatementPtr);

    WARNIFFALSE(FinalizeResult == SQLITE_OK,
        "FinalizeResult == [{}]: [{}]", FinalizeResult, sqlite3_errmsg(DbHandle));

    StatementPtr = nullptr;
    DbHandle = nullptr;
}

//\/\/\/\/\/\/\/\/\/

void SQLite_Statement::UnRegisterFromSentinel()
{
    std::shared_ptr<SQLite_StatementSentinel> SharedSentinel = Sentinel.lock();
    if (!SharedSentinel) return;
    SharedSentinel->Unregister(this);
}

///////////////////

sqlite3_stmt* SQLite_Statement::Get() const
{
    if (!IsValid()) return nullptr;
    CONFIRM(StatementPtr);
    return StatementPtr;
}

///////////////////

bool SQLite_Statement::IsValid() const
{
    return StatementPtr != nullptr && DbHandle != nullptr;
}

///////////////////

bool SQLite_Statement::Step()
{
    CrashIfNotValid();

    const int StepResult = sqlite3_step(StatementPtr);
    switch (StepResult)
    {
        case SQLITE_ROW:
            return true;
        case SQLITE_DONE:
            return false;
        default:
            CONFIRMS(false, "Step Failed: {} = {}",
                StepResult, sqlite3_errmsg(DbHandle));
            return false;
    }
}

//\/\/\/\/\/\/\/\/\/

void SQLite_Statement::CrashIfNotValid()
{
    if (IsValid()) return;
    Finalize();
    CONFIRMS(IsValid(),
        "SQLite_Statement is no longer valid: {}", sqlite3_errmsg(DbHandle));
}

////////////////////

void SQLite_Statement::Reset()
{
    CrashIfNotValid();
    const int ResetResult = sqlite3_reset(StatementPtr);
    CONFIRMS(!ResetResult,
        "Error [{}]: [{}]", ResetResult, sqlite3_errmsg(DbHandle));
}


////////////////////
////////////////////
////////////////////


void SQLite_StatementSentinel::Register(SQLite_Statement* Statement)
{
    std::scoped_lock Lock(Mutex);
    ActiveStatements.insert(Statement);
}

////////////////////

void SQLite_StatementSentinel::Unregister(SQLite_Statement* Statement)
{
    std::scoped_lock Lock(Mutex);
    ActiveStatements.erase(Statement);
}

////////////////////

SQLite_StatementSentinel::~SQLite_StatementSentinel()
{
    InvalidateAll();
}

//\/\/\/\/\/\/\/\/\/

void SQLite_StatementSentinel::InvalidateAll() noexcept
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

