
#include <mutex>
////////
#include "SQLiteConnector.h"

////////////////////

void DbChecker::CheckResult(sqlite3* DbHandle, const int ResultCode, const char* OperatonType)
{
    if (ResultCode == SQLITE_OK) return;
    CONFIRM(DbHandle);
    const int ErrorCode = sqlite3_extended_errcode(DbHandle);
    std::print(std::cerr, 
        "ErrorCode: {}, Operation: [{}]: {}\n", ErrorCode, OperatonType, sqlite3_errmsg(DbHandle));
    CONFIRM(false);
}

void DbChecker::WarnIfFalse(const bool Condition, const char* WarningMsg)
{
    if (Condition) return;
    CONFIRM(WarningMsg);
    std::print(std::cerr, "Warning: {}\n", WarningMsg);
}

////////////////////

void ConnectionSentinel::Register(SQLiteStatement* Statement)
{
    std::scoped_lock Lock(Mutex);
    ActiveStatements.insert(Statement);
}

void ConnectionSentinel::Unregister(SQLiteStatement* Statement)
{
    std::scoped_lock Lock(Mutex);
    ActiveStatements.erase(Statement);
}

void ConnectionSentinel::InvalidateAll()
{
    std::unordered_set<SQLiteStatement*> Snapshot;
    {
        std::scoped_lock Lock(Mutex);
        Snapshot = ActiveStatements;
        ActiveStatements.clear();
    }

    for (SQLiteStatement* Statement : Snapshot)
    {
        if (Statement == nullptr) return;
        Statement->FinalizeFromSentinel(); // Must be safe to call multiple times
    }
}

ConnectionSentinel::~ConnectionSentinel()
{
    InvalidateAll();
}
