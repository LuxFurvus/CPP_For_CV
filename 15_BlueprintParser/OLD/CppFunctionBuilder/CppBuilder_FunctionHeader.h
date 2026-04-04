
#pragma once

#include <string>
#include <optional>
////////
#include "../CppFunctionBuilder/FunctionStructs.h"


class CppBuilder_FunctionHeader
{
private:

    static std::string BuildOneParam(
        const FFuncHeader::FFunctionParam& Param);

    static std::string BuildParams(
        const std::vector<FFuncHeader::FFunctionParam>& Params);

    static std::string BuildReturnType(
        const std::vector<EDataType>& ReturnType);

    static std::string MakeFuncName(
        const std::optional<std::string> &InName);

public:

    static std::optional<std::string> BuildHeader(
        const FFuncHeader& HeaderObject);

};
