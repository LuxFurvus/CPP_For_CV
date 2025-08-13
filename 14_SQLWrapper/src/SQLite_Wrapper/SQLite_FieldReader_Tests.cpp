
#include "SQLite_FieldReader.h"
////////
#include <sqlite3.h>
#include <gtest/gtest.h>
////////
#include "../ConditionChecker/ConditionChecker.h"


// ================================
// ░ Test Case Descriptions ░
// ================================

// T01: correctly extracts an INTEGER field (42) as int64_t
// T02: correctly extracts a FLOAT field (3.1415) as double
// T03: correctly extracts a TEXT field ("Hello, World!") as std::string
// T04: correctly extracts an empty TEXT field ("") as std::string
// T05: correctly extracts a BLOB field ({0x01, 0x02, 0x03}) as std::vector<uint8_t>

// T06: correctly extracts a NULL field as std::nullptr_t
// T07: throws on unsupported SQLite column type (simulated invalid type)
// T08: correctly extracts minimum int64_t (-9223372036854775808)
// T09: correctly extracts maximum int64_t (9223372036854775807)
// T10: correctly extracts minimal and maximal double values (IEEE-754 limits)

// T11: correctly extracts a BLOB field containing embedded null bytes
// T12: correctly extracts a multibyte UTF-8 string from a TEXT column
// T13: correctly extracts an empty BLOB as an empty std::vector<uint8_t>
// T14: correctly extracts all supported SQLite field types (INTEGER, FLOAT, TEXT, BLOB, NULL) from a single row
// T15: throws when accessing out-of-bounds column index

// T16: correctly returns std::nullptr_t when TEXT column contains NULL
// T17: correctly extracts BLOB with all byte values from 0x00 to 0xFF
// T18: throws when GetFieldValue is called before stepping the statement
// T19: throws when GetFieldValue is called after finalizing the statement
// T20: throws when using a null sqlite3_stmt* pointer

// T21: extracts extremely large TEXT (≥ 1 MiB) correctly
// T22: extracts extremely large BLOB (≥ 1 MiB) correctly
// T23: extracts maximum-sized TEXT supported by SQLite (≥ 10 MiB) or fails safely
// T24: extracts maximum-sized BLOB supported by SQLite (≥ 10 MiB) or fails safely
// T25: attempts to extract malformed double stored as TEXT, expects std::runtime_error or fallback
// T26: attempts to extract non-UTF-8 byte sequence from TEXT, expects valid recovery or exact byte copy

// ==========================
// ⬛ TC01: Integer Extraction
// ==========================
TEST(SQLite_FieldReader, T01_ExtractInteger)
{
    SCOPED_TRACE("⬛ TC01: Extract INTEGER column from SQLite statement");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &DB));
    sqlite3_exec(DB, "CREATE TABLE T (Val INTEGER);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (42);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<int64_t>(Value));
    EXPECT_EQ(std::get<int64_t>(Value), 42);

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==========================
// ⬛ TC02: Float Extraction
// ==========================
TEST(SQLite_FieldReader, T02_ExtractFloat)
{
    SCOPED_TRACE("⬛ TC02: Extract FLOAT column from SQLite statement");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &DB));
    sqlite3_exec(DB, "CREATE TABLE T (Val REAL);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (3.1415);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<double>(Value));
    EXPECT_DOUBLE_EQ(std::get<double>(Value), 3.1415);

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==========================
// ⬛ TC03: Text Extraction
// ==========================
TEST(SQLite_FieldReader, T03_ExtractText)
{
    SCOPED_TRACE("⬛ TC03: Extract TEXT column from SQLite statement");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &DB));
    sqlite3_exec(DB, "CREATE TABLE T (Val TEXT);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES ('Hello, World!');", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<std::string>(Value));
    EXPECT_EQ(std::get<std::string>(Value), "Hello, World!");

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==========================
// ⬛ TC04: Empty Text
// ==========================
TEST(SQLite_FieldReader, T04_ExtractEmptyText)
{
    SCOPED_TRACE("⬛ TC04: Extract empty string from TEXT column");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &DB));
    sqlite3_exec(DB, "CREATE TABLE T (Val TEXT);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (\"\");", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<std::string>(Value));
    EXPECT_EQ(std::get<std::string>(Value), "");

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==========================
// ⬛ TC05: Blob Extraction
// ==========================
TEST(SQLite_FieldReader, T05_ExtractBlob)
{
    SCOPED_TRACE("⬛ TC05: Extract BLOB column containing bytes {01, 02, 03}");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &DB));
    sqlite3_exec(DB, "CREATE TABLE T (Val BLOB);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (x'010203');", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<std::vector<uint8_t>>(Value));
    std::vector<uint8_t> Expected = {0x01, 0x02, 0x03};
    EXPECT_EQ(std::get<std::vector<uint8_t>>(Value), Expected);

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==========================
// ⬛ TC06: Null Extraction
// ==========================
TEST(SQLite_FieldReader, T06_ExtractNull)
{
    SCOPED_TRACE("⬛ TC06: Extract NULL column from SQLite statement");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &DB));
    sqlite3_exec(DB, "CREATE TABLE T (Val TEXT);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (NULL);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<std::nullptr_t>(Value));

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==========================
// ⬛ TC07: Invalid Column Type
// ==========================
TEST(SQLite_FieldReader, T07_CorruptTypeShouldThrow)
{
    SCOPED_TRACE("⬛ TC07: Simulate unknown column type via isolated test-only function");

    struct SQLite_FieldReader_TestHook
    {
        static FieldValue SimulateInvalidColumnType()
        {
            const int FakeType = 999;
            switch (FakeType)
            {
                case SQLITE_INTEGER: return int64_t(0);
                case SQLITE_FLOAT: return 0.0;
                case SQLITE_TEXT: return std::string("test");
                case SQLITE_BLOB: return std::vector<uint8_t>{};
                case SQLITE_NULL: return nullptr;
                default:
                    CONFIRMS(false, "Unknown SQLite column type: {}", FakeType);
            }
        }
    };

    EXPECT_THROW({
        SQLite_FieldReader_TestHook::SimulateInvalidColumnType();
    }, std::runtime_error);
}

// ==============================
// ⬛ TC08: Extract Min Int64
// ==============================
TEST(SQLite_FieldReader, T08_ExtractMinInt64)
{
    SCOPED_TRACE("⬛ TC08: Extract minimal int64_t value from INTEGER column");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &DB));
    sqlite3_exec(DB, "CREATE TABLE T (Val INTEGER);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (-9223372036854775808);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<int64_t>(Value));
    EXPECT_EQ(std::get<int64_t>(Value), std::numeric_limits<int64_t>::min());

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ TC09: Extract Max Int64
// ==============================
TEST(SQLite_FieldReader, T09_ExtractMaxInt64)
{
    SCOPED_TRACE("⬛ TC09: Extract maximal int64_t value from INTEGER column");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &DB));
    sqlite3_exec(DB, "CREATE TABLE T (Val INTEGER);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (9223372036854775807);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<int64_t>(Value));
    EXPECT_EQ(std::get<int64_t>(Value), std::numeric_limits<int64_t>::max());

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ TC10: Extract Double Limits
// ==============================
TEST(SQLite_FieldReader, T10_ExtractDoubleLimits)
{
    SCOPED_TRACE("⬛ TC10: Extract min and max double from REAL column");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt1 = nullptr;
    sqlite3_stmt* Stmt2 = nullptr;
    ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &DB));
    sqlite3_exec(DB, "CREATE TABLE T (Val REAL);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (1.7976931348623157e+308);", nullptr, nullptr, nullptr); // max double
    sqlite3_exec(DB, "INSERT INTO T VALUES (2.2250738585072014e-308);", nullptr, nullptr, nullptr); // min positive normal

    sqlite3_prepare_v2(DB, "SELECT Val FROM T LIMIT 1 OFFSET 0;", -1, &Stmt1, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T LIMIT 1 OFFSET 1;", -1, &Stmt2, nullptr);

    ASSERT_EQ(sqlite3_step(Stmt1), SQLITE_ROW);
    FieldValue V1 = SQLite_FieldReader::GetFieldValue(Stmt1, 0);
    ASSERT_TRUE(std::holds_alternative<double>(V1));
    EXPECT_DOUBLE_EQ(std::get<double>(V1), std::numeric_limits<double>::max());

    ASSERT_EQ(sqlite3_step(Stmt2), SQLITE_ROW);
    FieldValue V2 = SQLite_FieldReader::GetFieldValue(Stmt2, 0);
    ASSERT_TRUE(std::holds_alternative<double>(V2));
    EXPECT_DOUBLE_EQ(std::get<double>(V2), std::numeric_limits<double>::min());

    sqlite3_finalize(Stmt1);
    sqlite3_finalize(Stmt2);
    sqlite3_close(DB);
}

// ==============================
// ⬛ TC11: Blob With Null Bytes
// ==============================
TEST(SQLite_FieldReader, T11_ExtractBlobWithNulls)
{
    SCOPED_TRACE("⬛ TC11: Extract BLOB containing embedded null bytes");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(SQLITE_OK, sqlite3_open(":memory:", &DB));
    sqlite3_exec(DB, "CREATE TABLE T (Val BLOB);", nullptr, nullptr, nullptr);
    const char* InsertSQL = "INSERT INTO T VALUES (?);";
    sqlite3_prepare_v2(DB, InsertSQL, -1, &Stmt, nullptr);

    const uint8_t Raw[] = {0x00, 0x00, 0x01, 0x02};
    sqlite3_bind_blob(Stmt, 1, Raw, sizeof(Raw), SQLITE_STATIC);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_DONE);
    sqlite3_finalize(Stmt);

    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);
    FieldValue V = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<std::vector<uint8_t>>(V));
    EXPECT_EQ(std::get<std::vector<uint8_t>>(V), std::vector<uint8_t>({0x00, 0x00, 0x01, 0x02}));

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ TC12: Multibyte UTF-8 String
// ==============================
TEST(SQLite_FieldReader, T12_ExtractMultibyteUTF8)
{
    SCOPED_TRACE("⬛ TC12: Extract UTF-8 multibyte characters from TEXT column");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);
    sqlite3_exec(DB, "CREATE TABLE T (Val TEXT);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES ('Привет 🌍');", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<std::string>(Value));
    EXPECT_EQ(std::get<std::string>(Value), "Привет 🌍");

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ TC13: Empty Blob
// ==============================
TEST(SQLite_FieldReader, T13_ExtractEmptyBlob)
{
    SCOPED_TRACE("⬛ TC13: Extract BLOB column with zero-length content");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);
    sqlite3_exec(DB, "CREATE TABLE T (Val BLOB);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (x'');", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<std::vector<uint8_t>>(Value));
    EXPECT_TRUE(std::get<std::vector<uint8_t>>(Value).empty());

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ TC14: Extract All Fields
// ==============================
TEST(SQLite_FieldReader, T14_ExtractMultipleFields)
{
    SCOPED_TRACE("⬛ TC14: Extract multiple field types from single row");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);
    sqlite3_exec(DB, "CREATE TABLE T (I INTEGER, F REAL, T TEXT, B BLOB, N TEXT);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (7, 2.5, 'abc', x'ff01', NULL);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT * FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue V0 = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    FieldValue V1 = SQLite_FieldReader::GetFieldValue(Stmt, 1);
    FieldValue V2 = SQLite_FieldReader::GetFieldValue(Stmt, 2);
    FieldValue V3 = SQLite_FieldReader::GetFieldValue(Stmt, 3);
    FieldValue V4 = SQLite_FieldReader::GetFieldValue(Stmt, 4);

    ASSERT_TRUE(std::holds_alternative<int64_t>(V0));
    EXPECT_EQ(std::get<int64_t>(V0), 7);

    ASSERT_TRUE(std::holds_alternative<double>(V1));
    EXPECT_DOUBLE_EQ(std::get<double>(V1), 2.5);

    ASSERT_TRUE(std::holds_alternative<std::string>(V2));
    EXPECT_EQ(std::get<std::string>(V2), "abc");

    ASSERT_TRUE(std::holds_alternative<std::vector<uint8_t>>(V3));
    EXPECT_EQ(std::get<std::vector<uint8_t>>(V3), std::vector<uint8_t>({0xff, 0x01}));

    ASSERT_TRUE(std::holds_alternative<std::nullptr_t>(V4));

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ TC15: Extract Invalid Column Index
// ==============================
TEST(SQLite_FieldReader, T15_InvalidColumnIndex)
{
    SCOPED_TRACE("⬛ TC15: Use out-of-range column index — should hit CONFIRMS");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);
    sqlite3_exec(DB, "CREATE TABLE T (Val INTEGER);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (55);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    EXPECT_THROW({
        SQLite_FieldReader::GetFieldValue(Stmt, 2); // Only one column (index 0)
    }, std::runtime_error);

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ TC16: Extract NULL As Other Types
// ==============================
TEST(SQLite_FieldReader, T16_NullMisinterpreted)
{
    SCOPED_TRACE("⬛ TC16: Column declared as TEXT but row is NULL — should return nullptr");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);
    sqlite3_exec(DB, "CREATE TABLE T (Val TEXT);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (NULL);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<std::nullptr_t>(Value));

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ TC17: Blob Full Byte Range
// ==============================
TEST(SQLite_FieldReader, T17_BlobFullByteRange)
{
    SCOPED_TRACE("⬛ TC17: Extract BLOB containing all byte values (0x00–0xFF)");

    sqlite3* DB = nullptr;
    sqlite3_stmt* InsertStmt = nullptr;
    sqlite3_stmt* SelectStmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);
    sqlite3_exec(DB, "CREATE TABLE T (Val BLOB);", nullptr, nullptr, nullptr);

    const std::string InsertSQL = "INSERT INTO T VALUES (?);";
    sqlite3_prepare_v2(DB, InsertSQL.c_str(), -1, &InsertStmt, nullptr);

    std::vector<uint8_t> Blob(256);
    for (int i = 0; i < 256; ++i)
        Blob[i] = static_cast<uint8_t>(i);

    sqlite3_bind_blob(InsertStmt, 1, Blob.data(), static_cast<int>(Blob.size()), SQLITE_STATIC);
    ASSERT_EQ(sqlite3_step(InsertStmt), SQLITE_DONE);
    sqlite3_finalize(InsertStmt);

    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &SelectStmt, nullptr);
    ASSERT_EQ(sqlite3_step(SelectStmt), SQLITE_ROW);

    FieldValue V = SQLite_FieldReader::GetFieldValue(SelectStmt, 0);
    ASSERT_TRUE(std::holds_alternative<std::vector<uint8_t>>(V));
    EXPECT_EQ(std::get<std::vector<uint8_t>>(V), Blob);

    sqlite3_finalize(SelectStmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ T18: Statement Not Stepped
// ==============================
TEST(SQLite_FieldReader, T18_UnsteppedStatement)
{
    SCOPED_TRACE("⬛ T18: Attempt to call GetFieldValue without stepping the statement");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);
    sqlite3_exec(DB, "CREATE TABLE T (Val INTEGER);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (99);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);

    // DO NOT step the statement
    EXPECT_THROW({
        SQLite_FieldReader::GetFieldValue(Stmt, 0);
    }, std::runtime_error); // Should trigger CONFIRMS or garbage data read

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ T19: Statement Already Finalized
// ==============================
TEST(SQLite_FieldReader, T19_FinalizedStatement_DeathTest)
{
    SCOPED_TRACE("⬛ T19 (DeathTest): Accessing finalized sqlite3_stmt should crash");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);
    sqlite3_exec(DB, "CREATE TABLE T (Val INTEGER);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES (123);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    sqlite3_step(Stmt);
    sqlite3_finalize(Stmt);

    EXPECT_THROW({
        SQLite_FieldReader::GetFieldValue(Stmt, 0); // should crash
    }, std::runtime_error);

    sqlite3_close(DB);
}

// ==============================
// ⬛ T20: Null sqlite3_stmt* Pointer
// ==============================
TEST(SQLite_FieldReader, T20_NullStatementPointer)
{
    SCOPED_TRACE("⬛ T20: Use null sqlite3_stmt* — must throw via ValidateParams");

    sqlite3_stmt* NullStmt = nullptr;
    EXPECT_THROW({
        SQLite_FieldReader::GetFieldValue(NullStmt, 0);
    }, std::runtime_error); // ValidateParams should throw
}

// ==============================
// ⬛ T21: Extracts extremely large TEXT (≥ 1 MiB) correctly
// ==============================
TEST(SQLite_FieldReader, T21_LargeText_1MiB)
{
    SCOPED_TRACE("⬛ T21: Extracts extremely large TEXT (1 MiB)");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);

    std::string LargeText(1024 * 1024, 'A'); // 1 MiB of 'A'

    sqlite3_exec(DB, "CREATE TABLE T (Val TEXT);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "INSERT INTO T VALUES (?);", -1, &Stmt, nullptr);
    sqlite3_bind_text(Stmt, 1, LargeText.c_str(), static_cast<int>(LargeText.size()), SQLITE_STATIC);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_DONE);
    sqlite3_finalize(Stmt);

    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<std::string>(Value));
    EXPECT_EQ(std::get<std::string>(Value).size(), LargeText.size());
    EXPECT_EQ(std::get<std::string>(Value), LargeText);

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ T22: Extracts extremely large BLOB (≥ 1 MiB) correctly
// ==============================
TEST(SQLite_FieldReader, T22_LargeBlob_1MiB)
{
    SCOPED_TRACE("⬛ T22: Extracts extremely large BLOB (1 MiB)");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);

    std::vector<uint8_t> LargeBlob(1024 * 1024); // 1 MiB
    for (size_t i = 0; i < LargeBlob.size(); ++i)
        LargeBlob[i] = static_cast<uint8_t>(i % 256);

    sqlite3_exec(DB, "CREATE TABLE T (Val BLOB);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "INSERT INTO T VALUES (?);", -1, &Stmt, nullptr);
    sqlite3_bind_blob(Stmt, 1, LargeBlob.data(), static_cast<int>(LargeBlob.size()), SQLITE_STATIC);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_DONE);
    sqlite3_finalize(Stmt);

    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<std::vector<uint8_t>>(Value));
    EXPECT_EQ(std::get<std::vector<uint8_t>>(Value), LargeBlob);

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ T23: Extracts maximum-sized TEXT (≥ 10 MiB) or fails safely
// ==============================
TEST(SQLite_FieldReader, T23_MaxText_10MiB)
{
    SCOPED_TRACE("⬛ T23: Extracts maximum-sized TEXT (10 MiB) or fails safely");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);

    std::string MaxText(10 * 1024 * 1024, 'X'); // 10 MiB of 'X'

    sqlite3_exec(DB, "CREATE TABLE T (Val TEXT);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "INSERT INTO T VALUES (?);", -1, &Stmt, nullptr);
    sqlite3_bind_text(Stmt, 1, MaxText.c_str(), static_cast<int>(MaxText.size()), SQLITE_STATIC);
    int StepResult = sqlite3_step(Stmt);
    sqlite3_finalize(Stmt);
    ASSERT_EQ(StepResult, SQLITE_DONE);

    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    EXPECT_NO_THROW({
        FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
        ASSERT_TRUE(std::holds_alternative<std::string>(Value));
        EXPECT_EQ(std::get<std::string>(Value), MaxText);
    });

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ T24: Extracts maximum-sized BLOB (≥ 10 MiB) or fails safely
// ==============================
TEST(SQLite_FieldReader, T24_MaxBlob_10MiB)
{
    SCOPED_TRACE("⬛ T24: Extracts maximum-sized BLOB (10 MiB) or fails safely");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);

    std::vector<uint8_t> MaxBlob(10 * 1024 * 1024); // 10 MiB
    for (size_t i = 0; i < MaxBlob.size(); ++i)
        MaxBlob[i] = static_cast<uint8_t>(i % 256);

    sqlite3_exec(DB, "CREATE TABLE T (Val BLOB);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "INSERT INTO T VALUES (?);", -1, &Stmt, nullptr);
    sqlite3_bind_blob(Stmt, 1, MaxBlob.data(), static_cast<int>(MaxBlob.size()), SQLITE_STATIC);
    int StepResult = sqlite3_step(Stmt);
    sqlite3_finalize(Stmt);
    ASSERT_EQ(StepResult, SQLITE_DONE);

    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    EXPECT_NO_THROW({
        FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
        ASSERT_TRUE(std::holds_alternative<std::vector<uint8_t>>(Value));
        EXPECT_EQ(std::get<std::vector<uint8_t>>(Value), MaxBlob);
    });

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ T25: Extracts malformed double stored as TEXT
// ==============================
TEST(SQLite_FieldReader, T25_MalformedDoubleText)
{
    SCOPED_TRACE("⬛ T25: Attempt to extract malformed double stored as TEXT");

    sqlite3* DB = nullptr;
    sqlite3_stmt* Stmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);

    sqlite3_exec(DB, "CREATE TABLE T (Val TEXT);", nullptr, nullptr, nullptr);
    sqlite3_exec(DB, "INSERT INTO T VALUES ('not_a_number');", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &Stmt, nullptr);
    ASSERT_EQ(sqlite3_step(Stmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(Stmt, 0);
    ASSERT_TRUE(std::holds_alternative<std::string>(Value)); // Reader does not interpret, returns string

    std::string Retrieved = std::get<std::string>(Value);
    EXPECT_EQ(Retrieved, "not_a_number");

    sqlite3_finalize(Stmt);
    sqlite3_close(DB);
}

// ==============================
// ⬛ T26: Extracts non-UTF8 TEXT bytes exactly
// ==============================
TEST(SQLite_FieldReader, T26_InvalidUTF8Text)
{
    SCOPED_TRACE("⬛ T26: Extract non-UTF-8 byte sequence from TEXT");

    sqlite3* DB = nullptr;
    sqlite3_stmt* InsertStmt = nullptr;
    sqlite3_stmt* SelectStmt = nullptr;
    ASSERT_EQ(sqlite3_open(":memory:", &DB), SQLITE_OK);

    sqlite3_exec(DB, "CREATE TABLE T (Val TEXT);", nullptr, nullptr, nullptr);
    sqlite3_prepare_v2(DB, "INSERT INTO T VALUES (?);", -1, &InsertStmt, nullptr);

    // Non-UTF8 byte sequence
    const char InvalidUTF8[] = { static_cast<char>(0xC3), static_cast<char>(0x28), 0x00 }; // Invalid 2-byte UTF-8
    sqlite3_bind_text(InsertStmt, 1, InvalidUTF8, 2, SQLITE_STATIC);
    ASSERT_EQ(sqlite3_step(InsertStmt), SQLITE_DONE);
    sqlite3_finalize(InsertStmt);

    sqlite3_prepare_v2(DB, "SELECT Val FROM T;", -1, &SelectStmt, nullptr);
    ASSERT_EQ(sqlite3_step(SelectStmt), SQLITE_ROW);

    FieldValue Value = SQLite_FieldReader::GetFieldValue(SelectStmt, 0);
    ASSERT_TRUE(std::holds_alternative<std::string>(Value));
    EXPECT_EQ(std::get<std::string>(Value), std::string(InvalidUTF8, 2));

    sqlite3_finalize(SelectStmt);
    sqlite3_close(DB);
}
