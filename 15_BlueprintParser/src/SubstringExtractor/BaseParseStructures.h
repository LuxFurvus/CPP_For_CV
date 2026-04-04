#pragma once
// BaseParseStructures.h

#include <string>
#include <vector>
#include <functional>


/////////////////////////////

struct FIgnoredRangesRules
{
    const std::function<bool(const char)> IgnoredChar = [](const char) { return false; };
    const std::vector<std::pair<char, char>> IgnoredRangesBetween {};
    const bool bCaseInsensitive = false;
};///


struct FBlockMarkers
{
    const std::vector<std::string> StartMarkers;
    const std::vector<std::string> EndMarkers;
    const std::vector<std::string> EssentialMarkers{};

    bool IsValid() const
    {
        return !StartMarkers.empty() && !EndMarkers.empty();
    }
};///

struct FExtractionRules
{
    const FBlockMarkers BlockMarkers;
    const FIgnoredRangesRules IgnoredRangesRules{};
    const bool bExcludeMarkers = false;

    bool IsValid() const
    {
        return BlockMarkers.IsValid();
    }
};

//     //     //     //     //     //

struct FSubstringRange
{
     size_t Start = std::string::npos;
     size_t End   = std::string::npos;

    bool IsValid() const
    {
        return true
            && Start < End
            && Start <= std::string::npos
            && End <= std::string::npos;
    }
};

//     //     //     //     //     //

struct FRangedSubstring
{
    const FSubstringRange Range;
    const std::string Substring;

    bool IsValid() const
    {
        return Range.IsValid() && !Substring.empty();
    }
};

//     //     //     //     //     //

