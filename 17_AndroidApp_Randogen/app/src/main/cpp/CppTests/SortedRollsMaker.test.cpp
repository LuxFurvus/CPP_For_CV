#include "SortedRollsMaker.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

namespace
{
    TEST(SortedRollsMakerTests, GetRoll_ReturnsEmpty_WhenDiceNumberIsNegative)
{
    const SortedRollsMaker Maker;

    const std::vector<int> Result
        = SortedRollsMaker::GetRoll(-1, 6);

    EXPECT_TRUE(Result.empty());
}

TEST(SortedRollsMakerTests, GetRoll_ReturnsEmpty_WhenSideNumberIsLessThanTwo)
{
    EXPECT_TRUE(SortedRollsMaker::GetRoll(3, 1).empty());
    EXPECT_TRUE(SortedRollsMaker::GetRoll(3, 0).empty());
    EXPECT_TRUE(SortedRollsMaker::GetRoll(3, -5).empty());
}

TEST(SortedRollsMakerTests, GetRoll_ReturnsEmpty_WhenDiceNumberIsZero)
{

const std::vector<int> Result
    = SortedRollsMaker::GetRoll(0, 6);

EXPECT_TRUE(Result.empty());
}

TEST(SortedRollsMakerTests, GetRoll_ReturnsRequestedNumberOfRolls_ForValidInput)
{
constexpr int DiceNumber = 5;
constexpr int SideNumber = 6;
const std::vector<int> Result
    = SortedRollsMaker::GetRoll(DiceNumber, SideNumber);

ASSERT_EQ(Result.size(), static_cast<std::size_t>(DiceNumber));
}

TEST(SortedRollsMakerTests, GetRoll_ReturnsValuesWithinExpectedRange_ForValidInput)
{
constexpr int DiceNumber = 100;
constexpr int SideNumber = 6;

const std::vector<int> Result
    = SortedRollsMaker::GetRoll(DiceNumber, SideNumber);

ASSERT_EQ(Result.size(), static_cast<std::size_t>(DiceNumber));

for (const int Value : Result)
{
    EXPECT_GE(Value, 1);
    EXPECT_LE(Value, SideNumber);
}
}

TEST(SortedRollsMakerTests, GetRoll_ReturnsValuesSortedInAscendingOrder_ForValidInput)
{
constexpr int DiceNumber = 50;
constexpr int SideNumber = 20;

const std::vector<int> Result
    = SortedRollsMaker::GetRoll(DiceNumber, SideNumber);

ASSERT_EQ(Result.size(), static_cast<std::size_t>(DiceNumber));
EXPECT_TRUE(std::is_sorted(Result.begin(), Result.end()));
}

TEST(SortedRollsMakerTests, GetRoll_ReturnsOnlyOneForSingleSidedOutcomeRangeOfTwo)
{
constexpr int DiceNumber = 1;
constexpr int SideNumber = 2;

const std::vector<int> Result
    = SortedRollsMaker::GetRoll(DiceNumber, SideNumber);

ASSERT_EQ(Result.size(), 1U);
EXPECT_GE(Result.front(), 1);
EXPECT_LE(Result.front(), SideNumber);
}

TEST(SortedRollsMakerTests, GetRoll_HandlesLargeValidRequest)
{
constexpr int DiceNumber = 1000;
constexpr int SideNumber = 100;

const std::vector<int> Result
    = SortedRollsMaker::GetRoll(DiceNumber, SideNumber);

ASSERT_EQ(Result.size(), static_cast<std::size_t>(DiceNumber));
EXPECT_TRUE(std::is_sorted(Result.begin(), Result.end()));

for (const int Value : Result)
{
EXPECT_GE(Value, 1);
EXPECT_LE(Value, SideNumber);
}
}

TEST(SortedRollsMakerTests, GetRoll_ReturnsEmpty_WhenBothInputsAreInvalid)
{

const std::vector<int> Result
    = SortedRollsMaker::GetRoll(-10, 1);

EXPECT_TRUE(Result.empty());
}
}  // namespace