
#pragma once

#include "SQL_Executor.h"
#include "SQL_TableValues.h"
#include "SQL_StatementSeparator.h"

struct SQL_ScriptExecutor
{
    static std::vector<std::pair<std::string, SQL_TableValues2D>> ExecuteScript(
        SQL_ExecutorInterface& Executor, const std::string &DbPath, const std::string &Script);
};
