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

sqlite3* CreateInMemoryDb()
{
    sqlite3* Db = nullptr;
    EXPECT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    sqlite3_extended_result_codes(Db, 1); // Enable extended error codes
    return Db;
}

//////////////////////////////////////////////
//          Unified Test Container
//////////////////////////////////////////////

TEST(SQLiteConnector, DbChecker)
{
    //////////////////////////////////////////////
    //              Normal Cases
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with SQLITE_OK should succeed");

        sqlite3* Db = CreateInMemoryDb();
        DbChecker Checker(Db);
        ASSERT_NO_FATAL_FAILURE(Checker.CheckResult(SQLITE_OK, "no-op"));
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //              Edge Cases
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with long operation name (format + utf8)");

        sqlite3* Db = CreateInMemoryDb();
        DbChecker Checker(Db);

        // Force an error
        const char* InvalidSQL = "INVALID SQL!";
        sqlite3_exec(Db, InvalidSQL, nullptr, nullptr, nullptr);

        ASSERT_DEATH(Checker.CheckResult(SQLITE_ERROR, "execute_invalid_sql_with_long_name"), ".*");
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //             Invalid Inputs
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with nullptr as operation type");

        sqlite3* Db = CreateInMemoryDb();
        DbChecker Checker(Db);

        sqlite3_exec(Db, "BAD;", nullptr, nullptr, nullptr);

        const char* Op = nullptr;
        ASSERT_DEATH(Checker.CheckResult(SQLITE_ERROR, Op), ".*");
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //             Empty Structures
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with empty string as operation type");

        sqlite3* Db = CreateInMemoryDb();
        DbChecker Checker(Db);

        sqlite3_exec(Db, "INVALID!", nullptr, nullptr, nullptr);

        ASSERT_DEATH(Checker.CheckResult(SQLITE_ERROR, ""), ".*");
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //        Maximums and Minimums
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with INT_MAX as result code");

        sqlite3* Db = CreateInMemoryDb();
        DbChecker Checker(Db);

        ASSERT_DEATH(Checker.CheckResult(INT_MAX, "overflow"), ".*");
        sqlite3_close(Db);
    }

    {
        SCOPED_TRACE("CheckResult with INT_MIN as result code");

        sqlite3* Db = CreateInMemoryDb();
        DbChecker Checker(Db);

        ASSERT_DEATH(Checker.CheckResult(INT_MIN, "underflow"), ".*");
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
        DbChecker Checker(Db);

        ASSERT_DEATH(Checker.CheckResult(rc, "open_invalid_path"), ".*");
        sqlite3_close(Db);
    }

    {
        SCOPED_TRACE("CheckResult with SQLITE_CONSTRAINT_NOTNULL (extended)");

        sqlite3* Db = CreateInMemoryDb();
        DbChecker Checker(Db);

        const char* Schema = "CREATE TABLE T (Id INTEGER PRIMARY KEY, Name TEXT NOT NULL);";
        const char* InsertFail = "INSERT INTO T (Id) VALUES (1);";

        sqlite3_exec(Db, Schema, nullptr, nullptr, nullptr);
        sqlite3_exec(Db, InsertFail, nullptr, nullptr, nullptr); // Should fail on NOT NULL

        ASSERT_DEATH(Checker.CheckResult(SQLITE_CONSTRAINT, "insert_violate_notnull"), ".*");
        sqlite3_close(Db);
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
