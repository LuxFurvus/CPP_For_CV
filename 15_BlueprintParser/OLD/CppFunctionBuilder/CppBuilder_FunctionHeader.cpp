
#include "../CppFunctionBuilder/CppBuilder_FunctionHeader.h"
#include "CppBuilder_FunctionHeader.h"


std::string CppBuilder_FunctionHeader::BuildOneParam(
    const FFuncHeader::FFunctionParam& Param)
{
    std::string Output;

    Output
        .append(EDataType(Param.ParamType))
        .append(" ")
        .append(Param.ParamName);

    if (Param.DefaultValue.has_value())
    {
        Output.append(" = ").append(Param.DefaultValue.value());
    }

    return Output;
}

std::string CppBuilder_FunctionHeader::BuildParams(
    const std::vector<FFuncHeader::FFunctionParam>& Params)
{
    std::string Output{"("};
    for (std::size_t Index = 0; Index < Params.size(); ++Index)
    {
        Output.append(BuildOneParam(Params[Index]));
        if (Index + 1 < Params.size())
        {
            Output.append(", ");
        }
    }
    Output.append(")");
    return Output;
}

std::string CppBuilder_FunctionHeader::BuildReturnType(
    const std::vector<EDataType>& ReturnType)
{
    if (ReturnType.empty()) return static_cast<std::string>(EDataType::Void);

    if (ReturnType.size() == 1)
    {
        const EDataType Type = ReturnType.front();

        if (!Type.IsValidType()) return static_cast<std::string>(EDataType::None);

        return static_cast<std::string>(Type);
    }

    std::string Output;
    bool HasValid = false;

    Output += "TTuple<";
    for (std::size_t i = 0; i < ReturnType.size(); ++i)
    {
        const EDataType Type = ReturnType[i];

        if (!Type.IsValidType() || Type.IsVoid()) continue;

        if (HasValid) Output.append(", ");

        const std::string_view TypeName = Type;
        Output.append(TypeName.data(), TypeName.size());
        HasValid = true;
    }
    Output += ">";

    if (!HasValid) return static_cast<std::string>(EDataType::None);

    return Output;
}

std::string CppBuilder_FunctionHeader::MakeFuncName(
    const std::optional<std::string>& InName)
{
    if (!InName.has_value() || InName->empty())
    {
        return "UnknownBlueprintFunction";
    }
    return *InName;
}

std::optional<std::string> CppBuilder_FunctionHeader::BuildHeader(
    const FFuncHeader& HeaderObject)
{
    const std::string ReturnType
        = BuildReturnType(HeaderObject.ReturnType);

    const std::string FuncName
            = MakeFuncName(HeaderObject.FuncName);

    const std::string Params
        = BuildParams(HeaderObject.Params);

    std::string Output;
    Output
        .append(ReturnType)
        .append(" ")
        .append(FuncName)
        .append(Params)
        .append(FLineFormatting::LineEnd);
    return Output;
}

