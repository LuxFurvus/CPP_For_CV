#include <string>
////////
#include <sqlite3.h>
#include <gtest/gtest.h>
////////
#include "SQLite_Statement.h"

// ================================
// ░ Test Case Descriptions ░
// ================================

// T01: Creates a valid statement from a valid single query
// T02: Throws on invalid SQL passed to constructor
// T03: Handles multiple statements in SQL query by preparing only the first and emitting a warning to std::cerr
// T04: Step returns true for single-row SELECT query
// T05: Step returns false for query with no rows
// T06: Reset allows Step to rerun successfully after execution

// T07: Get returns non-null pointer after valid construction
// T08: IsValid returns true after valid construction
// T09: IsValid returns false after Finalize is called
// T10: Calling Finalize multiple times is safe and idempotent
// T11: Destructor finalizes the statement without memory leaks or crash

// T12: Move constructor leaves source in invalid state, target valid
// T13: Move assignment finalizes target's old state and accepts new one
// T14: After move, source returns IsValid == false, Step fails safely
// T15: Reset or Step on finalized statement is handled with std::runtime_error throw
// T16: Move assignment to self is safe and leaves object in valid state
// T17: Chained move assignments leave only the final object valid, all intermediates invalid
// T18: Destroying a moved-from object is safe and does not affect moved-to target

//===================================================================
// T01: Creates a valid statement from a valid single query
TEST(SQLite_Statement, T01_ValidConstruction)
{
    SCOPED_TRACE("T01: Creates a valid statement from a valid single query");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    SQLite_Statement Statement(Db, "SELECT * FROM T;");
    EXPECT_TRUE(Statement.IsValid());
    EXPECT_NE(Statement.Get(), nullptr);

    Statement.Finalize();
    sqlite3_close(Db);
}

//===================================================================
// T02: Throws on invalid SQL passed to constructor
TEST(SQLite_Statement, T02_InvalidSQLThrows)
{
    SCOPED_TRACE("T02: Throws on invalid SQL passed to constructor");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);

    EXPECT_ANY_THROW({
        SQLite_Statement Statement(Db, "SELECT FROM;");
    });

    sqlite3_close(Db);
}

//===================================================================
// T03: Handles multiple statements in SQL query by preparing only the first and emitting a warning to std::cerr
TEST(SQLite_Statement, T03_WarnsOnExtraStatements)
{
    SCOPED_TRACE("T03: Handles multiple statements in SQL query by preparing only the first and emitting a warning to std::cerr");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    testing::internal::CaptureStderr();
    SQLite_Statement Statement(Db, "SELECT 1; SELECT 2;");
    std::string StderrOutput = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(Statement.IsValid());
    EXPECT_NE(Statement.Get(), nullptr);
    EXPECT_TRUE(StderrOutput.contains("Unused tail"));

    Statement.Finalize();
    sqlite3_close(Db);
}

//===================================================================
// T04: Step returns true for single-row SELECT query
TEST(SQLite_Statement, T04_StepReturnsTrue)
{
    SCOPED_TRACE("T04: Step returns true for single-row SELECT query");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);
    sqlite3_exec(Db, "INSERT INTO T VALUES (123);", nullptr, nullptr, nullptr);

    SQLite_Statement Statement(Db, "SELECT * FROM T;");
    EXPECT_TRUE(Statement.Step()); // First row exists
    EXPECT_FALSE(Statement.Step()); // Only one row

    Statement.Finalize();
    sqlite3_close(Db);
}

//===================================================================
// T05: Step returns false for query with no rows
TEST(SQLite_Statement, T05_StepReturnsFalseOnEmpty)
{
    SCOPED_TRACE("T05: Step returns false for query with no rows");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    SQLite_Statement Statement(Db, "SELECT * FROM T;");
    EXPECT_FALSE(Statement.Step());

    Statement.Finalize();
    sqlite3_close(Db);
}

//===================================================================
// T06: Reset allows Step to rerun successfully after execution
TEST(SQLite_Statement, T06_ResetAllowsRerun)
{
    SCOPED_TRACE("T06: Reset allows Step to rerun successfully after execution");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);
    sqlite3_exec(Db, "INSERT INTO T VALUES (1);", nullptr, nullptr, nullptr);

    SQLite_Statement Statement(Db, "SELECT * FROM T;");
    EXPECT_TRUE(Statement.Step());
    EXPECT_FALSE(Statement.Step());

    Statement.Reset();

    EXPECT_TRUE(Statement.Step());
    EXPECT_FALSE(Statement.Step());

    Statement.Finalize();
    sqlite3_close(Db);
}

//===================================================================
// T07: Get returns non-null pointer after valid construction
TEST(SQLite_Statement, T07_GetReturnsNonNull)
{
    SCOPED_TRACE("T07: Get returns non-null pointer after valid construction");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    SQLite_Statement Statement(Db, "SELECT * FROM T;");
    EXPECT_NE(Statement.Get(), nullptr);

    Statement.Finalize();
    sqlite3_close(Db);
}

//===================================================================
// T08: IsValid returns true after valid construction
TEST(SQLite_Statement, T08_IsValidAfterConstruction)
{
    SCOPED_TRACE("T08: IsValid returns true after valid construction");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    SQLite_Statement Statement(Db, "SELECT * FROM T;");
    EXPECT_TRUE(Statement.IsValid());

    Statement.Finalize();
    sqlite3_close(Db);
}

//===================================================================
// T09: IsValid returns false after Finalize is called
TEST(SQLite_Statement, T09_IsInvalidAfterFinalize)
{
    SCOPED_TRACE("T09: IsValid returns false after Finalize is called");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    SQLite_Statement Statement(Db, "SELECT * FROM T;");
    Statement.Finalize();

    EXPECT_FALSE(Statement.IsValid());
    EXPECT_EQ(Statement.Get(), nullptr);

    sqlite3_close(Db);
}

//===================================================================
// T10: Calling Finalize multiple times is safe and idempotent
TEST(SQLite_Statement, T10_MultipleFinalizeSafe)
{
    SCOPED_TRACE("T10: Calling Finalize multiple times is safe and idempotent");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    SQLite_Statement Statement(Db, "SELECT * FROM T;");
    Statement.Finalize();
    Statement.Finalize();
    Statement.Finalize();

    EXPECT_FALSE(Statement.IsValid());

    sqlite3_close(Db);
}

//===================================================================
// T11: Destructor finalizes the statement without memory leaks or crash
TEST(SQLite_Statement, T11_DestructorFinalizesSafely)
{
    SCOPED_TRACE("T11: Destructor finalizes the statement without memory leaks or crash");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    {
        SQLite_Statement Statement(Db, "SELECT * FROM T;");
        EXPECT_TRUE(Statement.IsValid());
    } // Destructor should safely finalize

    sqlite3_close(Db);
}

//===================================================================
// T12: Move constructor leaves source in invalid state, target valid
TEST(SQLite_Statement, T12_MoveConstructorTransfersOwnership)
{
    SCOPED_TRACE("T12: Move constructor leaves source in invalid state, target valid");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    SQLite_Statement Original(Db, "SELECT * FROM T;");
    SQLite_Statement Moved(std::move(Original));

    EXPECT_TRUE(Moved.IsValid());
    EXPECT_FALSE(Original.IsValid());

    Moved.Finalize();
    sqlite3_close(Db);
}

//===================================================================
// T13: Move assignment finalizes target's old state and accepts new one
TEST(SQLite_Statement, T13_MoveAssignmentReplacesState)
{
    SCOPED_TRACE("T13: Move assignment finalizes target's old state and accepts new one");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    SQLite_Statement First(Db, "SELECT * FROM T;");
    SQLite_Statement Second(Db, "SELECT 1;");
    Second = std::move(First);

    EXPECT_TRUE(Second.IsValid());
    EXPECT_FALSE(First.IsValid());

    Second.Finalize();
    sqlite3_close(Db);
}


//===================================================================
// T14: After move, source returns IsValid == false, Step fails safely
TEST(SQLite_Statement, T14_MovedFromStepIsSafe)
{
    SCOPED_TRACE("T14: After move, source returns IsValid == false, Step fails safely");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    SQLite_Statement Source(Db, "SELECT * FROM T;");
    SQLite_Statement Target(std::move(Source));

    // Source.Finalize(); // No need to finalize moved-from object

    EXPECT_FALSE(Source.IsValid());

    // Step() throws, if it is called on invalid object
    EXPECT_THROW(Source.Step(), std::runtime_error);

    Target.Finalize();
    sqlite3_close(Db);
}

//===================================================================
// T15: Reset or Step on finalized statement is handled with std::runtime_error throw
TEST(SQLite_Statement, T15_AccessAfterFinalizeIsSafe)
{
    SCOPED_TRACE("T15: Reset or Step on finalized statement is handled without crash or undefined behavior");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    SQLite_Statement Statement(Db, "SELECT * FROM T;");
    Statement.Finalize();

    // EXPECT_NO_FATAL_FAILURE(Statement.Reset());
    EXPECT_THROW(Statement.Step(), std::runtime_error);
    // EXPECT_FALSE(Statement.Step());
    EXPECT_THROW(Statement.Step(), std::runtime_error);

    sqlite3_close(Db);
}

//===================================================================
// T16: Move assignment to self is safe and leaves object in valid state
TEST(SQLite_Statement, T16_MoveAssignToSelfIsSafe)
{
    SCOPED_TRACE("T16: Move assignment to self is safe and leaves object in valid state");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wself-move"
#endif

    SQLite_Statement Statement(Db, "SELECT * FROM T;");
    Statement = std::move(Statement); // Self move-assignment

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif

    EXPECT_TRUE(Statement.IsValid());

    Statement.Finalize();
    sqlite3_close(Db);
}

//===================================================================
// T17: Chained move assignments leave only the final object valid, all intermediates invalid
TEST(SQLite_Statement, T17_ChainedMoveLeavesOnlyFinalValid)
{
    SCOPED_TRACE("T17: Chained move assignments leave only the final object valid, all intermediates invalid");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    SQLite_Statement A(Db, "SELECT * FROM T;");
    SQLite_Statement B = std::move(A);
    SQLite_Statement C = std::move(B);

    EXPECT_FALSE(A.IsValid());
    EXPECT_FALSE(B.IsValid());
    EXPECT_TRUE(C.IsValid());

    C.Finalize();
    sqlite3_close(Db);
}

//===================================================================
// T18: Destroying a moved-from object is safe and does not affect moved-to target
TEST(SQLite_Statement, T18_DestroyMovedFromIsSafe)
{
    SCOPED_TRACE("T18: Destroying a moved-from object is safe and does not affect moved-to target");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE T(Id INTEGER);", nullptr, nullptr, nullptr);

    std::optional<SQLite_Statement> A(std::in_place, Db, "SELECT * FROM T;");
    SQLite_Statement B = std::move(*A);
    A.reset(); // Destroy moved-from object

    EXPECT_TRUE(B.IsValid());

    B.Finalize();
    sqlite3_close(Db);
}
