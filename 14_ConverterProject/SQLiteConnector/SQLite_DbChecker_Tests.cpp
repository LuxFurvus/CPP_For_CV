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


TEST(SQLite_DbChecker, CheckResult_OK) {
    sqlite3* db = nullptr;
    EXPECT_NO_THROW(SQLite_DbChecker::CheckResult(db, SQLITE_OK, "test"));
}

TEST(SQLite_DbChecker, WarnIfFalse_False) {
    testing::internal::CaptureStderr();
    SQLite_DbChecker::WarnIfFalse(false, "test");
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_NE(output.find("Warning: test"), std::string::npos);
}

TEST(SQLite_DbChecker, WarnIfFalse_True) {
    EXPECT_NO_THROW(SQLite_DbChecker::WarnIfFalse(true, "test"));
}

TEST(SQLiteConnector, SQLite_DbChecker)
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
        ASSERT_NO_FATAL_FAILURE(SQLite_DbChecker::CheckResult(Db, SQLITE_OK, "no-op"));
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

        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, SQLITE_ERROR, "execute_invalid_sql_with_long_name"), std::runtime_error);
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
        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, SQLITE_ERROR, Op), std::runtime_error);
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //             Empty Structures
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with empty string as operation type");

        sqlite3* Db = CreateInMemoryDb();

        sqlite3_exec(Db, "INVALID!", nullptr, nullptr, nullptr);

        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, SQLITE_ERROR, ""), std::runtime_error);
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //        Maximums and Minimums
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with INT_MAX as result code");

        sqlite3* Db = CreateInMemoryDb();

        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, INT_MAX, "overflow"), std::runtime_error);
        sqlite3_close(Db);
    }

    {
        SCOPED_TRACE("CheckResult with INT_MIN as result code");

        sqlite3* Db = CreateInMemoryDb();

        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, INT_MIN, "underflow"), std::runtime_error);
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

        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, rc, "open_invalid_path"), std::runtime_error);
        sqlite3_close(Db);
    }

    {
        SCOPED_TRACE("CheckResult with SQLITE_CONSTRAINT_NOTNULL (extended)");

        sqlite3* Db = CreateInMemoryDb();

        const char* Schema = "CREATE TABLE T (Id INTEGER PRIMARY KEY, Name TEXT NOT NULL);";
        const char* InsertFail = "INSERT INTO T (Id) VALUES (1);";

        sqlite3_exec(Db, Schema, nullptr, nullptr, nullptr);
        sqlite3_exec(Db, InsertFail, nullptr, nullptr, nullptr); // Should fail on NOT NULL

        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, SQLITE_CONSTRAINT, "insert_violate_notnull"), std::runtime_error);
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //              WarnIfFalse Cases
    //////////////////////////////////////////////

    {
        SCOPED_TRACE("WarnIfFalse: Condition is true (no output expected)");
        std::stringstream buffer;
        auto* oldBuf = std::cerr.rdbuf(buffer.rdbuf());

        SQLite_DbChecker::WarnIfFalse(true, "Should not warn");

        std::cerr.rdbuf(oldBuf);
        const std::string output = buffer.str();
        EXPECT_EQ(output, "");
    }

    {
        SCOPED_TRACE("WarnIfFalse: Condition is false with valid message");
        std::stringstream buffer;
        auto* oldBuf = std::cerr.rdbuf(buffer.rdbuf());

        SQLite_DbChecker::WarnIfFalse(false, "Expected warning output");

        std::cerr.rdbuf(oldBuf);
        const std::string output = buffer.str();
        EXPECT_EQ(output, "Warning: Expected warning output\n");
    }

    {
        SCOPED_TRACE("WarnIfFalse: Empty string as warning message");
        std::stringstream buffer;
        auto* oldBuf = std::cerr.rdbuf(buffer.rdbuf());

        SQLite_DbChecker::WarnIfFalse(false, "");

        std::cerr.rdbuf(oldBuf);
        const std::string output = buffer.str();
        EXPECT_EQ(output, "Warning: \n");
    }

    {
        SCOPED_TRACE("WarnIfFalse: Warning message is nullptr (must crash due to CONFIRM)");
        const char* msg = nullptr;
        EXPECT_THROW({
            SQLite_DbChecker::WarnIfFalse(false, msg);
        }, std::runtime_error);
    }
}

