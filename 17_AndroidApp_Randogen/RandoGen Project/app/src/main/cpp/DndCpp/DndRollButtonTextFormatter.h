#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "DndRollPair.h"

class DndRollButtonTextFormatter
{
private:

    static void AppendModifier(
        std::ostringstream& ResultStream,
        const bool HasRollData,
        const int InRollModifier)
    {
        if (InRollModifier == 0)
        {
            return;
        }

        if (HasRollData && InRollModifier > 0)
        {
            ResultStream << " + ";
        }

        ResultStream << InRollModifier;
    }

public:

    static std::string GetRollButtonText(
        const std::vector<RollPair>& InRollData,
        const int InRollModifier)
    {
        if (InRollData.empty() && InRollModifier == 0)
        {
            return "";
        }

        std::ostringstream ResultStream;
        bool HasRollData = false;

        for (const RollPair& CurrentRollData : InRollData)
        {
            if (CurrentRollData.DiceNum <= 0 || CurrentRollData.SideNum <= 0)
            {
                continue;
            }

            if (HasRollData)
            {
                ResultStream << " + ";
            }

            ResultStream
                << CurrentRollData.DiceNum
                << 'd'
                << CurrentRollData.SideNum;

            HasRollData = true;
        }

        AppendModifier(
            ResultStream,
            HasRollData,
            InRollModifier
        );

        return ResultStream.str();
    }
};