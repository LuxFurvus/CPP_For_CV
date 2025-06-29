
#pragma once

#include "SQL_Executor.h"
#include "SQL_TableValues.h"
#include "SQL_StatementSeparator.h"

struct SQL_ScriptExecutor
{
    std::vector<std::pair<std::string, SQL_TableValues2D>> ExecuteScript(
        SQL_Executor& Executor, const std::string &DbPath, const std::string &Script)
    {
        SQL_StatementSeparator StatementSeparator(Script);

        const std::vector<std::string> Statements = StatementSeparator.SplitStatements();

        std::vector<std::pair<std::string, SQL_TableValues2D>> Result;

        for (const std::string& Statement : Statements)
        {
            Result.emplace_back(Statement, Executor.ExecuteQuery(DbPath, Statement));
        }
        return Result;
    }
};
