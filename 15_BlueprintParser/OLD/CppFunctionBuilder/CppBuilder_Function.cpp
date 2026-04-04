
#include "../CppFunctionBuilder/CppBuilder_Function.h"
////////
#include <vector>
#include <string_view>
////////
#include "../CppFunctionBuilder/DataType.h"
#include "../CppFunctionBuilder/FunctionStructs.h"
#include "../CppFunctionBuilder/CppBuilder_FunctionHeader.h"
#include "../CppFunctionBuilder/CppBuilder_FunctionBody.h"


std::optional<std::string> CppBuilder_Function::BuildFunction(const FFunction& FunctionObject)
{
    std::optional<std::string> Header
        = CppBuilder_FunctionHeader::BuildHeader(FunctionObject.Header);
    if (!Header.has_value()) return std::nullopt;

    std::optional<std::string> Body
        = CppBuilder_FunctionBody::BuildBody(FunctionObject.Body);
    if (!Body.has_value()) return std::nullopt;

    return Header.value().append(Body.value());
}
