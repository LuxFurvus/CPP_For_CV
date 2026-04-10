
#pragma once

#ifndef RANDOGEN_SORTEDROLLSMAKER_H
#define RANDOGEN_SORTEDROLLSMAKER_H

#include <vector>

struct SortedRollsMaker
{
    static std::vector<int> GetSortedRoll(
        const int DiceNumber,
        const int SideNumber);

    static int GetSuccessesNum(
        const std::vector<int>& inSortedRolls,
        const int inDifficulty,
        const bool inWithCancel);

    static int GetRollSum(
        const std::vector<int>& inRollSequence);
};

#endif //RANDOGEN_SORTEDROLLSMAKER_H
