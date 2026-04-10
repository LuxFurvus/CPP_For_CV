
#include "SortedRollsMaker.h"

#include <random>
#include <algorithm>
#include <numeric>
#include <functional>

std::vector<int> SortedRollsMaker::GetSortedRoll(
    const int DiceNumber,
    const int SideNumber)
{
    if (DiceNumber < 0 || SideNumber < 2)
    {
        return {};
    }

    static thread_local std::mt19937 Generator(std::random_device{}());
    std::uniform_int_distribution<int> Distribution(1, SideNumber);

    std::vector<int> Results;
    Results.reserve(DiceNumber);

    for (int Index = 0; Index < DiceNumber; ++Index)
    {
        Results.push_back(Distribution(Generator));
    }

    std::sort(Results.begin(), Results.end());

    return Results;
}

int SortedRollsMaker::GetRollSum(
        const std::vector<int> &inRollSequence)
{
    const int SumResult
        = std::accumulate(
            inRollSequence.begin(),
            inRollSequence.end(),
            0);
    return SumResult;
}

int SortedRollsMaker::GetSuccessesNum(
    const std::vector<int>& inSortedRolls,
    const int inDifficulty,
    const bool inWithCancel)
{
    const auto DifficultyChecker
        = [inDifficulty](const int RollValue) -> bool
        {
            return RollValue >= inDifficulty;
        };

    const auto RawSuccessCount
        = std::count_if(
            inSortedRolls.begin(),
            inSortedRolls.end(),
            DifficultyChecker);

    const int SuccessCountInt
        = static_cast<int>(RawSuccessCount);

    const int FailureCount = static_cast<int>(std::count(
        inSortedRolls.begin(),
        inSortedRolls.end(),
        1));

    if (inWithCancel && SuccessCountInt == 0 && FailureCount > 0)
    {
        return -FailureCount;
    }

    int FinalSuccessCount = SuccessCountInt;

    if (inWithCancel)
    {
        FinalSuccessCount -= FailureCount;
    }

    if (FinalSuccessCount < 0)
    {
        FinalSuccessCount = 0;
    }

    return FinalSuccessCount;
}

