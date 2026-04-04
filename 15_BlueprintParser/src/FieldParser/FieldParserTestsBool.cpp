
#include "../FieldParser/FieldParser.h"
////////
#include <gtest/gtest.h>
#include <string>
#include <optional>
////////

//////////////////////////////////////////////////////
// Test wrapper around ParseFieldBool
//////////////////////////////////////////////////////

const auto ParseBoolField = [](
    const std::string_view Source,
    const std::string_view Key,
    const FIgnoredRangesRules& Rules)
{
    return FieldParser::ParseFieldBool({Source, Key, Rules});
};

//////////////////////////////////////////////////////
// ParseFieldBool, T01_ValidTrue
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T01_ValidTrue)
{
    SCOPED_TRACE("T01: Extract valid 'true' boolean value after field key");

    const std::string_view Source = "bCanEverTick=True";

    const FIgnoredRangesRules Rules;

    const auto Result = ParseBoolField(Source, "bCanEverTick=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_TRUE(Result.value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T02_ValidFalse
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T02_ValidFalse)
{
    SCOPED_TRACE("T02: Extract valid 'false' boolean value after field key");

    const std::string_view Source = "bHiddenFalse";

    const FIgnoredRangesRules Rules;
    const auto Result = ParseBoolField(Source, "bHidden", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_FALSE(Result.value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T03_NoFieldKey
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T03_NoFieldKey)
{
    SCOPED_TRACE("T03: Field key not found, expect std::nullopt");

    const std::string_view Source = "Visible True";

    const FIgnoredRangesRules Rules;
    const auto Result = ParseBoolField(Source, "bHidden", Rules);

    EXPECT_FALSE(Result.has_value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T04_NoBooleanAfterKey
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T04_NoBooleanAfterKey)
{
    SCOPED_TRACE("T04: Field key found, but no boolean value after it");

    const std::string_view Source = "bVisible";

    const FIgnoredRangesRules Rules;
    const auto Result = ParseBoolField(Source, "bVisible", Rules);

    EXPECT_FALSE(Result.has_value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T05_HandlesWhitespaceAndCase
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T05_HandlesWhitespaceAndCase)
{
    SCOPED_TRACE("T05: Case-insensitive match and whitespace tolerant");

    const std::string_view Source = "  bIsVisible = TRUE ";

    const FIgnoredRangesRules Rules {
        .IgnoredChar = [](char c) { return std::isspace(static_cast<unsigned char>(c)); },
        .IgnoredRangesBetween = {},
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "bIsVisible=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_TRUE(Result.value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T06_InvalidBooleanText
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T06_InvalidBooleanText)
{
    SCOPED_TRACE("T06: Field key present but value is not 'true' or 'false'");

    const std::string_view Source = "bShouldTick maybe";

    const FIgnoredRangesRules Rules;
    const auto Result = ParseBoolField(Source, "bShouldTick", Rules);

    EXPECT_FALSE(Result.has_value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T07_EmptySource
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T07_EmptySource)
{
    SCOPED_TRACE("T07: Source block is empty");

    const std::string_view Source = "";

    const FIgnoredRangesRules Rules;
    const auto Result = ParseBoolField(Source, "bCanEverTick", Rules);

    EXPECT_FALSE(Result.has_value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T08_EmptyKey
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T08_EmptyKey)
{
    SCOPED_TRACE("T08: Field key is empty");

    const std::string_view Source = "bHidden false";

    const FIgnoredRangesRules Rules;
    const auto Result = ParseBoolField(Source, "", Rules);

    EXPECT_FALSE(Result.has_value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T09_FieldBetweenCommas
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T09_FieldBetweenCommas)
{
    SCOPED_TRACE("T09: Field is enclosed between commas with inner spaces");

    const std::string_view Source =
        "PinType.bIsConst=False,bH idden= False,bOrpha nedPin=False";

    const FIgnoredRangesRules Rules {
        .IgnoredChar = [](char c) { return std::isspace(static_cast<unsigned char>(c)); },
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "bHidden=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_FALSE(Result.value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T10_FieldNameOverlap
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T10_FieldNameOverlap)
{
    SCOPED_TRACE("T10: Ensure 'bHidden' is not confused with 'bHiddenDepth'");

    const std::string_view Source =
        "bHiddenDepth=true,bHidden=false";

    const FIgnoredRangesRules Rules {
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "bHidden=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_TRUE(Result.value() == false);
}

//////////////////////////////////////////////////////
// ParseFieldBool, T11_ValueInsideParentheses
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T11_ValueInsideParentheses)
{
    SCOPED_TRACE("T11: Boolean value enclosed in parentheses should not be parsed");

    const std::string_view Source =
        "bOrphanedPin=(false)";

    const FIgnoredRangesRules Rules {
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "bOrphanedPin=", Rules);

    EXPECT_FALSE(Result.has_value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T12_DuplicateKeys
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T12_DuplicateKeys)
{
    SCOPED_TRACE("T12: First occurrence of duplicate keys is parsed");

    const std::string_view Source =
        "bAdvancedView=False,bAdvancedView=True";

    const FIgnoredRangesRules Rules {
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "bAdvancedView=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_FALSE(Result.value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T13_NestedFieldName
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T13_NestedFieldName)
{
    SCOPED_TRACE("T13: Nested field name like 'PinType.bIsConst' is parsed");

    const std::string_view Source =
        "PinType.bIsConst=False";

    const FIgnoredRangesRules Rules {
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "PinType.bIsConst=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_FALSE(Result.value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T14_QuotedBooleanValue
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T14_QuotedBooleanValue)
{
    SCOPED_TRACE("T14: Quoted boolean value should not be parsed");

    const std::string_view Source =
        "bDefaultValueIsReadOnly=\"false\"";

    const FIgnoredRangesRules Rules {
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "bDefaultValueIsReadOnly", Rules);

    EXPECT_FALSE(Result.has_value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T15_SpaceBeforeField
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T15_SpaceBeforeField)
{
    SCOPED_TRACE("T15: Space before field should be ignored");

    const std::string_view Source =
        ", bNotConnectable=False,";

    const FIgnoredRangesRules Rules {
        .IgnoredChar = [](char c) { return std::isspace(static_cast<unsigned char>(c)); },
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "bNotConnectable=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_FALSE(Result.value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T16_MixedCaseBoolean
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T16_MixedCaseBoolean)
{
    SCOPED_TRACE("T16: Mixed-case boolean value is parsed if case-insensitive");

    const std::string_view Source =
        "bDefaultValueIsIgnored=TrUe";

    const FIgnoredRangesRules Rules {
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "bDefaultValueIsIgnored=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_TRUE(Result.value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T17_InvalidBooleanSuffix
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T17_InvalidBooleanSuffix)
{
    SCOPED_TRACE("T17: Invalid boolean with suffix should not be parsed");

    const std::string_view Source =
        "ssfbAdvancedView=Falsefied";

    const FIgnoredRangesRules Rules;

    const auto Result = ParseBoolField(Source, "bAdvancedView=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_FALSE(Result.value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T18_ValueInsideIgnoredRange
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T18_ValueInsideIgnoredRange)
{
    SCOPED_TRACE("T18: Boolean value inside ignored range should be skipped");

    const std::string_view Source = "bTest=(true)";

    const FIgnoredRangesRules Rules {
        .IgnoredRangesBetween = { { '(', ')' } },
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "bTest=", Rules);

    EXPECT_FALSE(Result.has_value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T19_EmptyValueAfterKey
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T19_EmptyValueAfterKey)
{
    SCOPED_TRACE("T19: Field key found but no value provided");

    const std::string_view Source = "bExperimental=";

    const FIgnoredRangesRules Rules {
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "bExperimental=", Rules);

    EXPECT_FALSE(Result.has_value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T20_BooleanAtEndOfLine
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T20_BooleanAtEndOfLine)
{
    SCOPED_TRACE("T20: Boolean value at end of string is parsed correctly");

    const std::string_view Source = "bVisible=False";

    const FIgnoredRangesRules Rules {
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "bVisible=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_FALSE(Result.value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T21_FieldNameSubstringInsideWord
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T21_FieldNameSubstringInsideWord)
{
    SCOPED_TRACE("T21: Field key that is a substring of a longer word should not match");

    const std::string_view Source = "sbHiddenOption=True";

    const FIgnoredRangesRules Rules {
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "bHidden=", Rules);

    EXPECT_FALSE(Result.has_value());
}

//////////////////////////////////////////////////////
// ParseFieldBool, T22_QuotedEmptyValue
//////////////////////////////////////////////////////

TEST(ParseFieldBool, T22_QuotedEmptyValue)
{
    SCOPED_TRACE("T22: Quoted empty string should not be interpreted as boolean");

    const std::string_view Source = "bIsEnabled=\"\"";

    const FIgnoredRangesRules Rules {
        .bCaseInsensitive = true
    };

    const auto Result = ParseBoolField(Source, "bIsEnabled=", Rules);

    EXPECT_FALSE(Result.has_value());
}
