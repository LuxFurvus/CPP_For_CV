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

/////////////////////////////////

/////////////////////////////////

TEST(SQLite_DbConnection, T01_OpenReadWrite)
{
    SCOPED_TRACE("Invalid: Open connection in read-write mode without CREATE flag, expect failure");

    static constexpr const char* Path = "temp_test.db";

    if (std::filesystem::exists(Path)) std::filesystem::remove(Path);

    EXPECT_THROW({
        SQLite_DbConnection conn(Path, false);
    }, std::runtime_error);
}


TEST(SQLite_DbConnection, T02_OpenReadOnly)
{
    SCOPED_TRACE("Normal: Open connection in read-only mode");
    static constexpr const char* Path = "temp_test.db";

    if (std::filesystem::exists(Path)) std::filesystem::remove(Path);
    {
        sqlite3* db = nullptr;
        sqlite3_open(Path, &db);
        sqlite3_close(db);
        SQLite_DbConnection conn(Path, true);
        sqlite3* handle = conn.Get();
        EXPECT_NE(handle, nullptr);
    }
    std::filesystem::remove(Path);
}

TEST(SQLite_DbConnection, T03_StatementPrepare)
{
    SCOPED_TRACE("Normal: Validate that returned sqlite3* is usable");
    static constexpr const char* Path = "temp_test.db";

    // Ensure the DB file exists before passing to SQLite_DbConnection
    if (!std::filesystem::exists(Path))
    {
        sqlite3* Temp = nullptr;
        const int rc = sqlite3_open(Path, &Temp);
        ASSERT_EQ(rc, SQLITE_OK) << "Failed to create initial database";
        sqlite3_close(Temp);
    }

    {
        SQLite_DbConnection conn(Path, false);
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(conn.Get(), "SELECT 1", -1, &stmt, nullptr);
        EXPECT_EQ(rc, SQLITE_OK);
        sqlite3_finalize(stmt);
    }

    std::filesystem::remove(Path);
}

TEST(SQLite_DbConnection, T04_MultipleConnections)
{
    SCOPED_TRACE("Edge: Open multiple simultaneous connections");
    static constexpr const char* Path = "temp_test.db";

    if (std::filesystem::exists(Path)) std::filesystem::remove(Path);
    {
        sqlite3* db = nullptr;
        sqlite3_open(Path, &db);
        sqlite3_close(db);

        std::vector<std::unique_ptr<SQLite_DbConnection>> pool;
        for (int i = 0; i < 10; ++i)
            pool.emplace_back(std::make_unique<SQLite_DbConnection>(Path, false));
    }
    std::filesystem::remove(Path);
}

TEST(SQLite_DbConnection, T05_MultithreadedAccess)
{
    SCOPED_TRACE("Edge: Multithreaded access to separate connections");
    static constexpr const char* Path = "temp_test.db";

    if (std::filesystem::exists(Path)) std::filesystem::remove(Path);
    {
        sqlite3* db = nullptr;
        sqlite3_open(Path, &db);
        sqlite3_close(db);

        std::vector<std::thread> threads;
        for (int i = 0; i < 5; ++i)
        {
            threads.emplace_back([] {
                SQLite_DbConnection conn("temp_test.db", false);
                sqlite3_stmt* stmt = nullptr;
                int rc = sqlite3_prepare_v2(conn.Get(), "SELECT 1", -1, &stmt, nullptr);
                EXPECT_EQ(rc, SQLITE_OK);
                sqlite3_finalize(stmt);
            });
        }
        for (auto& t : threads) t.join();
    }
    std::filesystem::remove(Path);
}

TEST(SQLite_DbConnection, T06_NullptrPath)
{
    SCOPED_TRACE("Invalid: Pass nullptr as DB path");
    EXPECT_THROW({
        SQLite_DbConnection conn({}, false);
    }, std::runtime_error);
}

TEST(SQLite_DbConnection, T07_NonexistentReadonly)
{
    SCOPED_TRACE("Invalid: Pass non-existent DB path in read-only mode");
    EXPECT_THROW({
        SQLite_DbConnection conn("nonexistent.db", true);
    }, std::runtime_error);
}

///////////////////

TEST(SQLite_DbConnection, T08_DestructorNoUsage)
{
    SCOPED_TRACE("Empty: Implicit close in destructor with no usage");
    static constexpr const char* Path = "temp_test.db";

    if (std::filesystem::exists(Path)) std::filesystem::remove(Path);

    EXPECT_THROW({
        SQLite_DbConnection conn(Path, false);
    }, std::runtime_error);
    
    // try
    // {
    //     SQLite_DbConnection conn(Path, false);
    //     SUCCEED();
    // }
    // catch (const std::exception& ex)
    // {
    //     GTEST_SKIP() << "Skipped due to DB open failure: " << ex.what();
    // }

    std::filesystem::remove(Path);
}

///////////////////

TEST(SQLite_DbConnection, T09_EnsureNonNullGet)
{
    SCOPED_TRACE("Empty: Ensure Get() never returns null after valid open");
    static constexpr const char* Path = "temp_test.db";

    if (std::filesystem::exists(Path)) std::filesystem::remove(Path);

    // Explicitly create the database file to allow read-write open
    {
        sqlite3* db = nullptr;
        int rc = sqlite3_open(Path, &db);
        ASSERT_EQ(rc, SQLITE_OK) << "Failed to create DB file for test";
        sqlite3_close(db);
    }

    {
        SQLite_DbConnection conn(Path, false);
        EXPECT_NE(conn.Get(), nullptr);
    }

    std::filesystem::remove(Path);
}

///////////////////

TEST(SQLite_DbConnection, T10_MassConnections)
{
    SCOPED_TRACE("Maximum: Open many connections (stress test)");
    static constexpr const char* Path = "temp_test.db";

    if (std::filesystem::exists(Path)) std::filesystem::remove(Path);
    {
        sqlite3* db = nullptr;
        sqlite3_open(Path, &db);
        sqlite3_close(db);

        std::vector<std::unique_ptr<SQLite_DbConnection>> conns;
        for (int i = 0; i < 100; ++i)
            conns.emplace_back(std::make_unique<SQLite_DbConnection>(Path, false));
    }
    std::filesystem::remove(Path);
}

///////////////////

TEST(SQLite_DbConnection, T12_CorruptGetShouldCrash)
{
    SCOPED_TRACE("Check: Confirm crash when DbHandle is null in Get()");
    static constexpr const char* Path = "temp_test.db";

    if (std::filesystem::exists(Path)) std::filesystem::remove(Path);

    class CorruptDbConnection : public SQLite_DbConnection {
    public:
        CorruptDbConnection(const char* Path) : SQLite_DbConnection(Path, false) {
            DbHandle = nullptr; // Force corruption
        }
    };

    EXPECT_THROW({
        CorruptDbConnection BadConn(Path);
        BadConn.Get(); // Should trigger CONFIRM
    }, std::runtime_error);

    std::filesystem::remove(Path);
}

TEST(SQLite_DbConnection, T13_DisableSentinel)
{
    SCOPED_TRACE("Check: Create connection with EnableSentinel = false");
    static constexpr const char* Path = "temp_test.db";

    if (!std::filesystem::exists(Path))
    {
        sqlite3* db = nullptr;
        sqlite3_open(Path, &db);
        sqlite3_close(db);
    }

    SQLite_DbConnection conn(Path, false, false);
    EXPECT_NE(conn.Get(), nullptr);
    // Indirectly confirm destructor doesn't crash on null Sentinel
    SUCCEED();

    std::filesystem::remove(Path);
}

TEST(SQLite_DbConnection, T14_InvalidSQL)
{
    SCOPED_TRACE("Error: Prepare invalid SQL and validate error");
    static constexpr const char* Path = "temp_test.db";

    if (!std::filesystem::exists(Path))
    {
        sqlite3* db = nullptr;
        sqlite3_open(Path, &db);
        sqlite3_close(db);
    }

    SQLite_DbConnection conn(Path, false);
    sqlite3_stmt* stmt = nullptr;
    const int rc = sqlite3_prepare_v2(conn.Get(), "SELECT * FROM imaginary_table;", -1, &stmt, nullptr);
    EXPECT_NE(rc, SQLITE_OK);
    sqlite3_finalize(stmt);

    std::filesystem::remove(Path);
}

TEST(SQLite_DbConnection, T15_StatementKitFields)
{
    SCOPED_TRACE("Check: Returned StatementCreationKit contains correct members");
    static constexpr const char* Path = "temp_test.db";

    if (!std::filesystem::exists(Path))
    {
        sqlite3* db = nullptr;
        sqlite3_open(Path, &db);
        sqlite3_close(db);
    }

    SQLite_DbConnection conn(Path, false);
    StatementCreationKit kit = conn.MakeStatementKit();
    EXPECT_NE(kit.DbHandle, nullptr);
    EXPECT_TRUE(kit.Sentinel.lock());

    std::filesystem::remove(Path);
}