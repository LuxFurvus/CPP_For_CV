#include <format>
#include <string>
#include <iostream>
#include <cassert>
////////
#include <sqlite3.h>
////////
#include "../ConditionChecker/ConditionChecker.h"

class DbChecker
{
    friend class SQLiteConnector_DbChecker_Test;
private:
    sqlite3*& DbHandle;

    DbChecker() = delete;
public:
    DbChecker(sqlite3*& DbHandleParam) : DbHandle(DbHandleParam) {}

    void CheckResult(const int ResultCode, const char* OperatonType) const
    {
        if (ResultCode == SQLITE_OK) return;
        const int ErrorCode = sqlite3_extended_errcode(DbHandle);
        std::print(std::cerr, 
            "ErrorCode {} while [{}]: {}\n", ErrorCode, OperatonType, sqlite3_errmsg(DbHandle));
        CONFIRM(false);
    }
};

///////////////////////////////

class DbConnection
{
    friend class SQLiteConnector_DbConnection_Test;
private:
    sqlite3* DbHandle = nullptr;
    DbChecker Checker;

public:
    DbConnection(const char* WayToDb, const bool IsReadOnly) : Checker(DbHandle)
    {
        CONFIRM(WayToDb);
        const int OpenFlags = IsReadOnly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE;
        const int AllFlags = OpenFlags | SQLITE_OPEN_EXRESCODE | SQLITE_OPEN_FULLMUTEX;
        const int OpenResult = sqlite3_open_v2(WayToDb, &DbHandle, AllFlags, nullptr);
        Checker.CheckResult(OpenResult, "open database");
    }
    ~DbConnection()
    {
        const int CloseResult = sqlite3_close(DbHandle);
        Checker.CheckResult(CloseResult, "close database");
    }
    sqlite3* Get() const
    {
        CONFIRM(DbHandle);
        return DbHandle;
    }
};

///////////////////////////////
