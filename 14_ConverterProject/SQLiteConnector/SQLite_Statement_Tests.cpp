#include <gtest/gtest.h>
#include <sqlite3.h>
#include <string>
#include "SQLiteConnector.h"

////////////////////// SQLite_Statement: WarnOnUnusedTail - No Warning //////////////////////
TEST(SQLite_Statement, T01_UnusedTailNullOrEmpty_NoWarning)
{
    SCOPED_TRACE("WarnOnUnusedTail with null or empty tail should issue no warning");

    const char* nullTail = nullptr;
    SQLite_Statement::WarnOnUnusedTail(nullTail);

    const char* emptyTail = "";
    SQLite_Statement::WarnOnUnusedTail(emptyTail);
}

////////////////////// SQLite_Statement: WarnOnUnusedTail - Warning Expected //////////////////////
TEST(SQLite_Statement, T02_UnusedTailPresent_ShouldWarn)
{
    SCOPED_TRACE("WarnOnUnusedTail with non-empty tail should issue warning");

    const char* extra = "SELECT * FROM Table; extra";
    SQLite_Statement::WarnOnUnusedTail(extra);
}

////////////////////// SQLite_Statement: Prepare Valid Statement //////////////////////
TEST(SQLite_Statement, T03_PrepareValidStatement_ShouldInitialize)
{
    SCOPED_TRACE("PrepareStatement should initialize StatementPtr correctly");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);

    const char* sql = "CREATE TABLE Test (Id INTEGER);";
    SQLite_Statement stmt(Db, sql);
    ASSERT_NE(stmt.Get(), nullptr);

    sqlite3_close(Db);
}

////////////////////// SQLite_Statement: Constructor Prepares Correctly //////////////////////
TEST(SQLite_Statement, T04_ConstructorValidInput_ShouldInitialize)
{
    SCOPED_TRACE("Constructor initializes DbHandle and prepares statement");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);

    const char* sql = "CREATE TABLE Person (Name TEXT);";
    SQLite_Statement stmt(Db, sql);
    ASSERT_NE(stmt.Get(), nullptr);

    sqlite3_close(Db);
}

////////////////////// SQLite_Statement: Destructor Finalizes //////////////////////
TEST(SQLite_Statement, T05_DestructorShouldFinalizeStatement)
{
    SCOPED_TRACE("Destructor finalizes the statement without exception");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);

    {
        const char* sql = "CREATE TABLE Thing (Value TEXT);";
        SQLite_Statement stmt(Db, sql);
    }

    sqlite3_close(Db);
}

////////////////////// SQLite_Statement: Get Returns StatementPtr //////////////////////
TEST(SQLite_Statement, T06_GetReturnsCorrectPointer)
{
    SCOPED_TRACE("Get should return initialized StatementPtr");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);

    const char* sql = "CREATE TABLE Animal (Type TEXT);";
    SQLite_Statement stmt(Db, sql);
    ASSERT_EQ(stmt.Get(), stmt.Get());

    sqlite3_close(Db);
}

////////////////////// SQLite_Statement: Step -> SQLITE_DONE //////////////////////
TEST(SQLite_Statement, T07_StepShouldReturnFalseWhenDone)
{
    SCOPED_TRACE("Step should return false when execution is complete (SQLITE_DONE)");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);

    sqlite3_exec(Db, "CREATE TABLE Person(Id INTEGER); INSERT INTO Person VALUES (1);", nullptr, nullptr, nullptr);
    SQLite_Statement stmt(Db, "DELETE FROM Person WHERE Id = 2;");
    ASSERT_FALSE(stmt.Step());

    sqlite3_close(Db);
}

////////////////////// SQLite_Statement: Step -> SQLITE_ROW //////////////////////
TEST(SQLite_Statement, T08_StepShouldReturnTrueWhenRow)
{
    SCOPED_TRACE("Step should return true when a row is available (SQLITE_ROW)");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);

    sqlite3_exec(Db, "CREATE TABLE Person(Id INTEGER); INSERT INTO Person VALUES (42);", nullptr, nullptr, nullptr);
    SQLite_Statement stmt(Db, "SELECT * FROM Person;");
    ASSERT_TRUE(stmt.Step());

    sqlite3_close(Db);
}

////////////////////// SQLite_Statement: Step Fails on Invalid Query //////////////////////
TEST(SQLite_Statement, T09_CorruptStepShouldCrash)
{
    SCOPED_TRACE("Step on corrupted statement should trigger Checker");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);

    EXPECT_THROW({
        SQLite_Statement stmt(Db, "SELECT nonexistent FROM missing;");
    }, std::runtime_error);

    sqlite3_close(Db);
}

////////////////////// SQLite_Statement: Reset Valid //////////////////////
TEST(SQLite_Statement, T10_ResetShouldSucceed)
{
    SCOPED_TRACE("Reset should successfully reset valid statement");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);

    sqlite3_exec(Db, "CREATE TABLE Demo(Id INTEGER); INSERT INTO Demo VALUES (1);", nullptr, nullptr, nullptr);
    SQLite_Statement stmt(Db, "SELECT * FROM Demo;");
    stmt.Step();  // read 1
    stmt.Reset(); // rewind
    ASSERT_TRUE(stmt.Step()); // should return same row again

    sqlite3_close(Db);
}

////////////////////// SQLite_Statement: Prepare Fails on Malformed SQL //////////////////////
TEST(SQLite_Statement, T11_MalformedSqlShouldCrash)
{
    SCOPED_TRACE("Malformed SQL triggers failure in CheckResult during statement preparation");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);

    EXPECT_THROW({
        SQLite_Statement stmt(Db, "SELECT FROM");
    }, std::runtime_error);

    sqlite3_close(Db);
}

////////////////////// SQLite_Statement: Step and Reset Can Repeat //////////////////////
TEST(SQLite_Statement, T12_StepAndResetRepeatShouldWork)
{
    SCOPED_TRACE("Step and Reset can be called multiple times correctly");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);

    sqlite3_exec(Db, "CREATE TABLE Repeat(Id INTEGER); INSERT INTO Repeat VALUES (10);", nullptr, nullptr, nullptr);
    SQLite_Statement stmt(Db, "SELECT * FROM Repeat;");
    ASSERT_TRUE(stmt.Step());
    stmt.Reset();
    ASSERT_TRUE(stmt.Step());

    sqlite3_close(Db);
}
