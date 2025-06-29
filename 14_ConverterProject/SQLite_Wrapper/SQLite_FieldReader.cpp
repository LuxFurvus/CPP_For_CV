
#include <mutex>
////////
#include <sqlite3.h>
////////
#include "../ConditionChecker/ConditionChecker.h"
////////
#include "SQLite_FieldReader.h"



////////////////////

////////////////////



FieldValue SQLite_FieldReader::GetFieldValue(sqlite3_stmt* Statement, int ColumnIndex)
{
    ValidateParams(Statement, ColumnIndex);

    const int Type = sqlite3_column_type(Statement, ColumnIndex);

    switch (Type)
    {
        case SQLITE_INTEGER:
            return ExtractInteger(Statement, ColumnIndex);
        case SQLITE_FLOAT:
            return ExtractDouble(Statement, ColumnIndex);
        case SQLITE_TEXT:
            return ExtractText(Statement, ColumnIndex);
        case SQLITE_BLOB:
            return ExtractBlob(Statement, ColumnIndex);
        case SQLITE_NULL:
            return nullptr;
        default:
            CONFIRMS(false, "Unknown SQLite column type: {}", Type);
    }
}

//\/\/\/\/\/\/\/\/\/

int64_t SQLite_FieldReader::ExtractInteger(sqlite3_stmt* Statement, int ColumnIndex)
{
    return static_cast<int64_t>(sqlite3_column_int64(Statement, ColumnIndex));
}

//=================

double SQLite_FieldReader::ExtractDouble(sqlite3_stmt* Statement, int ColumnIndex)
{
    return sqlite3_column_double(Statement, ColumnIndex);
}

//=================

std::string SQLite_FieldReader::ExtractText(sqlite3_stmt* Statement, int ColumnIndex)
{
    const unsigned char* TextPtr = sqlite3_column_text(Statement, ColumnIndex);
    const int Bytes = sqlite3_column_bytes(Statement, ColumnIndex);
    return std::string(reinterpret_cast<const char*>(TextPtr), Bytes);
}

//=================

std::vector<uint8_t> SQLite_FieldReader::ExtractBlob(sqlite3_stmt* Statement, int ColumnIndex)
{
    const void* BlobPtr = sqlite3_column_blob(Statement, ColumnIndex);
    const int Size = sqlite3_column_bytes(Statement, ColumnIndex);
    const uint8_t* ByteData = static_cast<const uint8_t*>(BlobPtr);
    return std::vector<uint8_t>(ByteData, ByteData + Size);
}

//=================

void SQLite_FieldReader::ValidateParams(sqlite3_stmt* Statement, int ColumnIndex)
{
    ValidateStatementReadiness(Statement);
    ValidateColumnIndex(Statement, ColumnIndex);
}

//\/\/\/\/\/\/\/\/\/

void SQLite_FieldReader::ValidateColumnIndex(sqlite3_stmt* Statement, int ColumnIndex)
{
    const int ColumnCount = sqlite3_column_count(Statement);
    CONFIRMS(ColumnIndex >= 0 && ColumnIndex < ColumnCount,
            "Invalid ColumnIndex: {}, statement has {} columns", ColumnIndex, ColumnCount);
}

//=================

void SQLite_FieldReader::ValidateStatementReadiness(sqlite3_stmt* Statement)
{
    CONFIRMS(Statement,
        "sqlite3_stmt* Statement is not valid.");

    const int RowCount = sqlite3_data_count(Statement);
    CONFIRMS(RowCount > 0,
        "No active row: statement was either not stepped or has been finalized.");
}



////////////////////


