
#include "SQLite_DbValidator.h"
////////
#include <fstream>
#include <filesystem>
////////
#include <gtest/gtest.h>
#include <sqlite3.h>

/////////////////////////////////

/////////////////////////////////

// ================================
// ░ Test Case Descriptions ░
// ================================

// T01: correctly validates a well-formed SQLite file with valid header and structure
// T02: fails validation when the file has incorrect magic header (not SQLite format)
// T03: fails validation when the structure is invalid (wrong page size or format bytes)
// T04: fails when the input file does not exist
// T05: fails when the file is empty and contains no bytes
// T06: fails when file is shorter than 16 bytes (header cannot be read)
// T07: fails when file is shorter than 100 bytes (structure cannot be read)
// T08: passes for valid file with page size = 512 (minimum allowed power-of-two)
// T09: passes for valid file with page size = 32768 (maximum allowed power-of-two)
// T10: passes for valid file with page size = 1 (SQLite special case)
// T11: fails when page size is not a power of two (e.g., 1000)
// T12: fails for file with correct length but invalid magic header content
// T13: fails for file with correct header but corrupted critical format fields

// ================================

namespace SQLite_DbValidator_Tests {

// Global DB path reused across tests
const std::string DbPath = "temp_t01.db";

// Creates a valid SQLite DB using SQLite API
sqlite3* CreateTempDb()
{
    sqlite3* DbHandle = nullptr;
    const int AllFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_EXRESCODE | SQLITE_OPEN_FULLMUTEX;
    const int OpenResult = sqlite3_open_v2(DbPath.c_str(), &DbHandle, AllFlags, nullptr);
    EXPECT_EQ(OpenResult, SQLITE_OK);

    // Ensure schema is written by creating a dummy table
    char* ErrorMsg = nullptr;
    const char* Sql = "CREATE TABLE Dummy(Id INTEGER);";
    int ExecResult = sqlite3_exec(DbHandle, Sql, nullptr, nullptr, &ErrorMsg);
    EXPECT_EQ(ExecResult, SQLITE_OK);
    sqlite3_free(ErrorMsg);

    return DbHandle;
}

// Closes and removes DB file
void RemoveTempDb(sqlite3* DbHandle)
{
    sqlite3_close(DbHandle);
    std::filesystem::remove(DbPath);
}

// ========== T01: Valid file, valid header, valid structure ==========

TEST(SQLite_DbValidator_T01_ValidDbFile, ReturnsTrueOnValidDb)
{
    SCOPED_TRACE("T01: Correctly validates a well-formed SQLite file with valid header and structure");
    
    sqlite3* DbHandle = CreateTempDb();
    SQLite_DbValidator Validator(DbHandle, DbPath);
    
    EXPECT_TRUE(Validator());
    
    RemoveTempDb(DbHandle);
}

// ========== T02: Invalid file header ==========

TEST(SQLite_DbValidator_T02_InvalidHeader, ReturnsFalseOnWrongMagicBytes)
{
    SCOPED_TRACE("T02: Fails validation when the file has incorrect magic header (not SQLite format)");

    std::ofstream File(DbPath, std::ios::binary);
    static constexpr char InvalidHeader[16] = {
        'N','O','T','S','Q','L','I','T','E','_','_','_','_','_','_','\0'
    };
    File.write(InvalidHeader, sizeof(InvalidHeader));

    SQLite_DbValidator Validator(nullptr, DbPath);
    EXPECT_FALSE(Validator());

    std::filesystem::remove(DbPath);
}

// ========== T03: Valid header, invalid structure ==========

TEST(SQLite_DbValidator_T03_InvalidStructure, ReturnsFalseOnInvalidStructure)
{
    SCOPED_TRACE("T03: Fails validation when the structure is invalid (wrong page size or format bytes)");

    std::ofstream File(DbPath, std::ios::binary);
    File.write("SQLite format 3", 16); // valid header
    std::array<unsigned char, 84> Padding = {};
    File.write(reinterpret_cast<char*>(Padding.data()), Padding.size());
    File.put(static_cast<unsigned char>(0x00)); // page size MSB
    File.put(static_cast<unsigned char>(0x10)); // page size LSB = 16 -> invalid (not power of two)
    File.put(static_cast<unsigned char>(0xFF)); // file format write version
    File.put(static_cast<unsigned char>(0xFF)); // file format read version
    File.put(static_cast<unsigned char>(0x00)); // reserved space
    File.put(static_cast<unsigned char>(0x00)); // max payload fraction
    File.put(static_cast<unsigned char>(0x00)); // min payload fraction
    File.close();

    SQLite_DbValidator Validator(nullptr, DbPath);
    EXPECT_FALSE(Validator());

    std::filesystem::remove(DbPath);
}

// ========== T04: File does not exist ==========

TEST(SQLite_DbValidator_T04_FileMissing, ReturnsFalseOnMissingFile)
{
    SCOPED_TRACE("T04: Fails when the input file does not exist");

    std::filesystem::remove(DbPath);

    SQLite_DbValidator Validator(nullptr, DbPath);
    EXPECT_FALSE(Validator());
}

// ========== T05: Empty file ==========

TEST(SQLite_DbValidator_T05_EmptyFile, ReturnsFalseOnEmptyFile)
{
    SCOPED_TRACE("T05: Fails when the file is empty and contains no bytes");

    std::ofstream(DbPath, std::ios::binary).close();

    SQLite_DbValidator Validator(nullptr, DbPath);
    EXPECT_FALSE(Validator());

    std::filesystem::remove(DbPath);
}

// ========== T06: Truncated file (<16 bytes) ==========

TEST(SQLite_DbValidator_T06_TruncatedHeader, ReturnsFalseOnShortHeader)
{
    SCOPED_TRACE("T06: Fails when file is shorter than 16 bytes (header cannot be read)");

    std::ofstream File(DbPath, std::ios::binary);
    File.write("SQLshort", 8);
    File.close();

    SQLite_DbValidator Validator(nullptr, DbPath);
    EXPECT_FALSE(Validator());

    std::filesystem::remove(DbPath);
}

// ========== T07: Truncated structure (<100 bytes) ==========

TEST(SQLite_DbValidator_T07_TruncatedStructure, ReturnsFalseOnShortStructure)
{
    SCOPED_TRACE("T07: Fails when file is shorter than 100 bytes (structure cannot be read)");

    std::ofstream File(DbPath, std::ios::binary);
    File.write("SQLite format 3", 16); // Valid header
    std::array<unsigned char, 50> Padding = {};
    File.write(reinterpret_cast<char*>(Padding.data()), Padding.size());
    File.close();

    SQLite_DbValidator Validator(nullptr, DbPath);
    EXPECT_FALSE(Validator());

    std::filesystem::remove(DbPath);
}

// ========== T08: Page size = 512 (minimum allowed) ==========

TEST(SQLite_DbValidator_T08_MinPageSize, ReturnsTrueOnMinValidPageSize)
{
    SCOPED_TRACE("T08: Passes for valid file with page size = 512 (minimum allowed power-of-two)");

    sqlite3* DbHandle = CreateTempDb();

    // Rewriting header bytes to force page size = 512
    {
        std::fstream File(DbPath, std::ios::in | std::ios::out | std::ios::binary);
        File.seekp(16);
        File.put(static_cast<unsigned char>(0x02)); // MSB
        File.put(static_cast<unsigned char>(0x00)); // LSB
        File.close();
    }

    SQLite_DbValidator Validator(DbHandle, DbPath);
    EXPECT_TRUE(Validator());

    RemoveTempDb(DbHandle);
}

// ========== T09: Page size = 32768 (maximum allowed) ==========

TEST(SQLite_DbValidator_T09_MaxPageSize, ReturnsTrueOnMaxValidPageSize)
{
    SCOPED_TRACE("T09: Passes for valid file with page size = 32768 (maximum allowed power-of-two)");

    sqlite3* DbHandle = CreateTempDb();

    {
        std::fstream File(DbPath, std::ios::in | std::ios::out | std::ios::binary);
        File.seekp(16);
        File.put(static_cast<unsigned char>(0x80)); // MSB
        File.put(static_cast<unsigned char>(0x00)); // LSB
        File.close();
    }

    SQLite_DbValidator Validator(DbHandle, DbPath);
    EXPECT_TRUE(Validator());

    RemoveTempDb(DbHandle);
}

// ========== T10: Page size = 1 (special valid SQLite case) ==========

TEST(SQLite_DbValidator_T10_SpecialPageSize, ReturnsTrueOnPageSizeOne)
{
    SCOPED_TRACE("T10: Passes for valid file with page size = 1 (SQLite special case)");

    sqlite3* DbHandle = CreateTempDb();

    {
        std::fstream File(DbPath, std::ios::in | std::ios::out | std::ios::binary);
        File.seekp(16);
        File.put(static_cast<unsigned char>(0x00)); // MSB
        File.put(static_cast<unsigned char>(0x01)); // LSB
        File.close();
    }

    SQLite_DbValidator Validator(DbHandle, DbPath);
    EXPECT_TRUE(Validator());

    RemoveTempDb(DbHandle);
}

// ========== T11: Page size not power-of-two (e.g., 1000) ==========

TEST(SQLite_DbValidator_T11_InvalidPageSize, ReturnsFalseOnNonPowerOfTwoPageSize)
{
    SCOPED_TRACE("T11: Fails when page size is not a power of two (e.g., 1000)");

    sqlite3* DbHandle = CreateTempDb();

    {
        std::fstream File(DbPath, std::ios::in | std::ios::out | std::ios::binary);
        File.seekp(16);
        File.put(static_cast<unsigned char>(0x03)); // MSB
        File.put(static_cast<unsigned char>(0xE8)); // LSB = 1000
        File.close();
    }

    SQLite_DbValidator Validator(DbHandle, DbPath);
    EXPECT_FALSE(Validator());

    RemoveTempDb(DbHandle);
}

// ========== T12: Incorrect magic bytes, correct length ==========

TEST(SQLite_DbValidator_T12_InvalidMagicButCorrectSize, ReturnsFalseOnBadMagic)
{
    SCOPED_TRACE("T12: Fails for file with correct length but invalid magic header content");

    std::ofstream File(DbPath, std::ios::binary);
    static constexpr char InvalidMagic[16] = {
        'x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','\0'
    };
    File.write(InvalidMagic, sizeof(InvalidMagic));

    std::array<unsigned char, 84> Padding = {};
    File.write(reinterpret_cast<char*>(Padding.data()), Padding.size());
    File.close();

    SQLite_DbValidator Validator(nullptr, DbPath);
    EXPECT_FALSE(Validator());

    std::filesystem::remove(DbPath);
}

// ========== T13: Corrupted format fields (e.g., wrong version bytes) ==========

TEST(SQLite_DbValidator_T13_CorruptedFormatFields, ReturnsFalseOnInvalidFormatVersion)
{
    SCOPED_TRACE("T13: Fails for file with correct header but corrupted critical format fields");

    sqlite3* DbHandle = CreateTempDb();

    {
        std::fstream File(DbPath, std::ios::in | std::ios::out | std::ios::binary);
        // corrupt write version and read version fields
        File.seekp(18);
        File.put(static_cast<unsigned char>(0x99)); // write version
        File.put(static_cast<unsigned char>(0x77)); // read version

        File.seekp(21);
        File.put(static_cast<unsigned char>(0x00)); // reserved space
        File.put(static_cast<unsigned char>(0x00)); // max payload
        File.put(static_cast<unsigned char>(0x00)); // min payload
        File.close();
    }

    SQLite_DbValidator Validator(DbHandle, DbPath);
    EXPECT_FALSE(Validator());

    RemoveTempDb(DbHandle);
}

} // namespace SQLite_DbValidator_Tests
