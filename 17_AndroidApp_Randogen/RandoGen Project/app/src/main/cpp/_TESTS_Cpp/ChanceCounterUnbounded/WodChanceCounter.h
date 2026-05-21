
#pragma once

#include <string>

class WodChanceCounter
{
private:

    static const int MinDiceCount = 1;
    static const int MaxDiceCount = 999;

    static const int MinSideNumber = 2;
    static const int MaxSideNumber = 999;

private:

    static bool IsInputValid(
        const int InDiceCount,
        const int InSideNumber,
        const int InDifficulty);

    static long double MultinomialCoefficient(
        const int N, const int S, const int C);

    static long double GetProbabilityChanceWithoutCancel(
        const int InDiceCount,
        const long double InSuccessProbability,
        const int InTargetSuccessNum);

    static long double GetBotchChance(
        const int InDiceCount,
        const int InSideNumber,
        const int InDifficulty);

public:

    static long double GetProbabilityChance(
        const int InDiceCount,
        const int InSideNumber,
        const int InDifficulty,
        const int InTargetSuccessNum,
        const bool InWithCancel);

    static std::string GetProbabilityList(
        const int InDiceCount,
        const int InSideNumber,
        const int InDifficulty,
        const bool InWithCancel);
};
