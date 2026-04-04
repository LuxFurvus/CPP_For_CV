
#include "../SubstringExtractor/SubstringExtractor.h"
////////
#include "../StringNormalizer/StringNormalizer.h"
#include "../SubstringSearcher/SubstringSearcher.h"
#include "../SubstringExtractor/BaseParseStructures.h"

std::optional<std::string> SubstringExtractor::ExtractSubstringByRange(
    const std::string_view Text,
    const FSubstringRange& Range)
{
    if (Text.empty() || Range.End > Text.size()) return std::nullopt;

    const bool IsStartMissing = (Range.Start == std::string::npos);
    const size_t RangeStart = IsStartMissing ? Range.End : Range.Start;

    if (RangeStart >= Text.size()) return std::nullopt;

    const size_t RangeLength = IsStartMissing
        ? (Text.size() - RangeStart)
        : (Range.End > RangeStart ? Range.End - RangeStart : 0);

    if (RangeLength == 0) return std::nullopt;

    const std::string ReturnString = std::string(Text.substr(RangeStart, RangeLength));
    if (ReturnString.empty()) return std::nullopt;

    return ReturnString;
}

std::optional<std::string_view> SubstringExtractor::ExtractSubstringViewByRange(
    std::string_view Text,
    const FSubstringRange& Range)
{
    if (!Range.IsValid() || Range.End > Text.size())
    {
        return std::nullopt;
    }

    const std::string_view ReturnView
        = std::string_view(Text.data() + Range.Start, Range.End - Range.Start);
    if (ReturnView.empty()) return std::nullopt;

    return ReturnView;
}

FSubstringRange SubstringExtractor::GetStartRange(
    std::string_view Text, const FExtractionRules& Rules)
{
    if (Text.empty() || !Rules.IsValid()) return {};

    const FIgnoredRangesRules& Ignored = Rules.IgnoredRangesRules;
    const std::vector<std::string>& StartMarkers = Rules.BlockMarkers.StartMarkers;

    for (const std::string& StartMarker : StartMarkers)
    {
        const auto [StartRange, _]
            = SubstringSearcher::FindSubtringRangeByRules({Text, {StartMarker}, Ignored});
        if (!StartRange.IsValid()) continue;

        return StartRange;
    }

    return {};
}

FSubstringRange SubstringExtractor::GetEndRange(
    std::string_view Text,
    const FExtractionRules& Rules,
    const size_t NewSearchStart)
{
    if (Text.empty() || !Rules.IsValid() || NewSearchStart >= Text.size()) return {};

    const FIgnoredRangesRules& Ignored = Rules.IgnoredRangesRules;
    const std::vector<std::string>& EndMarkers = Rules.BlockMarkers.EndMarkers;

    const FSubstringRange RemainingRange {
        .Start = NewSearchStart,
        .End   = Text.size()
    };
    if (!RemainingRange.IsValid()) return {};

    const std::optional<std::string_view> RemainingResult
        = ExtractSubstringViewByRange(Text, RemainingRange);
    if (!RemainingResult) return {};
    const std::string_view RemainingText = RemainingResult.value();

    for (const std::string& EndMarker : EndMarkers)
    {
        const auto [Candidate, _]
            = SubstringSearcher::FindSubtringRangeByRules({RemainingText, {EndMarker}, Ignored});
        if (!Candidate.IsValid()) continue;

        const FSubstringRange AdjustedCandidate {
            .Start = NewSearchStart + Candidate.Start,
            .End   = NewSearchStart + Candidate.End
        };
        if (!AdjustedCandidate.IsValid()) continue;

        return AdjustedCandidate;
    }

    return {};
}

bool SubstringExtractor::AreEssentialMarkersFound(
    std::string_view Text,
    const FExtractionRules& Rules,
    const FSubstringRange& InnerRange)
{
    if (Text.empty() || !Rules.IsValid() || !InnerRange.IsValid()) return false;

    const FIgnoredRangesRules& Ignored = Rules.IgnoredRangesRules;
    const std::vector<std::string>& EssentialMarkers = Rules.BlockMarkers.EssentialMarkers;

    const std::optional<std::string_view> InnerViewOp = ExtractSubstringViewByRange(Text, InnerRange);
    if (!InnerViewOp) return false;
    const std::string_view InnerView = InnerViewOp.value();

    for (const std::string& Essential : EssentialMarkers)
    {
        const auto [Found, _]
            = SubstringSearcher::FindSubtringRangeByRules({InnerView, {Essential}, Ignored});
        if (!Found.IsValid()) return false;
    }
    return true;
}

//     //     //     //     //     //

FSubstringRange SubstringExtractor::GetRangeByRules(
    const std::string_view Text,
    const FExtractionRules& Rules)
{
    if (Text.empty() || !Rules.IsValid()) return {};

    const FSubstringRange StartRange = GetStartRange(Text, Rules);
    if (!StartRange.IsValid()) return {};

    const size_t NewSearchStart = StartRange.End;
    const FSubstringRange EndRange = GetEndRange(Text, Rules, NewSearchStart);
    if (!EndRange.IsValid()) return {};

    const FSubstringRange InnerRange {
        .Start = NewSearchStart,
        .End   = EndRange.Start
    };
    if (!InnerRange.IsValid()) return {};

    const bool AreMarkersFound = AreEssentialMarkersFound(Text, Rules, InnerRange);
    if (!AreMarkersFound) return {std::string::npos, EndRange.End};

    return FSubstringRange{ StartRange.Start, EndRange.End };
}


//     //     //     //     //     //

//     //     //     //     //     //

//     //     //     //     //     //


std::pair<FSubstringRange, std::optional<std::string>> 
    SubstringExtractor::ExtractFirstSubstring(
        std::string_view Text,
        const FExtractionRules& Rules)
{
    size_t SearchOffset = 0;

    while (SearchOffset < Text.size())
    {
        const std::string_view RemainingText = Text.substr(SearchOffset);
        const FSubstringRange Range = GetRangeByRules(RemainingText, Rules);
        
        // Handle case where essential markers are missing (start = npos, but end is valid)
        if (!Range.IsValid() && Range.Start == std::string::npos && Range.End != std::string::npos) {
            SearchOffset += Range.End;
            continue;
        }

        if (!Range.IsValid()) 
            break;

        // Determine the range to extract based on exclusion flag
        FSubstringRange ExtractionRange = Range;
        if (Rules.bExcludeMarkers)
        {
            // Adjust extraction range to exclude markers
            const auto StartRange = GetStartRange(RemainingText, Rules);
            const auto EndRange = GetEndRange(RemainingText, Rules, StartRange.End);
            
            if (StartRange.IsValid() && EndRange.IsValid())
            {
                // Check if there's actual content between markers
                if (EndRange.Start <= StartRange.End) {
                    // No content between markers, skip this block
                    SearchOffset += Range.End;
                    continue;
                }
                ExtractionRange.Start = StartRange.End;
                ExtractionRange.End = EndRange.Start;
            }
        }

        const std::optional<std::string> Extracted = 
            ExtractSubstringByRange(RemainingText, ExtractionRange);

        if (!Extracted.has_value() || Extracted->empty()) {
            // Skip empty results
            SearchOffset += Range.End > 0 ? Range.End : 1;
            continue;
        }

        const FSubstringRange AdjustedRange {
            .Start = SearchOffset + Range.Start,
            .End   = SearchOffset + Range.End
        };
        return {AdjustedRange, Extracted};
    }

    return {};
}



//     //     //     //     //     //


std::vector<std::string_view> SubstringExtractor::ExtractAllSubstrings(
    const std::string_view Text,
    const FExtractionRules& Rules)
{
    std::vector<std::string_view> Results;
    size_t SearchPosition = 0;

    while (SearchPosition < Text.size())
    {
        const std::string_view RemainingText = Text.substr(SearchPosition);
        const auto [LocalRange, HasMatch] = ExtractFirstSubstring(RemainingText, Rules);
        
        if (!LocalRange.IsValid() || !HasMatch) 
            break;

        // Adjust extraction based on exclusion flag
        size_t ExtractStart = LocalRange.Start;
        size_t ExtractEnd = LocalRange.End;
        
        if (Rules.bExcludeMarkers)
        {
            // Use the inner content between markers
            const auto StartRange = GetStartRange(RemainingText, Rules);
            const auto EndRange = GetEndRange(RemainingText, Rules, StartRange.End);
            
            if (StartRange.IsValid() && EndRange.IsValid())
            {
                ExtractStart = StartRange.End;
                ExtractEnd = EndRange.Start;
            }
        }

        const size_t AbsBegin = SearchPosition + ExtractStart;
        const size_t AbsEnd = SearchPosition + ExtractEnd;
        Results.emplace_back(Text.data() + AbsBegin, AbsEnd - AbsBegin);

        // Always advance by the full range including markers
        SearchPosition += LocalRange.End;
    }

    return Results;
}
