
#include "../FieldParser/BlueprintGuid.h"
////////
#include <string>
#include <optional>
////////
#include <gtest/gtest.h>

const auto MakeGuid = [](const std::string_view Input)
{
    return FBlueprintGuid::MakeGuidFromString(Input);
};

TEST(FBlueprintGuid, T01_ValidGuidAllZeroes)
{
    SCOPED_TRACE("T01: Parse valid 32-character zeroed hex string");

    const std::string_view Input = "00000000000000000000000000000000";
    const auto Result = MakeGuid(Input);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), FBlueprintGuid(0, 0, 0, 0));
}

TEST(FBlueprintGuid, T02_ValidGuidMixedHex)
{
    SCOPED_TRACE("T02: Parse valid 32-character hex string with mixed digits");

    const std::string_view Input = "E4D043AA430D8EB0F7166EB6BF5EE091";
    const auto Result = MakeGuid(Input);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), FBlueprintGuid(
        0xE4D043AAu, 0x430D8EB0u, 0xF7166EB6u, 0xBF5EE091u));
}

TEST(FBlueprintGuid, T03_TooShort)
{
    SCOPED_TRACE("T03: Reject too short string (less than 32 characters)");

    const std::string_view Input = "1234";
    const auto Result = MakeGuid(Input);

    EXPECT_FALSE(Result.has_value());
}

TEST(FBlueprintGuid, T04_TooLong)
{
    SCOPED_TRACE("T04: Reject too long string (more than 32 characters)");

    const std::string_view Input = "0000000000000000000000000000000000";
    const auto Result = MakeGuid(Input);

    EXPECT_FALSE(Result.has_value());
}

TEST(FBlueprintGuid, T05_InvalidCharacters)
{
    SCOPED_TRACE("T05: Reject input with non-hex characters");

    const std::string_view Input = "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ";
    const auto Result = MakeGuid(Input);

    EXPECT_FALSE(Result.has_value());
}

TEST(FBlueprintGuid, T06_ValidLowercaseHex)
{
    SCOPED_TRACE("T06: Accept lowercase hex input");

    const std::string_view Input = "e4d043aa430d8eb0f7166eb6bf5ee091";
    const auto Result = MakeGuid(Input);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), FBlueprintGuid(
        0xe4d043aau, 0x430d8eb0u, 0xf7166eb6u, 0xbf5ee091u));
}

TEST(FBlueprintGuid, T07_ValidMixedCaseHex)
{
    SCOPED_TRACE("T07: Accept mixed-case hex input");

    const std::string_view Input = "E4D043aa430d8eb0F7166Eb6bf5ee091";
    const auto Result = MakeGuid(Input);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), FBlueprintGuid(
        0xE4D043AAu, 0x430D8EB0u, 0xF7166EB6u, 0xBF5EE091u));
}
