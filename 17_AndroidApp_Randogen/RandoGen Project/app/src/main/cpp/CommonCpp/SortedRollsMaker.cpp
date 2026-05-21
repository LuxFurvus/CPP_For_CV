
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
