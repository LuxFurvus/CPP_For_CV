
#pragma once

#include <array>
#include <sstream>
#include <string>
#include <vector>

#include "DndRollPair.h"

class DndChancesCounter
{

private:

    static std::array<int, 3> GetMinAvgMaxValues(
        const std::vector<RollPair>& InRollData,
        const int InRollModifier)
    {
        if (InRollData.empty())
        {
            return {};
        }

        int MinValue = InRollModifier;
        int MaxValue = InRollModifier;

        for (const auto& RollData : InRollData)
        {
            MinValue += RollData.DiceNum;
            MaxValue += RollData.DiceNum * RollData.SideNum;
        }

        const int AvgValue = (MinValue + MaxValue) / 2;

        return { MinValue, AvgValue, MaxValue };
    }

public:

    static std::string GetMinAvgMaxValuesString(
        const std::vector<RollPair>& InRollData,
        const int InRollModifier)
    {
        if (InRollData.empty())
        {
            return std::to_string(InRollModifier);
        }

        const auto [MinValue, AvgValue, MaxValue]
            = GetMinAvgMaxValues(
                InRollData, InRollModifier);

        std::ostringstream ResultStream;
        ResultStream
            << "Min: " << MinValue << "\n\n"
            << "Avg: " << AvgValue << "\n\n"
            << "Max: " << MaxValue;
        return ResultStream.str();
    }

};
