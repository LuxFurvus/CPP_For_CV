#include "SQL_ScriptExecutor.h"

std::vector<std::pair<std::string, SQL_TableValues2D>>
    SQL_ScriptExecutor::ExecuteScript
        (SQL_ExecutorInterface &Executor, const std::string &DbPath, const std::string &Script)
{
    SQL_StatementSeparator StatementSeparator(Script);
    std::vector<std::string> Statements = StatementSeparator.SplitStatements();

    std::vector<std::pair<std::string, SQL_TableValues2D>> Result;
    Result.reserve(Statements.size());

    for (std::string& Statement : Statements)
    {
        Result.emplace_back(std::move(Statement), Executor.ExecuteQuery(DbPath, Statement));
    }

    return Result;
}

// USE THIS TO SOLVE WARNING FROM CPPCHECK ABOUT OPTION TO USE STD::TRANSFORM

    // std::ranges::transform(Statements, std::back_inserter(Result),
    // [&Executor, &DbPath](std::string& OneStatement)
    // {
    //     return std::make_pair(std::move(OneStatement),
    //         Executor.ExecuteQuery(DbPath, OneStatement));
    // });
