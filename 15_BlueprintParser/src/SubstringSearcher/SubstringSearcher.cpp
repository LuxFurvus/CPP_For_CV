
#include "../SubstringSearcher/SubstringSearcher.h"
////////
#include "../SubstringExtractor/BaseParseStructures.h"
#include "../StringNormalizer/StringNormalizer.h"


std::vector<std::pair<std::string, std::string_view>> SubstringSearcher::NormalizeCandidates(
    const std::vector<std::string_view>& Candidates,
    const FIgnoredRangesRules& Rules)
{
    std::vector<std::pair<std::string, std::string_view>> Result;
    Result.reserve(Candidates.size());

    for (const std::string_view Candidate : Candidates)
    {
        const std::string Normalized = StringNormalizer::NormalizeStringByRules(Candidate, Rules);
        if (!Normalized.empty())
        {
            Result.emplace_back(Normalized, Candidate);
        }
    }

    return Result;
}

std::optional<FSubstringRange> SubstringSearcher::MatchNormalizedSubstringAt(
    const std::string& NormalizedText,
    const std::string& Target,
    const std::vector<size_t>& Mapping,
    size_t Position)
{
    const size_t Length = Target.size();
    if (Position + Length > NormalizedText.size()) return std::nullopt;

    if (NormalizedText.compare(Position, Length, Target) == 0)
    {
        const size_t EndPos = Position + Length - 1;
        if (EndPos >= Mapping.size()) return std::nullopt;

        const size_t Start = Mapping[Position];
        const size_t End = Mapping[EndPos] + 1;
        return FSubstringRange{ Start, End };
    }

    return std::nullopt;
}

std::pair<FSubstringRange, std::string_view> SubstringSearcher::TryMatchAtPosition(
    const std::string& NormalizedText,
    const std::vector<size_t>& Mapping,
    const std::vector<std::pair<std::string, std::string_view>>& NormalizedCandidates,
    size_t Position)
{
    for (const auto& [Normalized, Original] : NormalizedCandidates)
    {
        if (auto Match = MatchNormalizedSubstringAt(NormalizedText, Normalized, Mapping, Position))
        {
            return { *Match, Original };
        }
    }

    return { {}, std::string_view{} };
}

std::pair<FSubstringRange, std::string_view> SubstringSearcher::FindSubtringRangeByRules(
    const FSubstringLookupTask& Task)
{
    if (!Task.IsValid())
    {
        return { {}, std::string_view{} };
    }

    const std::string_view EffectiveText = Task.Text.substr(Task.StartLookupAt);

    const size_t FindMatchOrFailAt
        = (Task.FindMatchOrFailAt == std::string_view::npos)
        ? EffectiveText.size()
        : std::min(Task.FindMatchOrFailAt + 1, EffectiveText.size());

    const auto NormalizedCandidates = NormalizeCandidates(Task.Candidates, Task.Rules);
    if (NormalizedCandidates.empty()) return { {}, std::string_view{} };

    const auto [NormalizedText, Mapping]
        = StringNormalizer::NormalizeTextWithMapping(EffectiveText, Task.Rules);
    if (Mapping.empty()) return { {}, std::string_view{} };

    for (size_t i = 0; i < FindMatchOrFailAt; ++i)
    {
        const auto [Range, Matched]
            = TryMatchAtPosition(NormalizedText, Mapping, NormalizedCandidates, i);
        if (!Range.IsValid()) continue;

        const FSubstringRange AdjustedRange {
            Range.Start + Task.StartLookupAt,
            Range.End + Task.StartLookupAt
        };
        return { AdjustedRange, Matched };
    }

    return { {}, std::string_view{} };
}
