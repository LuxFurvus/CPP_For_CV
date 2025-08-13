#pragma once
#ifndef SQLITE_FIELDREADER_H
#define SQLITE_FIELDREADER_H

#include <variant>
#include <vector>
#include <cstdint>
#include <string>


///////////////////


struct sqlite3_stmt;
using FieldValue = std::variant<int64_t, double, std::string, std::vector<uint8_t>, std::nullptr_t>;


///////////////////


class SQLite_FieldReader
{
    SQLite_FieldReader() = delete;

private:

    static int64_t ExtractInteger(sqlite3_stmt* Statement, int ColumnIndex);
    static double ExtractDouble(sqlite3_stmt* Statement, int ColumnIndex);
    static std::string ExtractText(sqlite3_stmt* Statement, int ColumnIndex);
    static std::vector<uint8_t> ExtractBlob(sqlite3_stmt* Statement, int ColumnIndex);

    static void ValidateStatementReadiness(sqlite3_stmt* Statement);
    static void ValidateColumnIndex(sqlite3_stmt* Statement, int ColumnIndex);
    static void ValidateParams(sqlite3_stmt* Statement, int ColumnIndex);

public:
    static FieldValue GetFieldValue(sqlite3_stmt* Statement, int ColumnIndex);
};


#endif // SQLITE_FIELDREADER_H
