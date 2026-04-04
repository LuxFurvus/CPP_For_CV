
#include "../GraphBuilder/BlueprintCodeParser.h"
//
#include <optional>
#include <string_view>
#include <vector>
#include <string>
//
#include <gtest/gtest.h>


namespace Test_ExtractFunctionReference_20250825 {

static void ExpectRefEquals(const std::string& Name, const std::string& Input, const std::string& Expected) {
    SCOPED_TRACE(Name);
    const std::string_view InputView = std::string_view(Input);
    const std::string_view ActualView = BlueprintCodeParser::ExtractFunctionReference(InputView);
    const std::string Actual = std::string(ActualView);
    EXPECT_EQ(Actual, Expected);
}

TEST(ExtractFunctionReference, T01_BasicContent) {
    SCOPED_TRACE(".T01: Returns first well-formed block after FunctionReference=");
    const std::string Input = R"(Header=() FunctionReference=(MemberName="Foo") Tail=())";
    const std::string Expected = R"(MemberName="Foo")";
    ExpectRefEquals(".T01", Input, Expected);
}

TEST(ExtractFunctionReference, T02_NestedParentheses) {
    SCOPED_TRACE(".T02: Handles nested parentheses inside target");
    const std::string Input = R"(Start FunctionReference=(MemberName="Bar", Meta=(A=1, B=(X=2,Y=3))) End)";
    const std::string Expected = R"(MemberName="Bar", Meta=(A=1, B=(X=2,Y=3)))";
    ExpectRefEquals(".T02", Input, Expected);
}

TEST(ExtractFunctionReference, T03_CommasAndEquals) {
    SCOPED_TRACE(".T03: Handles commas and equals inside value");
    const std::string Input = R"(Pre FunctionReference=(Owner=Obj, Flags=RF_Public|RF_Transient, Path="/Game/X"))";
    const std::string Expected = R"(Owner=Obj, Flags=RF_Public|RF_Transient, Path="/Game/X")";
    ExpectRefEquals(".T03", Input, Expected);
}

TEST(ExtractFunctionReference, T04_MidStringWithDistractors) {
    SCOPED_TRACE(".T04: Extracts correct block even with distractors before/after");
    const std::string Input = R"(OtherField=(IgnoreMe(1,2)) Junk (still) ignore FunctionReference=(MemberName="Baz", Id=64778C95) TrailingField=(Ok))";
    const std::string Expected = R"(MemberName="Baz", Id=64778C95)";
    ExpectRefEquals(".T04", Input, Expected);
}

TEST(ExtractFunctionReference, T05_KeyAbsent) {
    SCOPED_TRACE(".T05: Returns empty when FunctionReference= is missing");
    const std::string Input = R"(SomethingElse=(Value))";
    const std::string Expected = "";
    ExpectRefEquals(".T05", Input, Expected);
}

TEST(ExtractFunctionReference, T06_KeyWithoutParen) {
    SCOPED_TRACE(".T06: Returns empty when key not followed by '('");
    const std::string Input = R"(FunctionReference= MemberName="Foo" Tail())";
    const std::string Expected = "";
    ExpectRefEquals(".T06", Input, Expected);
}

TEST(ExtractFunctionReference, T07_UnbalancedParentheses) {
    SCOPED_TRACE(".T07: Returns empty when parentheses unbalanced");
    const std::string Input = R"(FunctionReference=(MemberName="Foo" Tail=()";
    const std::string Expected = "";
    ExpectRefEquals(".T07", Input, Expected);
}

TEST(ExtractFunctionReference, T08_EmptyInput) {
    SCOPED_TRACE(".T08: Returns empty on empty input");
    const std::string Input = "";
    const std::string Expected = "";
    ExpectRefEquals(".T08", Input, Expected);
}

TEST(ExtractFunctionReference, T09_KeyAtEnd) {
    SCOPED_TRACE(".T09: Returns empty when key at end of input");
    const std::string Input = R"(Preceding Stuff FunctionReference=)";
    const std::string Expected = "";
    ExpectRefEquals(".T09", Input, Expected);
}

TEST(ExtractFunctionReference, T10_SpacesAroundEquals) {
    SCOPED_TRACE(".T10: Tolerates spaces around '='");
    const std::string Input = R"(X FunctionReference = (MemberName="Foo") Y)";
    const std::string Expected = R"(MemberName="Foo")";
    ExpectRefEquals(".T10", Input, Expected);
}

TEST(ExtractFunctionReference, T11_NewlineAfterKey) {
    SCOPED_TRACE(".T11: Handles newline between key and '('");
    const std::string Input = "A FunctionReference=\n(MemberName=\"Bar\") B";
    const std::string Expected = R"(MemberName="Bar")";
    ExpectRefEquals(".T11", Input, Expected);
}

TEST(ExtractFunctionReference, T12_LeadingTrailingWhitespace) {
    SCOPED_TRACE(".T12: Handles leading and trailing whitespace");
    const std::string Input = " \t\n   FunctionReference=(MemberName=\"Baz\")   \n\t ";
    const std::string Expected = R"(MemberName="Baz")";
    ExpectRefEquals(".T12", Input, Expected);
}

TEST(ExtractFunctionReference, T13_UnrelatedFieldsAround) {
    SCOPED_TRACE(".T13: Works with multiple unrelated fields in string");
    const std::string Input = R"(NodePosX=(123) Extra=(Ignore) FunctionReference=(MemberName="Qux",Meta=(A=1)) Tail=(Ok))";
    const std::string Expected = R"(MemberName="Qux",Meta=(A=1))";
    ExpectRefEquals(".T13", Input, Expected);
}

TEST(ExtractFunctionReference, T14_FirstEmptySecondNonEmpty) {
    SCOPED_TRACE(".T14: Returns first occurrence even if empty, ignoring later");
    const std::string Input = R"(Head FunctionReference=() Gap=(x) FunctionReference=(MemberName="Later") Tail)";
    const std::string Expected = "";
    ExpectRefEquals(".T14", Input, Expected);
}

TEST(ExtractFunctionReference, T15_FirstNonEmptyWins) {
    SCOPED_TRACE(".T15: Returns content from first of multiple occurrences");
    const std::string Input = R"(FunctionReference=(MemberName="First",Id=1) Middle() FunctionReference=(MemberName="Second",Id=2))";
    const std::string Expected = R"(MemberName="First",Id=1)";
    ExpectRefEquals(".T15", Input, Expected);
}

TEST(ExtractFunctionReference, T16_DistractorsBetweenOccurrences) {
    SCOPED_TRACE(".T16: Still returns first even with parentheses noise before second");
    const std::string Input = R"(Start FunctionReference=(A=(B=(C=3))) (Ignore(1,2,3)) Another(Field=(K)) FunctionReference=(MemberName="Later"))";
    const std::string Expected = R"(A=(B=(C=3)))";
    ExpectRefEquals(".T16", Input, Expected);
}

TEST(ExtractFunctionReference, T17_ManyDistractorsBeforeKey) {
    SCOPED_TRACE(".T17: Works with many parentheses before target key");
    const std::string Input =
        R"((N0)(N1)(N2)(N3)(N4)(N5)(N6)(N7)(N8)(N9))"
        R"((N10)(N11)(N12)(N13)(N14)(N15)(N16)(N17)(N18)(N19))"
        R"((N20)(N21)(N22)(N23)(N24)(N25)(N26)(N27)(N28)(N29))"
        R"( FunctionReference=(MemberName="Deep") Tail())";
    const std::string Expected = R"(MemberName="Deep")";
    ExpectRefEquals(".T17", Input, Expected);
}

TEST(ExtractFunctionReference, T18_DeepNesting) {
    SCOPED_TRACE(".T18: Handles deeply nested parentheses in value");
    const std::string Input = R"(Pre FunctionReference=(A=(B=(C=(D=(E=(F=42)))))) Post)";
    const std::string Expected = R"(A=(B=(C=(D=(E=(F=42))))))";
    ExpectRefEquals(".T18", Input, Expected);
}

TEST(ExtractFunctionReference, T19_UnicodeContent) {
    SCOPED_TRACE(".T19: Handles Unicode characters inside value");
    const std::string Input = "Hdr FunctionReference=(MemberName=\"Функция_函数_Δ\", Note=\"✓\") Ftr";
    const std::string Expected = "MemberName=\"Функция_函数_Δ\", Note=\"✓\"";
    ExpectRefEquals(".T19", Input, Expected);
}


} // namespace Test_ExtractFunctionReference_20250825
















namespace Test_ExtractMemberParent_20250825 {


static void ExpectMemberParentEquals(const std::string& Name, const std::string& Input, const std::string& Expected) {
    SCOPED_TRACE(Name);
    const std::string_view InputView = std::string_view(Input);
    const std::string_view ActualView = BlueprintCodeParser::ExtractMemberParent(InputView);
    const std::string Actual = std::string(ActualView);
    EXPECT_EQ(Actual, Expected);
}


TEST(ExtractMemberParent, T01_BasicThreeSegments) {
    SCOPED_TRACE(".T01: Extracts the third segment of MemberParent chain");
    const std::string Input =
        R"(MemberParent="/Script/CoreUObject.Class'/Script/TraceUtilities.TraceUtilLibrary'")";
    const std::string Expected = "TraceUtilLibrary";
    ExpectMemberParentEquals(".T01", Input, Expected);
}

TEST(ExtractMemberParent, T02_WhitespaceAndCaseInsensitivity) {
    SCOPED_TRACE(".T02: Ignores whitespace and matches key case-insensitively");
    const std::string Input =
        "  \n  mEmBeRpArEnT  =  "
        "\"/Script/CoreUObject.Class' /Script/MyModule.MyLeaf'\"  ";
    const std::string Expected = "MyLeaf";
    ExpectMemberParentEquals(".T02", Input, Expected);
}

TEST(ExtractMemberParent, T03_FirstOccurrenceWins) {
    SCOPED_TRACE(".T03: Returns value from the first occurrence when multiple keys exist");
    const std::string Input =
        R"(MemberParent="/Script/Engine.Class'/Script/FirstModule.FirstLeaf')"  // first occurrence
        R"( OtherField=(X) )"
        R"(MemberParent="/Script/Engine.Class'/Script/SecondModule.SecondLeaf")"; // second ignored
    const std::string Expected = "FirstLeaf";
    ExpectMemberParentEquals(".T03", Input, Expected);
}

TEST(ExtractMemberParent, T04_KeyAbsent) {
    SCOPED_TRACE(".T04: Returns empty when MemberParent= is missing");
    const std::string Input =
        R"(OtherKey="/Script/Engine.Class'/Script/Foo.Bar'")";
    const std::string Expected = "";
    ExpectMemberParentEquals(".T04", Input, Expected);
}

TEST(ExtractMemberParent, T05_PresentButNoChain) {
    SCOPED_TRACE(".T05: Returns empty when delimiters do not form proper chain");
    const std::string Input =
        R"(MemberParent=/Script/Engine.Class/Script/BadFormat)";
    const std::string Expected = "";
    ExpectMemberParentEquals(".T05", Input, Expected);
}

TEST(ExtractMemberParent, T06_BrokenSequence) {
    SCOPED_TRACE(".T06: Returns empty when quotes are unbalanced or incomplete");
    const std::string Input =
        R"(MemberParent="/Script/Engine.Class'/Script/BadModule.BadLeaf)";
    const std::string Expected = "";
    ExpectMemberParentEquals(".T06", Input, Expected);
}

TEST(ExtractMemberParent, T07_OnlyTwoSegments) {
    SCOPED_TRACE(".T07: Returns empty when only two segments are parsed");
    const std::string Input =
        R"(MemberParent="/Script/Engine.Class'OnlyTwoSegment'")";
    const std::string Expected = "";
    ExpectMemberParentEquals(".T07", Input, Expected);
}

TEST(ExtractMemberParent, T08_UnicodeAndNoise) {
    SCOPED_TRACE(".T08: Handles Unicode segments and ignores distractor tokens");
    const std::string Input =
        "(N0)(N1)(N2) MemberParent=\"/Script/Модуль.Класс'/Script/Пакет.Лист'\" (Tail)(X)";
    const std::string Expected = "Лист";
    ExpectMemberParentEquals(".T08", Input, Expected);
}


} // namespace Test_ExtractMemberParent_20250825














namespace Test_ExtractBlueprintSubstrings {

static void ExpectBlueprintSubstringsEquals(
    const std::string& TestName,
    const std::string& Input,
    const FBlockMarkers& Markers,
    const std::vector<std::string>& Expected)
{
    const std::string_view InputView = Input;
    struct Call_ExtractBlueprintSubstrings : public BlueprintCodeParser
    {
        std::vector<std::string_view> operator()(const std::string_view Input, const FBlockMarkers& InBlockMarkers) {
            return BlueprintCodeParser::ExtractBlueprintSubstrings(Input, InBlockMarkers);
        }
    };
    const std::vector<std::string_view> ActualViews
        = Call_ExtractBlueprintSubstrings{}(InputView, Markers);
    std::vector<std::string> Actual;
    for (const auto& View : ActualViews) {
        Actual.emplace_back(View);
    }
    EXPECT_EQ(Actual, Expected) << TestName;
}

TEST(ExtractBlueprintSubstrings, T01_EmptyInput) {
    const FBlockMarkers Markers{
        .StartMarkers = {"CustomProperties Pin"},
        .EndMarkers = {"CustomProperties", "End Object", "Begin Object"}
    };
    const std::string Input = "";
    const std::vector<std::string> Expected = {};
    ExpectBlueprintSubstringsEquals("T01: Empty input string returns empty vector", Input, Markers, Expected);
}

TEST(ExtractBlueprintSubstrings, T02_NoBlockMarkers) {
    const FBlockMarkers Markers{
        .StartMarkers = {"CustomProperties Pin"},
        .EndMarkers = {"CustomProperties", "End Object", "Begin Object"}
    };
    const std::string Input = "Just some random text without pin markers";
    const std::vector<std::string> Expected = {};
    ExpectBlueprintSubstringsEquals("T02: String with no pin markers returns empty vector", Input, Markers, Expected);
}

TEST(ExtractBlueprintSubstrings, T03_SingleValidPinBlock) {
    const FBlockMarkers Markers{
        .StartMarkers = {"CustomProperties Pin"},
        .EndMarkers = {"CustomProperties", "End Object", "Begin Object"}
    };
    const std::string Input = "CustomProperties Pin\nPinContent\nCustomProperties";
    const std::vector<std::string> Expected = {"CustomProperties Pin\nPinContent\nCustomProperties"};
    ExpectBlueprintSubstringsEquals("T03: Basic pin block extraction with valid markers", Input, Markers, Expected);
}

TEST(ExtractBlueprintSubstrings, T04_MultiplePinBlocksWithDifferentEndMarkers) {
    const FBlockMarkers Markers{
        .StartMarkers = {"CustomProperties Pin"},
        .EndMarkers = {"CustomProperties", "End Object", "Begin Object"}
    };
    const std::string Input = 
        "CustomProperties Pin\nFirstPin\nCustomProperties\n"
        "CustomProperties Pin\nSecondPin\nCustomPropertiesEnd Object\n"
        "CustomProperties Pin\nThirdPin\nCustomPropertiesBegin Object";
    const std::vector<std::string> Expected = {
        "CustomProperties Pin\nFirstPin\nCustomProperties",
        "CustomProperties Pin\nSecondPin\nCustomProperties",
        "CustomProperties Pin\nThirdPin\nCustomProperties"
    };
    ExpectBlueprintSubstringsEquals("T04: Multiple pin blocks with different end markers are correctly extracted", Input, Markers, Expected);
}

TEST(ExtractBlueprintSubstrings, T05_PinBlocksWithIgnoredRanges) {
    const FBlockMarkers Markers{
        .StartMarkers = {"CustomProperties Pin"},
        .EndMarkers = {"CustomProperties", "End Object", "Begin Object"}
    };
    {
        const std::string Input = "CustomProperties Pin (nested) content 'quoted' \"doublequoted\" CustomProperties";
        const std::vector<std::string> Expected = {"CustomProperties Pin (nested) content 'quoted' \"doublequoted\" CustomProperties"};
        ExpectBlueprintSubstringsEquals("T05: Pin blocks containing ignored ranges are handled correctly", Input, Markers, Expected);
    }
    {
        const std::string Input = "CustomProperties Pin (CustomProperties Pin inside parens) CustomProperties";
        const std::vector<std::string> Expected = {"CustomProperties Pin (CustomProperties Pin inside parens) CustomProperties"};
        ExpectBlueprintSubstringsEquals("T05: Pin markers inside ignored ranges don't interfere", Input, Markers, Expected);
    }
}

} // namespace Test_ExtractBlueprintSubstrings












namespace Test_GetNodeSubstrings_20250825 {


static void ExpectNodeSubstringsEquals(const std::string& Name, const std::string& Input, const std::vector<std::string>& Expected) {
    SCOPED_TRACE(Name);
    const std::string_view InputView = std::string_view(Input);
    const std::vector<std::string_view> ActualViews
        = BlueprintCodeParser::ExtractNodeSubstrings(InputView);
    std::vector<std::string> Actual;
    for (const auto& V : ActualViews) {
        Actual.emplace_back(std::string(V));
    }
    EXPECT_EQ(Actual, Expected);
}


TEST(ExtractNodeSubstrings, T01_SingleCompleteBlock) {
    SCOPED_TRACE(".T01: Extracts one complete node block between Begin and End markers");
    {
        // Single complete block
        const std::string Input =
            "Begin Object Class=/Script/BlueprintGraph.Node\n"
            "Name=Node_1\n"
            "End Object\n";
        const std::vector<std::string> Expected {
            "Begin Object Class=/Script/BlueprintGraph.Node\nName=Node_1\nEnd Object"
        };
        ExpectNodeSubstringsEquals(".T01", Input, Expected);
    }
}


TEST(ExtractNodeSubstrings, T02_MultipleSequentialBlocks) {
    SCOPED_TRACE(".T02: Extracts multiple blocks in order when they appear sequentially");
    {
        // Multiple sequential blocks
        const std::string Input =
            "Begin Object Class=/Script/BlueprintGraph.Node\nName=Node_1\nEnd Object\n"
            "Begin Object Class=/Script/BlueprintGraph.Node\nName=Node_2\nEnd Object\n";
        const std::vector<std::string> Expected {
            "Begin Object Class=/Script/BlueprintGraph.Node\nName=Node_1\nEnd Object",
            "Begin Object Class=/Script/BlueprintGraph.Node\nName=Node_2\nEnd Object"
        };
        ExpectNodeSubstringsEquals(".T02", Input, Expected);
    }
}


TEST(ExtractNodeSubstrings, T03_IgnoredRangesAndWhitespace) {
    SCOPED_TRACE(".T03: Ignores whitespace and nested ranges inside parentheses or quotes");
    {
        // Block with ignored whitespace and ranges
        const std::string Input =
            "   Begin Object Class=/Script/BlueprintGraph.Node\n"
            "Name=(Ignored(Stuff)) \"Also Ignored\" 'Another'\n"
            "End Object   \n";
        const std::vector<std::string> Expected {
            "Begin Object Class=/Script/BlueprintGraph.Node\nName=(Ignored(Stuff)) \"Also Ignored\" 'Another'\nEnd Object"
        };
        ExpectNodeSubstringsEquals(".T03", Input, Expected);
    }
}


TEST(ExtractNodeSubstrings, T04_MalformedOrMissingMarkers) {
    SCOPED_TRACE(".T04: Returns only well-formed blocks and ignores malformed or incomplete ones");
    {
        // Missing end marker
        const std::string Input1 =
            "Begin Object Class=/Script/BlueprintGraph.Node\n"
            "Name=BadNode\n";
        const std::vector<std::string> Expected1 {};
        ExpectNodeSubstringsEquals(".T04.MissingEnd", Input1, Expected1);
    }
    {
        // End marker with no start
        const std::string Input2 =
            "Name=Foo\n"
            "End Object\n";
        const std::vector<std::string> Expected2 {};
        ExpectNodeSubstringsEquals(".T04.NoStart", Input2, Expected2);
    }
    {
        // Empty text
        const std::string Input3 = "";
        const std::vector<std::string> Expected3 {};
        ExpectNodeSubstringsEquals(".T04.Empty", Input3, Expected3);
    }
}


TEST(ExtractNodeSubstrings, T05_CaseInsensitivity) {
    SCOPED_TRACE(".T05: Matches markers correctly regardless of case");
    {
        // Mixed-case markers
        const std::string Input =
            "bEgIn oBjEcT Class=/Script/BlueprintGraph.Node\n"
            "Name=MixedCase\n"
            "EnD oBjEcT\n";
        const std::vector<std::string> Expected {
            "bEgIn oBjEcT Class=/Script/BlueprintGraph.Node\nName=MixedCase\nEnD oBjEcT"
        };
        ExpectNodeSubstringsEquals(".T05", Input, Expected);
    }
}


} // namespace Test_GetNodeSubstrings_20250825










#if 0
namespace Test_ExtractPinBody_20250825 {


static void ExpectPinBodyEquals(const std::string& Name, const std::string& Input, const std::string& Expected) {
    SCOPED_TRACE(Name);
    const std::string_view InputView = std::string_view(Input);
    const std::string_view ActualView = BlueprintCodeParser::ExtractPinBody(InputView);
    const std::string Actual = std::string(ActualView);
    EXPECT_EQ(Actual, Expected);
}


TEST(ExtractPinBody, T01_BasicSinglePin) {
    SCOPED_TRACE(".T01: Extracts Pin body when single well-formed section exists");
    {
        const std::string Input =
            R"(Begin Object Class=/Script/BlueprintGraph.K2Node_CallFunction
               CustomProperties Pin (PinId=CMD,PinName="Command",PinType.PinCategory="string",DefaultValue="cmd")
               End Object)";
        const std::string Expected = "PinId=CMD,PinName=\"Command\",PinType.PinCategory=\"string\",DefaultValue=\"cmd\"";
        ExpectPinBodyEquals(".T01", Input, Expected);
    }
}


TEST(ExtractPinBody, T02_NoPinPresent) {
    SCOPED_TRACE(".T02: Returns empty string when no Pin section exists");
    {
        const std::string Input =
            R"(Begin Object Class=/Script/BlueprintGraph.K2Node_CallFunction
               FunctionReference=(MemberName="DoSomething")
               End Object)";
        const std::string Expected = "";
        ExpectPinBodyEquals(".T02", Input, Expected);
    }
}


TEST(ExtractPinBody, T03_WhitespaceAndCase) {
    SCOPED_TRACE(".T03: Handles extra whitespace and case-insensitive key match");
    {
        const std::string Input =
            "  \n  CuStOmPrOpErTiEs   pIn   ( PinId = XX , DefaultValue =  \"42\"  )";
        const std::string Expected = " PinId = XX , DefaultValue =  \"42\"  ";
        ExpectPinBodyEquals(".T03", Input, Expected);
    }
}


TEST(ExtractPinBody, T04_QuotedContentWithCommasAndParens) {
    SCOPED_TRACE(".T04: Correctly parses when quotes contain commas or parentheses");
    {
        const std::string Input 
            = R"ZoZo(CustomProperties Pin (PinName="Func(Command,Option)",DefaultValue="val(1,2,3)"))ZoZo";
        const std::string Expected
            = R"ZoZo(PinName="Func(Command,Option)",DefaultValue="val(1,2,3)")ZoZo";
        ExpectPinBodyEquals(".T04", Input, Expected);
    }
}


TEST(ExtractPinBody, T05_MalformedOrIncomplete) {
    SCOPED_TRACE(".T05: Returns empty string when malformed or incomplete section is present");
    {
        const std::string Input1 = R"(CustomProperties Pin () )";
        const std::string Expected1 = "";
        ExpectPinBodyEquals(".T05a", Input1, Expected1);
    }
    {
        const std::string Input2 = R"(CustomProperties Pin (PinId="X",PinName="Y") UnclosedText )";
        const std::string Expected2 = "PinId=\"X\",PinName=\"Y\"";
        ExpectPinBodyEquals(".T05b", Input2, Expected2);
    }
    {
        const std::string Input3 = R"(CustomProperties Pin (PinId="Broken)";
        const std::string Expected3 = "";
        ExpectPinBodyEquals(".T05c", Input3, Expected3);
    }
}


} // namespace Test_ExtractPinBody_20250825

#endif








namespace Test_ExtractSimpleValueByKey_20250825 {


static void ExpectExtractEquals(const std::string& Name,
                                const std::string& PinBody,
                                const std::string& InKey,
                                const std::string& Expected)
{
    SCOPED_TRACE(Name);
    const std::string_view PinBodyView = std::string_view(PinBody);
    const std::string_view InKeyView = std::string_view(InKey);
    struct Call_ExtractSimpleValueByKey : public BlueprintCodeParser
    {
        auto operator()(
            const std::string_view& PinBody,
            const std::string_view& InKey) const
        {
            return ExtractSimpleValueByKey(PinBody, InKey);
        }
    };
    const std::string_view ActualView = Call_ExtractSimpleValueByKey{}(PinBodyView, InKeyView);
    const std::string Actual = std::string(ActualView);
    EXPECT_EQ(Actual, Expected);
}


// T01: Empty input edge cases
TEST(ExtractSimpleValueByKey, T01_PinBodyEmpty) {
    SCOPED_TRACE(".T01: Returns empty string when PinBody is empty");
    const std::string PinBody = "";
    const std::string InKey = "Key";
    const std::string Expected = "";
    ExpectExtractEquals(".T01_PinBodyEmpty", PinBody, InKey, Expected);
}

TEST(ExtractSimpleValueByKey, T02_InKeyEmpty) {
    SCOPED_TRACE(".T02: Returns empty string when InKey is empty");
    const std::string PinBody = "SomeKey=\"Value\"";
    const std::string InKey = "";
    const std::string Expected = "";
    ExpectExtractEquals(".T02_InKeyEmpty", PinBody, InKey, Expected);
}

TEST(ExtractSimpleValueByKey, T03_BothEmpty) {
    SCOPED_TRACE(".T03: Returns empty string when both PinBody and InKey are empty");
    const std::string PinBody = "";
    const std::string InKey = "";
    const std::string Expected = "";
    ExpectExtractEquals(".T03_BothEmpty", PinBody, InKey, Expected);
}


// T04: Key not found
TEST(ExtractSimpleValueByKey, T04_KeyNotFound_Unrelated) {
    SCOPED_TRACE(".T04: Returns empty string when key not present at all");
    const std::string PinBody = "OtherKey=\"SomeValue\"";
    const std::string InKey = "TargetKey";
    const std::string Expected = "";
    ExpectExtractEquals(".T04_Unrelated", PinBody, InKey, Expected);
}

TEST(ExtractSimpleValueByKey, T05_KeyNotFound_Partial) {
    SCOPED_TRACE(".T05: Returns empty string when key only partially matches");
    const std::string PinBody = "Target=\"Value\"";
    const std::string InKey = "TargetKey";
    const std::string Expected = "";
    ExpectExtractEquals(".T05_Partial", PinBody, InKey, Expected);
}


// T06: Key present, properly quoted
TEST(ExtractSimpleValueByKey, T06_KeyAtStart) {
    SCOPED_TRACE(".T06: Extracts value when key is at the start");
    const std::string PinBody = "TargetKey=\"Alpha\" OtherKey=123";
    const std::string InKey = "TargetKey";
    const std::string Expected = "Alpha";
    ExpectExtractEquals(".T06_KeyAtStart", PinBody, InKey, Expected);
}

TEST(ExtractSimpleValueByKey, T07_KeyInMiddle) {
    SCOPED_TRACE(".T07: Extracts value when key is in the middle");
    const std::string PinBody = "OtherKey=123 TargetKey=\"Beta\" Next=321";
    const std::string InKey = "TargetKey";
    const std::string Expected = "Beta";
    ExpectExtractEquals(".T07_KeyInMiddle", PinBody, InKey, Expected);
}

TEST(ExtractSimpleValueByKey, T08_KeyAtEnd) {
    SCOPED_TRACE(".T08: Extracts value when key is at the end");
    const std::string PinBody = "X=Y TargetKey=\"Gamma\"";
    const std::string InKey = "TargetKey";
    const std::string Expected = "Gamma";
    ExpectExtractEquals(".T08_KeyAtEnd", PinBody, InKey, Expected);
}

TEST(ExtractSimpleValueByKey, T09_FirstOccurrenceWins) {
    SCOPED_TRACE(".T09: Extracts first occurrence when multiple keys exist");
    const std::string PinBody = "TargetKey=\"Delta\" TargetKey=\"Epsilon\"";
    const std::string InKey = "TargetKey";
    const std::string Expected = "Delta";
    ExpectExtractEquals(".T09_FirstOccurrence", PinBody, InKey, Expected);
}


// T10: Quoted value with special content
TEST(ExtractSimpleValueByKey, T10_ValueWithSpaces) {
    SCOPED_TRACE(".T10: Extracts value with spaces inside quotes");
    const std::string PinBody = "TargetKey=\"Value With Spaces\"";
    const std::string InKey = "TargetKey";
    const std::string Expected = "Value With Spaces";
    ExpectExtractEquals(".T10_ValueWithSpaces", PinBody, InKey, Expected);
}

TEST(ExtractSimpleValueByKey, T11_ValueWithPunctuation) {
    SCOPED_TRACE(".T11: Extracts value containing punctuation");
    const std::string PinBody = "TargetKey=\"Val!@#$%^&*()_+<>?\"";
    const std::string InKey = "TargetKey";
    const std::string Expected = "Val!@#$%^&*()_+<>?";
    ExpectExtractEquals(".T11_ValueWithPunctuation", PinBody, InKey, Expected);
}

TEST(ExtractSimpleValueByKey, T12_ValueWithUnicode) {
    SCOPED_TRACE(".T12: Extracts value containing Unicode characters");
    const std::string PinBody = "TargetKey=\"ПриветМир\"";
    const std::string InKey = "TargetKey";
    const std::string Expected = "ПриветМир";
    ExpectExtractEquals(".T12_ValueWithUnicode", PinBody, InKey, Expected);
}

TEST(ExtractSimpleValueByKey, T13_ValueWithEscapedQuotes) {
    SCOPED_TRACE(".T13: Extracts value with escaped quotes inside");
    const std::string PinBody = "TargetKey=\"Val with \\\"escaped quotes\\\" end\"";
    const std::string InKey = "TargetKey";
    const std::string Expected = "Val with \\";
    ExpectExtractEquals(".T13_ValueWithEscapedQuotes", PinBody, InKey, Expected);
}


// T14: Key present but malformed or incomplete
TEST(ExtractSimpleValueByKey, T14_UnmatchedOpeningQuote) {
    SCOPED_TRACE(".T14: Returns empty string when value has unmatched opening quote");
    const std::string PinBody = "TargetKey=\"IncompleteValue";
    const std::string InKey = "TargetKey";
    const std::string Expected = "";
    ExpectExtractEquals(".T14_UnmatchedOpeningQuote", PinBody, InKey, Expected);
}

TEST(ExtractSimpleValueByKey, T15_EmptyQuotes) {
    SCOPED_TRACE(".T15: Returns empty string when value is empty inside quotes");
    const std::string PinBody = "TargetKey=\"\"";
    const std::string InKey = "TargetKey";
    const std::string Expected = "";
    ExpectExtractEquals(".T15_EmptyQuotes", PinBody, InKey, Expected);
}

TEST(ExtractSimpleValueByKey, T16_NoQuotesAtAll) {
    SCOPED_TRACE(".T16: Returns empty string when no quotes are present");
    const std::string PinBody = "TargetKey=NoQuotes";
    const std::string InKey = "TargetKey";
    const std::string Expected = "";
    ExpectExtractEquals(".T16_NoQuotesAtAll", PinBody, InKey, Expected);
}


} // namespace Test_ExtractSimpleValueByKey_20250825

