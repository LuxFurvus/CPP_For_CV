#pragma once
// StringNormalizer.h

#include <string>
#include <vector>
#include <functional>
////////
#include "../SubstringExtractor/BaseParseStructures.h"

class StringNormalizer
{
public:

    static std::string ProcessCharByChar(
        const std::string_view Text,
        const std::function<char(char)> &Processor);

    static std::string ToLowerCase(
        const std::string_view Text);

    static std::string RemoveIgnoredChars(
        const std::string_view Text,
        const std::function<bool(const char)>& IgnoredCharPredicate);

    static std::pair<std::string, std::vector<size_t>> NormalizeTextWithMapping(
        std::string_view Text,
        const FIgnoredRangesRules& Rules);

    static std::string RemoveAllIgnoredRanges(
        const std::string_view Text,
        const std::vector<std::pair<char, char>>& IgnoredRanges);

    static std::string NormalizeStringByRules(
        const std::string_view Text,
        const FIgnoredRangesRules& Rules);
};
