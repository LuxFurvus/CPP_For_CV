//  SQL_TableValues2D.h

#pragma once
#ifndef SQL_TABLEVALUES2D_H
#define SQL_TABLEVALUES2D_H


///////////////////


#include <variant>
#include <vector>
#include <cstdint>
#include <string>


///////////////////


using FieldValue = std::variant<int64_t, double, std::string, std::vector<uint8_t>, std::nullptr_t>;
using TableValues1D = std::vector<FieldValue>;
using TableValues2D = std::vector<TableValues1D>;


///////////////////


enum class SQL_FieldType
{
    Integer = 1,
    Float = 2,
    Text = 3,
    Blob = 4,
    Null = 5
};


///////////////////


struct SQL_TableValues2D
{
public:
    TableValues2D TableData;
    
public:

    explicit SQL_TableValues2D();
    explicit SQL_TableValues2D(const TableValues2D& InTableData);
    explicit SQL_TableValues2D(TableValues2D&& InTableData) noexcept;

    std::pair<int, int> GetSize() const;
    FieldValue GetField(int RowIndex, int ColumnIndex) const;

    bool IsEmpty() const;
    bool IsFieldOfType(int RowIndex, int ColumnIndex, SQL_FieldType FieldType) const;
};


#endif // SQL_TABLEVALUES2D_H