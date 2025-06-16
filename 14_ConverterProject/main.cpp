
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

///////////////////////////////

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