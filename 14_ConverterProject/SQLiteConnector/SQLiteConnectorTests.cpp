#include <format>
#include <iostream>
#include <cassert>
#include <filesystem>
////////
#include <sqlite3.h>
#include <gtest/gtest.h>
////////
#include "../ConditionChecker/ConditionChecker.h"
#include "SQLiteConnector.h"

// #define EXAMINE_MEMORY_LEAK_WITHOUT_ConnectionSentinel

//////////////////////////////////////////////
//          Unified Test Container
//////////////////////////////////////////////

TEST(SQLiteConnector, DbChecker)
{
    const auto CreateInMemoryDb = []()
    {
        sqlite3* Db = nullptr;
        EXPECT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
        sqlite3_extended_result_codes(Db, 1); // Enable extended error codes
        return Db;
    };

    //////////////////////////////////////////////
    //              Normal Cases
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with SQLITE_OK should succeed");

        sqlite3* Db = CreateInMemoryDb();
        ASSERT_NO_FATAL_FAILURE(DbChecker::CheckResult(Db, SQLITE_OK, "no-op"));
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //              Edge Cases
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with long operation name (format + utf8)");

        sqlite3* Db = CreateInMemoryDb();

        // Force an error
        const char* InvalidSQL = "INVALID SQL!";
        sqlite3_exec(Db, InvalidSQL, nullptr, nullptr, nullptr);

        ASSERT_DEATH(DbChecker::CheckResult(Db, SQLITE_ERROR, "execute_invalid_sql_with_long_name"), ".*");
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //             Invalid Inputs
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with nullptr as operation type");

        sqlite3* Db = CreateInMemoryDb();

        sqlite3_exec(Db, "BAD;", nullptr, nullptr, nullptr);

        const char* Op = nullptr;
        ASSERT_DEATH(DbChecker::CheckResult(Db, SQLITE_ERROR, Op), ".*");
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //             Empty Structures
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with empty string as operation type");

        sqlite3* Db = CreateInMemoryDb();

        sqlite3_exec(Db, "INVALID!", nullptr, nullptr, nullptr);

        ASSERT_DEATH(DbChecker::CheckResult(Db, SQLITE_ERROR, ""), ".*");
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //        Maximums and Minimums
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with INT_MAX as result code");

        sqlite3* Db = CreateInMemoryDb();

        ASSERT_DEATH(DbChecker::CheckResult(Db, INT_MAX, "overflow"), ".*");
        sqlite3_close(Db);
    }

    {
        SCOPED_TRACE("CheckResult with INT_MIN as result code");

        sqlite3* Db = CreateInMemoryDb();

        ASSERT_DEATH(DbChecker::CheckResult(Db, INT_MIN, "underflow"), ".*");
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //         Extended Result Code Cases
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with extended error code (e.g. SQLITE_IOERR)");

        sqlite3* Db = nullptr;
        // Force failure: opening invalid path
        int rc = sqlite3_open("/invalid/path/to.db", &Db);
        ASSERT_NE(rc, SQLITE_OK);
        sqlite3_extended_result_codes(Db, 1);

        ASSERT_DEATH(DbChecker::CheckResult(Db, rc, "open_invalid_path"), ".*");
        sqlite3_close(Db);
    }

    {
        SCOPED_TRACE("CheckResult with SQLITE_CONSTRAINT_NOTNULL (extended)");

        sqlite3* Db = CreateInMemoryDb();

        const char* Schema = "CREATE TABLE T (Id INTEGER PRIMARY KEY, Name TEXT NOT NULL);";
        const char* InsertFail = "INSERT INTO T (Id) VALUES (1);";

        sqlite3_exec(Db, Schema, nullptr, nullptr, nullptr);
        sqlite3_exec(Db, InsertFail, nullptr, nullptr, nullptr); // Should fail on NOT NULL

        ASSERT_DEATH(DbChecker::CheckResult(Db, SQLITE_CONSTRAINT, "insert_violate_notnull"), ".*");
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //              WarnIfFalse Cases
    //////////////////////////////////////////////

    {
        SCOPED_TRACE("WarnIfFalse: Condition is true (no output expected)");
        std::stringstream buffer;
        auto* oldBuf = std::cerr.rdbuf(buffer.rdbuf());

        DbChecker::WarnIfFalse(true, "Should not warn");

        std::cerr.rdbuf(oldBuf);
        const std::string output = buffer.str();
        EXPECT_EQ(output, "");
    }

    {
        SCOPED_TRACE("WarnIfFalse: Condition is false with valid message");
        std::stringstream buffer;
        auto* oldBuf = std::cerr.rdbuf(buffer.rdbuf());

        DbChecker::WarnIfFalse(false, "Expected warning output");

        std::cerr.rdbuf(oldBuf);
        const std::string output = buffer.str();
        EXPECT_EQ(output, "Warning: Expected warning output\n");
    }

    {
        SCOPED_TRACE("WarnIfFalse: Empty string as warning message");
        std::stringstream buffer;
        auto* oldBuf = std::cerr.rdbuf(buffer.rdbuf());

        DbChecker::WarnIfFalse(false, "");

        std::cerr.rdbuf(oldBuf);
        const std::string output = buffer.str();
        EXPECT_EQ(output, "Warning: \n");
    }

    {
        SCOPED_TRACE("WarnIfFalse: Warning message is nullptr (must crash due to CONFIRM)");
        const char* msg = nullptr;
        ASSERT_DEATH({
            DbChecker::WarnIfFalse(false, msg);
        }, ".*");
    }
}

/////////////////////////////////

/////////////////////////////////


TEST(SQLiteConnector, DbConnection)
{
    static constexpr const auto CreateTempTestDb = []()->const char*
    {
        static const char* path = "temp_test.db";

        // If file exists, delete it for a clean state
        if (std::filesystem::exists(path))
            std::filesystem::remove(path);

        sqlite3* db = nullptr;
        if (sqlite3_open(path, &db) != SQLITE_OK)
        {
            std::cerr << "Failed to create temporary test database." << std::endl;
            std::terminate();
        }

        // Optional: Create a dummy table
        sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Dummy (Id INTEGER);", nullptr, nullptr, nullptr);

        sqlite3_close(db);
        return path;
    };

    ///////////////////////////////
    // Normal cases
    ///////////////////////////////

    {
        SCOPED_TRACE("Normal: Open connection in read-write mode");
        EXPECT_NO_THROW({
            DbConnection conn(CreateTempTestDb(), false);
            sqlite3* handle = conn.Get();
            EXPECT_NE(handle, nullptr);
        });
        std::filesystem::remove("temp_test.db");
    }

    {
        SCOPED_TRACE("Normal: Open connection in read-only mode");
        EXPECT_NO_THROW({
            DbConnection conn(CreateTempTestDb(), true);
            sqlite3* handle = conn.Get();
            EXPECT_NE(handle, nullptr);
        });
        std::filesystem::remove("temp_test.db");
    }

    {
        SCOPED_TRACE("Normal: Validate that returned sqlite3* is usable");
        DbConnection conn(CreateTempTestDb(), false);
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(conn.Get(), "SELECT 1", -1, &stmt, nullptr);
        EXPECT_EQ(rc, SQLITE_OK);
        sqlite3_finalize(stmt);
        std::filesystem::remove("temp_test.db");
    }

    ///////////////////////////////
    // Edge cases
    ///////////////////////////////

    {
        SCOPED_TRACE("Edge: Open multiple simultaneous connections");
        const char* PathToDb = CreateTempTestDb();
        std::vector<std::unique_ptr<DbConnection>> pool;
        EXPECT_NO_THROW({
            for (int i = 0; i < 10; ++i)
                pool.emplace_back(std::make_unique<DbConnection>(PathToDb, false));
        });
        std::filesystem::remove("temp_test.db");
    }

    {
        SCOPED_TRACE("Edge: Multithreaded access to separate connections");
        std::vector<std::thread> threads;
        EXPECT_NO_THROW({
            for (int i = 0; i < 5; ++i)
            {
                threads.emplace_back([] {
                    DbConnection conn(CreateTempTestDb(), false);
                    sqlite3_stmt* stmt = nullptr;
                    int rc = sqlite3_prepare_v2(conn.Get(), "SELECT 1", -1, &stmt, nullptr);
                    EXPECT_EQ(rc, SQLITE_OK);
                    sqlite3_finalize(stmt);
                });
            }
            for (auto& t : threads) t.join();
        });
        std::filesystem::remove("temp_test.db");
    }

    ///////////////////////////////
    // Invalid inputs
    ///////////////////////////////

    {
        SCOPED_TRACE("Invalid: Pass nullptr as DB path");
        EXPECT_DEATH({
            DbConnection conn(nullptr, false);
        }, ".*");
    }

    {
        SCOPED_TRACE("Invalid: Pass non-existent DB path in read-only mode");
        EXPECT_DEATH({
            DbConnection conn("nonexistent.db", true);
        }, ".*");
    }

    ///////////////////////////////
    // Empty structures
    ///////////////////////////////

    {
        SCOPED_TRACE("Empty: Implicit close in destructor with no usage");
        {
            DbConnection conn(CreateTempTestDb(), false);
        }
        SUCCEED();  // Should destruct cleanly
        std::filesystem::remove("temp_test.db");
    }

    {
        SCOPED_TRACE("Empty: Ensure Get() never returns null after valid open");
        DbConnection conn(CreateTempTestDb(), false);
        EXPECT_NE(conn.Get(), nullptr);
        std::filesystem::remove("temp_test.db");
    }

    ///////////////////////////////
    // Maximums and minimums
    ///////////////////////////////

    {
        SCOPED_TRACE("Maximum: Open many connections (stress test)");
        const char* PathToDb = CreateTempTestDb();
        std::vector<std::unique_ptr<DbConnection>> conns;
        EXPECT_NO_THROW({
            for (int i = 0; i < 100; ++i)
                conns.emplace_back(std::make_unique<DbConnection>(PathToDb, false));
        });
        std::filesystem::remove("temp_test.db");
    }

    ///////////////////////////////
    // Result code checking
    ///////////////////////////////

    {
        SCOPED_TRACE("Check: Opening and closing report SQLITE_OK");
        class VerboseDbChecker : public DbChecker {
        public:
            using DbChecker::DbChecker;
            void CheckResult(int code, const char* context) {
                EXPECT_EQ(code, SQLITE_OK) << "Failure during: " << context;
            }
        };

        EXPECT_NO_THROW({
            DbConnection conn(CreateTempTestDb(), false);
        });
        std::filesystem::remove("temp_test.db");
    }

    {
        SCOPED_TRACE("Check: Confirm crash when DbHandle is null in Get()");

        class CorruptDbConnection : public DbConnection {
        public:
            CorruptDbConnection(const char* Path) : DbConnection(Path, false) {
                DbHandle = nullptr; // Simulate corruption
            }
        };

        EXPECT_DEATH({
            CorruptDbConnection BadConn(CreateTempTestDb());
            BadConn.Get(); // Should trigger CONFIRM
        }, ".*");
        std::filesystem::remove("temp_test.db");
    }

    ///////////////////////////////
    // Cleanup
    ///////////////////////////////

    std::filesystem::remove("temp_test.db");
}


////////////////////////////////////////////

///////////////////////////////////////////


TEST(SQLiteConnector, SQLiteStatement)
{
    // Helper to open an in-memory DB
    constexpr const auto OpenInMemoryDb = [](void(*StatementUse)(sqlite3* db))
    {
        sqlite3* Db = nullptr;
        int rc = sqlite3_open(":memory:", &Db);
        EXPECT_EQ(rc, SQLITE_OK);
        StatementUse(Db);
        sqlite3_close(Db);
    };

    /////////////// WarnOnUnusedTail: No Warning Expected ///////////////
    {
        SCOPED_TRACE("WarnOnUnusedTail with null and empty tail should issue no warning");
        const char* nullTail = nullptr;
        SQLiteStatement::WarnOnUnusedTail(nullTail); // Should silently return

        const char* emptyTail = "";
        SQLiteStatement::WarnOnUnusedTail(emptyTail); // Should silently return
    }

    /////////////// WarnOnUnusedTail: Warning Expected ///////////////
    {
        SCOPED_TRACE("WarnOnUnusedTail with non-empty tail should issue warning");
        const char* extra = "SELECT * FROM Table; extra";
        SQLiteStatement::WarnOnUnusedTail(extra); // Should trigger warning
    }

    /////////////// PrepareStatement: Valid Statement ///////////////
    {
        SCOPED_TRACE("PrepareStatement should initialize StatementPtr correctly");
        const auto StatementUse = [](sqlite3* db) {
            const char* sql = "CREATE TABLE Test (Id INTEGER);";
            SQLiteStatement stmt(db, sql);
            ASSERT_NE(stmt.Get(), nullptr);
        };
        OpenInMemoryDb(StatementUse);
    }

    /////////////// Constructor: Valid Inputs ///////////////
    {
        SCOPED_TRACE("Constructor initializes DbHandle, Checker, and prepares statement");
        const auto StatementUse = [](sqlite3* db) {
            const char* sql = "CREATE TABLE Person (Name TEXT);";
            SQLiteStatement stmt(db, sql);
            ASSERT_NE(stmt.Get(), nullptr); // Verifies StatementPtr is initialized
        };
        OpenInMemoryDb(StatementUse);
    }


    /////////////// Destructor: Statement Finalization ///////////////
    {
        SCOPED_TRACE("Destructor finalizes the statement without exception");
        const auto StatementUse = [](sqlite3* db) {
            const char* sql = "CREATE TABLE Thing (Value TEXT);";
            {
                SQLiteStatement stmt(db, sql);
            } // Destructor should finalize statement
        };
        OpenInMemoryDb(StatementUse);
    }

    /////////////// Get: Returns Correct StatementPtr ///////////////
    {
        SCOPED_TRACE("Get should return initialized StatementPtr");
        const auto StatementUse = [](sqlite3* db) {
            const char* sql = "CREATE TABLE Animal (Type TEXT);";
            SQLiteStatement stmt(db, sql);
            ASSERT_EQ(stmt.Get(), stmt.Get()); // Identity check
        };
        OpenInMemoryDb(StatementUse);
    }

    /////////////// Step: SQLITE_DONE -> false ///////////////
    {
        SCOPED_TRACE("Step should return false when execution is complete (SQLITE_DONE)");
        const auto StatementUse = [](sqlite3* db) {
            sqlite3_exec(db, "CREATE TABLE Person(Id INTEGER); INSERT INTO Person VALUES (1);", nullptr, nullptr, nullptr);
            SQLiteStatement stmt(db, "DELETE FROM Person WHERE Id = 2;");
            bool hasRow = stmt.Step();
            ASSERT_FALSE(hasRow);
        };
        OpenInMemoryDb(StatementUse);
    }

    /////////////// Step: SQLITE_ROW -> true ///////////////
    {
        SCOPED_TRACE("Step should return true when a row is available (SQLITE_ROW)");
        const auto StatementUse = [](sqlite3* db) {
            sqlite3_exec(db, "CREATE TABLE Person(Id INTEGER); INSERT INTO Person VALUES (42);", nullptr, nullptr, nullptr);
            SQLiteStatement stmt(db, "SELECT * FROM Person;");
            bool hasRow = stmt.Step();
            ASSERT_TRUE(hasRow);
        };
        OpenInMemoryDb(StatementUse);
    }

    /////////////// Step: Unexpected Return -> Triggers CheckResult ///////////////
    {
        SCOPED_TRACE("Step on corrupted statement should trigger Checker");
        const auto StatementUse = [](sqlite3* db) {
            EXPECT_DEATH({
                SQLiteStatement stmt(db, "SELECT nonexistent FROM missing;");
            }, ".*");
        };
        OpenInMemoryDb(StatementUse);
    }

    /////////////// Reset: Valid Statement ///////////////
    {
        SCOPED_TRACE("Reset should successfully reset valid statement");
        const auto StatementUse = [](sqlite3* db) {
            sqlite3_exec(db, "CREATE TABLE Demo(Id INTEGER); INSERT INTO Demo VALUES (1);", nullptr, nullptr, nullptr);
            SQLiteStatement stmt(db, "SELECT * FROM Demo;");
            stmt.Step();  // First read
            stmt.Reset(); // Reset to beginning
            stmt.Step();  // Second read
        };
        OpenInMemoryDb(StatementUse);
    }

    ///////////////// Prepare: Malformed SQL should abort //////////////////
    {
        SCOPED_TRACE("Malformed SQL triggers failure in CheckResult during statement preparation");
        const auto StatementUse = [](sqlite3* db) {
            EXPECT_DEATH({
                SQLiteStatement stmt(db, "SELECT FROM");
            }, ".*");
        };
        OpenInMemoryDb(StatementUse);
    }

    /////////////// Step and Reset: Multiple Execution ///////////////
    {
        SCOPED_TRACE("Step and Reset can be called multiple times correctly");
        const auto StatementUse = [](sqlite3* db) {
            sqlite3_exec(db, "CREATE TABLE Repeat(Id INTEGER); INSERT INTO Repeat VALUES (10);", nullptr, nullptr, nullptr);
            SQLiteStatement stmt(db, "SELECT * FROM Repeat;");
            ASSERT_TRUE(stmt.Step());
            stmt.Reset();
            ASSERT_TRUE(stmt.Step());
        };
        OpenInMemoryDb(StatementUse);
    }
}

//////////////////////////////
//          Connection Sentinel
//////////////////////////////

class SQLiteConnector_ConnectionSentinel : public ::testing::Test
{
protected:
    const char* TestDbPath = "test.db";
    void SetUp() override
    {
        // Ensure clean test DB file is created (in-memory DB is also an option)
        sqlite3* Db;
        sqlite3_open(TestDbPath, &Db);
        sqlite3_exec(Db, "CREATE TABLE IF NOT EXISTS dummy (id INTEGER);", nullptr, nullptr, nullptr);
        sqlite3_close(Db);
    }
    ~SQLiteConnector_ConnectionSentinel()
    {
        std::filesystem::remove("test.db");
    }
};

TEST_F(SQLiteConnector_ConnectionSentinel, Cases)
{
    //////////////////// TC1: Basic Independent Destruction Without Sentinel ////////////////////
    {
        SCOPED_TRACE("TC1: Independent destruction without sentinel.");
        DbConnection Db(TestDbPath, false, false);
        SQLiteStatement Stmt(Db.Get(), "SELECT 1;");
        EXPECT_NE(Stmt.Get(), nullptr);
    }

    //////////////////// TC2: Automatic Invalidation When DbConnection Is Destroyed First ////////////////////
    {
        SCOPED_TRACE("TC2: Sentinel invalidates statements when DbConnection is destroyed first.");
        SQLiteStatement* StatementPtr = nullptr;

        {
            DbConnection Db(TestDbPath, false, true);
            StatementCreationKit Kit = Db.MakeStatementKit();
            StatementPtr = new SQLiteStatement(Kit, "SELECT 1;");
        }

        // DbConnection is destroyed here, invalidating statement.
        StatementPtr->Step();  // Should not crash even if StatementPtr is already finalized.
        delete StatementPtr;
    }

    //////////////////// TC3: Early Destruction of SQLiteStatement ////////////////////
    {
        SCOPED_TRACE("TC3: Statement destroyed before DbConnection, unregisters cleanly.");
        DbConnection Db(TestDbPath, false, true);
        StatementCreationKit Kit = Db.MakeStatementKit();
        {
            SQLiteStatement Stmt1(Kit, "SELECT 1;");
            SQLiteStatement Stmt2(Kit, "SELECT 2;");
        }  // Stmt1 and Stmt2 destroyed here

        // Should not cause issues during DbConnection destruction
    }

    //////////////////// TC4: Statement Created Without Sentinel ////////////////////
    {
        SCOPED_TRACE("TC4: Statement with nullptr sentinel behaves safely.");
        sqlite3* RawDb = nullptr;
        sqlite3_open(TestDbPath, &RawDb);

        StatementCreationKit Kit{ std::weak_ptr<ConnectionSentinel>{}, RawDb };
        {
            SQLiteStatement Stmt(Kit, "SELECT 1;");
            EXPECT_NE(Stmt.Get(), nullptr);
        }

        sqlite3_close(RawDb);
    }

    //////////////////// TC5: Idempotency of FinalizeFromSentinel ////////////////////
    {
        SCOPED_TRACE("TC5: FinalizeFromSentinel can be safely called multiple times.");
        DbConnection Db(TestDbPath, false, true);
        StatementCreationKit Kit = Db.MakeStatementKit();
        SQLiteStatement Stmt(Kit, "SELECT 1;");
        Stmt.FinalizeFromSentinel();
        Stmt.FinalizeFromSentinel();  // Should be safe
    }

    //////////////////// TC6: Thread Safety Under Concurrent Access ////////////////////
    {
        SCOPED_TRACE("TC6: Concurrent registration and invalidation do not cause race conditions.");
        std::vector<std::thread> Threads;
        std::shared_ptr<ConnectionSentinel> SharedSentinel = std::make_shared<ConnectionSentinel>();

        for (int i = 0; i < 10; ++i)
        {
            Threads.emplace_back([SharedSentinel, this]() {
                sqlite3* DbHandle = nullptr;
                sqlite3_open(TestDbPath, &DbHandle);

                 // Delay overlaps with InvalidateAll
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                StatementCreationKit Kit{ SharedSentinel, DbHandle };

                // SQLiteStatement must be destroyed before DbHandle, to avoid memory leak
                {
                    SQLiteStatement Stmt(Kit, "SELECT 1;");
                }

                sqlite3_close(DbHandle);
            });
        }

        SharedSentinel->InvalidateAll();

        for (auto& Thread : Threads)
        {
            Thread.join();
        }
    }
}

#ifdef EXAMINE_MEMORY_LEAK_WITHOUT_ConnectionSentinel
TEST(MemoryLeakTest, WithoutConnectionSentinel_1)
{
    //////////////////// TC7: Memory Leak Detection Without Sentinel ////////////////////
    {
        SCOPED_TRACE("TC7: Statement created without sentinel is not invalidated automatically.");
        sqlite3* DbHandle = nullptr;
        sqlite3_open(":memory:", &DbHandle);
        // We deliberately omit finalization before closing DB to simulate leak
        SQLiteStatement* Stmt = new SQLiteStatement(DbHandle, "SELECT 1;");
        sqlite3_close(DbHandle);  // Finalize will now do nothing, likely leaking memory
        delete Stmt;              // Finalize should fail silently
    }
}
#endif // EXAMINE_MEMORY_LEAK_WITHOUT_ConnectionSentinel