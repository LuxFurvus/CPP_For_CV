#pragma once


////////
#include "../SubstringExtractor/BaseParseStructures.h"

struct BlueprintBlockParsingRules
{

//     //     //     //     //     //

    static inline const FIgnoredRangesRules Nodes_IgnoreRules
    {
        .IgnoredChar = [](char c) { return !isgraph(static_cast<unsigned char>(c)); },
        .IgnoredRangesBetween = { {'(', ')'}, {'"', '"'}, {'\'', '\''} },
        .bCaseInsensitive = true
    };

    static inline const FBlockMarkers Nodes_BlockMarkers
    {
        .StartMarkers = { "Begin Object", "Class=", "ExportPath=", "FunctionReference=" },
        .EndMarkers = { "End Object" },
        .EssentialMarkers = {"CustomProperties Pin", "NodeGuid=", "FunctionReference="}
    };

//     //     //     //     //     //

    static inline const FIgnoredRangesRules CustomProperties_IgnoreRules
    {
        .IgnoredChar = [](const char c) { return !isgraph(static_cast<unsigned char>(c)) && !'\n'; },
        .IgnoredRangesBetween = { {'"', '"'}, {'\'', '\''} },
        .bCaseInsensitive = true
    };

    static inline const FBlockMarkers CustomProperties_BlockMarkers
    {
        .StartMarkers = { "CustomProperties" },
        .EndMarkers = { ")\n" },
        .EssentialMarkers = { "PinName=", "PinType" }
    };

//     //     //     //     //     //

    static inline const FIgnoredRangesRules NodeHeader_IgnoreRules
    {
        .IgnoredChar = [](const char c) { return !isgraph(static_cast<unsigned char>(c)); },
        .IgnoredRangesBetween = { {'"', '"'}, {'\'', '\''}, {'(', ')'} },
        .bCaseInsensitive = true
    };

    static inline const FBlockMarkers NodeHeader_BlockMarkers
    {
        .StartMarkers = { "Begin Object" },
        .EndMarkers = { "CustomProperties" },
        .EssentialMarkers = { "Class=", "ExportPath=", "FunctionReference=", "NodeGuid=" }
    };

//     //     //     //     //     //

};
