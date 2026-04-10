
#pragma once

#include <string>

class FSuccessChanceCounter
{
private:

    static long double BinomialCoefficient(
        int N, int K);

    static long double MultinomialCoefficient(
        int N, int S, int C);

    static long double GetProbabilityChanceWithoutCancel(
        int InDiceCount,
        long double InSuccessProbability,
        int InTargetSuccessNum);

    static long double GetBotchChance(
        const int InDiceCount,
        const int InSideNumber,
        const int InDifficulty);

    static long double GetProbabilityChance(
        const int InDiceCount,
        const int InSideNumber,
        const int InDifficulty,
        const int InTargetSuccessNum,
        const bool InWithCancel);

public:

    static std::string GetProbabilityList(
        const int InDiceCount,
        const int InSideNumber,
        const int InDifficulty,
        const bool InWithCancel);
};
