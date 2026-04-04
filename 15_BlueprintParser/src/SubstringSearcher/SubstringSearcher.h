#pragma once

#include <vector>
#include <optional>
#include <string>
#include <string_view>
////////
struct FIgnoredClosingChar;
struct FIgnoredRangesRules;


struct FSubstringLookupTask
{
    const std::string_view Text;
    const std::vector<std::string_view> Candidates;
    const FIgnoredRangesRules Rules;
    const size_t StartLookupAt = 0;
    const size_t FindMatchOrFailAt = std::string_view::npos;

    bool IsValid() const
    {
        if (Text.empty() || Candidates.empty())
            return false;
        if (StartLookupAt >= Text.size())
            return false;
        return true;
    }
};


class SubstringSearcher
{

protected:

    static std::vector<std::pair<std::string, std::string_view>> NormalizeCandidates(
        const std::vector<std::string_view>& Candidates,
        const FIgnoredRangesRules& Rules);

    static std::optional<FSubstringRange> MatchNormalizedSubstringAt(
        const std::string& NormalizedText,
        const std::string& Target,
        const std::vector<size_t>& Mapping,
        size_t Position);

    static std::pair<FSubstringRange, std::string_view> TryMatchAtPosition(
        const std::string& NormalizedText,
        const std::vector<size_t>& Mapping,
        const std::vector<std::pair<std::string, std::string_view>>& NormalizedCandidates,
        size_t Position);

public:

    static std::pair<FSubstringRange, std::string_view> FindSubtringRangeByRules(
        const FSubstringLookupTask& Task);
};
