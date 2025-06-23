
#include <format>
#include <string>
#include <iostream>
#include <cassert>
#include <variant>
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


///////////////////////////////

class SQLiteStatementExecutor
{
public:

    static std::vector<std::string> GetText(const SQLite_Statement& Statement)
    {
        std::vector<std::string> ReturnStrings;
        while (Statement.Step())
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
    const SQLite_DbConnection DbHandle(Invariants::WayToDb, true);

    const SQLite_Statement Statement01(DbHandle.Get(), "SELECT ContactName FROM Customers LIMIT @Limit;"); 

    SQLite_NamedParamBinder::BindParamsByName(Statement01.Get(), { {"@Limit", 3} });
    
    const auto ResultTexts = SQLiteStatementExecutor::GetText(Statement01);

    std::print(std::cout, "First contact name: {}\n", ResultTexts);
}

int main()
{
    ReadFirstContactName();
}