
#pragma once

#include <optional>
#include <string>
//
#include "../BlueprintToCppMapper/NodeSeparator.h"


class BlueprintToCppMapper_FunctionBody
{
public:

    static std::vector<std::string> GetReturnArgument(
        const FSeparatedNodes& SeparatedNodes)
    {
        const auto& FunctionResultNode
            = SeparatedNodes.FunctionResultNode;

        if (!FunctionResultNode.has_value()) return {};

        const FBlueprintNodeObject& ResultNode
            = FunctionResultNode.value();

        const std::vector<std::string> TypePinNames
            = ResultNode.GetTypePinNames();

        return TypePinNames;
    }

};