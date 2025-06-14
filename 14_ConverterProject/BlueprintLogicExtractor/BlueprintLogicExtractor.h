#pragma once

#include "string"
#include "vector"
////////

class BlueprintLogicExtractor
{
    friend class BlueprintLogicExtractor_RemoveLinkedToHexStrings_Test;
private:

    static inline const std::vector<std::string> Delimiters { ",", " " };

    static inline const std::vector<std::string> RemoveMarkers
    {
        "Class=",
        "ExportPath=",
        "bIsEditable=",
        "NodePos",
        "NodeGuid=",
        "ExtraFlags=",
        "PersistentGuid=",
        "bHidden=",
        "bAdvancedView=",
        "PinId=",
        "MemberParent=",
        "bDefaultValueIsReadOnly=",
        "CustomProperties Pin (",
        "bDefaultValueIsIgnored=",
        "bOrphanedPin=",
        "bNotConnectable=",
    };

    static inline const std::vector<std::string> Endings
    {
        "=False",
        "=()",
        "=None",
        "\"\""
    };

    static inline const std::vector<std::pair<std::string, std::string>> SubstringsToReplace
    {
        {"Begin Object ", "\nBegin Object\n    "},
        {"   CustomProperties ", "    "},
        {"   \n", ""},
        {"   ,", "    "},
        {"\")\n", "\"\n"},
        {"   FunctionReference=", "    "},
        {"   (,", "    "},
        {"    (", "    "},
        {"     ", "    "},
        {",)\n", "\n"},
        {"),\n", ")\n"},
    };

private:

    static void RemoveLinkedToHexStrings(std::string& Text);
    
public:

    static void ExtractCoreBlueprintLogic(std::string& BlueprintCode);
};

