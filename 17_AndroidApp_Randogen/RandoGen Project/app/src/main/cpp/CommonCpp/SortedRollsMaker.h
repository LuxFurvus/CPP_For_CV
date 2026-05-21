
#pragma once

#ifndef RANDOGEN_SORTEDROLLSMAKER_H
#define RANDOGEN_SORTEDROLLSMAKER_H

#include <vector>

struct SortedRollsMaker
{
    static std::vector<int> GetSortedRoll(
        const int InDiceNumber,
        const int InSideNumber);
};

#endif //RANDOGEN_SORTEDROLLSMAKER_H
