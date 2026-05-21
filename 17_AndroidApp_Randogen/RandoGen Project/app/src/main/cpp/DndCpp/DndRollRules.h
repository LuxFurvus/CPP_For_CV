
#pragma once

#ifndef RANDOGEN_DNDROLLRULES_H
#define RANDOGEN_DNDROLLRULES_H

#include <numeric>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "../CommonCpp/SortedRollsMaker.h"
#include "DndRollPair.h"


struct DndRollRulesLib
{
private:

    static std::string FormatRollLine(
        const RollPair& InRollData,
        const std::vector<int>& InRollSequence,
        const int InRollSum)
    {
        std::ostringstream ResultStream;

        ResultStream
            << "* "
            << InRollData.DiceNum
            << 'd'
            << InRollData.SideNum
            << ": ";

        for (std::size_t Index = 0; Index < InRollSequence.size(); ++Index)
        {
            if (Index > 0)
            {
                ResultStream << ", ";
            }

            ResultStream << InRollSequence[Index];
        }

        if (InRollSequence.size() > 1)
        {
            ResultStream << " = " << InRollSum;
        }

        return ResultStream.str();
    }

public:

    static std::tuple<std::string, int> GetDndRollResults(
        const std::vector<RollPair>& InRollData,
        const int InRollModifier)
    {
        if (InRollData.empty())
        {
            return {};
        }

        std::ostringstream ResultStream;
        int TotalSum = InRollModifier;
        bool IsFirstLine = true;

        for (const RollPair& CurrentRollData : InRollData)
        {
            const std::vector<int> CurrentSequence =
                SortedRollsMaker::GetSortedRoll(
                    CurrentRollData.DiceNum,
                    CurrentRollData.SideNum);

            if (CurrentSequence.empty())
            {
                continue;
            }

            const int CurrentSequenceSum = std::accumulate(
                CurrentSequence.begin(),
                CurrentSequence.end(),
                0);

            if (!IsFirstLine)
            {
                ResultStream << '\n';
            }

            ResultStream << FormatRollLine(
                CurrentRollData,
                CurrentSequence,
                CurrentSequenceSum);

            TotalSum += CurrentSequenceSum;
            IsFirstLine = false;
        }

        if (IsFirstLine)
        {
            return {};
        }

        return std::make_tuple(ResultStream.str(), TotalSum);
    }
};

#endif // RANDOGEN_DNDROLLRULES_H
