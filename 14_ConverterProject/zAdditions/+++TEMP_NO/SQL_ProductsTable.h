#pragma once

#ifndef SQL_PRODUCTSTABLE_H
#define SQL_PRODUCTSTABLE_H

#include <optional>
#include <string>
#include <vector>
#include <memory>
////////
#include "../ConditionChecker/ConditionChecker.h"
////////
#include "../SQL_Generals/SQL_TableValues.h"
#include "../SQL_Generals/SQL_Executor.h"



class SQL_ProductsTable
{
private:
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

    const std::string DbPath;
    std::unique_ptr<SQL_Executor> Executor;

public:
    explicit SQL_ProductsTable(
        std::unique_ptr<SQL_Executor> InExecutor, const std::string& InDbPath)
        : Executor(std::move(InExecutor)), DbPath(InDbPath) {}

    void LoadById(int TargetProductID)
    {
        const std::string Query = "SELECT * FROM Products WHERE ProductID = " + std::to_string(TargetProductID) + ";";
        const SQL_TableValues2D RawTable = Executor->ExecuteQuery(DbPath, Query);

        CONFIRMS(!RawTable.IsEmpty(), "No product found with ProductID = {}", TargetProductID);

        const std::pair<int, int> Size = RawTable.GetSize();
        const int ColumnCount = Size.second;
        CONFIRMS(ColumnCount == 10, "Expected 10 columns in Products table, got {}", ColumnCount);

        FillFromRow(RawTable, 0);
    }

    void GetFirstNRow(const int NumberOfRows)
    {
        const std::string Query = std::format("SELECT * FROM Products LIMIT {};", NumberOfRows);

        const SQL_TableValues2D RawTable = Executor->ExecuteQuery(DbPath, Query);

        CONFIRMS(!RawTable.IsEmpty(), "Products table is empty");

        const std::pair<int, int> Size = RawTable.GetSize();
        const int ColumnCount = Size.second;
        CONFIRMS(ColumnCount == 10, "Expected 10 columns in Products table, got {}", ColumnCount);

        FillFromRow(RawTable, 0);
    }

private:

    void FillFromRow(const SQL_TableValues2D& Table, int RowIndex)
    {
        ProductID = GetIntField(Table, RowIndex, 0);
        ProductName = GetStringField(Table, RowIndex, 1);
        SupplierID = GetOptionalIntField(Table, RowIndex, 2);
        CategoryID = GetOptionalIntField(Table, RowIndex, 3);
        QuantityPerUnit = GetOptionalStringField(Table, RowIndex, 4);
        UnitPrice = GetOptionalDoubleField(Table, RowIndex, 5);
        UnitsInStock = GetOptionalIntField(Table, RowIndex, 6);
        UnitsOnOrder = GetOptionalIntField(Table, RowIndex, 7);
        ReorderLevel = GetOptionalIntField(Table, RowIndex, 8);
        Discontinued = GetBoolFromTextField(Table, RowIndex, 9);
    }

    static int GetIntField(const SQL_TableValues2D& Table, int Row, int Col)
    {
        return static_cast<int>(std::get<int64_t>(Table.GetField(Row, Col)));
    }

    static std::optional<int> GetOptionalIntField(const SQL_TableValues2D& Table, int Row, int Col)
    {
        const FieldValue& Value = Table.GetField(Row, Col);
        if (std::holds_alternative<std::nullptr_t>(Value)) return std::nullopt;
        return static_cast<int>(std::get<int64_t>(Value));
    }

    static std::string GetStringField(const SQL_TableValues2D& Table, int Row, int Col)
    {
        return std::get<std::string>(Table.GetField(Row, Col));
    }

    static std::optional<std::string> GetOptionalStringField(const SQL_TableValues2D& Table, int Row, int Col)
    {
        const FieldValue& Value = Table.GetField(Row, Col);
        if (std::holds_alternative<std::nullptr_t>(Value)) return std::nullopt;
        return std::get<std::string>(Value);
    }

    static std::optional<double> GetOptionalDoubleField(const SQL_TableValues2D& Table, int Row, int Col)
    {
        const FieldValue& Value = Table.GetField(Row, Col);
        if (std::holds_alternative<std::nullptr_t>(Value)) return std::nullopt;
        return std::get<double>(Value);
    }

    static bool GetBoolFromTextField(const SQL_TableValues2D& Table, int Row, int Col)
    {
        const std::string Text = std::get<std::string>(Table.GetField(Row, Col));
        return Text != "0";
    }
};


#endif // SQL_PRODUCTS_TABLE_H
