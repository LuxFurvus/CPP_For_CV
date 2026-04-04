#pragma once

#include <string>
#include <optional>
////////
#include "../CppFunctionBuilder/FunctionStructs.h"


class CppBuilder_Function
{

public:

    static std::optional<std::string> BuildFunction(
        const FFunction& FunctionObject);

};
