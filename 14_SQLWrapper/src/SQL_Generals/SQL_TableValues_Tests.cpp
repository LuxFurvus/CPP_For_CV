
#include <variant>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <utility>
////////
#include <gtest/gtest.h>
////////
#include "SQL_TableValues.h"


// ================================
// ░ Test Case Descriptions ░
// ================================

// T01: IsEmpty() correctly returns true on empty table
// T02: IsEmpty() correctly returns false on non-empty table

// T03: GetSize() returns {0, 0} when table is empty
// T04: GetSize() returns correct size on uniform non-empty table
// T05: GetSize() uses last row’s column count even if rows differ in size

// T06: GetField() returns correct value for valid row/column
// T07: GetField() throws on negative row index
// T08: GetField() throws on negative column index
// T09: GetField() throws on row index >= row count
// T10: GetField() throws on column index >= column size of that row

// T11: IsFieldOfType() returns true when type matches (Integer)
// T12: IsFieldOfType() returns false when type mismatches (Float vs Integer)
// T13: IsFieldOfType() returns true when checking Null type
// T14: IsFieldOfType() throws on invalid row/column indices
// T15: IsFieldOfType() throws on unknown SQL_FieldType value


// ---------------------- TEST CASES ----------------------

// T01: IsEmpty() correctly returns true on empty table
TEST(SQL_TableValues2D, T01_EmptyTableReturnsTrue) {
    SCOPED_TRACE("\n//========================================\n// T01: IsEmpty() returns true\n//========================================");
    SQL_TableValues2D Table;
    EXPECT_TRUE(Table.IsEmpty());
}

// T02: IsEmpty() correctly returns false on non-empty table
TEST(SQL_TableValues2D, T02_NonEmptyTableReturnsFalse) {
    SCOPED_TRACE("\n//========================================\n// T02: IsEmpty() returns false\n//========================================");
    SQL_TableValues2D Table;
    Table.TableData = {{42}};
    EXPECT_FALSE(Table.IsEmpty());
}

// T03: GetSize() returns {0, 0} when table is empty
TEST(SQL_TableValues2D, T03_SizeEmptyTable) {
    SCOPED_TRACE("\n//========================================\n// T03: GetSize() on empty table\n//========================================");
    SQL_TableValues2D Table;
    EXPECT_EQ(Table.GetSize(), std::make_pair(0, 0));
}

// T04: GetSize() returns correct size on uniform non-empty table
TEST(SQL_TableValues2D, T04_SizeNonEmptyUniformTable) {
    SCOPED_TRACE("\n//========================================\n// T04: GetSize() on 3x4 table\n//========================================");
    SQL_TableValues2D Table;
    Table.TableData = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };
    EXPECT_EQ(Table.GetSize(), std::make_pair(3, 4));
}

// T05: GetSize() uses last row’s column count even if rows differ in size
TEST(SQL_TableValues2D, T05_SizeWithNonUniformRows) {
    SCOPED_TRACE("\n//========================================\n// T05: GetSize() uses last row's size\n//========================================");
    SQL_TableValues2D Table;
    Table.TableData = {
        {1, 2},
        {3, 4, 5},
        {6}
    };
    EXPECT_EQ(Table.GetSize(), std::make_pair(3, 1));
}

// T06: GetField() returns correct value for valid row/column
TEST(SQL_TableValues2D, T06_GetValidField) {
    SCOPED_TRACE("\n//========================================\n// T06: GetField() valid indices\n//========================================");
    SQL_TableValues2D Table;
    Table.TableData = {{42, 3.14}};
    EXPECT_EQ(std::get<int64_t>(Table.GetField(0, 0)), 42);
    EXPECT_EQ(std::get<double>(Table.GetField(0, 1)), 3.14);
}

// T07: GetField() throws on negative row index
TEST(SQL_TableValues2D, T07_GetFieldNegativeRowIndex) {
    SCOPED_TRACE("\n//========================================\n// T07: GetField() with negative row index\n//========================================");
    SQL_TableValues2D Table;
    Table.TableData = {{1}};
    EXPECT_THROW(Table.GetField(-1, 0), std::runtime_error);
}

// T08: GetField() throws on negative column index
TEST(SQL_TableValues2D, T08_GetFieldNegativeColIndex) {
    SCOPED_TRACE("\n//========================================\n// T08: GetField() with negative column index\n//========================================");
    SQL_TableValues2D Table;
    Table.TableData = {{1}};
    EXPECT_THROW(Table.GetField(0, -1), std::runtime_error);
}

// T09: GetField() throws on row index >= row count
TEST(SQL_TableValues2D, T09_GetFieldRowIndexTooLarge) {
    SCOPED_TRACE("\n//========================================\n// T09: GetField() with out-of-range row index\n//========================================");
    SQL_TableValues2D Table;
    Table.TableData = {{1}};
    EXPECT_THROW(Table.GetField(1, 0), std::runtime_error);
}

// T10: GetField() throws on column index >= column count
TEST(SQL_TableValues2D, T10_GetFieldColIndexTooLarge) {
    SCOPED_TRACE("\n//========================================\n// T10: GetField() with out-of-range column index\n//========================================");
    SQL_TableValues2D Table;
    Table.TableData = {{1}};
    EXPECT_THROW(Table.GetField(0, 1), std::runtime_error);
}

// T11: IsFieldOfType() returns true when type matches (Integer)
TEST(SQL_TableValues2D, T11_IsFieldInteger_True) {
    SCOPED_TRACE("\n//========================================\n// T11: IsFieldOfType() detects Integer correctly\n//========================================");
    SQL_TableValues2D Table;
    Table.TableData = {{int64_t(100)}};
    EXPECT_TRUE(Table.IsFieldOfType(0, 0, SQL_FieldType::Integer));
}

// T12: IsFieldOfType() returns false when type mismatches (Float vs Integer)
TEST(SQL_TableValues2D, T12_IsFieldInteger_False) {
    SCOPED_TRACE("\n//========================================\n// T12: IsFieldOfType() detects mismatched type\n//========================================");
    SQL_TableValues2D Table;
    Table.TableData = {{3.14}};
    EXPECT_FALSE(Table.IsFieldOfType(0, 0, SQL_FieldType::Integer));
}

// T13: IsFieldOfType() returns true when checking Null type
TEST(SQL_TableValues2D, T13_IsFieldNull_True) {
    SCOPED_TRACE("\n//========================================\n// T13: IsFieldOfType() detects nullptr correctly\n//========================================");
    SQL_TableValues2D Table;
    Table.TableData = {{nullptr}};
    EXPECT_TRUE(Table.IsFieldOfType(0, 0, SQL_FieldType::Null));
}

// T14: IsFieldOfType() throws on invalid row/column indices
TEST(SQL_TableValues2D, T14_IsFieldOfType_OutOfRange) {
    SCOPED_TRACE("\n//========================================\n// T14: IsFieldOfType() throws on bad indices\n//========================================");
    SQL_TableValues2D Table;
    Table.TableData = {{int64_t(1)}};
    EXPECT_THROW(Table.IsFieldOfType(1, 0, SQL_FieldType::Integer), std::runtime_error);
}

// T15: IsFieldOfType() throws on unknown SQL_FieldType value
TEST(SQL_TableValues2D, T15_IsFieldOfType_InvalidEnum) {
    SCOPED_TRACE("\n//========================================\n// T15: IsFieldOfType() throws on invalid enum\n//========================================");
    SQL_TableValues2D Table;
    Table.TableData = {{int64_t(1)}};
    SQL_FieldType InvalidType = static_cast<SQL_FieldType>(999);
    EXPECT_THROW(Table.IsFieldOfType(0, 0, InvalidType), std::runtime_error);
}
