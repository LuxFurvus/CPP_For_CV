
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

    constexpr const char* Query1 = "SELECT ProductName, UnitPrice FROM Products ORDER BY UnitPrice DESC LIMIT 3;";
    // TEXT + NUMERIC — fetch top 3 most expensive products

    constexpr const char* Query2 = "SELECT CompanyName, ContactName, Phone FROM Suppliers WHERE Country = 'USA';";
    // TEXT + TEXT + TEXT — suppliers based in USA with contact details

    constexpr const char* Query3 = "SELECT EmployeeID, LastName, FirstName, HireDate FROM Employees WHERE Title = 'Sales Representative';";
    // INTEGER + TEXT + TEXT + (assuming DATE as TEXT) — employees by role

    constexpr const char* Query4 = "SELECT COUNT(*) AS TotalOrders, CustomerID FROM Orders GROUP BY CustomerID HAVING TotalOrders > 5 LIMIT 5;";
    // INTEGER + TEXT — customers with more than 5 orders (aggregation + grouping)

    constexpr const char* Query5 = "SELECT ProductID, ProductName, Discontinued FROM Products WHERE Discontinued = '1' LIMIT 5;";
    // INTEGER + TEXT + TEXT — products marked as discontinued

};

///////////////////////////////


///////////////////////////////

// class SQLiteStatementExecutor
// {
// public:
//     static std::vector<std::string> GetText(const SQLite_Statement& Statement)
//     {
//         std::vector<std::string> ReturnStrings;
//         while (Statement.Step())
//         {
//             const unsigned char* RawText = sqlite3_column_text(Statement.Get(), 0);
//             std::string ContactName = RawText ? reinterpret_cast<const char*>(RawText) : "";
//             ReturnStrings.push_back(ContactName);
//         }
//         return ReturnStrings;
//     }
// };
// void ReadFirstContactName()
// {
//     const SQLite_DbConnection DbHandle(Invariants::WayToDb, true);
//     const SQLite_Statement Statement01(DbHandle.Get(), "SELECT ContactName FROM Customers LIMIT @Limit;"); 
//     SQLite_NamedParamBinder::BindParamsByName(Statement01.Get(), { {"@Limit", 3} });
//     const auto ResultTexts = SQLiteStatementExecutor::GetText(Statement01);
//     std::print(std::cout, "First contact name: {}\n", ResultTexts);
// }

void ReadSecondContactName()
{
    const TableValues2D Result = SQLite_Executor::ExecuteQuery(Invariants::WayToDb, Invariants::Query5);

    if (Result.empty())
    {
        std::puts("No data found.");
        return;
    }

    for (std::size_t RowIndex = 0; RowIndex < Result.size(); ++RowIndex)
    {
        const TableValues1D& Row = Result[RowIndex];
        std::printf("Row %zu:\n", RowIndex);

        for (std::size_t ColIndex = 0; ColIndex < Row.size(); ++ColIndex)
        {
            const FieldValue& Value = Row[ColIndex];

            std::printf("  Column %zu: ", ColIndex);

            std::visit([](const auto& V)
            {
                using T = std::decay_t<decltype(V)>;

                if constexpr (std::is_same_v<T, std::nullptr_t>)
                {
                    std::puts("NULL");
                }
                else if constexpr (std::is_same_v<T, std::string>)
                {
                    std::printf("%s\n", V.c_str());
                }
                else if constexpr (std::is_same_v<T, std::vector<uint8_t>>)
                {
                    std::printf("<BLOB, %zu bytes>\n", V.size());
                }
                else if constexpr (std::is_same_v<T, int64_t>)
                {
                    std::printf("%lld\n", static_cast<long long>(V));
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    std::printf("%.3f\n", V);
                }
                else
                {
                    static_assert(sizeof(T) == 0, "Unhandled FieldValue type.");
                }

            }, Value);
        }
    }
}


int main()
{
    // ReadFirstContactName();
    ReadSecondContactName();
}