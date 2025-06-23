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

////////////////////////////////////////////

///////////////////////////////////////////

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
        SQLite_DbConnection Db(TestDbPath, false, false);
        SQLite_Statement Stmt(Db.Get(), "SELECT 1;");
        EXPECT_NE(Stmt.Get(), nullptr);
    }

    //////////////////// TC2: Automatic Invalidation When SQLite_DbConnection Is Destroyed First ////////////////////
    {
        SCOPED_TRACE("TC2: Sentinel invalidates statements when SQLite_DbConnection is destroyed first.");
        SQLite_Statement* StatementPtr = nullptr;

        {
            SQLite_DbConnection Db(TestDbPath, false, true);
            StatementCreationKit Kit = Db.MakeStatementKit();
            StatementPtr = new SQLite_Statement(Kit, "SELECT 1;");
        }

        // SQLite_DbConnection is destroyed here, invalidating statement.
        StatementPtr->Step();  // Should not crash even if StatementPtr is already finalized.
        delete StatementPtr;
    }

    //////////////////// TC3: Early Destruction of SQLite_Statement ////////////////////
    {
        SCOPED_TRACE("TC3: Statement destroyed before SQLite_DbConnection, unregisters cleanly.");
        SQLite_DbConnection Db(TestDbPath, false, true);
        StatementCreationKit Kit = Db.MakeStatementKit();
        {
            SQLite_Statement Stmt1(Kit, "SELECT 1;");
            SQLite_Statement Stmt2(Kit, "SELECT 2;");
        }  // Stmt1 and Stmt2 destroyed here

        // Should not cause issues during SQLite_DbConnection destruction
    }

    //////////////////// TC4: Statement Created Without Sentinel ////////////////////
    {
        SCOPED_TRACE("TC4: Statement with nullptr sentinel behaves safely.");
        sqlite3* RawDb = nullptr;
        sqlite3_open(TestDbPath, &RawDb);

        StatementCreationKit Kit{ std::weak_ptr<ConnectionSentinel>{}, RawDb };
        {
            SQLite_Statement Stmt(Kit, "SELECT 1;");
            EXPECT_NE(Stmt.Get(), nullptr);
        }

        sqlite3_close(RawDb);
    }

    //////////////////// TC5: Idempotency of FinalizeFromSentinel ////////////////////
    {
        SCOPED_TRACE("TC5: FinalizeFromSentinel can be safely called multiple times.");
        SQLite_DbConnection Db(TestDbPath, false, true);
        StatementCreationKit Kit = Db.MakeStatementKit();
        SQLite_Statement Stmt(Kit, "SELECT 1;");
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

                // SQLite_Statement must be destroyed before DbHandle, to avoid memory leak
                {
                    SQLite_Statement Stmt(Kit, "SELECT 1;");
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
        SQLite_Statement* Stmt = new SQLite_Statement(DbHandle, "SELECT 1;");
        sqlite3_close(DbHandle);  // Finalize will now do nothing, likely leaking memory
        delete Stmt;              // Finalize should fail silently
    }
}
#endif // EXAMINE_MEMORY_LEAK_WITHOUT_ConnectionSentinel

///////////////////


#ifdef BUKA_BABUKA


///////////////////

TEST(SQLiteConnector, SQLiteParamVisitor)
{
    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    sqlite3_exec(Db, "CREATE TABLE Test (Val);", nullptr, nullptr, nullptr);

    auto PrepareStatement = [&](const char* Sql) -> sqlite3_stmt* {
        sqlite3_stmt* Stmt = nullptr;
        sqlite3_prepare_v2(Db, Sql, -1, &Stmt, nullptr);
        return Stmt;
    };

    auto ExecuteInsertAndReadBack = [&](sqlite3_stmt* Stmt, auto ColumnCheck) {
        ASSERT_EQ(sqlite3_step(Stmt), SQLITE_DONE);
        sqlite3_finalize(Stmt);

        sqlite3_stmt* ReadStmt = PrepareStatement("SELECT Val FROM Test;");
        ASSERT_EQ(sqlite3_step(ReadStmt), SQLITE_ROW);
        ColumnCheck(ReadStmt);
        sqlite3_finalize(ReadStmt);

        sqlite3_exec(Db, "DELETE FROM Test;", nullptr, nullptr, nullptr); // Clean up
    };

    ///// TC01: Binding a standard integer value (33) /////
    {
        SCOPED_TRACE("TC01: Binding an int (33) using sqlite3_bind_int64.");
        sqlite3_stmt* Stmt = PrepareStatement("INSERT INTO Test VALUES (?);");
        SQLiteParamVisitor Visitor(Stmt, 1);
        ASSERT_EQ(Visitor(33), SQLITE_OK);
        ExecuteInsertAndReadBack(Stmt, [](sqlite3_stmt* Row) {
            EXPECT_EQ(sqlite3_column_int64(Row, 0), 33);
        });
    }

    ///// TC02: Binding a double value (3.1415) /////
    {
        SCOPED_TRACE("TC02: Binding a double (3.1415) using sqlite3_bind_double.");
        sqlite3_stmt* Stmt = PrepareStatement("INSERT INTO Test VALUES (?);");
        SQLiteParamVisitor Visitor(Stmt, 1);
        ASSERT_EQ(Visitor(3.1415), SQLITE_OK);
        ExecuteInsertAndReadBack(Stmt, [](sqlite3_stmt* Row) {
            EXPECT_DOUBLE_EQ(sqlite3_column_double(Row, 0), 3.1415);
        });
    }

    ///// TC03: Binding a standard string ("hello") /////
    {
        SCOPED_TRACE("TC03: Binding a string (\"hello\") using sqlite3_bind_text.");
        sqlite3_stmt* Stmt = PrepareStatement("INSERT INTO Test VALUES (?);");
        SQLiteParamVisitor Visitor(Stmt, 1);
        ASSERT_EQ(Visitor(std::string("hello")), SQLITE_OK);
        ExecuteInsertAndReadBack(Stmt, [](sqlite3_stmt* Row) {
            EXPECT_STREQ(reinterpret_cast<const char*>(sqlite3_column_text(Row, 0)), "hello");
        });
    }

    ///// TC04: Binding a null value /////
    {
        SCOPED_TRACE("TC04: Binding nullptr using sqlite3_bind_null.");
        sqlite3_stmt* Stmt = PrepareStatement("INSERT INTO Test VALUES (?);");
        SQLiteParamVisitor Visitor(Stmt, 1);
        ASSERT_EQ(Visitor(nullptr), SQLITE_OK);
        ExecuteInsertAndReadBack(Stmt, [](sqlite3_stmt* Row) {
            EXPECT_EQ(sqlite3_column_type(Row, 0), SQLITE_NULL);
        });
    }

    ///// TC05: Binding integer boundary values (INT64_MAX) /////
    {
        SCOPED_TRACE("TC05: Binding INT64_MAX to check boundary handling.");
        sqlite3_stmt* Stmt = PrepareStatement("INSERT INTO Test VALUES (?);");
        SQLiteParamVisitor Visitor(Stmt, 1);
        ASSERT_EQ(Visitor(std::numeric_limits<int64_t>::max()), SQLITE_OK);
        ExecuteInsertAndReadBack(Stmt, [](sqlite3_stmt* Row) {
            EXPECT_EQ(sqlite3_column_int64(Row, 0), std::numeric_limits<int64_t>::max());
        });
    }

    ///// TC06: Binding double boundary value (DBL_MAX) /////
    {
        SCOPED_TRACE("TC06: Binding DBL_MAX to check floating-point boundary.");
        sqlite3_stmt* Stmt = PrepareStatement("INSERT INTO Test VALUES (?);");
        SQLiteParamVisitor Visitor(Stmt, 1);
        ASSERT_EQ(Visitor(std::numeric_limits<double>::max()), SQLITE_OK);
        ExecuteInsertAndReadBack(Stmt, [](sqlite3_stmt* Row) {
            EXPECT_DOUBLE_EQ(sqlite3_column_double(Row, 0), std::numeric_limits<double>::max());
        });
    }

    ///// TC07: Binding empty string /////
    {
        SCOPED_TRACE("TC07: Binding an empty string.");
        sqlite3_stmt* Stmt = PrepareStatement("INSERT INTO Test VALUES (?);");
        SQLiteParamVisitor Visitor(Stmt, 1);
        ASSERT_EQ(Visitor(std::string("")), SQLITE_OK);
        ExecuteInsertAndReadBack(Stmt, [](sqlite3_stmt* Row) {
            EXPECT_STREQ(reinterpret_cast<const char*>(sqlite3_column_text(Row, 0)), "");
        });
    }

    ///// TC08: Binding UTF-8 string /////
    {
        SCOPED_TRACE("TC08: Binding a UTF-8 string (\"Привет\").");
        sqlite3_stmt* Stmt = PrepareStatement("INSERT INTO Test VALUES (?);");
        SQLiteParamVisitor Visitor(Stmt, 1);
        ASSERT_EQ(Visitor(std::string("Привет")), SQLITE_OK);
        ExecuteInsertAndReadBack(Stmt, [](sqlite3_stmt* Row) {
            EXPECT_STREQ(reinterpret_cast<const char*>(sqlite3_column_text(Row, 0)), "Привет");
        });
    }

    ///// TC09: Binding a very large string (1MB) /////
    {
        SCOPED_TRACE("TC09: Binding a very large string (~1MB).");
        sqlite3_stmt* Stmt = PrepareStatement("INSERT INTO Test VALUES (?);");
        std::string LargeString(1024 * 1024, 'A');
        SQLiteParamVisitor Visitor(Stmt, 1);
        ASSERT_EQ(Visitor(LargeString), SQLITE_OK);
        ExecuteInsertAndReadBack(Stmt, [&](sqlite3_stmt* Row) {
            const char* Text = reinterpret_cast<const char*>(sqlite3_column_text(Row, 0));
            ASSERT_NE(Text, nullptr);
            EXPECT_EQ(std::string(Text).size(), LargeString.size());
        });
    }

    ///// TC10: Binding to invalid parameter index (0) /////
    {
        SCOPED_TRACE("TC10: Binding using index 0 (invalid, expected abort).");
        sqlite3_stmt* Stmt = PrepareStatement("INSERT INTO Test VALUES (?);");
        EXPECT_THROW({SQLiteParamVisitor Visitor(Stmt, 0);}, std::runtime_error);
        sqlite3_finalize(Stmt);
    }

    ///// TC11: Binding to a null (invalid) statement pointer /////
    {
        SCOPED_TRACE("TC11: Binding with a null sqlite3_stmt* (expected abort).");
        sqlite3_stmt* Stmt = nullptr;
        EXPECT_THROW({SQLiteParamVisitor Visitor(Stmt, 1);}, std::runtime_error);
        sqlite3_finalize(Stmt);
    }

    ///// TC12: Binding twice to same parameter (overwrite check) /////
    {
        SCOPED_TRACE("TC12: Binding twice to the same parameter (expect overwrite).");
        sqlite3_stmt* Stmt = PrepareStatement("INSERT INTO Test VALUES (?);");
        SQLiteParamVisitor Visitor(Stmt, 1);
        ASSERT_EQ(Visitor(std::string("initial")), SQLITE_OK);
        ASSERT_EQ(Visitor(std::string("overwritten")), SQLITE_OK);
        ExecuteInsertAndReadBack(Stmt, [](sqlite3_stmt* Row) {
            EXPECT_STREQ(reinterpret_cast<const char*>(sqlite3_column_text(Row, 0)), "overwritten");
        });
    }

    sqlite3_close(Db);
}




//-------------------------------------------------------------
// TEST(SQLiteConnector, SQLite_NamedParamBinder)
//-------------------------------------------------------------

TEST(SQLiteConnector, SQLite_NamedParamBinder)
{
    sqlite3* DbHandle = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DbHandle), SQLITE_OK);

    auto Prepare = [](sqlite3* Db, const char* Sql, sqlite3_stmt** StmtOut) -> void {
        ASSERT_EQ(sqlite3_prepare_v2(Db, Sql, -1, StmtOut, nullptr), SQLITE_OK);
    };

    auto Finalize = [](sqlite3_stmt* Stmt) -> void {
        ASSERT_EQ(sqlite3_finalize(Stmt), SQLITE_OK);
    };

    ////////////////////// TC01: Empty Params //////////////////////
    {
        SCOPED_TRACE("TC01: No parameters in SQL or bind vector.");
        sqlite3_stmt* Stmt = nullptr;
        Prepare(DbHandle, "SELECT 1;", &Stmt);

        std::vector<BindablePair> Params{};
        SQLite_NamedParamBinder::BindParamsByName(Stmt, Params);

        Finalize(Stmt);
    }

    ////////////////////// TC02: Exact Match //////////////////////
    {
        SCOPED_TRACE("TC02: One named parameter is correctly bound.");
        sqlite3_stmt* Stmt = nullptr;
        Prepare(DbHandle, "SELECT :a;", &Stmt);

        std::vector<BindablePair> Params{ {":a", int64_t(123)} };
        SQLite_NamedParamBinder::BindParamsByName(Stmt, Params);

        Finalize(Stmt);
    }

    ////////////////////// TC03: Missing Param //////////////////////
    {
        SCOPED_TRACE("TC03: SQL has more params than provided (should abort).");
        sqlite3_stmt* Stmt = nullptr;
        Prepare(DbHandle, "SELECT :a, :b;", &Stmt);

        std::vector<BindablePair> Params{ {":a", int64_t(1)} };
        EXPECT_THROW(SQLite_NamedParamBinder::BindParamsByName(Stmt, Params), std::runtime_error);

        Finalize(Stmt);
    }

    ////////////////////// TC04: Extra Param //////////////////////
    {
        SCOPED_TRACE("TC04: Bind vector has extra unused parameter (should be ignored).");
        sqlite3_stmt* Stmt = nullptr;
        Prepare(DbHandle, "SELECT :x;", &Stmt);

        const auto lambda = [Stmt]()
        {
            std::vector<BindablePair> Params{ {":x", int64_t(1)}, {":y", int64_t(2)} };
            SQLite_NamedParamBinder::BindParamsByName(Stmt, Params);
        };
        EXPECT_THROW({lambda();}, std::runtime_error);

        Finalize(Stmt);
    }

    ////////////////////// TC05: Incorrect Name Format //////////////////////
    {
        SCOPED_TRACE("TC05: Parameter name without colon prefix (should fail).");
        sqlite3_stmt* Stmt = nullptr;
        Prepare(DbHandle, "SELECT :a;", &Stmt);

        std::vector<BindablePair> Params{ {"a", int64_t(1)} };
        EXPECT_THROW(SQLite_NamedParamBinder::BindParamsByName(Stmt, Params), std::runtime_error);

        Finalize(Stmt);
    }

    ////////////////////// TC06: Null Value //////////////////////
    {
        SCOPED_TRACE("TC06: Binding nullptr_t to named parameter.");
        sqlite3_stmt* Stmt = nullptr;
        Prepare(DbHandle, "SELECT :a;", &Stmt);

        std::vector<BindablePair> Params{ {":a", nullptr} };
        SQLite_NamedParamBinder::BindParamsByName(Stmt, Params);

        Finalize(Stmt);
    }

    ////////////////////// TC07: Double Value //////////////////////
    {
        SCOPED_TRACE("TC07: Binding double value.");
        sqlite3_stmt* Stmt = nullptr;
        Prepare(DbHandle, "SELECT :a;", &Stmt);

        std::vector<BindablePair> Params{ {":a", 3.14} };
        SQLite_NamedParamBinder::BindParamsByName(Stmt, Params);

        Finalize(Stmt);
    }

    ////////////////////// TC08: String Value //////////////////////
    {
        SCOPED_TRACE("TC08: Binding std::string.");
        sqlite3_stmt* Stmt = nullptr;
        Prepare(DbHandle, "SELECT :a;", &Stmt);

        std::vector<BindablePair> Params{ {":a", std::string("test")} };
        SQLite_NamedParamBinder::BindParamsByName(Stmt, Params);

        Finalize(Stmt);
    }

    ////////////////////// TC09: Index Zero //////////////////////
    {
        SCOPED_TRACE("TC09: Parameter does not exist in SQL (bind index zero).");
        sqlite3_stmt* Stmt = nullptr;
        Prepare(DbHandle, "SELECT 1;", &Stmt);

        std::vector<BindablePair> Params{ {":missing", int64_t(5)} };
        EXPECT_THROW(SQLite_NamedParamBinder::BindParamsByName(Stmt, Params), std::runtime_error);

        Finalize(Stmt);
    }

    ////////////////////// TC10: Reset Failure Simulation //////////////////////
    {
        SCOPED_TRACE("TC10: Manually invalidate statement to simulate reset failure.");
        sqlite3_stmt* Stmt = nullptr;
        Prepare(DbHandle, "SELECT :a;", &Stmt);

        sqlite3_finalize(Stmt); // Invalidates
        std::vector<BindablePair> Params{ {":a", int64_t(5)} };

        EXPECT_THROW(SQLite_NamedParamBinder::BindParamsByName(Stmt, Params), std::runtime_error);
    }

    ////////////////////// TC11: Clear Bindings Failure Skipped //////////////////////
    // Not testable unless sqlite3 library is modified/mocked.

    ////////////////////// TC12: Bind Failure with Unsupported Type //////////////////////
    // Not testable unless variant contains unsupported type.

    ////////////////////// TC13: Rebinding Statement //////////////////////
    {
        SCOPED_TRACE("TC13: Rebind same statement twice with different values.");
        sqlite3_stmt* Stmt = nullptr;
        Prepare(DbHandle, "SELECT :x;", &Stmt);

        std::vector<BindablePair> Params1{ {":x", int64_t(10)} };
        SQLite_NamedParamBinder::BindParamsByName(Stmt, Params1);

        std::vector<BindablePair> Params2{ {":x", std::string("hello")} };
        SQLite_NamedParamBinder::BindParamsByName(Stmt, Params2);

        Finalize(Stmt);
    }

    ////////////////////// TC14: Massive Params //////////////////////
    {
        SCOPED_TRACE("TC14: Bind 1000 named parameters in single statement.");
        std::string Sql = "SELECT ";
        std::vector<BindablePair> Params;
        for (int i = 1; i <= 1000; ++i)
        {
            if (i > 1) Sql += ", ";
            Sql += ":" + std::to_string(i);
            Params.emplace_back(":" + std::to_string(i), int64_t(i));
        }

        sqlite3_stmt* Stmt = nullptr;
        Prepare(DbHandle, Sql.c_str(), &Stmt);
        SQLite_NamedParamBinder::BindParamsByName(Stmt, Params);
        Finalize(Stmt);
    }

    ////////////////////// TC15: UTF-8 Parameter Name //////////////////////
    {
        SCOPED_TRACE("TC15: Binding UTF-8 parameter name.");
        sqlite3_stmt* Stmt = nullptr;
        Prepare(DbHandle, reinterpret_cast<const char*>(u8"SELECT :ключ;"), &Stmt);

        std::vector<BindablePair> Params{ {reinterpret_cast<const char*>(u8":ключ"), int64_t(777)} };
        SQLite_NamedParamBinder::BindParamsByName(Stmt, Params);

        Finalize(Stmt);
    }

    ////////////////////// TC16: Duplicate Param Name Entries //////////////////////
    {
        SCOPED_TRACE("TC16: Bind vector has duplicated entries for same param used twice (should NOT abort).");

        sqlite3* DB = nullptr;
        ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);

        const char* CreateTableSQL =
            "CREATE TABLE Items (Id INTEGER PRIMARY KEY, Name TEXT);"
            "INSERT INTO Items (Name) VALUES ('Alpha');"
            "INSERT INTO Items (Name) VALUES ('Beta');"
            "INSERT INTO Items (Name) VALUES ('Gamma');";

        char* ErrorMsg = nullptr;
        ASSERT_EQ(sqlite3_exec(DB, CreateTableSQL, nullptr, nullptr, &ErrorMsg), SQLITE_OK);

        const char* SelectSQL =
            "SELECT Id, Name FROM Items WHERE Name = @Key OR @Key = 'Alpha';";

        sqlite3_stmt* Statement = nullptr;
        ASSERT_EQ(sqlite3_prepare_v2(DB, SelectSQL, -1, &Statement, nullptr), SQLITE_OK);

        // Duplicate entries for the same named parameter
        std::vector<BindablePair> Params = {
            { "@Key", std::string("Beta") },
            { "@Key", std::string("Beta") }
        };

        SQLite_NamedParamBinder::BindParamsByName(Statement, Params);

        sqlite3_finalize(Statement);
        sqlite3_close(DB);
    }

    sqlite3_close(DbHandle);
}

#endif // #ifdef BUKA_BABUKA




