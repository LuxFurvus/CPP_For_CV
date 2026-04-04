#include "../FieldParser/FieldParser.h"
////////
#include <string>
#include <optional>
#include <vector>
////////
#include <gtest/gtest.h>
////////

const auto ParseNestedStrings = [](
    const std::string_view Source,
    const std::string_view FieldKey,
    const std::vector<std::pair<char, char>>& Delimiters)
{
    FParseFieldParams Params
    {
        .SourceBlock = Source,
        .FieldKey = FieldKey
    };
    return FieldParser::ParseFieldNestedStrings(Params, Delimiters);
};

TEST(ParseFieldNestedStrings, T01_SingleNestedParentheses)
{
    SCOPED_TRACE("T01: "
        "IF: Delimiters == (), Input has nested content, "
        "THEN: Extract value inside parentheses");

    const std::string_view Source = "PinType.PinSubCategoryMemberReference=TRASH(SomeValue)";
    const std::vector<std::pair<char, char>> Delimiters = { { '(', ')' } };

    const auto Result = ParseNestedStrings(Source, "PinType.PinSubCategoryMemberReference=", Delimiters);

    ASSERT_EQ(Result.size(), 1u);
    EXPECT_EQ(Result.at(0), "SomeValue");
}

TEST(ParseFieldNestedStrings, T02_DoubleNestedQuotes)
{
    SCOPED_TRACE("T02: "
        "IF: Delimiters reflect nested content, "
        "THEN: Extract parenthesis value and inner quoted value");

    const std::string_view Source = "PinType=(PinCategory=\"int\")";
    const std::vector<std::pair<char, char>> Delimiters = { { '(', '"' }, { '"', '"' } };

    const auto Result = ParseNestedStrings(Source, "PinType=", Delimiters);

    ASSERT_EQ(Result.size(), 2u);
    EXPECT_EQ(Result.at(0), "PinCategory=");
    EXPECT_EQ(Result.at(1), "int");
}

TEST(ParseFieldNestedStrings, T03_NoFieldKeyFound)
{
    SCOPED_TRACE("T03: "
        "IF: Field key is not present, "
        "THEN: Expect no value");

    const std::string_view Source = "OtherField=(A=\"B\")";
    const std::vector<std::pair<char, char>> Delimiters = { { '(', ')' }, { '"', '"' } };

    const auto Result = ParseNestedStrings(Source, "MissingField=", Delimiters);

    EXPECT_TRUE(Result.empty());
}

TEST(ParseFieldNestedStrings, T04_MissingInnerDelimiter)
{
    SCOPED_TRACE("T04: "
        "IF: Outer delimiter exists, Inner quoted delimiter is missing, "
        "THEN: Return only the outer block");

    const std::string_view Source = "PinType=(PinCategory=int)";
    const std::vector<std::pair<char, char>> Delimiters = { { '(', ')' }, { '"', '"' } };

    const auto Result = ParseNestedStrings(Source, "PinType=", Delimiters);

    EXPECT_EQ(Result.size(), 1u);
    EXPECT_EQ(Result.at(0), "PinCategory=int");
}

TEST(ParseFieldNestedStrings, T05_EmptySource)
{
    SCOPED_TRACE("T05: "
        "IF: Source is empty, "
        "THEN: No result");

    const std::string_view Source = "";
    const std::vector<std::pair<char, char>> Delimiters = { { '(', ')' }, { '"', '"' } };

    const auto Result = ParseNestedStrings(Source, "PinType=", Delimiters);

    EXPECT_TRUE(Result.empty());
}

TEST(ParseFieldNestedStrings, T06_SupportsOpenCharZero)
{
    SCOPED_TRACE("T06: "
        "IF: First delimiter is 0, "
        "THEN: Extract from beginning to closing delimiter");

    const std::string_view Source = "Value=good(Path=/Script/Object)";
    const std::vector<std::pair<char, char>> Delimiters = { { 0, ')' } };

    const auto Result = ParseNestedStrings(Source, "Value=", Delimiters);

    ASSERT_EQ(Result.size(), 1u);
    EXPECT_EQ(Result.at(0), "good(Path=/Script/Object");
}

TEST(ParseFieldNestedStrings, T07_SupportsCloseCharZero)
{
    SCOPED_TRACE("T07: "
        "IF: Second delimiter is 0, "
        "THEN: Extract from the First delimiter till the end");

    const std::string_view Source = "Value=good(Path=/Script/Object)";
    const std::vector<std::pair<char, char>> Delimiters = { { '(', 0 } };

    const auto Result = ParseNestedStrings(Source, "Value=", Delimiters);

    ASSERT_EQ(Result.size(), 1u);
    EXPECT_EQ(Result.at(0), "Path=/Script/Object)");
}

TEST(ParseFieldNestedStrings, T08_NoDelimitersUsed)
{
    SCOPED_TRACE("T08: "
        "IF: Both Delimiters are 0, "
        "THEN: Return whole value after field key");

    const std::string_view Source = "DesiredPinDirection=EGPD_Output";
    const std::vector<std::pair<char, char>> Delimiters = { { 0, 0 } };

    const auto Result = ParseNestedStrings(Source, "DesiredPinDirection=", Delimiters);

    ASSERT_EQ(Result.size(), 1u);
    EXPECT_EQ(Result.at(0), "EGPD_Output");
}

TEST(ParseFieldNestedStrings, T09_EmptyParentheses)
{
    SCOPED_TRACE("T09: "
        "IF: Delimiters wrap empty string, "
        "THEN: Return empty value");

    const std::string_view Source = "PinType.PinSubCategoryMemberReference=()";
    const std::vector<std::pair<char, char>> Delimiters = { { '(', ')' } };

    const auto Result = ParseNestedStrings(Source, "PinType.PinSubCategoryMemberReference=", Delimiters);

    ASSERT_EQ(Result.size(), 1u);
    EXPECT_EQ(Result.at(0), "");
}

TEST(ParseFieldNestedStrings, E10_MultipleEmptyParentheses)
{
    SCOPED_TRACE("E10: "
        "IF: Two empty groups, "
        "THEN: Return first empty value");

    const std::string_view Source
        = "PinType.PinValueType=(),PinType.PinSubCategoryMemberReference=(lol)";
    const std::vector<std::pair<char, char>> Delimiters = { { '(', ')' } };

    const auto Result = ParseNestedStrings(Source, "PinType.PinValueType=", Delimiters);

    ASSERT_EQ(Result.size(), 1u);
    EXPECT_EQ(Result.at(0), "");
}

TEST(ParseFieldNestedStrings, E11_FieldWithCommaAndSpaceInsideParentheses)
{
    SCOPED_TRACE("E11: "
        "IF: Value has comma inside, "
        "THEN: Return full string inside parentheses");

    const std::string_view Source = "PinType=(PinCategory=\"int\",PinSubCategory=\"\")";
    const std::vector<std::pair<char, char>> Delimiters = { { '(', ')' } };

    const auto Result = ParseNestedStrings(Source, "PinType=", Delimiters);

    ASSERT_EQ(Result.size(), 1u);
    EXPECT_EQ(Result.at(0), "PinCategory=\"int\",PinSubCategory=\"\"");
}

TEST(ParseFieldNestedStrings, E12_InnerDelimiterInsideQuotedString)
{
    SCOPED_TRACE("E12: "
        "IF: ')' inside quotes, "
        "THEN: Still extract correct string");

    const std::string_view Source = "PinType=(PinCategory=\"int with ) in string\")";
    const std::vector<std::pair<char, char>> Delimiters = { { '(', ')' }, { '"', '"' } };

    const auto Result = ParseNestedStrings(Source, "PinType=", Delimiters);

    ASSERT_EQ(Result.size(), 1u);
    EXPECT_EQ(Result.at(0), "PinCategory=\"int with ");
}

TEST(ParseFieldNestedStrings, E13_ConsequtiveDelimiterExtraction)
{
    SCOPED_TRACE("E13: "
        "IF: Sereies of overlapping delimiters, "
        "THEN: Extract each block separately");

    const std::string_view Source = "PinType=(Path=\"/Script/Engine\")";
    const std::vector<std::pair<char, char>> Delimiters = { { '(', '"' }, { '"', '"' } };

    const auto Result = ParseNestedStrings(Source, "PinType=", Delimiters);

    ASSERT_EQ(Result.size(), 2u);
    EXPECT_EQ(Result.at(0), "Path=");
    EXPECT_EQ(Result.at(1), "/Script/Engine");
}

TEST(ParseFieldNestedStrings, E14_TrailingCommaAfterClosingParen)
{
    SCOPED_TRACE("E14: "
        "IF: Value ends with comma, "
        "THEN: Include it in result");

    const std::string_view Source = "LinkedTo=(NodeA ABCD1234,),";
    const std::vector<std::pair<char, char>> Delimiters = { { '(', ')' } };

    const auto Result = ParseNestedStrings(Source, "LinkedTo=", Delimiters);

    ASSERT_EQ(Result.size(), 1u);
    EXPECT_EQ(Result.at(0), "NodeA ABCD1234,");
}

TEST(ParseFieldNestedStrings, E15_ClosingParenWithoutOpening)
{
    SCOPED_TRACE("E15: "
        "IF: ')' has no '(', "
        "THEN: Return no result");

    const std::string_view Source = "PinName=SomeName)";
    const std::vector<std::pair<char, char>> Delimiters = { { '(', ')' } };

    const auto Result = ParseNestedStrings(Source, "PinName=", Delimiters);

    EXPECT_TRUE(Result.empty());
}

TEST(ParseFieldNestedStrings, E16_RealWorldScenario)
{
    SCOPED_TRACE("E15: "
        "IF: RealWorldScenario "
        "THEN: Must work");

    const std::string_view Source = R"(Begin Object Class=/Script/BlueprintGraph.K2Node_FunctionEntry Name="K2Node_FunctionEntry_0" ExportPath="/Script/BlueprintGraph.K2Node_FunctionEntry'/Game/Blueprints/Meta/Controllers/MetaController_ProfileInitializer.MetaController_ProfileInitializer:BpExampleFunc.K2Node_FunctionEntry_0'"
   ExtraFlags=1543634944
   FunctionReference=(MemberName="BpExampleFunc"))";

    const std::vector<std::pair<char, char>> DelimitersClass = { { 0, '.' }, { '.', ' ' } };
    const auto ResultClass = ParseNestedStrings(Source, "Class=", DelimitersClass);

    ASSERT_EQ(ResultClass.size(), 2u);
    EXPECT_EQ(ResultClass.at(0), "/Script/BlueprintGraph");
    EXPECT_EQ(ResultClass.at(1), "K2Node_FunctionEntry");


    const std::vector<std::pair<char, char>> DelimitersName = { { '"', '"' } };
    const auto ResultName = ParseNestedStrings(Source, "Name=", DelimitersName);

    ASSERT_EQ(ResultName.size(), 1u);
    EXPECT_EQ(ResultName.at(0), "K2Node_FunctionEntry_0");


    const std::vector<std::pair<char, char>> DelimitersExportPath
        = { { '"', '\'' }, { '\'', ':' }, { ':', '.' }, { '.', '\'' } };
    const auto ResultExportPath = ParseNestedStrings(Source, "ExportPath=", DelimitersExportPath);

    ASSERT_EQ(ResultExportPath.size(), 4u);
    EXPECT_EQ(ResultExportPath.at(0), "/Script/BlueprintGraph.K2Node_FunctionEntry");
    EXPECT_EQ(ResultExportPath.at(1), "/Game/Blueprints/Meta/Controllers/MetaController_ProfileInitializer.MetaController_ProfileInitializer");
    EXPECT_EQ(ResultExportPath.at(2), "BpExampleFunc");
    EXPECT_EQ(ResultExportPath.at(3), "K2Node_FunctionEntry_0");

    const std::vector<std::pair<char, char>> DelimitersFunctionReference
        = { { '(', '"' }, { '"', '"' } };
    const auto ResultFunctionReference = ParseNestedStrings(Source, "FunctionReference=", DelimitersFunctionReference);

    ASSERT_EQ(ResultFunctionReference.size(), 2u);
    EXPECT_EQ(ResultFunctionReference.at(0), "MemberName=");
    EXPECT_EQ(ResultFunctionReference.at(1), "BpExampleFunc");
}
