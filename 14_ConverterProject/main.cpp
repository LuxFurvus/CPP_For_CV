
#include <format>
#include <string>
#include <iostream>
#include <cassert>
////////
#include <sqlite3.h>
////////
#include "ConditionChecker/ConditionChecker.h"
#include "SQLiteConnector/SQLiteConnector.h"

namespace Invariants
{
    constexpr const char* WayToDb = "northwind.db";
};

////////////////

class SQLiteStatement
{
private:
    sqlite3_stmt* StatementPtr = nullptr;
    sqlite3* DbHandle = nullptr;
    DbChecker Checker;

public:
    SQLiteStatement(sqlite3* DbHandleParam, const char* SqlQuery) : Checker(DbHandleParam)
    {
        CONFIRM(DbHandleParam);
        CONFIRM(SqlQuery);

        DbHandle = DbHandleParam;
        const int PrepareResult = sqlite3_prepare_v2(DbHandle, SqlQuery, -1, &StatementPtr, nullptr);
        Checker.CheckResult(PrepareResult, "prepare");
    }
    ~SQLiteStatement()
    {
        const int FinalizeResult = sqlite3_finalize(StatementPtr);
        Checker.CheckResult(FinalizeResult, "finalize");
    }
    sqlite3_stmt* Get() const
    {
        CONFIRM(StatementPtr);
        return StatementPtr;
    }
    bool Step() const
    {
        const int StepResult = sqlite3_step(StatementPtr);
        switch(StepResult)
        {
            case SQLITE_ROW:
                return true;
            case SQLITE_DONE:
                return false;
            default:
                Checker.CheckResult(StepResult, "step");
                return false;
        }
    }
    void Reset() const
    {
        const int ResetResult = sqlite3_reset(StatementPtr);
        Checker.CheckResult(ResetResult, "reset");
    }
};

////////////////

class SQLiteStatementExecutor
{
private:
    const SQLiteStatement& Statement;

public:
    SQLiteStatementExecutor(const SQLiteStatement& InStatement) : Statement(InStatement)
    {}

    std::vector<std::string> GetText() const
    {
        std::vector<std::string> ReturnStrings;

        for (size_t i = 0; Statement.Step(); ++i)
        {
            const unsigned char* RawText = sqlite3_column_text(Statement.Get(), 0);
            std::string ContactName = RawText ? reinterpret_cast<const char*>(RawText) : "";
            ReturnStrings.push_back(ContactName);
        }
        return ReturnStrings;
    }
};

void ReadFirstContactName()
{
    const DbConnection DbHandle(Invariants::WayToDb, true);

    const SQLiteStatement Statement(DbHandle.Get(), "SELECT ContactName FROM Customers LIMIT 1;"); 
    
    SQLiteStatementExecutor Executor(Statement);

    const std::vector<std::string> ContactName = Executor.GetText();
    std::print(std::cout, "First contact name: {}\n", ContactName);
}

int main()
{ 
    ReadFirstContactName();
}