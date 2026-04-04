#pragma once

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <limits>
////////
#include "../SubstringExtractor/BaseParseStructures.h"

struct FParseFieldParams
{
    const std::string_view SourceBlock;
    const std::string_view FieldKey;
    const FIgnoredRangesRules IgnoredRules{};
    const std::vector<std::pair<char, char>> Delimiters{};

    bool IsValid() const
    {
        return !SourceBlock.empty() && !FieldKey.empty();
    }
};

class FieldParser
{

private:

    static std::optional<size_t> FindKeyEndPos(
        const FParseFieldParams& ParseParams);

    static std::optional<std::string_view> GetValuePart(
        const FParseFieldParams& ParseParams);

    static std::optional<uint64_t> ConvertStringToNumber(
        const std::string_view NumberString);

    static std::string CollectDigitsInString(
        const std::string_view InString,
        const std::function<bool(char)> IgnoredChars);

    static std::optional<std::string_view> GetFirstEnclosedSubstring(
        const std::string_view ValueBlock,
        const std::pair<char, char>& Delimiters);

    static std::vector<std::string_view> CollectEnclosedStrings(
        const std::string_view ValueBlock,
        const std::vector<std::pair<char, char>> &Delimiters);

public:

    static std::optional<bool> ParseFieldBool(
        const FParseFieldParams &ParseParams);

    static std::optional<uint64_t> ParseNumericField(
        const FParseFieldParams &ParseParams);

    static std::optional<FBlueprintGuid> ParseGuidField(
        const FParseFieldParams &ParseParams);

    static std::vector<std::string_view> ParseFieldNestedStrings(
        const FParseFieldParams &ParseParams,
        const std::vector<std::pair<char, char>> &Delimiters);

    [[deprecated]] static std::optional<bool> ParseFieldBoolv0(
        const FParseFieldParams& ParseParams);

};
