#include "SuccessChanceCounter.h"

#include <gtest/gtest.h>

#include <cmath>

namespace
{
void ExpectLongDoubleNear(
    const long double Actual,
    const long double Expected,
    const long double Tolerance = 1e-12L)
{
    EXPECT_LE(std::fabs(Actual - Expected), Tolerance);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsMinusOne_WhenDiceCountIsNegative)
{
    const long double Result =
        FSuccessChanceCounter::GetProbabilityChance(-1, 6, 5, 1);

    ExpectLongDoubleNear(Result, -1.0L);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsMinusOne_WhenSideNumberIsTooSmall)
{
    const long double Result =
        FSuccessChanceCounter::GetProbabilityChance(3, 1, 1, 1);

    ExpectLongDoubleNear(Result, -1.0L);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsMinusOne_WhenDifficultyIsTooSmall)
{
    const long double Result =
        FSuccessChanceCounter::GetProbabilityChance(3, 6, 1, 1);

    ExpectLongDoubleNear(Result, -1.0L);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsMinusOne_WhenDifficultyExceedsSideNumber)
{
    const long double Result =
        FSuccessChanceCounter::GetProbabilityChance(3, 6, 7, 1);

    ExpectLongDoubleNear(Result, -1.0L);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsMinusOne_WhenTargetSuccessNumberIsZero)
{
    const long double Result =
        FSuccessChanceCounter::GetProbabilityChance(3, 6, 5, 0);

    ExpectLongDoubleNear(Result, -1.0L);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsMinusOne_WhenTargetSuccessNumberIsNegative)
{
    const long double Result =
        FSuccessChanceCounter::GetProbabilityChance(3, 6, 5, -1);

    ExpectLongDoubleNear(Result, -1.0L);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsZero_WhenTargetSuccessesExceedDiceCount)
{
    const long double ResultWithoutCancel =
        FSuccessChanceCounter::GetProbabilityChance(2, 6, 5, 3, false);

    const long double ResultWithCancel =
        FSuccessChanceCounter::GetProbabilityChance(2, 6, 5, 3, true);

    ExpectLongDoubleNear(ResultWithoutCancel, 0.0L);
    ExpectLongDoubleNear(ResultWithCancel, 0.0L);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsZero_WhenDiceCountIsZeroAndTargetIsPositive)
{
    const long double ResultWithoutCancel =
        FSuccessChanceCounter::GetProbabilityChance(0, 6, 5, 1, false);

    const long double ResultWithCancel =
        FSuccessChanceCounter::GetProbabilityChance(0, 6, 5, 1, true);

    ExpectLongDoubleNear(ResultWithoutCancel, 0.0L);
    ExpectLongDoubleNear(ResultWithCancel, 0.0L);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsExactProbabilityWithoutCancel_ForSingleDie)
{
    const long double Result =
        FSuccessChanceCounter::GetProbabilityChance(1, 6, 6, 1, false);

    const long double Expected = 1.0L / 6.0L;

    ExpectLongDoubleNear(Result, Expected);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsExactProbabilityWithoutCancel_ForMultipleDice)
{
    const long double Result =
        FSuccessChanceCounter::GetProbabilityChance(3, 6, 5, 2, false);

    // Success probability = 2 / 6 = 1 / 3
    // P(X >= 2) for X ~ Binomial(3, 1/3) = 3*(1/3)^2*(2/3) + (1/3)^3 = 7/27
    const long double Expected = 7.0L / 27.0L;

    ExpectLongDoubleNear(Result, Expected);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsExactProbabilityWithoutCancel_WhenAllDiceMustSucceed)
{
    const long double Result =
        FSuccessChanceCounter::GetProbabilityChance(3, 6, 5, 3, false);

    // Success probability = 2 / 6 = 1 / 3
    // Need all 3 dice to succeed: (1/3)^3 = 1/27
    const long double Expected = 1.0L / 27.0L;

    ExpectLongDoubleNear(Result, Expected);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsExactProbabilityWithCancel_ForTwoDice)
{
    const long double Result =
        FSuccessChanceCounter::GetProbabilityChance(2, 6, 6, 1, true);

    // On d6 at difficulty 6:
    // success = 1/6, cancel = 1/6, other = 4/6
    // Need net successes >= 1
    // Valid cases:
    //   S=1, C=0, O=1: 2 * (1/6) * (4/6) = 8/36
    //   S=2, C=0, O=0: 1 * (1/6)^2       = 1/36
    // Total = 9/36 = 1/4
    const long double Expected = 1.0L / 4.0L;

    ExpectLongDoubleNear(Result, Expected);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsSameValueWithAndWithoutCancel_ForSingleDie)
{
    const long double ResultWithoutCancel =
        FSuccessChanceCounter::GetProbabilityChance(1, 6, 6, 1, false);

    const long double ResultWithCancel =
        FSuccessChanceCounter::GetProbabilityChance(1, 6, 6, 1, true);

    ExpectLongDoubleNear(ResultWithoutCancel, 1.0L / 6.0L);
    ExpectLongDoubleNear(ResultWithCancel, 1.0L / 6.0L);
    ExpectLongDoubleNear(ResultWithCancel, ResultWithoutCancel);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsLowerProbabilityWithCancel_WhenCancelsCanRemoveSuccesses)
{
    const long double ResultWithoutCancel =
        FSuccessChanceCounter::GetProbabilityChance(2, 10, 2, 1, false);

    const long double ResultWithCancel =
        FSuccessChanceCounter::GetProbabilityChance(2, 10, 2, 1, true);

    // Without cancel:
    // success probability = 9/10
    // P(at least 1 success) = 1 - (1/10)^2 = 99/100
    const long double ExpectedWithoutCancel = 99.0L / 100.0L;

    // With cancel:
    // success = 9/10, cancel = 1/10, other = 0
    // Need net successes >= 1
    // Only S=2, C=0 remains valid: (9/10)^2 = 81/100
    const long double ExpectedWithCancel = 81.0L / 100.0L;

    ExpectLongDoubleNear(ResultWithoutCancel, ExpectedWithoutCancel);
    ExpectLongDoubleNear(ResultWithCancel, ExpectedWithCancel);
    EXPECT_LT(ResultWithCancel, ResultWithoutCancel);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_ReturnsValueBetweenZeroAndOne_ForRepresentativeValidInputs)
{
    const long double ResultA =
        FSuccessChanceCounter::GetProbabilityChance(4, 6, 4, 2, false);

    const long double ResultB =
        FSuccessChanceCounter::GetProbabilityChance(4, 6, 4, 2, true);

    const long double ResultC =
        FSuccessChanceCounter::GetProbabilityChance(8, 10, 7, 3, false);

    const long double ResultD =
        FSuccessChanceCounter::GetProbabilityChance(8, 10, 7, 3, true);

    EXPECT_GE(ResultA, 0.0L);
    EXPECT_LE(ResultA, 1.0L);

    EXPECT_GE(ResultB, 0.0L);
    EXPECT_LE(ResultB, 1.0L);

    EXPECT_GE(ResultC, 0.0L);
    EXPECT_LE(ResultC, 1.0L);

    EXPECT_GE(ResultD, 0.0L);
    EXPECT_LE(ResultD, 1.0L);
}

TEST(FSuccessChanceCounterTests,
     GetProbabilityChance_DoesNotThrow_ForRepresentativeInputs)
{
    EXPECT_NO_THROW(
        FSuccessChanceCounter::GetProbabilityChance(3, 6, 5, 2, false));

    EXPECT_NO_THROW(
        FSuccessChanceCounter::GetProbabilityChance(3, 6, 5, 2, true));

    EXPECT_NO_THROW(
        FSuccessChanceCounter::GetProbabilityChance(-1, 6, 5, 1, true));
}
}  // namespace