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


////////////////////////////////////////////

///////////////////////////////////////////

#ifdef BUKA_BABUKA


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




