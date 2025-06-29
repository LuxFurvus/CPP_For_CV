
#include <format>
#include <string>
#include <iostream>
#include <cassert>
#include <variant>
#include <vector>
#include <filesystem>
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

struct Products
{
    int ProductID;
    std::string ProductName;
    std::optional<int> SupplierID;
    std::optional<int> CategoryID;
    std::optional<std::string> QuantityPerUnit;
    std::optional<double> UnitPrice;
    std::optional<int> UnitsInStock;
    std::optional<int> UnitsOnOrder;
    std::optional<int> ReorderLevel;
    bool Discontinued;
};

///////////////////////////////




int main()
{

}
