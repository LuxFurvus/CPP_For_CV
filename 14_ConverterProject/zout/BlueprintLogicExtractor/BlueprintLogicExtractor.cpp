
#include "../TextCleaner/TextCleaner.h"
////////
#include "BlueprintLogicExtractor.h"

/////////////////////////////////////////////

void BlueprintLogicExtractor::RemoveLinkedToHexStrings(std::string& Text)
{
    const std::string LinkedToMarker = "LinkedTo=(";
    size_t Position = 0;
    while (true)
    {
        const auto [LinkedToPos, LinkedToLength] = TextCleaner::FindFirstSubstringInRange(Text, { LinkedToMarker }, { Position, std::string::npos });
        const size_t StartContent = LinkedToPos + LinkedToLength;
        const size_t EndContent = Text.find(')', StartContent);

        if (LinkedToPos == std::string::npos
            || StartContent == std::string::npos
            || EndContent == std::string::npos) return;

        const std::string Content = Text.substr(StartContent, EndContent - StartContent - 1);

        const size_t SpacePos = Content.find(' ');
        if (SpacePos == std::string::npos)
        {
            Position = EndContent + 1;
            continue;
        }

        const std::string NodeName = Content.substr(0, SpacePos);
        const std::string Original = Text.substr(LinkedToPos, EndContent - LinkedToPos + 1);
        const std::string Replacement = std::string{LinkedToMarker}.append(NodeName).append(")");

        Text.replace(LinkedToPos, Original.length(), Replacement);

        Position = LinkedToPos + Replacement.length();
    }
}

/////////////////////////////////////////////

void BlueprintLogicExtractor::ExtractCoreBlueprintLogic(std::string& BlueprintCode)
{
    TextCleaner::RemoveAllSubstringsFromAnyMarkerTillDelimiter(BlueprintCode, RemoveMarkers, Delimiters);

    TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(BlueprintCode, "PinType.", Endings, Delimiters);

    RemoveLinkedToHexStrings(BlueprintCode);

    TextCleaner::ReplaceSubstringsFromSet(BlueprintCode, SubstringsToReplace);

    TextCleaner::ReplaceConsecutiveCharsWithSingle(BlueprintCode, ',');
    TextCleaner::ReplaceSubstrings(BlueprintCode, ",\n", "\n");
}