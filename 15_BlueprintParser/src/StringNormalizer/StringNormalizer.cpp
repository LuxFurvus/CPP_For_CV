#include "StringNormalizer.h"

std::string StringNormalizer::ProcessCharByChar(
    const std::string_view Text,
    const std::function<char(char)>& Processor)
{
    std::string Result;
    Result.reserve(Text.size());

    for (char Ch : Text)
    {
        Result.push_back(Processor(Ch));
    }

    return Result;
}

std::string StringNormalizer::ToLowerCase(
    const std::string_view Text)
{
    const std::string Lowered = ProcessCharByChar(
        Text, [](char Ch) { return static_cast<char>(tolower(Ch)); });

    return Lowered;
}

std::string StringNormalizer::RemoveIgnoredChars(
    const std::string_view Text,
    const std::function<bool(const char)>& IgnoredCharPredicate)
{
    std::string Result;
    Result.reserve(Text.size());

    for (char Ch : Text)
    {
        if (!IgnoredCharPredicate(Ch))
        {
            Result.push_back(Ch);
        }
    }

    return Result;
}


static std::vector<size_t> ExtractNonIgnoredPositions(
    std::string_view Text,
    const std::vector<std::pair<char,char>>& IgnoredRanges)
{
    struct FActiveRange { char ClosingChar; };
    std::vector<FActiveRange> ActiveRanges;

    std::vector<size_t> Positions;
    Positions.reserve(Text.size());

    for (size_t i = 0; i < Text.size(); ++i)
    {
        char Ch = Text[i];

        // 1. Close any matching range
        bool bClosed = false;
        for (int idx = int(ActiveRanges.size()) - 1; idx >= 0; --idx)
        {
            if (Ch == ActiveRanges[idx].ClosingChar)
            {
                ActiveRanges.erase(ActiveRanges.begin() + idx);
                bClosed = true;
                break;
            }
        }
        if (bClosed) continue;

        // 2. Open a new range when encountering an opening delimiter
        bool bOpened = false;
        for (auto const& Range : IgnoredRanges)
        {
            if (Ch == Range.first)
            {
                ActiveRanges.push_back({ Range.second });
                bOpened = true;
                break;
            }
        }
        if (bOpened) continue;

        // 3. Record position only when not inside any range
        if (ActiveRanges.empty())
        {
            Positions.push_back(i);
        }
    }

    return Positions;
}



std::string StringNormalizer::RemoveAllIgnoredRanges(
    const std::string_view Text,
    const std::vector<std::pair<char,char>>& IgnoredRanges)
{
    const std::vector<size_t> Positions =
        ExtractNonIgnoredPositions(Text, IgnoredRanges);

    std::string Result;
    Result.reserve(Positions.size());

    for (const size_t idx : Positions)
    {
        Result.push_back(Text[idx]);
    }

    return Result;
}


std::pair<std::string, std::vector<size_t>> StringNormalizer::NormalizeTextWithMapping(
    std::string_view Text,
    const FIgnoredRangesRules& Rules)
{
    std::vector<size_t> Positions =
        ExtractNonIgnoredPositions(Text, Rules.IgnoredRangesBetween);

    std::string OutNormalizedText;
    OutNormalizedText.reserve(Positions.size());

    std::vector<size_t> OutMapping;
    OutMapping.reserve(Positions.size());

    for (size_t idx : Positions)
    {
        char Ch = Text[idx];
        if (Rules.IgnoredChar(Ch))
            continue;

        char NormCh = Rules.bCaseInsensitive
            ? static_cast<char>(tolower(Ch))
            : Ch;

        OutNormalizedText.push_back(NormCh);
        OutMapping.push_back(idx);
    }

    return { std::move(OutNormalizedText), std::move(OutMapping) };
}


std::string StringNormalizer::NormalizeStringByRules(
    const std::string_view Text,
    const FIgnoredRangesRules& Rules)
{
    const std::string Stage1
        = (Rules.bCaseInsensitive)
            ? ToLowerCase(Text)
            : std::string(Text);

    const std::string Stage2
        = RemoveAllIgnoredRanges(Stage1, Rules.IgnoredRangesBetween);

        const std::string FinalResult
        = RemoveIgnoredChars(Stage2, Rules.IgnoredChar);

    return FinalResult;
}
