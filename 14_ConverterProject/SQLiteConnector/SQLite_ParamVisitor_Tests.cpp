#include <sqlite3.h>
#include <variant>
#include <string>
#include <stdexcept>
////////
#include <gtest/gtest.h>
////////
#include "SQLiteConnector.h"


// ─────────────────────────────────────────────
// T01: Bind int64_t should succeed
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T01_BindInt64ShouldSucceed)
{
    SCOPED_TRACE("=== T01: Bind int64_t to ? should succeed ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT * FROM (SELECT ? AS Val)", -1, &Statement, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Statement, 1);
    EXPECT_EQ(Visitor(static_cast<int64_t>(42)), SQLITE_OK);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T02: Bind int should succeed
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T02_BindIntShouldSucceed)
{
    SCOPED_TRACE("=== T02: Bind int to ? should succeed ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT * FROM (SELECT ? AS Val)", -1, &Statement, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Statement, 1);
    EXPECT_EQ(Visitor(7), SQLITE_OK);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T03: Bind double should succeed
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T03_BindDoubleShouldSucceed)
{
    SCOPED_TRACE("=== T03: Bind double to ? should succeed ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT * FROM (SELECT ? AS Val)", -1, &Statement, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Statement, 1);
    EXPECT_EQ(Visitor(3.14), SQLITE_OK);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T04: Bind std::string should succeed
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T04_BindStringShouldSucceed)
{
    SCOPED_TRACE("=== T04: Bind std::string to ? should succeed ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT * FROM (SELECT ? AS Val)", -1, &Statement, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Statement, 1);
    EXPECT_EQ(Visitor(std::string("Hello")), SQLITE_OK);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T05: Bind empty string should succeed
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T05_BindEmptyStringShouldSucceed)
{
    SCOPED_TRACE("=== T05: Bind empty string should succeed ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT * FROM (SELECT ? AS Val)", -1, &Statement, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Statement, 1);
    EXPECT_EQ(Visitor(std::string("")), SQLITE_OK);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T06: Bind Unicode string should succeed
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T06_BindUnicodeStringShouldSucceed)
{
    SCOPED_TRACE("=== T06: Bind Unicode string should succeed ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT * FROM (SELECT ? AS Val)", -1, &Statement, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Statement, 1);
    EXPECT_EQ(Visitor(std::string("Привет")), SQLITE_OK);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T07: Bind nullptr should succeed
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T07_BindNullShouldSucceed)
{
    SCOPED_TRACE("=== T07: Bind nullptr should succeed ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT * FROM (SELECT ? AS Val)", -1, &Statement, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Statement, 1);
    EXPECT_EQ(Visitor(nullptr), SQLITE_OK);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T08: ParamIndex == 0 should throw
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T08_ParamIndexZeroShouldThrow)
{
    SCOPED_TRACE("=== T08: ParamIndex == 0 should throw ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT * FROM (SELECT ? AS Val)", -1, &Statement, nullptr), SQLITE_OK);

    EXPECT_THROW(SQLite_ParamVisitor(Statement, 0), std::runtime_error);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T09: ParamIndex < 0 should throw
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T09_ParamIndexNegativeShouldThrow)
{
    SCOPED_TRACE("=== T09: ParamIndex < 0 should throw ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT * FROM (SELECT ? AS Val)", -1, &Statement, nullptr), SQLITE_OK);

    EXPECT_THROW(SQLite_ParamVisitor(Statement, -1), std::runtime_error);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T10: Statement == nullptr should throw
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T10_NullStatementShouldThrow)
{
    SCOPED_TRACE("=== T10: Statement == nullptr should throw ===");

    EXPECT_THROW(SQLite_ParamVisitor(nullptr, 1), std::runtime_error);
}

// ─────────────────────────────────────────────
// T11: ParamIndex > count should return error
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T11_ParamIndexOutOfBoundsShouldFail)
{
    SCOPED_TRACE("=== T11: ParamIndex > bound count should return error ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT ?1, ?2", -1, &Statement, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Statement, 3);
    EXPECT_NE(Visitor(1), SQLITE_OK);  // Binding to ?3 when only ?1, ?2 exist

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T12: Bind to last valid index
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T12_BindToLastParamIndexShouldSucceed)
{
    SCOPED_TRACE("=== T12: Bind to last valid param index ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT ?1, ?2", -1, &Statement, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Statement, 2);
    EXPECT_EQ(Visitor(std::string("End")), SQLITE_OK);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T13: Reuse visitor with multiple types
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T13_ReuseVisitorAcrossTypes)
{
    SCOPED_TRACE("=== T13: Reuse visitor with multiple value types ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT * FROM (SELECT ? AS Val)", -1, &Statement, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Statement, 1);
    EXPECT_EQ(Visitor(123), SQLITE_OK);
    EXPECT_EQ(Visitor(456.78), SQLITE_OK);
    EXPECT_EQ(Visitor(std::string("Text")), SQLITE_OK);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T14: Embedded null in string should bind
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T14_BindEmbeddedNullStringShouldSucceed)
{
    SCOPED_TRACE("=== T14: Bind string with embedded null ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    std::string S("abc\0def", 7); // Embedded null

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT * FROM (SELECT ? AS Val)", -1, &Statement, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Statement, 1);
    EXPECT_EQ(Visitor(S), SQLITE_OK);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T15: Bind large int64_t should succeed
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T15_BindLargeInt64ShouldSucceed)
{
    SCOPED_TRACE("=== T15: Bind large int64_t ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT * FROM (SELECT ? AS Val)", -1, &Statement, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Statement, 1);
    EXPECT_EQ(Visitor(static_cast<int64_t>(1e18)), SQLITE_OK);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T16: Bind large double should succeed
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T16_BindLargeDoubleShouldSucceed)
{
    SCOPED_TRACE("=== T16: Bind large double ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Statement = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT * FROM (SELECT ? AS Val)", -1, &Statement, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Statement, 1);
    EXPECT_EQ(Visitor(1e308), SQLITE_OK);

    sqlite3_finalize(Statement);
    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T17: Bind Very Large String (~1MB)
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T17_BindVeryLargeStringShouldSucceed)
{
    SCOPED_TRACE("=== T17: Bind ~1MB string should succeed ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Stmt = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_exec(Db, "CREATE TABLE Test (Val);", nullptr, nullptr, nullptr), SQLITE_OK);

    // Generate ~1MB string
    std::string LargeString(1024 * 1024, 'X');
    ASSERT_EQ(sqlite3_prepare_v2(Db, "INSERT INTO Test VALUES (?);", -1, &Stmt, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Stmt, 1);
    ASSERT_EQ(Visitor(LargeString), SQLITE_OK);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_DONE);
    sqlite3_finalize(Stmt);

    // Read and check
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT Val FROM Test;", -1, &Stmt, nullptr), SQLITE_OK);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);
    const char* Data = reinterpret_cast<const char*>(sqlite3_column_text(Stmt, 0));
    ASSERT_NE(Data, nullptr);
    EXPECT_EQ(strlen(Data), LargeString.size());
    sqlite3_finalize(Stmt);

    sqlite3_close(Db);
}

// ─────────────────────────────────────────────
// T18: Overwrite Same Parameter Slot
// ─────────────────────────────────────────────
TEST(SQLite_ParamVisitor, T18_BindOverwriteSameParamShouldUseLastValue)
{
    SCOPED_TRACE("=== T18: Bind same param twice; expect second value to persist ===");

    sqlite3* Db = nullptr;
    sqlite3_stmt* Stmt = nullptr;

    ASSERT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
    ASSERT_EQ(sqlite3_exec(Db, "CREATE TABLE Test (Val);", nullptr, nullptr, nullptr), SQLITE_OK);

    ASSERT_EQ(sqlite3_prepare_v2(Db, "INSERT INTO Test VALUES (?);", -1, &Stmt, nullptr), SQLITE_OK);

    SQLite_ParamVisitor Visitor(Stmt, 1);
    ASSERT_EQ(Visitor(std::string("initial")), SQLITE_OK);
    ASSERT_EQ(Visitor(std::string("overwritten")), SQLITE_OK); // Overwrite

    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_DONE);
    sqlite3_finalize(Stmt);

    // Verify only "overwritten" was inserted
    ASSERT_EQ(sqlite3_prepare_v2(Db, "SELECT Val FROM Test;", -1, &Stmt, nullptr), SQLITE_OK);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);
    const char* Result = reinterpret_cast<const char*>(sqlite3_column_text(Stmt, 0));
    EXPECT_STREQ(Result, "overwritten");
    sqlite3_finalize(Stmt);

    sqlite3_close(Db);
}
