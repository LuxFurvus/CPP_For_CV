#pragma once

#include <string>
#include <utility>
#include <vector>

class WodChanceCounter
{
private:

    static const int MinDiceCount = 1;
    static const int MaxDiceCount = 999;

    static const int MinDifficulty = 2;
    static const int GlobalSideNumber = 10;

    static const int MaxTenReroll = 10;

    static constexpr long double MinDisplayedProbability = 0.0001L;

private:

    static bool IsInputValid(
        const int InDiceCount,
        const int InDifficulty);

    static int GetMaxSuccessPerDie(
        const bool InWithTenReroll);

    static std::vector<std::pair<int, long double>> GetSingleDieNetDistribution(
        const int InDifficulty,
        const bool InWithCancel,
        const bool InWithTenReroll);

    static std::vector<long double> GetNetDistribution(
        const int InDiceCount,
        const int InDifficulty,
        const bool InWithCancel,
        const bool InWithTenReroll,
        int& OutMinNet,
        int& OutMaxNet);

    static long double GetTailProbability(
        const std::vector<long double>& InNetDistribution,
        const int InMinNet,
        const int InMaxNet,
        const int InTargetSuccessNum);

    static long double GetBotchProbability(
        const std::vector<long double>& InNetDistribution,
        const int InMinNet);

public:

    static std::string GetProbabilityList(
        const int InDiceCount,
        const int InDifficulty,
        const bool InWithCancel,
        const bool InWithTenReroll);
};