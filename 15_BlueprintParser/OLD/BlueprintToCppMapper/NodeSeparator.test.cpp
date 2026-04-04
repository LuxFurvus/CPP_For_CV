
#include "../BlueprintToCppMapper/NodeSeparator.h"
//
#include <gtest/gtest.h>
#include <optional>
//
#include "BlueprintToCppStructs.h"
#include "NodeSeparator.h"

namespace
{

FBlueprintNodeObject MakeNode(const ENodeType NodeType, const std::string& NodeName = "")
{
    return FBlueprintNodeObject{
        .NodeType = NodeType,
        .FunctionReference = {.MemberName = NodeName, .MemberParent = ""},
        .Pins = {}
    };
}

const auto Call_SeparateNodes = [](auto&& Param1)
{
    return NodeSeparator::SeparateNodes(Param1);
};

} // namespace

TEST(NodeSeparator, T01_EmptyInputYieldsEmptyOutput)
{
    SCOPED_TRACE("T01: Returns empty results when input is empty");

    // Empty input
    {
        const std::vector<FBlueprintNodeObject> Nodes;
        const FSeparatedNodes Result = Call_SeparateNodes(Nodes);

        const bool bIsEntryEmpty = !Result.FunctionEntryNode.has_value();
        const bool bIsResultEmpty = !Result.FunctionResultNode.has_value();
        const bool bIsBodyEmpty = Result.CallFunctionNodes.empty();

        EXPECT_TRUE(bIsEntryEmpty);
        EXPECT_TRUE(bIsResultEmpty);
        EXPECT_TRUE(bIsBodyEmpty);
    }
}

TEST(NodeSeparator, T02_OnlyEntryAndResultNodesSeparatedCorrectly)
{
    SCOPED_TRACE("T02: Correctly separates entry and result when only those are present");

    {
        // Entry and result only
        const std::vector<FBlueprintNodeObject> Nodes {
            MakeNode(ENodeType::FunctionEntry),
            MakeNode(ENodeType::FunctionResult)
        };
        const FSeparatedNodes Result = Call_SeparateNodes(Nodes);

        const bool bEntryIsSet = Result.FunctionEntryNode.has_value();
        const bool bResultIsSet = Result.FunctionResultNode.has_value();
        const bool bBodyEmpty = Result.CallFunctionNodes.empty();

        EXPECT_TRUE(bEntryIsSet);
        EXPECT_TRUE(bResultIsSet);
        EXPECT_TRUE(bBodyEmpty);
    }
}

TEST(NodeSeparator, T03_OnlyBodyNodesGoIntoFunctionBodyList)
{
    SCOPED_TRACE("T03: Only body nodes present go into CallFunctionNodes");

    {
        // CallFunction and None
        const std::vector<FBlueprintNodeObject> Nodes {
            MakeNode(ENodeType::CallFunction),
            MakeNode(ENodeType::None)
        };
        const FSeparatedNodes Result = Call_SeparateNodes(Nodes);

        const bool bEntryEmpty = !Result.FunctionEntryNode.has_value();
        const bool bResultEmpty = !Result.FunctionResultNode.has_value();
        const size_t BodyCount = Result.CallFunctionNodes.size();

        EXPECT_TRUE(bEntryEmpty);
        EXPECT_TRUE(bResultEmpty);
        EXPECT_EQ(BodyCount, 2);
    }
}

TEST(NodeSeparator, T04_MixedNodeTypesSeparatedCorrectly)
{
    SCOPED_TRACE("T04: All node types separated correctly");

    {
        // Mixed input
        const std::vector<FBlueprintNodeObject> Nodes {
            MakeNode(ENodeType::FunctionEntry),
            MakeNode(ENodeType::CallFunction),
            MakeNode(ENodeType::FunctionResult),
            MakeNode(ENodeType::CallFunction),
            MakeNode(ENodeType::None)
        };
        const FSeparatedNodes Result = Call_SeparateNodes(Nodes);

        const bool bEntryIsSet = Result.FunctionEntryNode.has_value();
        const bool bResultIsSet = Result.FunctionResultNode.has_value();
        const size_t BodyCount = Result.CallFunctionNodes.size();

        EXPECT_TRUE(bEntryIsSet);
        EXPECT_TRUE(bResultIsSet);
        EXPECT_EQ(BodyCount, 3);
    }
}

TEST(NodeSeparator, T05_PicksFirstEntryAndFirstResult)
{
    SCOPED_TRACE("T05: When multiple nodes of same type exist, only first one is picked");

        const std::vector<FBlueprintNodeObject> Nodes {
            MakeNode(ENodeType::FunctionEntry, "FirstEntry"),
            MakeNode(ENodeType::FunctionEntry, "SecondEntry"),
            MakeNode(ENodeType::FunctionResult, "FirstResult"),
            MakeNode(ENodeType::FunctionResult, "SecondResult"),
            MakeNode(ENodeType::CallFunction)
        };

        const FSeparatedNodes Result = Call_SeparateNodes(Nodes);

        const bool bEntryMatchesFirst = true
            && Result.FunctionEntryNode.has_value()
            && Result.FunctionEntryNode->FunctionReference.MemberName == "FirstEntry";

        const bool bResultMatchesFirst = true
            && Result.FunctionResultNode.has_value()
            && Result.FunctionResultNode->FunctionReference.MemberName == "FirstResult";

        const size_t BodyCount = Result.CallFunctionNodes.size();

        EXPECT_TRUE(bEntryMatchesFirst);
        EXPECT_TRUE(bResultMatchesFirst);
        EXPECT_EQ(BodyCount, 1);
}
