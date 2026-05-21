
#pragma once

#include <vector>

struct RollAndKeepRulesLib
{
private:

static const int SideNumber = 10;

    static int GetSingleDieRoll();

    static int GetDieValue(
        const bool InExplodeTens,
        const bool InExplodeOnes);

public:

static std::tuple<std::vector<int>, int> GetRollAndKeepResults(
        const int InRollNum,
        const int InKeepNum,
        const int InBonus,
        const bool InExplodeTens,
        const bool InExplodeOnes);
};
