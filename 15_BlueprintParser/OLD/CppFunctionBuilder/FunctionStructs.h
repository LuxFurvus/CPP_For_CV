#pragma once

#include <string>
#include <vector>
#include <optional>
#include <string_view>
////////
#include "../CppFunctionBuilder/DataType.h"


struct FFuncHeader
{
    struct FFunctionParam
    {
        const std::string ParamName;
        const EDataType ParamType;
        const std::optional<std::string> DefaultValue;
    };

    const std::vector<EDataType> ReturnType;
    const std::optional<std::string> FuncName;
    const std::vector<FFunctionParam> Params;
};

struct FFuncBody
{
    struct FExecBlock
    {
        struct FExecResult
        {
            const EDataType ResultType;
            const std::string ResultName;
            bool IsValid() const
            {
                return !ResultName.empty() && ResultType < EDataType::MAX;
            }
        };
        const std::optional<FExecResult> ResultVariable;
        const std::optional<std::string> FunctionScope;
        const std::string FunctionName;
        const std::vector<std::string> Arguments;

        bool IsValid() const
        {
            return !FunctionName.empty();
        }
    };

    const std::vector<FExecBlock> ExecBlocks{};
    const std::vector<std::string> ReturnArguments{};

    bool IsValid() const
    {
        return !ExecBlocks.empty() || !ReturnArguments.empty();
    }
};

struct FFunction
{
    const FFuncHeader Header;
    const FFuncBody Body;

    bool IsValid() const
    {
        return Body.IsValid();
    }
};

struct FLineFormatting
{
    static constexpr std::string_view LineEnd = "\n";
    static constexpr std::string_view Indent = "    ";
    static constexpr std::string_view BlankLine = "\n\n";
    static constexpr std::string_view OpenBrace = "{\n";
    static constexpr std::string_view CloseBrace = "}";
    static constexpr std::string_view StatementEnd = ";";
};
