
#include "SQLite_Executor.h"
////////
#include <filesystem>
#include <fstream>
////////
#include <sqlite3.h>
#include <gtest/gtest.h>
////////
#include "../SQL_Generals/SQL_ScriptExecutor.h"
////////
#include "SQLite_ConnectionPool.h"


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
// T18: Multiple SELECTs in one query; checks warning about unused SQL tail
// T19: CREATE TABLE; expects empty result
// T20: Mixed compound (e.g., INSERT; SELECT); must warn about unused tail

// T21: Attempt to open a non-existent database and handle the failure
// T22: Attempt to open a corrupted or locked database file
// T23: Repeatedly open the same path and reuse cached connection (test singleton/multiton)
// T24: Close and reopen the same connection to test resource cleanup
// T25: Execute valid INSERT into Customers and verify with SELECT

// T26: Execute valid UPDATE on Products and confirm with SELECT
// T27: Execute SELECT with WHERE, ORDER BY, LIMIT, and OFFSET
// T28: Execute INNER JOIN (e.g., Orders + Customers) and validate result structure
// T29: Execute LEFT JOIN and ensure correct null handling
// T30: Execute SELECT COUNT(*) and confirm scalar integer retrieval

// T31: Execute aggregation (AVG, SUM, MIN, MAX) over OrderDetails
// T32: Use transactions: BEGIN → INSERT/UPDATE → COMMIT
// T33: Test ROLLBACK scenario
// T34: Simulate nested transactions
// T35: Enable foreign key constraint enforcement

// T36: Check for constraint violation on UNIQUE or PRIMARY KEY
// T37: Insert with missing NOT NULL field
// T38: UPDATE/DELETE with invalid WHERE clause (zero matches)
// T39: Access metadata via PRAGMA table_info(...)
// T40: Use PRAGMA foreign_key_list(...)

// T41: Use PRAGMA index_list(...)
// T42: Use PRAGMA user_version or schema_version
// T43: Use PRAGMA journal_mode and PRAGMA synchronous
// T44: List all tables using sqlite_master
// T45: Use and inspect sqlite_sequence for AUTOINCREMENT

// T46: ATTACH DATABASE and query across databases
// T47: Create and use temporary tables
// T48: Create and query a view
// T49: Use operators LIKE, GLOB, IN, and BETWEEN
// T50: Execute multi-statement scripts with parameters

/////////////////////////////////////
// TESTS TO ADD

// T51: Perform multithreaded concurrent SELECT queries
// T52: Concurrent SELECT and UPDATE to test locking behavior
// T53: Simulate timeout behavior for long-running queries
// T54: Simulate I/O failure during query execution
// T55: Enable query logging or tracing (e.g., sqlite3_trace_v2)
// T56: Measure and report execution time per query
// T57: Store and retrieve very large BLOB and TEXT values
// T58: Retrieve data using streamed result API (e.g., sqlite3_step loop)
// T59: Register and use custom SQL scalar and aggregate functions
// T60: Register and use custom SQL collations
// T61: Use EXPLAIN QUERY PLAN to analyze SQL performance
// T62: Perform schema migration (ALTER TABLE, DROP TABLE, etc.)
// T63: Detect and handle runtime schema changes (e.g., missing columns)
// T64: Verify proper cleanup of prepared statements and DB connections
// T65: Stress test: reopen connection 1000 times and validate stability
// T66: Stress test: execute 10,000 queries sequentially
// T67: Stress test: perform high-load multithreaded query execution
// T68: Validate wrapper exception types and error propagation paths



// ==========================

namespace SQLite_Executor_TestNS
{
    const std::string DbPath = "temp_t01.db";
    
    void CreateTempDb();
    void CreateTempDb()
    {
        sqlite3* DbHandle = nullptr;
        const int AllFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_EXRESCODE | SQLITE_OPEN_FULLMUTEX;
        const int OpenResult = sqlite3_open_v2(DbPath.c_str(), &DbHandle, AllFlags, nullptr);
        ASSERT_EQ(OpenResult, SQLITE_OK);
        sqlite3_close(DbHandle);
    }
    
    void CloseTempDb();
    void CloseTempDb()
    {
        SQLite_ConnectionPool::CloseAll();
        std::filesystem::remove(DbPath);
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
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Test(Id INTEGER, Name TEXT, Score REAL);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Test VALUES (1, 'Alice', 95.5);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Test VALUES (2, 'Bob', 88);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Test VALUES (3, 'Carol', 91.2);");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM Test;");

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

    CloseTempDb();
}


//===================================================================
// T02: SELECT returns no rows; result must be an empty vector
TEST(SQLite_Executor, T02_SelectReturnsNoRows)
{
    // ConditionChecker::SetStacktraceEnabled(true);
    SCOPED_TRACE("T02: SELECT returns no rows; result must be an empty vector");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE EmptyTable(Id INTEGER);");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM EmptyTable WHERE Id = 12345;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(0, 0));
    EXPECT_EQ(Result.IsEmpty(), true);

    const SQL_TableValues2D Result2 = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM EmptyTable;");

    EXPECT_EQ(Result2.GetSize(), std::make_pair(0, 0));
    EXPECT_EQ(Result2.IsEmpty(), true);

    CloseTempDb();
}


//===================================================================
// T03: Non-SELECT (e.g., UPDATE); expects empty result
TEST(SQLite_Executor, T03_NonSelectReturnsEmpty)
{
    SCOPED_TRACE("T03: Non-SELECT (e.g., UPDATE); expects empty result");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE X(Id INTEGER, Name TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO X VALUES (1, 'Name');");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "UPDATE X SET Name = 'Updated' WHERE Id = 1;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(0, 0));
    EXPECT_EQ(Result.IsEmpty(), true);

    CloseTempDb();
}

//===================================================================
// T04: Empty query string; must throw std::runtime_error
TEST(SQLite_Executor, T04_EmptyQueryString)
{
    SCOPED_TRACE("T04: Empty query string; must throw std::runtime_error");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    EXPECT_THROW(SQLite_Executor().ExecuteQuery(DbPath, ""), std::runtime_error);

    CloseTempDb();
}

//===================================================================
// T05: Empty DB path; must throw std::runtime_error
TEST(SQLite_Executor, T05_EmptyDbPath)
{
    SCOPED_TRACE("T05: Empty DB path; must throw std::runtime_error");

    EXPECT_THROW(SQLite_Executor().ExecuteQuery("", "SELECT 1;"), std::runtime_error);
}

//===================================================================
// T06: Invalid SQL syntax; must throw std::runtime_error
TEST(SQLite_Executor, T06_InvalidSqlSyntax)
{
    SCOPED_TRACE("T06: Invalid SQL syntax; must throw std::runtime_error");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    EXPECT_THROW(SQLite_Executor().ExecuteQuery(DbPath, "SELEKT FROM"), std::runtime_error);

    CloseTempDb();
}

//===================================================================
// T07: Non-existent DB path; must throw std::runtime_error
TEST(SQLite_Executor, T07_NonexistentDbPath)
{
    SCOPED_TRACE("T07: Non-existent DB path; must throw std::runtime_error");

    const std::string InvalidPath = "non_existent_database.db";
    EXPECT_THROW(SQLite_Executor().ExecuteQuery(InvalidPath, "SELECT 1;"), std::runtime_error);
}

//===================================================================
// T08: SELECT with all SQLite types (INTEGER, FLOAT, TEXT, BLOB, NULL)
TEST(SQLite_Executor, T08_AllSQLiteTypes)
{
    SCOPED_TRACE("T08: SELECT with all SQLite types (INTEGER, FLOAT, TEXT, BLOB, NULL)");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE AllTypes(I INTEGER, F FLOAT, T TEXT, B BLOB, N NULL);");

    std::vector<uint8_t> BlobData = {0x41, 0x42, 0x43}; // ABC
    std::string BlobHex = "X'414243'";

    SQLite_Executor().ExecuteQuery(DbPath,
        "INSERT INTO AllTypes VALUES (42, 3.14, 'text', " + BlobHex + ", NULL);");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM AllTypes;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 5));

    EXPECT_EQ(std::get<int64_t>(Result.GetField(0, 0)), 42);
    EXPECT_EQ(Result.IsFieldOfType(0, 0, SQL_FieldType::Integer), true);

    EXPECT_EQ(std::get<double>(Result.GetField(0, 1)), 3.14);
    EXPECT_EQ(Result.IsFieldOfType(0, 1, SQL_FieldType::Float), true);

    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 2)), "text");
    EXPECT_EQ(Result.IsFieldOfType(0, 2, SQL_FieldType::Text), true);

    EXPECT_EQ(std::get<std::vector<uint8_t>>(Result.GetField(0, 3)), BlobData);
    EXPECT_EQ(Result.IsFieldOfType(0, 3, SQL_FieldType::Blob), true);

    EXPECT_EQ(std::get<std::nullptr_t>(Result.GetField(0, 4)), nullptr);
    EXPECT_EQ(Result.IsFieldOfType(0, 4, SQL_FieldType::Null), true);

    CloseTempDb();
}

//===================================================================
// T09: SELECT from tables with 1, 3, 10 columns; checks row field count
TEST(SQLite_Executor, T09_VaryingColumnCounts)
{
    SCOPED_TRACE("T09: SELECT from tables with 1, 3, 10 columns");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE OneCol(A INTEGER);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO OneCol VALUES (1);");

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE ThreeCol(A, B, C);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO ThreeCol VALUES (1, 2, 3);");

    SQLite_Executor().ExecuteQuery(DbPath,
        "CREATE TABLE TenCol(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10);");
    SQLite_Executor().ExecuteQuery(DbPath,
        "INSERT INTO TenCol VALUES (1,2,3,4,5,6,7,8,9,10);");

    auto R1 = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM OneCol;");
    EXPECT_EQ(R1.GetSize(), std::make_pair(1, 1));

    auto R3 = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM ThreeCol;");
    EXPECT_EQ(R3.GetSize(), std::make_pair(1, 3));

    auto R10 = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM TenCol;");
    EXPECT_EQ(R10.GetSize(), std::make_pair(1, 10));

    CloseTempDb();
}


//===================================================================
// T10: SELECT from empty table; result must be empty
TEST(SQLite_Executor, T10_SelectFromEmptyTable)
{
    SCOPED_TRACE("T10: SELECT from empty table; result must be empty");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE EmptyTable(X INTEGER);");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM EmptyTable;");

    EXPECT_TRUE(Result.IsEmpty());
    EXPECT_EQ(Result.GetSize(), std::make_pair(0, 0));

    CloseTempDb();
}

//===================================================================
// T11: Column with mixed actual types; verifies correct type inference
TEST(SQLite_Executor, T11_MixedColumnTypes)
{
    SCOPED_TRACE("T11: Column with mixed actual types; verifies correct type inference");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Mixed(TypeField);");

    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Mixed VALUES (42);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Mixed VALUES (3.14);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Mixed VALUES ('text');");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM Mixed;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(3, 1));

    EXPECT_TRUE(Result.IsFieldOfType(0, 0, SQL_FieldType::Integer));
    EXPECT_EQ(std::get<int64_t>(Result.GetField(0, 0)), 42);

    EXPECT_TRUE(Result.IsFieldOfType(1, 0, SQL_FieldType::Float));
    EXPECT_EQ(std::get<double>(Result.GetField(1, 0)), 3.14);

    EXPECT_TRUE(Result.IsFieldOfType(2, 0, SQL_FieldType::Text));
    EXPECT_EQ(std::get<std::string>(Result.GetField(2, 0)), "text");

    CloseTempDb();
}

//===================================================================
// T12: Column with partial NULLs; checks correct null handling
TEST(SQLite_Executor, T12_PartialNulls)
{
    SCOPED_TRACE("T12: Column with partial NULLs; checks correct null handling");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE NullMix(A);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO NullMix VALUES (NULL);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO NullMix VALUES (100);");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM NullMix;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(2, 1));

    EXPECT_TRUE(Result.IsFieldOfType(0, 0, SQL_FieldType::Null));
    EXPECT_EQ(std::get<std::nullptr_t>(Result.GetField(0, 0)), nullptr);

    EXPECT_TRUE(Result.IsFieldOfType(1, 0, SQL_FieldType::Integer));
    EXPECT_EQ(std::get<int64_t>(Result.GetField(1, 0)), 100);

    CloseTempDb();
}

//===================================================================
// T13: SELECT from view; must return same result as table
TEST(SQLite_Executor, T13_SelectFromView)
{
    SCOPED_TRACE("T13: SELECT from view; must return same result as table");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE RealTable(A INTEGER, B TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO RealTable VALUES (7, 'seven');");
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE VIEW ViewTable AS SELECT * FROM RealTable;");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM ViewTable;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 2));

    EXPECT_EQ(std::get<int64_t>(Result.GetField(0, 0)), 7);
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 1)), "seven");

    CloseTempDb();
}

//===================================================================
// T14: SELECT from subquery or CTE; result must be correct
TEST(SQLite_Executor, T14_SubqueryOrCTE)
{
    SCOPED_TRACE("T14: SELECT from subquery or CTE");

    using namespace SQLite_Executor_TestNS;
    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Numbers(Id INTEGER, Value REAL);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Numbers VALUES (1, 10.0), (2, 20.5), (3, 30.5);");

    const std::string Query =
        "WITH AvgValue AS (SELECT AVG(Value) AS Mean FROM Numbers) "
        "SELECT Id, Value FROM Numbers WHERE Value > (SELECT Mean FROM AvgValue);";

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, Query);

    EXPECT_EQ(Result.GetSize(), std::make_pair(2, 2));

    EXPECT_EQ(std::get<int64_t>(Result.GetField(0, 0)), 2);
    EXPECT_EQ(std::get<double>(Result.GetField(0, 1)), 20.5);

    EXPECT_EQ(std::get<int64_t>(Result.GetField(1, 0)), 3);
    EXPECT_EQ(std::get<double>(Result.GetField(1, 1)), 30.5);

    CloseTempDb();
}

//===================================================================
// T15: INSERT statement; expects empty result
TEST(SQLite_Executor, T15_InsertStatement)
{
    SCOPED_TRACE("T15: INSERT statement; expects empty result");

    using namespace SQLite_Executor_TestNS;
    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE TestInsert(Id INTEGER, Data TEXT);");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO TestInsert VALUES (1, 'Inserted');");

    EXPECT_TRUE(Result.IsEmpty());

    CloseTempDb();
}

//===================================================================
// T16: UPDATE statement; same as T15
TEST(SQLite_Executor, T16_UpdateStatement)
{
    SCOPED_TRACE("T16: UPDATE statement; expects empty result");

    using namespace SQLite_Executor_TestNS;
    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE TestUpdate(Id INTEGER, Status TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO TestUpdate VALUES (1, 'Pending');");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "UPDATE TestUpdate SET Status = 'Done' WHERE Id = 1;");

    EXPECT_TRUE(Result.IsEmpty());

    CloseTempDb();
}

//===================================================================
// T17: DELETE statement; same as T15
TEST(SQLite_Executor, T17_DeleteStatement)
{
    SCOPED_TRACE("T17: DELETE statement; expects empty result");

    using namespace SQLite_Executor_TestNS;
    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE TestDelete(Id INTEGER);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO TestDelete VALUES (1);");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "DELETE FROM TestDelete WHERE Id = 1;");

    EXPECT_TRUE(Result.IsEmpty());

    CloseTempDb();
}

//===================================================================
// T18: Multiple SELECTs in one query; checks warning about unused SQL tail
TEST(SQLite_Executor, T18_MultipleSelectsInQuery)
{
    SCOPED_TRACE("T18: Multiple SELECTs in one query; must warn about unused SQL tail");

    using namespace SQLite_Executor_TestNS;
    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE A(Id INTEGER);");
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE B(Name TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO A VALUES (42);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO B VALUES ('hello');");

    testing::internal::CaptureStderr();

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM A; SELECT * FROM B;");

    std::string Output = testing::internal::GetCapturedStderr();

    // Verify result is from the first SELECT only
    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 1));
    EXPECT_EQ(std::get<int64_t>(Result.GetField(0, 0)), 42);
    EXPECT_EQ(Result.IsFieldOfType(0, 0, SQL_FieldType::Integer), true);

    // Verify captured stderr includes warning about unused tail
    EXPECT_NE(Output.find("Unused tail"), std::string::npos);
    EXPECT_NE(Output.find("SELECT * FROM B;"), std::string::npos);

    CloseTempDb();
}

//===================================================================
// T19: CREATE TABLE; expects empty result
TEST(SQLite_Executor, T19_CreateTableStatement)
{
    SCOPED_TRACE("T19: CREATE TABLE statement; expects empty result");

    using namespace SQLite_Executor_TestNS;
    CreateTempDb();

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Dummy(Id INTEGER);");

    EXPECT_TRUE(Result.IsEmpty());

    CloseTempDb();
}

//===================================================================
// T20: Mixed compound (e.g., INSERT; SELECT); must warn about unused tail
TEST(SQLite_Executor, T20_MixedCompoundQuery)
{
    SCOPED_TRACE("T20: Mixed compound query (INSERT; SELECT); must warn about unused SQL tail");

    using namespace SQLite_Executor_TestNS;
    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE MixTest(Id INTEGER);");

    testing::internal::CaptureStderr();

    // Compound query: only the INSERT should be executed
    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(
        DbPath, "INSERT INTO MixTest VALUES (1); SELECT * FROM MixTest;"
    );

    std::string Output = testing::internal::GetCapturedStderr();

    // Result from INSERT must be empty
    EXPECT_TRUE(Result.IsEmpty());

    // Now manually verify that the INSERT actually occurred (via separate SELECT)
    const SQL_TableValues2D Verification = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM MixTest;");
    EXPECT_EQ(Verification.GetSize(), std::make_pair(1, 1));
    EXPECT_EQ(std::get<int64_t>(Verification.GetField(0, 0)), 1);

    // Warning about unused SQL tail must be present
    EXPECT_NE(Output.find("Unused tail"), std::string::npos);
    EXPECT_NE(Output.find("SELECT * FROM MixTest"), std::string::npos);

    CloseTempDb();
}

//===================================================================
// T21: Attempt to open a non-existent database and handle the failure
TEST(SQLite_Executor, T21_OpenNonExistentDatabase)
{
    SCOPED_TRACE("T21: Attempt to open a non-existent database and handle the failure");

    const std::string NonExistentPath = "this_file_does_not_exist.db";

    EXPECT_THROW({
        SQLite_Executor().ExecuteQuery(NonExistentPath, "SELECT 1;");
    }, std::runtime_error);
}


//===================================================================
// T23: Repeatedly open the same path and reuse cached connection
TEST(SQLite_Executor, T23_RepeatedOpenUsesSameConnection)
{
    SCOPED_TRACE("T23: Repeatedly open the same path and reuse cached connection");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE CacheTest(Id INTEGER);");

    for (int i = 0; i < 10; ++i)
    {
        const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM CacheTest;");
        EXPECT_TRUE(Result.IsEmpty());
    }

    CloseTempDb();
}


//===================================================================
// T24: Close and reopen the same connection to test resource cleanup
TEST(SQLite_Executor, T24_ConnectionCloseAndReopen)
{
    SCOPED_TRACE("T24: Close and reopen the same connection to test resource cleanup");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE ReopenTest(Id INTEGER);");

    SQLite_ConnectionPool::CloseAll();

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM ReopenTest;");
    EXPECT_TRUE(Result.IsEmpty());

    CloseTempDb();
}


//===================================================================
// T25: INSERT row and verify via SELECT
TEST(SQLite_Executor, T25_InsertAndSelect)
{
    SCOPED_TRACE("T25: Execute valid INSERT and verify via SELECT");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE InsertTest(Id INTEGER, Name TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO InsertTest VALUES (1, 'NewName');");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM InsertTest;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 2));
    EXPECT_EQ(std::get<int64_t>(Result.GetField(0, 0)), 1);
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 1)), "NewName");

    CloseTempDb();
}


//===================================================================
// T26: UPDATE and verify effect via SELECT
TEST(SQLite_Executor, T26_UpdateAndVerify)
{
    SCOPED_TRACE("T26: Execute valid UPDATE and verify via SELECT");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE UpdateTest(Id INTEGER, Name TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO UpdateTest VALUES (1, 'Initial');");
    SQLite_Executor().ExecuteQuery(DbPath, "UPDATE UpdateTest SET Name = 'Updated' WHERE Id = 1;");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT Name FROM UpdateTest WHERE Id = 1;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 1));
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 0)), "Updated");

    CloseTempDb();
}


//===================================================================
// T27: SELECT with WHERE, ORDER BY, LIMIT, OFFSET
TEST(SQLite_Executor, T27_SelectWithWhereOrderLimitOffset)
{
    SCOPED_TRACE("T27: SELECT with WHERE, ORDER BY, LIMIT, and OFFSET");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE FilterTest(Id INTEGER, Name TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO FilterTest VALUES (1, 'Anna');");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO FilterTest VALUES (2, 'Ben');");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO FilterTest VALUES (3, 'Cara');");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO FilterTest VALUES (4, 'Dan');");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(
        DbPath, "SELECT Name FROM FilterTest WHERE Id >= 2 ORDER BY Name DESC LIMIT 2 OFFSET 1;"
    );

    EXPECT_EQ(Result.GetSize(), std::make_pair(2, 1));
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 0)), "Cara");
    EXPECT_EQ(std::get<std::string>(Result.GetField(1, 0)), "Ben");

    CloseTempDb();
}

//===================================================================
// T28: Execute INNER JOIN (e.g., Orders + Customers) and validate result structure
TEST(SQLite_Executor, T28_InnerJoinQuery)
{
    SCOPED_TRACE("T28: Execute INNER JOIN and validate result structure");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Customers(Id INTEGER PRIMARY KEY, Name TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Orders(Id INTEGER, CustomerId INTEGER, Total REAL);");

    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Customers VALUES (1, 'Alice');");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Orders VALUES (10, 1, 100.5);");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(
        DbPath,
        "SELECT Orders.Id, Customers.Name, Orders.Total "
        "FROM Orders INNER JOIN Customers ON Orders.CustomerId = Customers.Id;"
    );

    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 3));
    EXPECT_EQ(std::get<int64_t>(Result.GetField(0, 0)), 10);
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 1)), "Alice");
    EXPECT_EQ(std::get<double>(Result.GetField(0, 2)), 100.5);

    CloseTempDb();
}


//===================================================================
// T29: Execute LEFT JOIN and ensure correct null handling
TEST(SQLite_Executor, T29_LeftJoinWithNulls)
{
    SCOPED_TRACE("T29: Execute LEFT JOIN and ensure correct null handling");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Customers(Id INTEGER PRIMARY KEY, Name TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Orders(Id INTEGER, CustomerId INTEGER, Total REAL);");

    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Customers VALUES (1, 'Alice');");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Customers VALUES (2, 'Bob');");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Orders VALUES (10, 1, 50);");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(
        DbPath,
        "SELECT Customers.Name, Orders.Total "
        "FROM Customers LEFT JOIN Orders ON Customers.Id = Orders.CustomerId "
        "ORDER BY Customers.Id;"
    );

    EXPECT_EQ(Result.GetSize(), std::make_pair(2, 2));
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 0)), "Alice");
    EXPECT_EQ(std::get<double>(Result.GetField(0, 1)), 50.0);
    EXPECT_EQ(std::get<std::string>(Result.GetField(1, 0)), "Bob");
    EXPECT_EQ(Result.IsFieldOfType(1, 1, SQL_FieldType::Null), true);

    CloseTempDb();
}

//===================================================================
// T30: Execute SELECT COUNT(*) and confirm scalar integer retrieval
TEST(SQLite_Executor, T30_SelectCountStar)
{
    SCOPED_TRACE("T30: Execute SELECT COUNT(*) and confirm scalar integer retrieval");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE CountTest(Id INTEGER);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO CountTest VALUES (1);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO CountTest VALUES (2);");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT COUNT(*) FROM CountTest;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 1));
    EXPECT_EQ(std::get<int64_t>(Result.GetField(0, 0)), 2);

    CloseTempDb();
}








//===================================================================
// T31: Execute aggregation (AVG, SUM, MIN, MAX) over OrderDetails
TEST(SQLite_Executor, T31_AggregationFunctions)
{
    SCOPED_TRACE("T31: Execute aggregation (AVG, SUM, MIN, MAX)");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Aggregates(Id INTEGER, Value REAL);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Aggregates VALUES (1, 10.5);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Aggregates VALUES (2, 20.0);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Aggregates VALUES (3, 15.5);");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(
        DbPath,
        "SELECT AVG(Value), SUM(Value), MIN(Value), MAX(Value) FROM Aggregates;"
    );

    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 4));
    EXPECT_DOUBLE_EQ(std::get<double>(Result.GetField(0, 0)), (10.5 + 20.0 + 15.5) / 3.0);
    EXPECT_DOUBLE_EQ(std::get<double>(Result.GetField(0, 1)), 46.0);
    EXPECT_DOUBLE_EQ(std::get<double>(Result.GetField(0, 2)), 10.5);
    EXPECT_DOUBLE_EQ(std::get<double>(Result.GetField(0, 3)), 20.0);

    CloseTempDb();
}

//===================================================================
// T32: Use transactions: BEGIN → INSERT/UPDATE → COMMIT
TEST(SQLite_Executor, T32_TransactionCommit)
{
    SCOPED_TRACE("T32: Use transactions: BEGIN → INSERT/UPDATE → COMMIT");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE TxTable(Id INTEGER PRIMARY KEY, Value TEXT);");

    SQLite_Executor().ExecuteQuery(DbPath, "BEGIN;");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO TxTable VALUES (1, 'Alpha');");
    SQLite_Executor().ExecuteQuery(DbPath, "UPDATE TxTable SET Value = 'Beta' WHERE Id = 1;");
    SQLite_Executor().ExecuteQuery(DbPath, "COMMIT;");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT Value FROM TxTable WHERE Id = 1;");
    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 1));
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 0)), "Beta");

    CloseTempDb();
}


//===================================================================
// T33: Test ROLLBACK scenario
TEST(SQLite_Executor, T33_TransactionRollback)
{
    SCOPED_TRACE("T33: Test ROLLBACK scenario");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    SQLite_Executor Executor;
    SQL_ScriptExecutor ScriptExec;

    // Create table outside the transaction
    Executor.ExecuteQuery(DbPath, "CREATE TABLE RollbackTable(Id INTEGER PRIMARY KEY, Value TEXT);");

    // Perform rollback inside transaction
    const std::string Script =
        "BEGIN;"
        "INSERT INTO RollbackTable VALUES (1, 'ShouldNotPersist');"
        "ROLLBACK;";

    ScriptExec.ExecuteScript(Executor, DbPath, Script);

    const SQL_TableValues2D Result = Executor.ExecuteQuery(DbPath, "SELECT * FROM RollbackTable;");
    
    // Accept both outcomes: 0x2 if metadata is extracted, or 0x0 if it's not
    EXPECT_TRUE(Result.GetSize() == std::make_pair(0, 2) || Result.GetSize() == std::make_pair(0, 0));

    // Additionally verify that table schema still exists:
    const SQL_TableValues2D Schema = Executor.ExecuteQuery(DbPath, "PRAGMA table_info(RollbackTable);");
    EXPECT_EQ(Schema.IsEmpty(), false);  // schema must exist and be non-empty

    EXPECT_TRUE(Result.IsEmpty());

    CloseTempDb();
}


//===================================================================
// T34: Simulate nested transactions
TEST(SQLite_Executor, T34_NestedTransactionEmulation)
{
    SCOPED_TRACE("T34: Simulate nested transactions (emulated savepoint)");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE NestedTx(Id INTEGER PRIMARY KEY, Val TEXT);");

    SQLite_Executor().ExecuteQuery(DbPath, "BEGIN;");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO NestedTx VALUES (1, 'Outer');");

    // Simulated nested transaction via SAVEPOINT
    SQLite_Executor().ExecuteQuery(DbPath, "SAVEPOINT inner_tx;");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO NestedTx VALUES (2, 'Inner');");
    SQLite_Executor().ExecuteQuery(DbPath, "ROLLBACK TO inner_tx;");
    SQLite_Executor().ExecuteQuery(DbPath, "RELEASE inner_tx;");

    SQLite_Executor().ExecuteQuery(DbPath, "COMMIT;");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM NestedTx;");
    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 2));
    EXPECT_EQ(std::get<int64_t>(Result.GetField(0, 0)), 1);
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 1)), "Outer");

    CloseTempDb();
}

//===================================================================
// T35: Enable foreign key constraint enforcement
TEST(SQLite_Executor, T35_EnforceForeignKey)
{
    SCOPED_TRACE("T35: Enable foreign key constraint enforcement");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    SQLite_Executor Executor;
    SQL_ScriptExecutor ScriptExec;

    // Enable FK and create valid data
    const std::string SetupScript =
        "PRAGMA foreign_keys = ON;"
        "CREATE TABLE Parent(Id INTEGER PRIMARY KEY);"
        "CREATE TABLE Child(Id INTEGER PRIMARY KEY, ParentId INTEGER, "
        "FOREIGN KEY(ParentId) REFERENCES Parent(Id));"
        "INSERT INTO Parent VALUES (1);"
        "INSERT INTO Child VALUES (10, 1);";

    ScriptExec.ExecuteScript(Executor, DbPath, SetupScript);

    // Capture stderr and verify error message upon FK violation
    testing::internal::CaptureStderr();

    EXPECT_THROW({
        Executor.ExecuteQuery(DbPath, "INSERT INTO Child VALUES (11, 999);");
    }, std::runtime_error);

    const std::string Output = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(Output.contains("FOREIGN KEY") ||
                Output.contains("foreign key") ||
                Output.contains("constraint failed"))
        << "Expected foreign key error in output:\n" << Output;

    CloseTempDb();
}








//===================================================================
// T36: Check for constraint violation on UNIQUE or PRIMARY KEY
TEST(SQLite_Executor, T36_UniqueOrPrimaryKeyViolation)
{
    SCOPED_TRACE("T36: Check for constraint violation on UNIQUE or PRIMARY KEY");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor Executor;

    Executor.ExecuteQuery(DbPath, "CREATE TABLE UniqueTest(Id INTEGER PRIMARY KEY, Email TEXT UNIQUE);");
    Executor.ExecuteQuery(DbPath, "INSERT INTO UniqueTest VALUES (1, 'a@b.com');");

    // PRIMARY KEY violation
    testing::internal::CaptureStderr();
    EXPECT_THROW({
        Executor.ExecuteQuery(DbPath, "INSERT INTO UniqueTest VALUES (1, 'c@d.com');");
    }, std::runtime_error);
    const std::string Output1 = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(Output1.contains("PRIMARY") ||
                Output1.contains("UNIQUE") ||
                Output1.contains("constraint failed"))
        << "Expected PRIMARY KEY error in output:\n" << Output1;

    // UNIQUE constraint violation
    testing::internal::CaptureStderr();
    EXPECT_THROW({
        Executor.ExecuteQuery(DbPath, "INSERT INTO UniqueTest VALUES (2, 'a@b.com');");
    }, std::runtime_error);
    const std::string Output2 = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(Output2.contains("UNIQUE") ||
                Output2.contains("constraint failed"))
        << "Expected UNIQUE constraint error in output:\n" << Output2;

    CloseTempDb();
}


//===================================================================
// T37: Insert with missing NOT NULL field
TEST(SQLite_Executor, T37_MissingNotNull)
{
    SCOPED_TRACE("T37: Insert with missing NOT NULL field");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor Executor;

    Executor.ExecuteQuery(DbPath, "CREATE TABLE NotNullTest(Id INTEGER NOT NULL, Name TEXT);");

    testing::internal::CaptureStderr();
    EXPECT_THROW({
        Executor.ExecuteQuery(DbPath, "INSERT INTO NotNullTest(Name) VALUES ('Unnamed');");
    }, std::runtime_error);
    const std::string Output = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(Output.contains("NOT NULL") ||
                Output.contains("constraint failed"))
        << "Expected NOT NULL constraint error in output:\n" << Output;

    CloseTempDb();
}

//===================================================================
// T38: UPDATE/DELETE with invalid WHERE clause (zero matches)
TEST(SQLite_Executor, T38_UpdateDeleteWithInvalidWhere)
{
    SCOPED_TRACE("T38: UPDATE/DELETE with invalid WHERE clause (zero matches)");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE T(Id INTEGER, Value TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO T VALUES (1, 'A'), (2, 'B');");

    // UPDATE with no matching WHERE
    SQL_TableValues2D UpdateResult = SQLite_Executor().ExecuteQuery(DbPath, "UPDATE T SET Value = 'X' WHERE Id = 999;");
    EXPECT_TRUE(UpdateResult.IsEmpty());

    // DELETE with no matching WHERE
    SQL_TableValues2D DeleteResult = SQLite_Executor().ExecuteQuery(DbPath, "DELETE FROM T WHERE Id = 999;");
    EXPECT_TRUE(DeleteResult.IsEmpty());

    // Ensure table remains unchanged
    SQL_TableValues2D VerifyResult = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM T;");
    EXPECT_EQ(VerifyResult.GetSize(), std::make_pair(2, 2));

    CloseTempDb();
}

//===================================================================
// T39: Access metadata via PRAGMA table_info(...)
TEST(SQLite_Executor, T39_PragmaTableInfo)
{
    SCOPED_TRACE("T39: Access metadata via PRAGMA table_info(...)");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE MetaTest(Id INTEGER PRIMARY KEY, Name TEXT, Age INTEGER);");

    SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "PRAGMA table_info(MetaTest);");

    EXPECT_FALSE(Result.IsEmpty());
    EXPECT_EQ(Result.GetSize().second, 6); // PRAGMA table_info returns 6 columns
    EXPECT_EQ(Result.GetSize().first, 3);  // 3 columns in table

    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 1)), "Id");
    EXPECT_EQ(std::get<std::string>(Result.GetField(1, 1)), "Name");
    EXPECT_EQ(std::get<std::string>(Result.GetField(2, 1)), "Age");

    CloseTempDb();
}

//===================================================================
// T40: Use PRAGMA foreign_key_list(...)
TEST(SQLite_Executor, T40_PragmaForeignKeyList)
{
    SCOPED_TRACE("T40: Use PRAGMA foreign_key_list(...)");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "PRAGMA foreign_keys = ON;");
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Parent(Id INTEGER PRIMARY KEY);");
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Child(Id INTEGER PRIMARY KEY, ParentId INTEGER REFERENCES Parent(Id));");

    SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "PRAGMA foreign_key_list(Child);");

    EXPECT_FALSE(Result.IsEmpty());
    EXPECT_GE(Result.GetSize().second, 8); // foreign_key_list has >=8 fields
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 2)), "Parent");   // Target table
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 3)), "ParentId"); // Local column
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 4)), "Id");       // Target column

    CloseTempDb();
}









//===================================================================
// T41: Use PRAGMA index_list(...)
TEST(SQLite_Executor, T41_PragmaIndexList)
{
    SCOPED_TRACE("T41: Use PRAGMA index_list(...)");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    // This table only has explicit index on 'Name'
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Indexed(Id INTEGER PRIMARY KEY, Name TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE INDEX NameIndex ON Indexed(Name);");

    SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "PRAGMA index_list(Indexed);");

    // Should return 1 index (NameIndex)
    EXPECT_EQ(Result.GetSize().first, 1);
    EXPECT_GE(Result.GetSize().second, 3); // at least: seq, name, unique

    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 1)), "NameIndex");

    CloseTempDb();
}

//===================================================================
// T42: Use PRAGMA user_version or schema_version
TEST(SQLite_Executor, T42_PragmaUserVersion)
{
    SCOPED_TRACE("T42: Use PRAGMA user_version or schema_version");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    // Ensure schema version is incremented
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Dummy(X INTEGER);");

    SQLite_Executor().ExecuteQuery(DbPath, "PRAGMA user_version = 33;");
    SQL_TableValues2D UserVersion = SQLite_Executor().ExecuteQuery(DbPath, "PRAGMA user_version;");
    EXPECT_EQ(UserVersion.GetSize(), std::make_pair(1, 1));
    EXPECT_EQ(std::get<int64_t>(UserVersion.GetField(0, 0)), 33);

    SQL_TableValues2D SchemaVersion = SQLite_Executor().ExecuteQuery(DbPath, "PRAGMA schema_version;");
    EXPECT_EQ(SchemaVersion.GetSize(), std::make_pair(1, 1));
    EXPECT_GE(std::get<int64_t>(SchemaVersion.GetField(0, 0)), 1);

    CloseTempDb();
}

//===================================================================
// T43: Use PRAGMA journal_mode and PRAGMA synchronous
TEST(SQLite_Executor, T43_PragmaJournalAndSync)
{
    SCOPED_TRACE("T43: Use PRAGMA journal_mode and PRAGMA synchronous");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();

    SQL_TableValues2D JournalMode = SQLite_Executor().ExecuteQuery(DbPath, "PRAGMA journal_mode;");
    EXPECT_EQ(JournalMode.GetSize(), std::make_pair(1, 1));
    EXPECT_TRUE(std::holds_alternative<std::string>(JournalMode.GetField(0, 0)));

    SQL_TableValues2D SyncLevel = SQLite_Executor().ExecuteQuery(DbPath, "PRAGMA synchronous;");
    EXPECT_EQ(SyncLevel.GetSize(), std::make_pair(1, 1));
    EXPECT_TRUE(std::holds_alternative<int64_t>(SyncLevel.GetField(0, 0)));

    CloseTempDb();
}

//===================================================================
// T44: List all tables using sqlite_master
TEST(SQLite_Executor, T44_ListAllTables_UsingSqliteMaster)
{
    SCOPED_TRACE("T44: List all tables using sqlite_master");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Table1(Id INTEGER);");
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Table2(Name TEXT);");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(
        DbPath, "SELECT name FROM sqlite_master WHERE type = 'table' ORDER BY name;");

    EXPECT_EQ(Result.GetSize().first, 2);
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 0)), "Table1");
    EXPECT_EQ(std::get<std::string>(Result.GetField(1, 0)), "Table2");

    CloseTempDb();
}


//===================================================================
// T45: Use and inspect sqlite_sequence for AUTOINCREMENT
TEST(SQLite_Executor, T45_CheckSqliteSequence_ForAutoincrement)
{
    SCOPED_TRACE("T45: Use and inspect sqlite_sequence for AUTOINCREMENT");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath,
        "CREATE TABLE AutoTable(Id INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO AutoTable(Name) VALUES ('One');");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO AutoTable(Name) VALUES ('Two');");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT name, seq FROM sqlite_sequence;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 2));
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 0)), "AutoTable");
    EXPECT_EQ(std::get<int64_t>(Result.GetField(0, 1)), 2);

    CloseTempDb();
}







//===================================================================
// T46: ATTACH DATABASE and query across databases
TEST(SQLite_Executor, T46_AttachDatabase_AndCrossQuery)
{
    SCOPED_TRACE("T46: ATTACH DATABASE and query across databases");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    const std::string DbPath2 = "temp_attached.db";

    // Create attached DB with a table and data
    {
        sqlite3* DbHandle = nullptr;
        sqlite3_open_v2(DbPath2.c_str(), &DbHandle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
        sqlite3_exec(DbHandle, "CREATE TABLE AttachedTable(Id INTEGER, Value TEXT);", nullptr, nullptr, nullptr);
        sqlite3_exec(DbHandle, "INSERT INTO AttachedTable VALUES (1, 'Attached');", nullptr, nullptr, nullptr);
        sqlite3_close(DbHandle);
    }

    SQLite_Executor Executor;

    // Capture warnings due to tailing SQL
    testing::internal::CaptureStderr();

    std::string AttachScript;
    AttachScript.append("ATTACH DATABASE '").append(DbPath2).append("' AS Attached;");
    AttachScript.append("CREATE TABLE MainTable(Id INTEGER);");
    AttachScript.append("INSERT INTO MainTable VALUES (1);");

    const SQL_TableValues2D SetupResult = Executor.ExecuteQuery(DbPath, AttachScript);

    const std::string Warnings = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(SetupResult.IsEmpty());
    EXPECT_TRUE(!Warnings.empty());
    EXPECT_TRUE(Warnings.find("Unused tail") != std::string::npos ||
                Warnings.find("tail") != std::string::npos)
        << "Expected SQL tail warning, but got:\n" << Warnings;

    // Query from attached DB
    const SQL_TableValues2D Result = Executor.ExecuteQuery(DbPath, "SELECT Value FROM Attached.AttachedTable;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 1));
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 0)), "Attached");

    std::filesystem::remove(DbPath2);
    CloseTempDb();
}


//===================================================================
// T47: Create and use temporary tables
TEST(SQLite_Executor, T47_TemporaryTables_Behavior)
{
    SCOPED_TRACE("T47: Create and use temporary tables");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TEMP TABLE TempTable(Id INTEGER, Note TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO TempTable VALUES (1, 'Temporary');");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT * FROM TempTable;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 2));
    EXPECT_EQ(std::get<int64_t>(Result.GetField(0, 0)), 1);
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 1)), "Temporary");

    CloseTempDb();
}


//===================================================================
// T48: Create and query a view
TEST(SQLite_Executor, T48_CreateAndQueryView)
{
    SCOPED_TRACE("T48: Create and query a view");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Data(Id INTEGER, Label TEXT);");
    SQLite_Executor().ExecuteQuery(DbPath, "INSERT INTO Data VALUES (1, 'A'), (2, 'B');");
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE VIEW ViewData AS SELECT * FROM Data WHERE Id = 2;");

    const SQL_TableValues2D Result = SQLite_Executor().ExecuteQuery(DbPath, "SELECT Label FROM ViewData;");

    EXPECT_EQ(Result.GetSize(), std::make_pair(1, 1));
    EXPECT_EQ(std::get<std::string>(Result.GetField(0, 0)), "B");

    CloseTempDb();
}


//===================================================================
// T49: Use operators LIKE, GLOB, IN, and BETWEEN
TEST(SQLite_Executor, T49_Operators_LikeGlobInBetween)
{
    SCOPED_TRACE("T49: Use operators LIKE, GLOB, IN, and BETWEEN");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQLite_Executor().ExecuteQuery(DbPath, "CREATE TABLE Items(Name TEXT, Value INTEGER);");
    SQLite_Executor().ExecuteQuery(DbPath,
        "INSERT INTO Items VALUES "
        "('apple', 10), ('banana', 20), ('apricot', 30), ('grape', 40);");

    const auto LikeResult
        = SQLite_Executor().ExecuteQuery(
            DbPath, "SELECT Name FROM Items WHERE Name LIKE 'ap%';");
    EXPECT_EQ(LikeResult.GetSize().first, 2);

    const auto GlobResult
        = SQLite_Executor().ExecuteQuery(
            DbPath, "SELECT Name FROM Items WHERE Name GLOB 'a*e';");
    EXPECT_EQ(GlobResult.GetSize().first, 1);

    const auto InResult
        = SQLite_Executor().ExecuteQuery(
            DbPath, "SELECT Name FROM Items WHERE Value IN (10, 40);");
    EXPECT_EQ(InResult.GetSize().first, 2);

    const auto BetweenResult
        = SQLite_Executor().ExecuteQuery(
            DbPath, "SELECT Name FROM Items WHERE Value BETWEEN 15 AND 35;");
    EXPECT_EQ(BetweenResult.GetSize().first, 2);

    CloseTempDb();
}


//===================================================================
// T50: Execute multi-statement scripts with parameters
TEST(SQLite_Executor, T50_MultiStatementScript_WithValues)
{
    SCOPED_TRACE("T50: Execute multi-statement scripts with parameters");
    using namespace SQLite_Executor_TestNS;

    CreateTempDb();
    SQL_ScriptExecutor ScriptExecutor;
    SQLite_Executor Executor;

    const std::string Script =
        "CREATE TABLE Logs(Id INTEGER, Message TEXT);"
        "INSERT INTO Logs VALUES (1, 'Init');"
        "INSERT INTO Logs VALUES (2, 'Run');"
        "SELECT * FROM Logs ORDER BY Id;";

    const std::vector<std::pair<std::string, SQL_TableValues2D>> Executed =
        ScriptExecutor.ExecuteScript(Executor, DbPath, Script);

    // Validate number of executed statements
    EXPECT_EQ(Executed.size(), 4);

    // Validate SQL strings match original script
    EXPECT_EQ(Executed[0].first, "CREATE TABLE Logs(Id INTEGER, Message TEXT)");
    EXPECT_EQ(Executed[1].first, "INSERT INTO Logs VALUES (1, 'Init')");
    EXPECT_EQ(Executed[2].first, "INSERT INTO Logs VALUES (2, 'Run')");
    EXPECT_EQ(Executed[3].first, "SELECT * FROM Logs ORDER BY Id");

    // First three statements are structural or INSERTs → must return empty result sets
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_TRUE(Executed[i].second.IsEmpty());
        EXPECT_EQ(Executed[i].second.GetSize(), std::make_pair(0, 0));
    }

    // Validate SELECT result
    const SQL_TableValues2D& SelectResult = Executed[3].second;
    EXPECT_EQ(SelectResult.GetSize(), std::make_pair(2, 2));  // 2 rows, 2 columns

    // Validate first row contents
    EXPECT_EQ(std::get<int64_t>(SelectResult.GetField(0, 0)), 1);
    EXPECT_EQ(std::get<std::string>(SelectResult.GetField(0, 1)), "Init");

    EXPECT_TRUE(SelectResult.IsFieldOfType(0, 0, SQL_FieldType::Integer));
    EXPECT_TRUE(SelectResult.IsFieldOfType(0, 1, SQL_FieldType::Text));

    // Validate second row contents
    EXPECT_EQ(std::get<int64_t>(SelectResult.GetField(1, 0)), 2);
    EXPECT_EQ(std::get<std::string>(SelectResult.GetField(1, 1)), "Run");

    EXPECT_TRUE(SelectResult.IsFieldOfType(1, 0, SQL_FieldType::Integer));
    EXPECT_TRUE(SelectResult.IsFieldOfType(1, 1, SQL_FieldType::Text));

    CloseTempDb();
}

//===================================================================
// End of tests
