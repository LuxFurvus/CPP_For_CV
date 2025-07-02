
#include "SQLite_Executor.h"
////////
#include <sqlite3.h>
////////
#include "../SQL_Generals/SQL_TableValues.h"
#include "../SQL_Generals/SQL_Executor.h"
#include "../ConditionChecker/ConditionChecker.h"
////////
#include "SQLite_Statement.h"
#include "SQLite_FieldReader.h"
#include "SQLite_ConnectionPool.h"


////////////////////


SQL_TableValues2D SQLite_Executor::ExecuteQuery(const std::string& DbPath, const std::string& Query)
{
    CONFIRM(!DbPath.empty());
    CONFIRM(!Query.empty());

    const SQLite_ConnectionCreationKit ConnectionKit{ DbPath, false, true };

    const std::shared_ptr<SQLite_DbConnection> Connection
        = SQLite_ConnectionPool::GetConnection(ConnectionKit);

    CONFIRM(Connection);

    const SQLite_StatementCreationKit StatementKit = Connection->MakeStatementKit();

    SQLite_Statement Statement(StatementKit, Query.c_str());

    CONFIRM(Statement.IsValid());

    return GetQueryResult(Statement);
}

//\/\/\/\/\/\/\/\/\/

SQL_TableValues2D SQLite_Executor::GetQueryResult(SQLite_Statement& Statement)
{
    CONFIRM(Statement.IsValid());

    sqlite3_stmt* RawStmt = Statement.Get();
    CONFIRMS(RawStmt, "RawStmt is not valid.");

    TableValues2D ResultValues2D;

    const int ColumnCount = sqlite3_column_count(RawStmt);

    while (Statement.Step())
    {
        ResultValues2D.emplace_back(GetOneRow(RawStmt, ColumnCount));
    }

    return SQL_TableValues2D(ResultValues2D);
}

//\/\/\/\/\/\/\/\/\/

TableValues1D SQLite_Executor::GetOneRow(sqlite3_stmt* RawStmt, const int ColumnCount)
{
    TableValues1D Row;

    for (int ColumnIndex = 0; ColumnIndex < ColumnCount; ++ColumnIndex)
    {
        Row.emplace_back(SQLite_FieldReader::GetFieldValue(RawStmt, ColumnIndex));
    }

    return Row;
}
