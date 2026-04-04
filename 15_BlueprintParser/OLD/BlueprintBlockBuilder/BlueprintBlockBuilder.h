#pragma once

#include <string>
#include <vector>
#include <string_view>

struct FBlueprintNodeBlocks
{
    const std::string HeaderBlock;
    const std::vector<std::string> CustomPropertyBlocks;

    bool IsValid() const;
};

class BlueprintBlockBuilder
{
protected:

    static std::vector<std::string> GetNodeBlocks(
        const std::string_view FullBlueprintText);

    static std::vector<std::string> GetCustomPropertiesBlocks(
        const std::string_view FullBlueprintText);

    static std::string GetNodeHeaderBlocks(
        const std::string_view FullBlueprintText);

public:

    static std::vector<FBlueprintNodeBlocks> BuildNodeBlockObjects(
        const std::string_view FullBlueprintText);
};
