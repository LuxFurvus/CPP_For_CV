
#pragma once

#include <string>
#include <memory>
#include <optional>
#include <vector>
//
#include "../GraphBuilder/BlueprintCodeParser.h"
#include "../GraphBuilder/GraphStructs.h"


struct FGraphBuilder
{
    static std::string Evaluate(const std::string_view GraphString)
    {
        if (GraphString.empty()) return {};

        const std::vector<FNodeObj> AllNodes
            = BlueprintCodeParser::BuildAllNodesObjects(GraphString);

        if (AllNodes.empty()) return {};

        const std::string_view FirstAndOnlyNodeStr
            = AllNodes.at(0).NodeSubstring;

        if (FirstAndOnlyNodeStr.empty()) return {};

        const std::string_view FuncRef
            = BlueprintCodeParser::ExtractFunctionReference(FirstAndOnlyNodeStr);
        const std::string_view MemberParent
            = BlueprintCodeParser::ExtractMemberParent(FuncRef);
        const std::string_view MemberName
            = BlueprintCodeParser::ExtractMemberName(FuncRef);

        const std::vector<std::string_view> PinBodies
            = BlueprintCodeParser::ExtractPinSubstrings(FirstAndOnlyNodeStr);
        const std::string ArgList
            = BlueprintCodeParser::ExtractArgList(PinBodies);

        std::string CppCode;
        CppCode.append("U");
        CppCode.append(MemberParent);
        CppCode.append("::");
        CppCode.append(MemberName);
        CppCode.append("(");
        CppCode.append(ArgList);
        CppCode.append(");");

        return CppCode;
    }
};
