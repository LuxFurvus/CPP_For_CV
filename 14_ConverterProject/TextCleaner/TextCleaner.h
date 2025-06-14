#pragma once

#include <cstddef>
#include <string>
#include <vector>

class TextCleaner
{
    friend class TextCleaner_ExtractSubstringBetweenMarkers_Test;

    friend class TextCleaner_FindFirstSubstringInRange_Test;

    friend class TextCleaner_ReplaceSubstrings_Test;
    friend class TextCleaner_ReplaceSubstringsFromSet_Test;
    friend class TextCleaner_ReplaceConsecutiveCharsWithSingle_Test;

    friend class TextCleaner_RemoveRange_Test;
    friend class TextCleaner_RemoveSubstrings_Test;
    friend class TextCleaner_RemoveSubstringInRange_Test;
    friend class TextCleaner_RemoveSubstringsFromSet_Test;
    friend class TextCleaner_RemoveSubstringFromMarkerTillDelimiter_Test;
    friend class TextCleaner_RemoveAllSubstringsFromMarkerTillDelimiter_Test;
    friend class TextCleaner_RemoveAllSubstringsFromAnyMarkerTillDelimiter_Test;
    friend class TextCleaner_RemoveSubstringStartingFromEndingWithBeforeDelimiter_Test;

////////////////////////////////////
    public:
////////////////////////////////////

    static std::string ExtractSubstringBetweenMarkers(const std::string& Text, const std::string& StartMarker, const std::string& EndMarker);

    static std::pair<size_t, size_t> FindFirstSubstringInRange(const std::string& Text, const std::vector<std::string>& Substrings, const std::pair<size_t, size_t>& Range);
 
////////////////////////////////////

    static void ReplaceSubstrings(std::string& Text, const std::string& From, const std::string& To);

    static void ReplaceSubstringsFromSet(std::string& Text, const std::vector<std::pair<std::string, std::string>>& SubstringsToReplace);

    static void ReplaceConsecutiveCharsWithSingle(std::string& Text, char CharToReplace);

////////////////////////////////////

    static void RemoveRange(std::string& Text, size_t StartPosition, size_t EndPosition);

    static void RemoveSubstrings(std::string& Text, const std::string& SubstringToRemove);

    static void RemoveSubstringsFromSet(std::string& Text, const std::vector<std::string>& SubstringsToRemove);

    static void RemoveSubstringStartingFromEndingWithBeforeDelimiter(std::string& Text, const std::string& Marker, const std::vector<std::string>& Endings, const std::vector<std::string>& Delimiters);

    static void RemoveSubstringInRange(std::string &Text, std::pair<size_t, size_t> Range);

    static size_t RemoveSubstringFromMarkerTillDelimiter(std::string& Text, size_t StartPos, const std::string& Marker, const std::vector<std::string>& Delimiters);

    static void RemoveAllSubstringsFromMarkerTillDelimiter(std::string &Text, const std::string &Marker, const std::vector<std::string>& Delimiters);

    static void RemoveAllSubstringsFromAnyMarkerTillDelimiter(std::string& Text, const std::vector<std::string>& Markers, const std::vector<std::string>& Delimiters);

////////////////////////////////////
    public:
////////////////////////////////////






};
