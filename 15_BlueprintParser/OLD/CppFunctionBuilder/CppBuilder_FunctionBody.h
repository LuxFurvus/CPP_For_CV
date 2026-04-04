
#pragma once

#include <string>
#include <optional>
////////
#include "../CppFunctionBuilder/FunctionStructs.h"


class CppBuilder_FunctionBody
{
private:

    static std::string BuildExecResult(
        const std::optional<FFuncBody::FExecBlock::FExecResult>& Result);

    static std::string BuildExecScope(
        const std::optional<std::string>& Scope);

    static std::string BuildArgumentList(
        const std::vector<std::string>& Arguments);

    static std::string BuildExecLine(
        const FFuncBody::FExecBlock& ExecBlock);

    static std::string BuildExecBody(
        const std::vector<FFuncBody::FExecBlock>& ExecBlocks);

    static std::string BuildReturnLine(
        const std::vector<std::string>& ReturnArguments);

public:

    static std::optional<std::string> BuildBody(
        const FFuncBody& BodyObject);

};
