
#include "RollAndKeepChanceCounter.h"

#include <gtest/gtest.h>

TEST(RollAndKeepChanceCounterLibTests,
     GetProbabilityList_ReturnsExpectedText_ForSingleDieWithoutExplosions)
{
    constexpr int RollNum = 1;
    constexpr int KeepNum = 1;
    constexpr int Bonus = 0;

    constexpr bool ExplodeTens = false;
    constexpr bool ExplodeOnes = false;

    const std::string Result = RollAndKeepChanceCounter::GetProbabilityList(
        RollNum,
        KeepNum,
        Bonus,
        ExplodeTens,
        ExplodeOnes
    );

    const std::string Expected =
        " 5: 60.00%\n"
        "10: 10.00%\n"
        "15: 00.00%";

    EXPECT_EQ(Result, Expected);
}

TEST(RollAndKeepChanceCounterLibTests,
     GetProbabilityList_ReturnsExpectedText_ForSingleDieWithExplodingTens)
{
    constexpr int RollNum = 1;
    constexpr int KeepNum = 1;
    constexpr int Bonus = 0;

    constexpr bool ExplodeTens = true;
    constexpr bool ExplodeOnes = false;

    const std::string Result = RollAndKeepChanceCounter::GetProbabilityList(
        RollNum,
        KeepNum,
        Bonus,
        ExplodeTens,
        ExplodeOnes
    );

    const std::string Expected =
        " 5: 60.04%\n"
        "10:  9.97%\n"
        "15:  5.97%\n"
        "20:  0.98%";

    EXPECT_EQ(Result, Expected);
}

TEST(RollAndKeepChanceCounterLibTests,
     GetProbabilityList_ReturnsExpectedText_ForSingleDieWithExplodingOnes)
{
    constexpr int RollNum = 1;
    constexpr int KeepNum = 1;
    constexpr int Bonus = 0;

    constexpr bool ExplodeTens = false;
    constexpr bool ExplodeOnes = true;

    const std::string Result = RollAndKeepChanceCounter::GetProbabilityList(
        RollNum,
        KeepNum,
        Bonus,
        ExplodeTens,
        ExplodeOnes
    );

    const std::string Expected =
        " 5: 65.94%\n"
        "10: 10.96%\n"
        "15: 00.00%";

    EXPECT_EQ(Result, Expected);
}

TEST(RollAndKeepChanceCounterLibTests,
     GetProbabilityList_ReturnsExpectedText_ForSingleDieWithExplodingTensAndOnes)
{
    constexpr int RollNum = 1;
    constexpr int KeepNum = 1;
    constexpr int Bonus = 0;

    constexpr bool ExplodeTens = true;
    constexpr bool ExplodeOnes = true;

    const std::string Result = RollAndKeepChanceCounter::GetProbabilityList(
        RollNum,
        KeepNum,
        Bonus,
        ExplodeTens,
        ExplodeOnes
    );

    const std::string Expected =
        " 5: 66.00%\n"
        "10: 11.05%\n"
        "15:  6.63%\n"
        "20:  1.11%\n"
        "25:  0.68%";

    EXPECT_EQ(Result, Expected);
}
