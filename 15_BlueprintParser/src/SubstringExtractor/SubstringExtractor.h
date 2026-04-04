#pragma once

#include <string>
#include <optional>
////////
#include "../SubstringExtractor/BaseParseStructures.h"
////////
struct FSubstringRange;
struct FExtractionRules;


class SubstringExtractor
{

protected:

    static std::optional<std::string> ExtractSubstringByRange(
        const std::string_view Text,
        const FSubstringRange& Range);

    static std::optional<std::string_view> ExtractSubstringViewByRange(
        std::string_view Text,
        const FSubstringRange& Range);

    static FSubstringRange GetStartRange(
        std::string_view Text, const FExtractionRules& Rules);

    static FSubstringRange GetEndRange(
        std::string_view Text,
        const FExtractionRules& Rules,
        const size_t NewSearchStart);

    static bool AreEssentialMarkersFound(
        std::string_view Text,
        const FExtractionRules& Rules,
        const FSubstringRange& InnerRange);

    static FSubstringRange GetRangeByRules(
        const std::string_view Text,
        const FExtractionRules& Rules);

public:

    static std::pair<FSubstringRange, std::optional<std::string>>
        ExtractFirstSubstring(
            std::string_view Text,
            const FExtractionRules& Rules);

    static std::vector<std::string_view> ExtractAllSubstrings(
        std::string_view Text,
        const FExtractionRules& Rules);
};
