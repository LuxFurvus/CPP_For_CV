
#pragma once

#include <vector>

struct WodSuccessCounter
{
public:

    static constexpr int GlobalSideNumber = 10;

    static constexpr int MaxTenReroll = 10;

private:

    static int RollSingleDie();

    static std::pair<std::vector<int>, int> GetTenRerollResults(
        const int InDifficulty);

public:

    static std::pair<std::vector<int>, int> GetSuccessesNum(
        const int InDiceCount,
        const int InDifficulty,
        const bool InWithCancel,
        const bool InWithTenReroll);
};
