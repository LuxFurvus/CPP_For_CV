
#include "../BlueprintToCppMapper/BlueprintToCppMapper_FunctionHeader.h"
//
#include "../BlueprintToCppMapper/BlueprintToCppStructs.h"


FFuncHeader BlueprintToCppMapper_FunctionHeader::GetFunctionHeader(
    const FSeparatedNodes &SeparatedNodes)
{
    const auto& EntryNode
        = SeparatedNodes.FunctionEntryNode.value_or(FBlueprintNodeObject{});

    const auto& ResultNode
        = SeparatedNodes.FunctionResultNode.value_or(FBlueprintNodeObject{});

    return FFuncHeader
    {
        .ReturnType = ResultNode.GetTypePinCategories(),
        .FuncName = EntryNode.GetNodeName(),
        .Params = EntryNode.GetFunctionParameters()
    };
}