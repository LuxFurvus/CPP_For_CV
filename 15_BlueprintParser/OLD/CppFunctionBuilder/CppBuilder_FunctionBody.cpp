
#include "../CppFunctionBuilder/CppBuilder_FunctionBody.h"

std::string CppBuilder_FunctionBody::BuildExecResult(
    const std::optional<FFuncBody::FExecBlock::FExecResult>& Result)
{
    if (!Result.has_value()) return {};

    std::string Output{EDataType(Result->ResultType)};
    Output.append(" ").append(Result->ResultName).append(" = ");
    return Output;
}

std::string CppBuilder_FunctionBody::BuildExecScope(
    const std::optional<std::string>& Scope)
{
    if (!Scope.has_value()) return {};
    std::string Output{*Scope};
    Output.append("::");
    return Output;
}

std::string CppBuilder_FunctionBody::BuildArgumentList(
    const std::vector<std::string>& Arguments)
{
    std::string Output{"("};
    for (std::size_t Index = 0; Index < Arguments.size(); ++Index)
    {
        Output.append(Arguments[Index]);
        if (Index + 1 < Arguments.size())
        {
            Output.append(", ");
        }
    }
    Output.append(")").append(FLineFormatting::StatementEnd);
    return Output;
}

std::string CppBuilder_FunctionBody::BuildExecLine(
    const FFuncBody::FExecBlock& ExecBlock)
{
    if (!ExecBlock.IsValid()) return {};

    std::string Output;
    Output
        .append(FLineFormatting::Indent)
        .append(BuildExecResult(ExecBlock.ResultVariable))
        .append(BuildExecScope(ExecBlock.FunctionScope))
        .append(ExecBlock.FunctionName)
        .append(BuildArgumentList(ExecBlock.Arguments))
        .append(FLineFormatting::LineEnd);
    return Output;
}

std::string CppBuilder_FunctionBody::BuildExecBody(
    const std::vector<FFuncBody::FExecBlock>& ExecBlocks)
{
    std::string Output;
    for (const FFuncBody::FExecBlock& Block : ExecBlocks)
    {
        Output.append(BuildExecLine(Block));
    }
    return Output;
}

std::string CppBuilder_FunctionBody::BuildReturnLine(
    const std::vector<std::string>& ReturnArguments)
{
    // 0 arguments → no return line
    if (ReturnArguments.empty()) return {};

    // 1 argument → "return <Arg>;"
    if (ReturnArguments.size() == 1)
    {
        std::string Output;
        Output
            .append(FLineFormatting::Indent)
            .append("return ")
            .append(ReturnArguments.front())
            .append(FLineFormatting::StatementEnd)
            .append(FLineFormatting::LineEnd);
        return Output;
    }

    // 2+ arguments → "return MakeTuple(a, b, ...);"
    std::string Output;
    Output
        .append(FLineFormatting::Indent)
        .append("return MakeTuple(");

    for (std::size_t Index = 0; Index < ReturnArguments.size(); ++Index)
    {
        Output.append(ReturnArguments[Index]);
        if (Index + 1 < ReturnArguments.size())
        {
            Output.append(", ");
        }
    }

    Output
        .append(")")
        .append(FLineFormatting::StatementEnd)
        .append(FLineFormatting::LineEnd);

    return Output;
}

std::optional<std::string> CppBuilder_FunctionBody::BuildBody(
    const FFuncBody& BodyObject)
{
    if (!BodyObject.IsValid()) return std::nullopt;

    const std::string ExecBody
        = BuildExecBody(BodyObject.ExecBlocks);

    const std::string ReturnLine
        = BuildReturnLine(BodyObject.ReturnArguments);

    const std::string_view LastEmptyLine =
        (!ExecBody.empty() && !ReturnLine.empty()) ? FLineFormatting::LineEnd : std::string_view{};

    std::string Output;
    Output
        .append(FLineFormatting::OpenBrace)
        .append(ExecBody)
        .append(LastEmptyLine)
        .append(ReturnLine)
        .append(FLineFormatting::CloseBrace);

    return Output;
}


