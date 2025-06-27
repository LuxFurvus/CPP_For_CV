#include <stdexcept>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <unordered_set>
////////
#include <gtest/gtest.h>
#include <sqlite3.h>
////////
#include "SQLiteConnector.h"


// ─────────────────────────────────────────────────────────────────────────────
// Test Fixture Base
// ─────────────────────────────────────────────────────────────────────────────
class SQLite_NamedParamBinderTest : public ::testing::Test
{
protected:
    sqlite3* Db = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    std::vector<BindablePair> Pairs{};

    void SetUp() override
    {
        int Result = sqlite3_open(":memory:", &Db);
        ASSERT_EQ(Result, SQLITE_OK);
        Stmt = nullptr;
    }

    void TearDown() override
    {
        if (Stmt)
        {
            sqlite3_finalize(Stmt);
            Stmt = nullptr;
        }
        if (Db)
        {
            sqlite3_close(Db);
            Db = nullptr;
        }
        Pairs.clear();
    }

    void PrepareStatement(const std::string& Sql)
    {
        int Result = sqlite3_prepare_v2(Db, Sql.c_str(), -1, &Stmt, nullptr);
        ASSERT_EQ(Result, SQLITE_OK);
    }

    void SetPair(const std::string& Name, const FieldValue& Value)
    {
        Pairs.emplace_back(Name, Value);
    }

    void ExecuteBinding()
    {
        SQLite_NamedParamBinder::BindParamsByName(Stmt, Pairs);
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// T01: Bind all valid param types
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T01_BindAllValidParamTypes)
{
    SCOPED_TRACE("Bind int, int64_t, double, string, nullptr_t to SQLite statement");

    PrepareStatement("SELECT :a, :b, :c, :d, :e");

    SetPair(":a", 42);
    SetPair(":b", static_cast<int64_t>(1234567890123));
    SetPair(":c", 3.14);
    SetPair(":d", std::string("hello"));
    SetPair(":e", nullptr);

    EXPECT_NO_THROW(ExecuteBinding());
}

// ─────────────────────────────────────────────────────────────────────────────
// T02: Bind with all valid prefixes
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T02_BindWithAllPrefixes)
{
    SCOPED_TRACE("Bind parameter using all three prefixes: :, @, $");

    PrepareStatement("SELECT :id, @id, $id");

    SetPair(":id", 1);
    SetPair("@id", 2);
    SetPair("$id", 3);

    EXPECT_NO_THROW(ExecuteBinding());
}

// ─────────────────────────────────────────────────────────────────────────────
// T03: Bind with missing parameter
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T03_BindMissingParamShouldThrow)
{
    SCOPED_TRACE("Provide param name not present in SQL — expect throw");

    PrepareStatement("SELECT :exists");

    SetPair(":nonexistent", 5);

    EXPECT_THROW(ExecuteBinding(), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// T04: Bind with empty parameter name
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T04_EmptyParameterNameShouldThrow)
{
    SCOPED_TRACE("Pass an empty parameter name, expect failure");

    PrepareStatement("SELECT 1");

    SetPair("", 10);

    EXPECT_THROW(ExecuteBinding(), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// T05: Duplicate param names with different prefixes
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T05_DuplicateParamNamesDifferentPrefixes)
{
    SCOPED_TRACE("Use duplicate param names with different prefixes: :val, @val");

    PrepareStatement("SELECT :val, @val");

    SetPair(":val", 1);
    SetPair("@val", 2);

    EXPECT_NO_THROW(ExecuteBinding());
}

// ─────────────────────────────────────────────────────────────────────────────
// T06: Extra bindable pairs not in SQL
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T06_ExcessBindablePairsShouldThrow)
{
    SCOPED_TRACE("SQL has 1 param, vector has 2 — should fail validation");

    PrepareStatement("SELECT :x");

    SetPair(":x", 1);
    SetPair(":y", 2);

    EXPECT_THROW(ExecuteBinding(), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// T07: Missing bindable pair should throw
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T07_MissingBindablePairsShouldThrow)
{
    SCOPED_TRACE("SQL has 2 params, vector provides 1 — should throw");

    PrepareStatement("SELECT :x, :y");

    SetPair(":x", 42);

    EXPECT_THROW(ExecuteBinding(), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// T08: Manual invalid index to BindOneParam
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T08_InvalidParamIndexShouldThrow)
{
    SCOPED_TRACE("Manually pass index = 0 to BindOneParam — should throw");

    PrepareStatement("SELECT 1");

    EXPECT_THROW(SQLite_NamedParamBinder::BindOneParam(Stmt, 0, 100), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// T09: Null statement pointer
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T09_NullStatementPointerShouldThrow)
{
    SCOPED_TRACE("Pass nullptr instead of statement — expect throw");

    SetPair(":x", 42);

    EXPECT_THROW(SQLite_NamedParamBinder::BindParamsByName(nullptr, Pairs), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// T10: Empty bindable vector
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T10_EmptyBindableVectorShouldThrow)
{
    SCOPED_TRACE("Pass empty bindable vector — expect throw");

    PrepareStatement("SELECT :x");

    EXPECT_THROW(ExecuteBinding(), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// T11: Valid bindable but wrong prefix in provided name
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T11_ValidBindableWrongPrefixShouldResolve)
{
    SCOPED_TRACE("Bind @id to SQL with :id — expect normalization to match");

    PrepareStatement("SELECT :id");

    SetPair("@id", 123);

    EXPECT_THROW(ExecuteBinding(), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// T12: Statement with unnamed parameters should be skipped
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T12_UnnamedParametersShouldBeIgnored)
{
    SCOPED_TRACE("SQL with ?1 positional parameters — validator should ignore them");

    PrepareStatement("SELECT ?1");

    // No named parameters expected, validation should fail due to missing
    SetPair(":x", 10);

    EXPECT_THROW(ExecuteBinding(), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// T13: Repeated parameter name in SQL should bind both
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T13_RepeatedParamShouldBindAllOccurrences)
{
    SCOPED_TRACE("Repeated param :val should bind all appearances");

    PrepareStatement("SELECT :val + :val");

    SetPair(":val", 9);

    EXPECT_NO_THROW(ExecuteBinding());
}

// ─────────────────────────────────────────────────────────────────────────────
// T14: Param name with special characters
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T14_SpecialCharactersInNameShouldNotBind)
{
    SCOPED_TRACE("Use special characters in param name: :us#r");

    PrepareStatement("SELECT :user");

    SetPair(":us#r", 555);

    EXPECT_THROW(ExecuteBinding(), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// T15: UTF-8 characters in param name
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T15_UTF8CharactersInParamNameShouldBind)
{
    SCOPED_TRACE("Param name includes Unicode characters: :имя, @ユーザ");

    PrepareStatement("SELECT :имя, @ユーザ");

    SetPair(":имя", 11);
    SetPair("@ユーザ", 22);

    EXPECT_NO_THROW(ExecuteBinding());
}

// ─────────────────────────────────────────────────────────────────────────────
// T16: Extremely long parameter name
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T16_VeryLongParamNameShouldBind)
{
    SCOPED_TRACE("Use a very long param name (1024+ characters)");

    std::string LongName(1024, 'a');
    PrepareStatement("SELECT :" + LongName);

    SetPair(":" + LongName, 77);

    EXPECT_NO_THROW(ExecuteBinding());
}

// ─────────────────────────────────────────────────────────────────────────────
// T17: SQL with no bindable parameters
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T17_NoBindableParamsInSQLShouldThrow)
{
    SCOPED_TRACE("SQL has no bindable parameters — expect failure");

    PrepareStatement("SELECT 1");

    SetPair(":x", 1);

    EXPECT_THROW(ExecuteBinding(), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// T18: Rebind without reset/clear should succeed via internal reset
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T18_RebindShouldResetAndClearAutomatically)
{
    SCOPED_TRACE("Calling binder twice must internally reset and clear");

    PrepareStatement("SELECT :v");

    SetPair(":v", 1);
    EXPECT_NO_THROW(ExecuteBinding());

    Pairs.clear();
    SetPair(":v", 2);
    EXPECT_NO_THROW(ExecuteBinding());
}

// ─────────────────────────────────────────────────────────────────────────────
// T19: Full bind-execute roundtrip
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T19_BindAndExecuteSelect)
{
    SCOPED_TRACE("Bind param and run full SELECT execution");

    PrepareStatement("SELECT :x");

    SetPair(":x", 42);
    EXPECT_NO_THROW(ExecuteBinding());

    int StepResult = sqlite3_step(Stmt);
    EXPECT_EQ(StepResult, SQLITE_ROW);
    EXPECT_EQ(sqlite3_column_int(Stmt, 0), 42);
}

// ─────────────────────────────────────────────────────────────────────────────
// T20: Bind after statement is finalized
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T20_BindAfterFinalizeShouldThrow)
{
    SCOPED_TRACE("Finalize statement and attempt binding — must throw");

    PrepareStatement("SELECT :x");

    SetPair(":x", 100);
    sqlite3_finalize(Stmt);
    Stmt = nullptr;

    EXPECT_THROW(ExecuteBinding(), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// T21: Provided param without prefix should not match — expect failure
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T21_BindWithNoPrefixProvidedShouldThrow)
{
    SCOPED_TRACE("=== T21: Provided param has no prefix — should not bind ===");

    PrepareStatement("SELECT :A + @A + $A");

    SetPair("A", 1); // Missing prefix

    EXPECT_THROW(ExecuteBinding(), std::runtime_error);
}

// ─────────────────────────────────────────────────────────────────────────────
// T22: Bind long string value (content, not name)
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T22_BindLargeStringContent)
{
    SCOPED_TRACE("=== T22: Bind very long string value ===");

    std::string LongString(10000, 'x');
    PrepareStatement("SELECT LENGTH(:Text)");

    SetPair(":Text", LongString);

    EXPECT_NO_THROW(ExecuteBinding());
    EXPECT_EQ(SQLITE_ROW, sqlite3_step(Stmt));
    EXPECT_EQ(10000, sqlite3_column_int(Stmt, 0));
}

// ─────────────────────────────────────────────────────────────────────────────
// T23: Bind floating-point edge values
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T23_BindExtremeDoubleValues)
{
    SCOPED_TRACE("=== T23: Bind DBL_MAX value into SQLite ===");

    PrepareStatement("SELECT :X");

    SetPair(":X", DBL_MAX);

    EXPECT_NO_THROW(ExecuteBinding());
    EXPECT_EQ(SQLITE_ROW, sqlite3_step(Stmt));
    EXPECT_DOUBLE_EQ(DBL_MAX, sqlite3_column_double(Stmt, 0));
}

// ─────────────────────────────────────────────────────────────────────────────
// T24: Bind null value individually
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T24_BindNullIndividually)
{
    SCOPED_TRACE("=== T24: Bind nullptr_t as single parameter ===");

    PrepareStatement("SELECT :X IS NULL");

    SetPair(":X", nullptr);

    EXPECT_NO_THROW(ExecuteBinding());
    EXPECT_EQ(SQLITE_ROW, sqlite3_step(Stmt));
    EXPECT_EQ(1, sqlite3_column_int(Stmt, 0)); // true
}

// ─────────────────────────────────────────────────────────────────────────────
// T25: Bind UTF-8 string value (content)
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T25_BindUnicodeStringValue)
{
    SCOPED_TRACE("=== T25: Bind UTF-8 string content (имя) ===");

    const std::string Utf8Text = reinterpret_cast<const char*>(u8"имя");
    PrepareStatement("SELECT :Name");

    SetPair(":Name", Utf8Text);

    EXPECT_NO_THROW(ExecuteBinding());
    EXPECT_EQ(SQLITE_ROW, sqlite3_step(Stmt));

    const char* ResultText = reinterpret_cast<const char*>(sqlite3_column_text(Stmt, 0));
    EXPECT_EQ(Utf8Text, std::string(ResultText));
}

// ─────────────────────────────────────────────────────────────────────────────
// T26: Bind 1000+ parameters in a single statement
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T26_MassiveBindings)
{
    SCOPED_TRACE("=== T26: Bind 1000 named parameters in one statement ===");

    std::string Sql = "SELECT ";
    for (int i = 1; i <= 1000; ++i)
    {
        if (i > 1) Sql += ", ";
        Sql += ":" + std::to_string(i);
        SetPair(":" + std::to_string(i), int64_t(i));
    }

    PrepareStatement(Sql);
    EXPECT_NO_THROW(ExecuteBinding());
    EXPECT_EQ(SQLITE_ROW, sqlite3_step(Stmt));
}

// ─────────────────────────────────────────────────────────────────────────────
// T27: Bind vector contains duplicate entries for same parameter
// ─────────────────────────────────────────────────────────────────────────────
TEST_F(SQLite_NamedParamBinderTest, T27_DuplicateEntriesInVector)
{
    SCOPED_TRACE("=== T27: Duplicate entries for same parameter name should not cause failure ===");

    const char* InitSQL =
        "CREATE TABLE Items (Id INTEGER PRIMARY KEY, Name TEXT);"
        "INSERT INTO Items (Name) VALUES ('Alpha');"
        "INSERT INTO Items (Name) VALUES ('Beta');"
        "INSERT INTO Items (Name) VALUES ('Gamma');";
    char* ErrorMsg = nullptr;
    ASSERT_EQ(sqlite3_exec(Db, InitSQL, nullptr, nullptr, &ErrorMsg), SQLITE_OK);

    PrepareStatement("SELECT Id FROM Items WHERE Name = @Key OR @Key = 'Alpha'");

    SetPair("@Key", std::string("Beta"));
    SetPair("@Key", std::string("Beta")); // Duplicate

    EXPECT_NO_THROW(ExecuteBinding());
    EXPECT_EQ(SQLITE_ROW, sqlite3_step(Stmt));
}

// ─────────────────────────────────────────────────────────────────────────────
