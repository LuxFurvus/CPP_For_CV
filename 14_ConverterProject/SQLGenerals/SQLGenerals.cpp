
#include <variant>
#include <vector>
#include <string>
#include <utility>
#include <format>
////////
#include "SQLGenerals.h"



///////////////////


SQL_TableValues2D::SQL_TableValues2D()
    : TableData() {}

//=================

SQL_TableValues2D::SQL_TableValues2D(const TableValues2D& InTableData)
    : TableData(InTableData) {}

//=================

SQL_TableValues2D::SQL_TableValues2D(TableValues2D&& InTableData) noexcept
    : TableData(std::move(InTableData)) {}


///////////////////


std::pair<int, int> SQL_TableValues2D::GetSize() const
{
    if (IsEmpty()) return {0, 0};

    const int RowCount = static_cast<int>(TableData.size());
    const int ColumnCount = static_cast<int>(TableData.back().size());

    return {RowCount, ColumnCount};
}

//\/\/\/\/\/\/\/\/\/

bool SQL_TableValues2D::IsEmpty() const
{
    return TableData.empty();
}


///////////////////


bool SQL_TableValues2D::IsFieldOfType(int RowIndex, int ColumnIndex, SQL_FieldType FieldType) const
{
    const FieldValue Value = GetField(RowIndex, ColumnIndex);

    switch (FieldType)
    {
        case SQL_FieldType::Integer:
            return std::holds_alternative<int64_t>(Value);

        case SQL_FieldType::Float:
            return std::holds_alternative<double>(Value);

        case SQL_FieldType::Text:
            return std::holds_alternative<std::string>(Value);

        case SQL_FieldType::Blob:
            return std::holds_alternative<std::vector<uint8_t>>(Value);

        case SQL_FieldType::Null:
            return std::holds_alternative<std::nullptr_t>(Value);

        default:
            CONFIRMS(false, "Unknown field type: {}", static_cast<int>(FieldType));
            return false;
    }
}

//\/\/\/\/\/\/\/\/\/

FieldValue SQL_TableValues2D::GetField(int RowIndex, int ColumnIndex) const
{
    const bool AreWrongParams = false
        || RowIndex < 0
        || ColumnIndex < 0
        || RowIndex >= static_cast<int>(TableData.size())
        || ColumnIndex >= static_cast<int>(TableData.at(RowIndex).size());
    CONFIRMS(!AreWrongParams,
        "Invalid RowIndex: {}, ColumnIndex: {}, statement has {} rows and {} columns",
        RowIndex, ColumnIndex, TableData.size(), TableData.back().size());

    return TableData.at(RowIndex).at(ColumnIndex);
}


///////////////////

