
#include "../BlueprintBlockBuilder/BlueprintBlockBuilder.h"
////////
#include "../SubstringExtractor/BaseParseStructures.h"
#include "../SubstringExtractor/SubstringExtractor.h"
#include "../BlueprintBlockBuilder/BlueprintBlockParsingRules.h"

bool FBlueprintNodeBlocks::IsValid() const
{
    return !HeaderBlock.empty() && !CustomPropertyBlocks.empty();
}

std::vector<std::string> BlueprintBlockBuilder::GetNodeBlocks(
    const std::string_view FullBlueprintText)
{
    const FExtractionRules Rules {
        .BlockMarkers = BlueprintBlockParsingRules::Nodes_BlockMarkers,
        .IgnoredRangesRules = BlueprintBlockParsingRules::Nodes_IgnoreRules
    };
    return SubstringExtractor::ExtractAllSubstrings(FullBlueprintText, Rules);
}

std::vector<std::string> BlueprintBlockBuilder::GetCustomPropertiesBlocks(
    const std::string_view FullBlueprintText)
{
    const FExtractionRules Rules {
        .BlockMarkers = BlueprintBlockParsingRules::CustomProperties_BlockMarkers,
        .IgnoredRangesRules = BlueprintBlockParsingRules::CustomProperties_IgnoreRules
    };
    return SubstringExtractor::ExtractAllSubstrings(FullBlueprintText, Rules);
}

std::string BlueprintBlockBuilder::GetNodeHeaderBlocks(
    const std::string_view FullBlueprintText)
{
    const FExtractionRules Rules {
        .BlockMarkers = BlueprintBlockParsingRules::NodeHeader_BlockMarkers,
        .IgnoredRangesRules = BlueprintBlockParsingRules::NodeHeader_IgnoreRules
    };
    const auto [_, Extracted]
        = SubstringExtractor::ExtractFirstSubstring(FullBlueprintText, Rules);
    if (!Extracted.has_value()) return {};

    return Extracted.value();
}

std::vector<FBlueprintNodeBlocks> BlueprintBlockBuilder::BuildNodeBlockObjects(
    const std::string_view FullBlueprintText)
{
    const std::vector<std::string> NodeBlocks
        = GetNodeBlocks(FullBlueprintText);
    if (NodeBlocks.empty()) return {};

    std::vector<FBlueprintNodeBlocks> NodeBlocksObjects;

    for (const std::string& NodeBlock : NodeBlocks)
    {
        const std::string HeaderBlock
            = GetNodeHeaderBlocks(NodeBlock);

        const std::vector<std::string> CustomPropertyBlocks
            = GetCustomPropertiesBlocks(NodeBlock);

        NodeBlocksObjects.emplace_back(HeaderBlock, CustomPropertyBlocks);
    }

    return NodeBlocksObjects;
}
