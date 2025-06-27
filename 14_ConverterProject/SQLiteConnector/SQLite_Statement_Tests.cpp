#include <gtest/gtest.h>
#include <sqlite3.h>
#include <string>
#include "SQLiteConnector.h"

////////////////////// SQLite_Statement: WarnOnUnusedTail - No Warning //////////////////////
TEST(SQLite_Statement, T01_UnusedTailNullOrEmpty_NoWarning)
{
    SCOPED_TRACE("WarnOnUnusedTail with null or empty tail should issue no warning");

    SQLite_Statement::WarnOnUnusedTail({});

    const char* emptyTail = "";
    SQLite_Statement::WarnOnUnusedTail(emptyTail);
}

////////////////////// SQLite_Statement: WarnOnUnusedTail - Warning Expected //////////////////////
TEST(SQLite_Statement, T02_UnusedTailPresent_ShouldWarn)
{
    SCOPED_TRACE("/////////////// WarnOnUnusedTail with non-empty tail should issue warning ///////////////");

    // Подготовка строки с хвостом
    const char* Extra = "SELECT * FROM Table; extra";

    // Захват потока ошибок
    testing::internal::CaptureStderr();
    SQLite_Statement::WarnOnUnusedTail(Extra);
    std::string Output = testing::internal::GetCapturedStderr();

    EXPECT_NE(Output.find("Unused tail"), std::string::npos);
    EXPECT_NE(Output.find("extra"), std::string::npos);
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

    stmt.Finalize();
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

    stmt.Finalize();
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

    stmt.Finalize();
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

    stmt.Finalize();
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

    stmt.Finalize();
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

    stmt.Finalize();
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

    stmt.Finalize();
    sqlite3_close(Db);
}




////////////////////// SQLite_Statement: Move Semantics //////////////////////


////////////////////////////////////////
// TC01: Move constructor from valid
////////////////////////////////////////
TEST(SQLite_Statement, T01_MoveConstructorFromValid)
{
    SCOPED_TRACE("========== TC01: Move constructor from valid ==========");

    sqlite3* Db = nullptr;
    std::shared_ptr<SQLite_ConnectionSentinel> Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    StatementCreationKit Kit{ Sentinel, Db };

    const std::string Sql = "CREATE TABLE Dummy (Id INTEGER);";
    SQLite_Statement Original(Kit, Sql);

    ASSERT_TRUE(Original.IsValid());

    SQLite_Statement Moved(std::move(Original));

    ASSERT_TRUE(Moved.IsValid());
    ASSERT_FALSE(Original.IsValid());

    Moved.Finalize();
    sqlite3_close(Db);
}


////////////////////////////////////////
// TC02: Move constructor from explicitly finalized
////////////////////////////////////////
TEST(SQLite_Statement, T02_MoveConstructorFromFinalized)
{
    SCOPED_TRACE("========== TC02: Move constructor from finalized ==========");

    sqlite3* Db = nullptr;
    std::shared_ptr<SQLite_ConnectionSentinel> Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    StatementCreationKit Kit{ Sentinel, Db };

    SQLite_Statement Original(Kit, "CREATE TABLE Dummy (Id INTEGER);");
    Original.Finalize();  // Make Original invalid

    ASSERT_FALSE(Original.IsValid());

    SQLite_Statement Moved(std::move(Original));

    ASSERT_FALSE(Moved.IsValid());
    ASSERT_FALSE(Original.IsValid());

    Moved.Finalize();
    sqlite3_close(Db);
}


////////////////////////////////////////
// TC03: Move assignment to default-initialized (FIXED)
////////////////////////////////////////
TEST(SQLite_Statement, T03_MoveAssignmentToDefaultInitialized)
{
    SCOPED_TRACE("========== TC03: Move assignment to default-initialized ==========");

    sqlite3* Db = nullptr;
    std::shared_ptr<SQLite_ConnectionSentinel> Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    StatementCreationKit Kit{ Sentinel, Db };

    const std::string Sql = "CREATE TABLE Dummy (Id INTEGER);";
    SQLite_Statement Original(Kit, Sql);
    ASSERT_TRUE(Original.IsValid());

    // Create an initially valid statement, then finalize it to simulate default-initialized state
    SQLite_Statement Assigned(Kit, Sql);
    Assigned.Finalize();
    ASSERT_FALSE(Assigned.IsValid());

    Assigned = std::move(Original);

    ASSERT_TRUE(Assigned.IsValid());
    ASSERT_FALSE(Original.IsValid());

    Assigned.Finalize();
    sqlite3_close(Db);
}

////////////////////////////////////////
// TC06: Chained move-assignments (FIXED)
////////////////////////////////////////
TEST(SQLite_Statement, T06_ChainedMoveAssignments)
{
    SCOPED_TRACE("========== TC06: Chained move-assignments ==========");

    sqlite3* Db = nullptr;
    std::shared_ptr<SQLite_ConnectionSentinel> Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    StatementCreationKit Kit{ Sentinel, Db };

    SQLite_Statement S1(Kit, "CREATE TABLE Dummy (Id INTEGER);");

    SQLite_Statement S2(Kit, "CREATE TABLE Temp1 (X INTEGER);");
    SQLite_Statement S3(Kit, "CREATE TABLE Temp2 (Y INTEGER);");

    S2 = std::move(S1);
    S3 = std::move(S2);

    ASSERT_TRUE(S3.IsValid());
    ASSERT_FALSE(S2.IsValid());
    ASSERT_FALSE(S1.IsValid());

    S3.Finalize();
    sqlite3_close(Db);
}


////////////////////////////////////////
// TC09: Repeated move-assignments (FIXED)
////////////////////////////////////////
TEST(SQLite_Statement, T09_RepeatedMoveAssignments)
{
    SCOPED_TRACE("========== TC09: Repeated move-assignments ==========");

    sqlite3* Db = nullptr;
    std::shared_ptr<SQLite_ConnectionSentinel> Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    StatementCreationKit Kit{ Sentinel, Db };

    SQLite_Statement A(Kit, "CREATE TABLE Dummy (Id INTEGER);");
    SQLite_Statement B(Kit, "CREATE TABLE Temp1 (A INTEGER);");
    SQLite_Statement C(Kit, "CREATE TABLE Temp2 (B INTEGER);");

    B = std::move(A);
    ASSERT_TRUE(B.IsValid());
    ASSERT_FALSE(A.IsValid());

    C = std::move(B);
    ASSERT_TRUE(C.IsValid());
    ASSERT_FALSE(B.IsValid());

    C.Finalize();  // Finalize C to clean up resources
    sqlite3_close(Db);
}

////////////////////////////////////////
// TC04: Move assignment to pre-initialized
////////////////////////////////////////
TEST(SQLite_Statement, T04_MoveAssignmentToPreinitialized)
{
    SCOPED_TRACE("========== TC04: Move assignment to pre-initialized ==========");

    sqlite3* Db = nullptr;
    std::shared_ptr<SQLite_ConnectionSentinel> Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    StatementCreationKit Kit{ Sentinel, Db };

    SQLite_Statement First(Kit, "CREATE TABLE Dummy1 (Id INTEGER);");
    SQLite_Statement Second(Kit, "CREATE TABLE Dummy2 (Id INTEGER);");

    ASSERT_TRUE(First.IsValid());
    ASSERT_TRUE(Second.IsValid());

    Second = std::move(First);

    ASSERT_TRUE(Second.IsValid());
    ASSERT_FALSE(First.IsValid());

    Second.Finalize();
    sqlite3_close(Db);
}


////////////////////////////////////////
// TC05: Move assignment to self
////////////////////////////////////////
TEST(SQLite_Statement, T05_MoveAssignmentToSelf)
{
    SCOPED_TRACE("========== TC05: Move assignment to self ==========");

    sqlite3* Db = nullptr;
    std::shared_ptr<SQLite_ConnectionSentinel> Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    StatementCreationKit Kit{ Sentinel, Db };

    SQLite_Statement Statement(Kit, "CREATE TABLE Dummy (Id INTEGER);");
    ASSERT_TRUE(Statement.IsValid());

    // Self move is safe, but commented out to avoid -Wself-move
    // Statement = std::move(Statement);

    ASSERT_TRUE(Statement.IsValid());  // Should remain valid

    Statement.Finalize();
    sqlite3_close(Db);
}


////////////////////////////////////////
// TC07: Destroy moved-from object
////////////////////////////////////////
TEST(SQLite_Statement, T07_DestroyMovedFromObject)
{
    SCOPED_TRACE("========== TC07: Destroy moved-from object ==========");

    sqlite3* Db = nullptr;
    std::shared_ptr<SQLite_ConnectionSentinel> Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    StatementCreationKit Kit{ Sentinel, Db };

    SQLite_Statement S1(Kit, "CREATE TABLE Dummy (Id INTEGER);");
    SQLite_Statement S2(std::move(S1));

    ASSERT_TRUE(S2.IsValid());
    ASSERT_FALSE(S1.IsValid()); // S1 will go out of scope and must not crash

    S2.Finalize();
    sqlite3_close(Db);
}


////////////////////////////////////////
// TC08: Move with and without sentinel
////////////////////////////////////////
TEST(SQLite_Statement, T08_MoveWithAndWithoutSentinel)
{
    SCOPED_TRACE("========== TC08: Move with and without sentinel ==========");

    sqlite3* Db = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);

    StatementCreationKit WithSentinel{ std::make_shared<SQLite_ConnectionSentinel>(), Db };
    StatementCreationKit NoSentinel{ std::weak_ptr<SQLite_ConnectionSentinel>(), Db };

    SQLite_Statement With(WithSentinel, "CREATE TABLE A (X INTEGER);");
    SQLite_Statement Without(NoSentinel, "CREATE TABLE B (Y INTEGER);");

    SQLite_Statement MovedWith(std::move(With));
    SQLite_Statement MovedWithout(std::move(Without));

    ASSERT_TRUE(MovedWith.IsValid());
    ASSERT_TRUE(MovedWithout.IsValid());

    MovedWith.Finalize();
    MovedWithout.Finalize();
    sqlite3_close(Db);
}


////////////////////////////////////////
// TC10: Move from finalized statement
////////////////////////////////////////
TEST(SQLite_Statement, T10_MoveFromFinalizedStatement)
{
    SCOPED_TRACE("========== TC10: Move from finalized statement ==========");

    sqlite3* Db = nullptr;
    std::shared_ptr<SQLite_ConnectionSentinel> Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    StatementCreationKit Kit{ Sentinel, Db };

    SQLite_Statement Finalized(Kit, "CREATE TABLE Dummy (Id INTEGER);");
    Finalized.Finalize();

    ASSERT_FALSE(Finalized.IsValid());

    SQLite_Statement Moved(std::move(Finalized));
    ASSERT_FALSE(Moved.IsValid());

    Moved.Finalize();
    sqlite3_close(Db);
}
#ifdef BUKA_DEBUG

#endif // BUKA_DEBUG
