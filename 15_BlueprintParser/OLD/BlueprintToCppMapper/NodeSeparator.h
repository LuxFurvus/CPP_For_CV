#pragma once

#include <vector>
#include <optional>
//
#include "../BlueprintToCppMapper/BlueprintToCppStructs.h"

struct FSeparatedNodes
{
    const std::optional<FBlueprintNodeObject> FunctionEntryNode;
    const std::optional<FBlueprintNodeObject> FunctionResultNode;
    const std::vector<FBlueprintNodeObject> CallFunctionNodes;
};

class NodeSeparator
{
private:

    static std::optional<FBlueprintNodeObject> GetNodeByType(
        const std::vector<FBlueprintNodeObject>& Nodes,
        const ENodeType NodeType);

    static std::optional<FBlueprintNodeObject> GetFunctionEntryNode(
        const std::vector<FBlueprintNodeObject>& Nodes);

    static std::optional<FBlueprintNodeObject> GetFunctionResultNode(
        const std::vector<FBlueprintNodeObject>& Nodes);

    static std::vector<FBlueprintNodeObject> GetFunctionBodyNodes(
        const std::vector<FBlueprintNodeObject>& Nodes);

public:

    static FSeparatedNodes SeparateNodes(
        const std::vector<FBlueprintNodeObject>& Nodes);
};
