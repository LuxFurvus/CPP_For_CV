
////////////////////////////////
#include "TextCleaner.h"

void TextCleaner::RemoveSubstrings(std::string& Text, const std::string& SubstringToRemove)
{
    if (Text.empty() || SubstringToRemove.empty()) return;

    const size_t SubstringLength = SubstringToRemove.length();
    
    size_t StartPos = 0;
    while ((StartPos = Text.find(SubstringToRemove, StartPos)) != std::string::npos)
    {
        Text.replace(StartPos, SubstringLength, "");
    }
}

///////////////////////////////////////

void TextCleaner::RemoveSubstringsFromSet(std::string& Text, const std::vector<std::string>& SubstringsToRemove)
{
    for (const std::string& Substring : SubstringsToRemove)
    {
        RemoveSubstrings(Text, Substring);
    }
}

///////////////////////////////////////

std::string TextCleaner::ExtractSubstringBetweenMarkers(const std::string& Text, const std::string& StartMarker, const std::string& EndMarker)
{
    if (Text.empty() || StartMarker.empty() || EndMarker.empty()) return "";

    const size_t StartPos = Text.find(StartMarker);
    if (StartPos == std::string::npos) return "";

    const size_t StartMarkerLength = StartMarker.length();
    
    const size_t EndPos = Text.find(EndMarker, StartPos + StartMarkerLength);
    if (EndPos == std::string::npos) return "";
    
    return Text.substr(StartPos + StartMarkerLength, EndPos - (StartPos + StartMarkerLength));
}

/////////////////////////////////////////////

void TextCleaner::ReplaceSubstrings(std::string& Text, const std::string& From, const std::string& To)
{
    if (Text.empty() || From.empty()) return;

    size_t StartPos = 0;
    while ((StartPos = Text.find(From, StartPos)) != std::string::npos)
    {
        Text.replace(StartPos, From.length(), To);
        StartPos += To.length();
    }
}

/////////////////////////////

void TextCleaner::ReplaceSubstringsFromSet(std::string& Text, const std::vector<std::pair<std::string, std::string>>& SubstringsToReplace)
{
    for (const auto& [From, To] : SubstringsToReplace)
    {
        ReplaceSubstrings(Text, From, To);
    }
}

/////////////////////////////

void TextCleaner::ReplaceConsecutiveCharsWithSingle(std::string& Text, char CharToReplace)
{
    if (Text.empty()) return;

    size_t WritePos = 0; // Position to write the deduplicated characters
    bool PreviousWasTarget = false;

    for (size_t ReadPos = 0; ReadPos < Text.size(); ++ReadPos)
    {
        char CurrentChar = Text[ReadPos];

        if (CurrentChar == CharToReplace)
        {
            if (!PreviousWasTarget)
            {
                // Only write the first occurrence in the sequence
                Text[WritePos++] = CurrentChar;
                PreviousWasTarget = true;
            }
            // Otherwise skip repeated target characters
        }
        else
        {
            // Non-target character, write normally
            Text[WritePos++] = CurrentChar;
            PreviousWasTarget = false;
        }
    }

    // Resize string to remove the extra characters
    Text.resize(WritePos);
}

/////////////////////////////////////////////

void TextCleaner::RemoveRange(std::string& Text, size_t StartPosition, size_t EndPosition)
{
    Text.erase(StartPosition, EndPosition - StartPosition);
}

//////////////////////////////////////

std::pair<size_t, size_t> TextCleaner::FindFirstSubstringInRange(const std::string& Text, const std::vector<std::string>& Substrings, const std::pair<size_t, size_t>& Range)
{
    const size_t FromPos = Range.first;
    const size_t ToPos = Range.second;

    const bool IsCallInvalid = false
        || Substrings.empty()
        || Text.empty()
        || FromPos == std::string::npos
        || ToPos < FromPos;
    if (IsCallInvalid) return { std::string::npos, 0 };

    size_t ClosestSubstrPosition = std::string::npos;
    size_t SubstrLength = 0;

    for (const std::string& Substr : Substrings)
    {
        const size_t SubstrPosition = Text.find(Substr, FromPos);
        
        const bool SubstringIsOutOfRange = SubstrPosition >= ToPos;
        const bool ThereIsNoSubstring = SubstrPosition == std::string::npos;
        const bool ThereIsCloserSubstring = SubstrPosition > ClosestSubstrPosition;

        if (ThereIsNoSubstring || ThereIsCloserSubstring || SubstringIsOutOfRange) continue;

        ClosestSubstrPosition = SubstrPosition;
        SubstrLength = Substr.length();
    }
    return { ClosestSubstrPosition, SubstrLength };
}

//////////////////////////////////////

void TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(std::string& Text, const std::string& Marker, const std::vector<std::string>& Endings, const std::vector<std::string>& Delimiters)
{
    const bool IsCallInvalid = false
        || Marker.empty()
        || Endings.empty()
        || Text.empty();
    if (IsCallInvalid) return;

    size_t Position = 0;

    while (true)
    {
        const size_t MarkerPosition = Text.find(Marker, Position);
        if (MarkerPosition == std::string::npos) return;

        const size_t FirstDelimiterPosition = TextCleaner::FindFirstSubstringInRange(Text,Delimiters, {MarkerPosition, std::string::npos}).first;

        auto [EndingPosition, EndingLength] = FindFirstSubstringInRange(Text, Endings, {MarkerPosition, FirstDelimiterPosition});

        if (EndingPosition != std::string::npos)
        {
            RemoveRange(Text, MarkerPosition, EndingPosition + EndingLength);
            // Remain at current position to catch another possible marker.
        }
        else
        {
            if (FirstDelimiterPosition == std::string::npos) return;
            Position = FirstDelimiterPosition + 1;
        }
    }
}

//////////////////////////////////////

void TextCleaner::RemoveSubstringInRange(std::string& Text, std::pair<size_t, size_t> Range)
{
    const size_t StartPos = Range.first;
    const size_t StopPos = Range.second;
    if (StartPos > StopPos || StopPos > Text.size()) return;
    Text.erase(StartPos, StopPos - StartPos);
}

//////////////////////////////////////

size_t TextCleaner::RemoveSubstringFromMarkerTillDelimiter(
    std::string& Text, 
    size_t StartPos,
    const std::string& Marker, 
    const std::vector<std::string>& Delimiters)
{
    // Locate the marker in the text, starting from the specified StartPos
    size_t MarkerPos = Text.find(Marker, StartPos);

    // Marker not found
    if (MarkerPos == std::string::npos) return std::string::npos;

    size_t SearchStartPos = MarkerPos + Marker.size();

    // If marker is at the very end of the string, remove only the marker
    if (SearchStartPos >= Text.size())
    {
        Text.erase(MarkerPos, Marker.size());
        return MarkerPos;
    }

    // Find the delimiter after the marker
    size_t StopPos = TextCleaner::FindFirstSubstringInRange(Text, Delimiters, {SearchStartPos, std::string::npos}).first;

    // If no delimiter found, remove the rest of the string
    if (StopPos == std::string::npos) StopPos = Text.size();

    // Remove the substring from marker to the delimiter
    RemoveSubstringInRange(Text, {MarkerPos, StopPos});

    return MarkerPos;
}

//////////////////////////////////////

void TextCleaner::RemoveAllSubstringsFromMarkerTillDelimiter(std::string& Text, const std::string& Marker, const std::vector<std::string>& Delimiters)
{
    size_t SearchPos = 0;
    while (true)
    {
        SearchPos = RemoveSubstringFromMarkerTillDelimiter(Text, SearchPos, Marker, Delimiters);
        if (SearchPos == std::string::npos) return;
    }
}

//////////////////////////////////////

void TextCleaner::RemoveAllSubstringsFromAnyMarkerTillDelimiter(std::string &Text, const std::vector<std::string> &Markers, const std::vector<std::string>& Delimiters)
{
    for (const std::string &Marker : Markers)
    {
    RemoveAllSubstringsFromMarkerTillDelimiter(Text, Marker, Delimiters);
    }
}
