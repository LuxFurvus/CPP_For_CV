
#include "../FieldParser/FieldParser.h"
////////
#include <string>
#include <optional>
////////
#include <gtest/gtest.h>


const auto ParseNumericFieldWrapper = [](
    const std::string_view Source,
    const std::string_view Key,
    const FIgnoredRangesRules& Rules)
{
    return FieldParser::ParseNumericField({Source, Key, Rules});
};


TEST(ParseNumericField, T01_SimpleValidInteger)
{
    SCOPED_TRACE("T01: Parse valid integer from field like 'NodePosX=176'");

    const std::string_view Source = "NodePosX=176";
    const FIgnoredRangesRules Rules;
    const auto Result = ParseNumericFieldWrapper(Source, "NodePosX=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), 176u);
}

TEST(ParseNumericField, T02_ValidIntegerAfterPrefix)
{
    SCOPED_TRACE("T02: Parse field with padding and text after number");

    const std::string_view Source = "   NodePosY=53  // comment";
    const FIgnoredRangesRules Rules;
    const auto Result = ParseNumericFieldWrapper(Source, "NodePosY=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), 53u);
}

TEST(ParseNumericField, T03_FieldKeyNotFound)
{
    SCOPED_TRACE("T03: Key 'NodePosZ=' is not in source text");

    const std::string_view Source = "NodePosX=176 NodePosY=53";
    const FIgnoredRangesRules Rules;
    const auto Result = ParseNumericFieldWrapper(Source, "NodePosZ=", Rules);

    EXPECT_FALSE(Result.has_value());
}

TEST(ParseNumericField, T04_EmptyNumericValue)
{
    SCOPED_TRACE("T04: Key is found, but no number follows");

    const std::string_view Source = "NodePosX=";
    const FIgnoredRangesRules Rules;
    const auto Result = ParseNumericFieldWrapper(Source, "NodePosX=", Rules);

    EXPECT_FALSE(Result.has_value());
}

TEST(ParseNumericField, T05_NumberWithIgnoredCharacters)
{
    SCOPED_TRACE("T05: Number interleaved with ignored underscores");

    const std::string_view Source = "NodePosY=1_2_3_4";

    FIgnoredRangesRules Rules
    {
        .IgnoredChar = [](char c) { return c == '_'; },
    };

    const auto Result = ParseNumericFieldWrapper(Source, "NodePosY=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), 1234u);
}

TEST(ParseNumericField, T06_Overflow)
{
    SCOPED_TRACE("T06: Overflow scenario with too large numeric value");

    const std::string_view Source = "Value=184467440737095516160";

    const FIgnoredRangesRules Rules;
    const auto Result = ParseNumericFieldWrapper(Source, "Value=", Rules);

    EXPECT_FALSE(Result.has_value());
}

TEST(ParseNumericField, T07_NonNumericCharactersEarly)
{
    SCOPED_TRACE("T07: Early non-numeric character after key");

    const std::string_view Source = "NodePosX=abc123";

    const FIgnoredRangesRules Rules;
    const auto Result = ParseNumericFieldWrapper(Source, "NodePosX=", Rules);

    EXPECT_FALSE(Result.has_value());
}

TEST(ParseNumericField, T08_LeadingWhitespaceIgnored)
{
    SCOPED_TRACE("T08: Ignored whitespace between key and number");

    const std::string_view Source = "NodePosY=   42";

    FIgnoredRangesRules Rules
    {
        .IgnoredChar = [](char c) { return c == ' '; },
    };

    const auto Result = ParseNumericFieldWrapper(Source, "NodePosY=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), 42u);
}
