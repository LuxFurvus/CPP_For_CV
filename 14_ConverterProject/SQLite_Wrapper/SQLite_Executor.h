#pragma once
#ifndef SQLITE_EXECUTOR_H
#define SQLITE_EXECUTOR_H

#include <variant>
#include <vector>
#include <cstdint>
////////
#include "../SQL_Generals/SQL_Executor.h"


///////////////////


class SQLite_Statement;
typedef struct sqlite3_stmt sqlite3_stmt;

using FieldValue = std::variant<int64_t, double, std::string, std::vector<uint8_t>, std::nullptr_t>;
using TableValues1D = std::vector<FieldValue>;
using TableValues2D = std::vector<TableValues1D>;


///////////////////


class SQLite_Executor : public SQL_Executor
{
private:
    TableValues1D GetOneRow(sqlite3_stmt* RawStmt, const int ColumnCount);
    SQL_TableValues2D GetQueryResult(SQLite_Statement& Statement);

public:
    SQLite_Executor() = default;
    SQL_TableValues2D ExecuteQuery(const std::string& DbPath, const std::string& Query) override;
};


#endif