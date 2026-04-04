#pragma once

#include "../CppFunctionBuilder/FunctionStructs.h"
#include "../BlueprintToCppMapper/NodeSeparator.h"


class BlueprintToCppMapper_FunctionHeader
{

public:

    static FFuncHeader GetFunctionHeader(
        const FSeparatedNodes& SeparatedNodes);

};
