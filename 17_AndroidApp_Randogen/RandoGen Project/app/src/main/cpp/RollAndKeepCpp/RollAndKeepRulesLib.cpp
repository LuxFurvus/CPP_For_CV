
#include "RollAndKeepRulesLib.h"

#include <algorithm>
#include <numeric>
#include <vector>
#include <array>
#include <tuple>

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#include "../CommonCpp/CommonMathLib.h"
#include "../CommonCpp/SortedRollsMaker.h"
#include "RollAndKeepChanceCounter.h"


int RollAndKeepRulesLib::GetSingleDieRoll()
{
    const std::vector<int> Rolls
        = SortedRollsMaker::GetSortedRoll(
            1, SideNumber);

    if (Rolls.empty())
    {
        return 0;
    }

    return Rolls[0];
}

int RollAndKeepRulesLib::GetDieValue(
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    int DieValue = GetSingleDieRoll();

    if (InExplodeOnes && DieValue == 1)
    {
        DieValue = GetSingleDieRoll();
    }

    if (!InExplodeTens || DieValue != SideNumber)
    {
        return DieValue;
    }

    int TotalValue = DieValue;

    while (true)
    {
        const int ExtraValue = GetSingleDieRoll();

        TotalValue += ExtraValue;

        if (ExtraValue != SideNumber)
        {
            break;
        }
    }

    return TotalValue;
}


std::tuple<std::vector<int>, int> RollAndKeepRulesLib::GetRollAndKeepResults(
    const int InRollNum,
    const int InKeepNum,
    const int InBonus,
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    const auto [ClampedRollNum, ClampedKeepNum, ClampedBonus]
        = RollAndKeepChanceCounter::ApplyTenDiceRule(
            {InRollNum, InKeepNum, InBonus});

    if (ClampedRollNum <= 0 || ClampedKeepNum <= 0)
    {
        return std::make_tuple(std::vector<int>{}, ClampedBonus);
    }

    std::vector<int> SelectedRolls;
    SelectedRolls.reserve(ClampedRollNum);

    for (int DieIndex = 0; DieIndex < ClampedRollNum; ++DieIndex)
    {
        const int OneDieValue = RollAndKeepRulesLib::GetDieValue(
            InExplodeTens, InExplodeOnes);

        SelectedRolls.emplace_back(OneDieValue);
    }

    if (SelectedRolls.empty())
    {
        return std::make_tuple(std::vector<int>{}, ClampedBonus);
    }

    const int SelectedRollsSize
        = static_cast<int>(SelectedRolls.size());

    const int CorrectedKeepNum
        = (ClampedKeepNum > SelectedRollsSize) ? SelectedRollsSize : ClampedKeepNum;

    std::sort(SelectedRolls.begin(), SelectedRolls.end());
    std::reverse(SelectedRolls.begin(), SelectedRolls.end());

    const int ReturnResult = std::accumulate(
        SelectedRolls.begin(),
        SelectedRolls.begin() + CorrectedKeepNum,
        ClampedBonus);

    std::sort(SelectedRolls.begin(), SelectedRolls.end());

    return std::make_tuple(SelectedRolls, ReturnResult);
}
