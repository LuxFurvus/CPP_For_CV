
#include "../FieldParser/FieldParser.h"
////////
#include <string>
#include <optional>
////////
#include <gtest/gtest.h>
////////
#include "../FieldParser/BlueprintGuid.h"

const auto ParseGuidFieldWrapper = [](
    const std::string_view Source,
    const std::string_view Key,
    const FIgnoredRangesRules& Rules)
{
    return FieldParser::ParseGuidField({Source, Key, Rules});
};

TEST(ParseGuidField, T01_ValidGuid)
{
    SCOPED_TRACE("T01: Parse valid GUID string after field key");

    const std::string_view Source = "NodeGuid=E4D043AA430D8EB0F7166EB6BF5EE091";
    const FIgnoredRangesRules Rules;
    const auto Result = ParseGuidFieldWrapper(Source, "NodeGuid=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), FBlueprintGuid(
        0xE4D043AAu, 0x430D8EB0u, 0xF7166EB6u, 0xBF5EE091u));
}

TEST(ParseGuidField, T02_FieldKeyNotFound)
{
    SCOPED_TRACE("T02: Key not found in input string");

    const std::string_view Source = "SomeOtherKey=00000000000000000000000000000000";
    const FIgnoredRangesRules Rules;
    const auto Result = ParseGuidFieldWrapper(Source, "NodeGuid=", Rules);

    EXPECT_FALSE(Result.has_value());
}

TEST(ParseGuidField, T03_InvalidGuidCharacters)
{
    SCOPED_TRACE("T03: GUID value contains invalid characters");

    const std::string_view Source = "NodeGuid=ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ";
    const FIgnoredRangesRules Rules;
    const auto Result = ParseGuidFieldWrapper(Source, "NodeGuid=", Rules);

    EXPECT_FALSE(Result.has_value());
}

TEST(ParseGuidField, T04_InvalidGuidLength)
{
    SCOPED_TRACE("T04: GUID value is too short");

    const std::string_view Source = "NodeGuid=1234";
    const FIgnoredRangesRules Rules;
    const auto Result = ParseGuidFieldWrapper(Source, "NodeGuid=", Rules);

    EXPECT_FALSE(Result.has_value());
}

TEST(ParseGuidField, T05_ValidGuidLowercase)
{
    SCOPED_TRACE("T05: Valid lowercase hexadecimal GUID string");

    const std::string_view Source = "NodeGuid=e4d043aa430d8eb0f7166eb6bf5ee091";
    const FIgnoredRangesRules Rules;
    const auto Result = ParseGuidFieldWrapper(Source, "NodeGuid=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), FBlueprintGuid(
        0xE4D043AAu, 0x430D8EB0u, 0xF7166EB6u, 0xBF5EE091u));
}

TEST(ParseGuidField, T06_GuidInDenseBlock)
{
    SCOPED_TRACE("T06: Extract NodeGuid from dense Blueprint-like block");

    const std::string_view Source =
        R"(erence=(MemberParent="/Script/CoreUObject.Class'/Script/Engine.KismetMathLibrary'",MemberName="Add_IntInt")
           NodePosX=176
           NodePosY=53
           NodeGuid=E4D043AA430D8EB0F7166EB6BF5EE091
           CustomProperties Pin (PinId=F82103034A69C56CC7ED9EB8015B8D31,PinName="A",PinToolTip="A\nInteger",PinType.PinCategory="int",PinType.PinSubCategory="")";

    const FIgnoredRangesRules Rules;
    const auto Result = ParseGuidFieldWrapper(Source, "NodeGuid=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), FBlueprintGuid(
        0xE4D043AAu, 0x430D8EB0u, 0xF7166EB6u, 0xBF5EE091u));
}

TEST(ParseGuidField, T07_EmbeddedGuidInCustomProperties)
{
    SCOPED_TRACE("T07: Extract PersistentGuid inside CustomProperties block");

    const std::string_view Source =
        R"(CustomProperties Pin (PinId=F82103034A69C56CC7ED9EB8015B8D31,
           PinName="A",PinToolTip="A\nInteger",
           PinType.PinCategory="int",
           PersistentGuid=00000000000000000000000000000000,
           bHidden=False))";

    const FIgnoredRangesRules Rules;
    const auto Result = ParseGuidFieldWrapper(Source, "PersistentGuid=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), FBlueprintGuid(0, 0, 0, 0));
}

TEST(ParseGuidField, T08_GuidPrecededByJunk)
{
    SCOPED_TRACE("T08: Valid GUID preceded by garbage text and line breaks");

    const std::string_view Source =
        R"(SomeJunkData!!!NodeGuid=E4D043AA430D8EB0F7166EB6BF5EE091
           ExtraTrailingData)";
    
    const FIgnoredRangesRules Rules;
    const auto Result = ParseGuidFieldWrapper(Source, "NodeGuid=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), FBlueprintGuid(
        0xE4D043AAu, 0x430D8EB0u, 0xF7166EB6u, 0xBF5EE091u));
}

TEST(ParseGuidField, T09_GuidAfterMultipleProperties)
{
    SCOPED_TRACE("T09: Extract GUID after many unrelated fields");

    const std::string_view Source =
        R"(PinType.PinCategory="int",PinType.PinSubCategory="",PinType.PinValueType=(),
           PinType.ContainerType=None,
           PersistentGuid=62FB163A4C4078FBEC7D02A88557D266,
           PinType.bIsReference=False)";

    const FIgnoredRangesRules Rules;
    const auto Result = ParseGuidFieldWrapper(Source, "PersistentGuid=", Rules);

    EXPECT_TRUE(Result.has_value());
    EXPECT_EQ(Result.value(), FBlueprintGuid(
        0x62FB163Au, 0x4C4078FBu, 0xEC7D02A8u, 0x8557D266u));
}
