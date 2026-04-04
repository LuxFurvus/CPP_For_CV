
#pragma once

//
#include "../FieldParser/FieldParser.h"
#include "../SubstringExtractor/SubstringExtractor.h"
//
#include "../GraphBuilder/GraphStructs.h"

class BlueprintCodeParser
{
protected:

static std::vector<std::string_view> ExtractBlueprintSubstrings(
    const std::string_view BlueprintString,
    const FBlockMarkers& BlockMarkers)
{
    if (BlueprintString.empty()) return {};

    static const FIgnoredRangesRules Nodes_IgnoreRules
    {
        .IgnoredChar = [](char c) { return !isgraph(static_cast<unsigned char>(c)); },
        .IgnoredRangesBetween = { {'(', ')'}, {'"', '"'}, {'\'', '\''} },
        .bCaseInsensitive = true
    };
    const FExtractionRules Rules {
        .BlockMarkers = BlockMarkers,
        .IgnoredRangesRules = Nodes_IgnoreRules
    };
    return SubstringExtractor::ExtractAllSubstrings(BlueprintString, Rules);
}

static std::string_view ExtractSimpleValueByKey(
    const std::string_view PinBody, const std::string_view InKey)
{
    if (PinBody.empty() || InKey.empty()) return "";
    const FParseFieldParams Params
    {
        .SourceBlock = PinBody,
        .FieldKey = InKey,
        .IgnoredRules =
        {
            .IgnoredChar = [](char c) { return !isgraph(static_cast<unsigned char>(c)); },
            .bCaseInsensitive = true
        }
    };
    const std::vector<std::pair<char, char>> Delimiters
    {
        { '"', '"' }
    };
    const std::vector<std::string_view> Substring
        = FieldParser::ParseFieldNestedStrings(Params, Delimiters);
    return !Substring.empty() ? Substring.at(0) : "";
}

public:

    static std::string_view ExtractFunctionReference(const std::string_view NodeBody)
    {
        const FParseFieldParams FuncRefParams
        {
            .SourceBlock = NodeBody,
            .FieldKey = "FunctionReference=",
            .IgnoredRules =
            {
                .IgnoredChar = [](char c) { return !isgraph(static_cast<unsigned char>(c)); },
                .IgnoredRangesBetween = { {'"', '"'}, {'\'', '\''} },
                .bCaseInsensitive = true
            }
        };
        const std::vector<std::pair<char, char>> FuncRefDelimiters
        {
            { '(', ')' }
        };
        const std::vector<std::string_view> FuncRefSubstring
            = FieldParser::ParseFieldNestedStrings(FuncRefParams, FuncRefDelimiters);

        return !FuncRefSubstring.empty() ? FuncRefSubstring.at(0) : "";
    }






    static std::string_view ExtractMemberParent(const std::string_view NodeBody)
    {
        const FParseFieldParams Params
        {
            .SourceBlock = NodeBody,
            .FieldKey = "MemberParent=",
            .IgnoredRules =
            {
                .IgnoredChar = [](char c) { return !isgraph(static_cast<unsigned char>(c)); },
                .bCaseInsensitive = true
            }
        };
        const std::vector<std::pair<char, char>> Delimiters
        {
            { '"', '\'' }, { '\'', '.' }, { '.', '\'' }
        };
        const std::vector<std::string_view> Substring
            = FieldParser::ParseFieldNestedStrings(Params, Delimiters);

        return Substring.size() == 3 ? Substring.at(2) : "";
    }




static std::vector<std::string_view> ExtractNodeSubstrings(
    const std::string_view BlueprintString)
{
    if (BlueprintString.empty()) return {};

    static const FBlockMarkers Nodes_BlockMarkers
    {
        .StartMarkers = { "Begin Object Class=/Script/BlueprintGraph." },
        .EndMarkers = { "End Object" },
    };

    return ExtractBlueprintSubstrings(BlueprintString, Nodes_BlockMarkers);
}

static std::vector<std::string_view> ExtractPinSubstrings(
    const std::string_view NodeSubstring)
{
    if (NodeSubstring.empty()) return {};

    static const FBlockMarkers Pins_BlockMarkers
    {
        .StartMarkers = { "CustomProperties Pin" },
        .EndMarkers = { "CustomProperties", "End Object", "Begin Object" },
    };

    return ExtractBlueprintSubstrings(NodeSubstring, Pins_BlockMarkers);
}




    static std::vector<FNodeObj> BuildAllNodesObjects(
        const std::string_view InGraphString)
    {
        std::vector<std::string_view> AllNodesStrings
            = ExtractNodeSubstrings(InGraphString);

        std::vector<FNodeObj> NewAllNodes;
        NewAllNodes.reserve(AllNodesStrings.size());

        for (std::string_view NodeSubstring : AllNodesStrings)
        {
            NewAllNodes.emplace_back(std::move(NodeSubstring));
        }

        return NewAllNodes;
    }











    static std::string_view ExtractMemberName(const std::string_view PinBody)
    {
        return ExtractSimpleValueByKey(PinBody, "MemberName=");
    }

    static std::string_view ExtractPinCategory(const std::string_view PinBody)
    {
        return ExtractSimpleValueByKey(PinBody, "PinType.PinCategory=");
    }

    static std::string_view ExtractDefaultValue(const std::string_view PinBody)
    {
        return ExtractSimpleValueByKey(PinBody, "DefaultValue=");
    }

    static bool AreStringsEqual(const std::string_view Str1, const std::string_view Str2)
    {
        return Str1.compare(Str2) == 0;
    }







static std::string ExtractArgList(const std::vector<std::string_view>& PinBodies)
{
    std::vector<std::string> Args;

    for (const auto& PinBody : PinBodies)
    {
        if (PinBody.empty()) continue;

        const std::string_view PinCategory = ExtractPinCategory(PinBody);
        if (PinCategory.empty()) continue;

        const std::string_view DefaultValue = ExtractDefaultValue(PinBody);
        if (DefaultValue.empty()) continue;

        if (AreStringsEqual(PinCategory, "string"))
        {
            Args.push_back("\"" + std::string(DefaultValue) + "\"");
        }
        else
        {
            Args.push_back(std::string(DefaultValue));
        }
    }
    return Join(Args, ", ");
}

static std::string Join(
    const std::vector<std::string>& Strings,
    const std::string_view Delimiter)
{
    std::string Result;
    for (size_t i = 0; i < Strings.size(); ++i)
    {
        if (i != 0)
            Result += Delimiter;
        Result += Strings[i];
    }
    return Result;
}


};
