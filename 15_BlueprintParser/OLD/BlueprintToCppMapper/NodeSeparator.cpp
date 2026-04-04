
#include "../BlueprintToCppMapper/NodeSeparator.h"


std::optional<FBlueprintNodeObject> NodeSeparator::GetNodeByType(
    const std::vector<FBlueprintNodeObject>& Nodes,
    const ENodeType NodeType)
{
    for (const auto& Node : Nodes)
    {
        if (Node.NodeType != NodeType) continue;
        return Node;
    }
    return {};
}

std::optional<FBlueprintNodeObject> NodeSeparator::GetFunctionEntryNode(
    const std::vector<FBlueprintNodeObject>& Nodes)
{
    return GetNodeByType(Nodes, ENodeType::FunctionEntry);
}

std::optional<FBlueprintNodeObject> NodeSeparator::GetFunctionResultNode(
    const std::vector<FBlueprintNodeObject>& Nodes)
{
    return GetNodeByType(Nodes, ENodeType::FunctionResult);
}

std::vector<FBlueprintNodeObject> NodeSeparator::GetFunctionBodyNodes(
    const std::vector<FBlueprintNodeObject>& Nodes)
{
    std::vector<FBlueprintNodeObject> CallFunctionNodes;
    for (const auto& Node : Nodes)
    {
        const bool bIsFunctionBodyNode = true
            && Node.NodeType != ENodeType::FunctionEntry
            && Node.NodeType != ENodeType::FunctionResult;
        if (!bIsFunctionBodyNode) continue;
        CallFunctionNodes.emplace_back(Node);
    }
    return CallFunctionNodes;
}

FSeparatedNodes NodeSeparator::SeparateNodes(
    const std::vector<FBlueprintNodeObject>& Nodes)
{
    const FSeparatedNodes SeparatedNodes
    {
        .FunctionEntryNode = GetFunctionEntryNode(Nodes),
        .FunctionResultNode = GetFunctionResultNode(Nodes),
        .CallFunctionNodes = GetFunctionBodyNodes(Nodes)
    };

    return SeparatedNodes;
}
