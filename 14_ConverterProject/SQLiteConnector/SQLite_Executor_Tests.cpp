
#include <filesystem>
////////
#include <sqlite3.h>
#include <gtest/gtest.h>
////////
#include "SQLiteConnector.h"

// ================================
// ░ Test Case Descriptions ░
// ================================

// T01: SELECT with multiple rows and mixed types; checks correct TableValues2D output
// T02: SELECT returns no rows; result must be an empty vector
// T03: Non-SELECT (e.g., UPDATE); expects empty result
// T04: Empty query string; must throw std::runtime_error
// T05: Empty DB path; must throw std::runtime_error
// T06: Invalid SQL syntax; must throw std::runtime_error
// T07: Non-existent DB path; must throw std::runtime_error
// T08: SELECT with all SQLite types (INTEGER, FLOAT, TEXT, BLOB, NULL); verifies correct variants
// T09: SELECT from tables with 1, 3, 10 columns; checks row field count
// T10: SELECT from empty table; result must be empty
// T11: Column with mixed actual types; verifies correct type inference
// T12: Column with partial NULLs; checks correct null handling
// T13: SELECT from view; must return same result as table
// T14: SELECT from subquery or CTE; result must be correct
// T15: INSERT statement; expects empty result
// T16: UPDATE statement; same as T15
// T17: DELETE statement; same as T15
// T18: Multiple SELECTs in one query; expects error or correct first result
// T19: CREATE TABLE; expects empty result
// T20: Mixed compound (e.g., INSERT; SELECT); must throw std::runtime_error

// ==========================

namespace SQLite_Executor_TestNS
{
    const std::string DbPath = "temp_t01.db";

    void CreateTempDb()
    {
        sqlite3* DbHandle = nullptr;
        const int AllFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_EXRESCODE | SQLITE_OPEN_FULLMUTEX;
        const int OpenResult = sqlite3_open_v2(DbPath.c_str(), &DbHandle, AllFlags, nullptr);
        ASSERT_EQ(OpenResult, SQLITE_OK);
        sqlite3_close(DbHandle);
    }
};

// ==========================

//===================================================================
// T01: SELECT with multiple rows and mixed types
TEST(SQLite_Executor, T01_MixedTypesMultipleRows)
{
    SCOPED_TRACE("T01: SELECT with multiple rows and mixed types");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    // Prepare test data
    SQLite_Executor::ExecuteQuery(DbPath, "CREATE TABLE Test(Id INTEGER, Name TEXT, Score REAL);");
    SQLite_Executor::ExecuteQuery(DbPath, "INSERT INTO Test VALUES (1, 'Alice', 95.5);");
    SQLite_Executor::ExecuteQuery(DbPath, "INSERT INTO Test VALUES (2, 'Bob', 88);");
    SQLite_Executor::ExecuteQuery(DbPath, "INSERT INTO Test VALUES (3, 'Carol', 91.2);");

    const SQL_TableValues2D Result = SQLite_Executor::ExecuteQuery(DbPath, "SELECT * FROM Test;");

    // Do actual tests
    EXPECT_EQ(Result.GetSize(), std::make_pair(3, 3));

    EXPECT_EQ(std::get<int64_t>(Result.GetField(0, 0)), 1);
    EXPECT_EQ(Result.IsFieldOfType(0, 0, SQL_FieldType::Integer), true);

    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 1)), "Alice");
    EXPECT_EQ(Result.IsFieldOfType(0, 1, SQL_FieldType::Text), true);

    EXPECT_EQ(std::get<double>(Result.GetField(0, 2)), 95.5);
    EXPECT_EQ(Result.IsFieldOfType(0, 2, SQL_FieldType::Float), true);

    EXPECT_EQ(std::get<int64_t>(Result.GetField(1, 0)), 2);
    EXPECT_EQ(Result.IsFieldOfType(1, 0, SQL_FieldType::Integer), true);

    EXPECT_EQ(std::get<std::string>(Result.GetField(1, 1)), "Bob");
    EXPECT_EQ(Result.IsFieldOfType(1, 1, SQL_FieldType::Text), true);

    EXPECT_EQ(std::get<double>(Result.GetField(1, 2)), 88);
    EXPECT_EQ(Result.IsFieldOfType(1, 2, SQL_FieldType::Float), true);

    EXPECT_EQ(std::get<int64_t>(Result.GetField(2, 0)), 3);
    EXPECT_EQ(Result.IsFieldOfType(2, 0, SQL_FieldType::Integer), true);

    EXPECT_EQ(std::get<std::string>(Result.GetField(2, 1)), "Carol");
    EXPECT_EQ(Result.IsFieldOfType(2, 1, SQL_FieldType::Text), true);

    EXPECT_EQ(std::get<double>(Result.GetField(2, 2)), 91.2);
    EXPECT_EQ(Result.IsFieldOfType(2, 2, SQL_FieldType::Float), true);

    // Clean up
    std::filesystem::remove(DbPath);
}

//===================================================================
// T02: SELECT returns no rows; result must be an empty vector
TEST(SQLite_Executor, T02_SelectReturnsNoRows)
{
    SCOPED_TRACE("T02: SELECT returns no rows; result must be an empty vector");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor::ExecuteQuery(DbPath, "CREATE TABLE EmptyTable(Id INTEGER);");

    const SQL_TableValues2D Result = SQLite_Executor::ExecuteQuery(DbPath, "SELECT * FROM EmptyTable WHERE Id = 12345;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(0, 0));
    EXPECT_EQ(Result.IsEmpty(), true);

    const SQL_TableValues2D Result2 = SQLite_Executor::ExecuteQuery(DbPath, "SELECT * FROM EmptyTable;");

    EXPECT_EQ(Result2.GetSize(), std::make_pair(0, 0));
    EXPECT_EQ(Result2.IsEmpty(), true);

    std::filesystem::remove(DbPath);
}

//===================================================================
// T03: Non-SELECT (e.g., UPDATE); expects empty result
TEST(SQLite_Executor, T03_NonSelectReturnsEmpty)
{
    SCOPED_TRACE("T03: Non-SELECT (e.g., UPDATE); expects empty result");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor::ExecuteQuery(DbPath, "CREATE TABLE X(Id INTEGER, Name TEXT);");
    SQLite_Executor::ExecuteQuery(DbPath, "INSERT INTO X VALUES (1, 'Name');");

    const SQL_TableValues2D Result = SQLite_Executor::ExecuteQuery(DbPath, "UPDATE X SET Name = 'Updated' WHERE Id = 1;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(0, 0));
    EXPECT_EQ(Result.IsEmpty(), true);

    std::filesystem::remove(DbPath);
}

//===================================================================
// T04: Empty query string; must throw std::runtime_error
TEST(SQLite_Executor, T04_EmptyQueryString)
{
    SCOPED_TRACE("T04: Empty query string; must throw std::runtime_error");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    EXPECT_THROW(SQLite_Executor::ExecuteQuery(DbPath, ""), std::runtime_error);

    std::filesystem::remove(DbPath);
}

//===================================================================
// T05: Empty DB path; must throw std::runtime_error
TEST(SQLite_Executor, T05_EmptyDbPath)
{
    SCOPED_TRACE("T05: Empty DB path; must throw std::runtime_error");

    EXPECT_THROW(SQLite_Executor::ExecuteQuery("", "SELECT 1;"), std::runtime_error);
}

//===================================================================
// T06: Invalid SQL syntax; must throw std::runtime_error
TEST(SQLite_Executor, T06_InvalidSqlSyntax)
{
    SCOPED_TRACE("T06: Invalid SQL syntax; must throw std::runtime_error");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    EXPECT_THROW(SQLite_Executor::ExecuteQuery(DbPath, "SELEKT FROM"), std::runtime_error);

    std::filesystem::remove(DbPath);
}

//===================================================================
// T07: Non-existent DB path; must throw std::runtime_error
TEST(SQLite_Executor, T07_NonexistentDbPath)
{
    SCOPED_TRACE("T07: Non-existent DB path; must throw std::runtime_error");

    const std::string InvalidPath = "non_existent_database.db";
    EXPECT_THROW(SQLite_Executor::ExecuteQuery(InvalidPath, "SELECT 1;"), std::runtime_error);
}
