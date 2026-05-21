#include "WodChanceCounter.h"

#include <iomanip>
#include <sstream>

bool WodChanceCounter::IsInputValid(
    const int InDiceCount,
    const int InDifficulty)
{
    return
        InDiceCount >= MinDiceCount &&
        InDiceCount <= MaxDiceCount &&
        InDifficulty >= MinDifficulty &&
        InDifficulty <= GlobalSideNumber;
}

int WodChanceCounter::GetMaxSuccessPerDie(
    const bool InWithTenReroll)
{
    return InWithTenReroll ? MaxTenReroll : 1;
}

std::vector<std::pair<int, long double>> WodChanceCounter::GetSingleDieNetDistribution(
    const int InDifficulty,
    const bool InWithCancel,
    const bool InWithTenReroll)
{
    std::vector<long double> NetProbability(MaxTenReroll + 2, 0.0L);

    const long double OneFaceProbability =
        1.0L / static_cast<long double>(GlobalSideNumber);

    if (InWithCancel)
    {
        NetProbability[0] += OneFaceProbability;
    }
    else
    {
        NetProbability[1] += OneFaceProbability;
    }

    const int InitialFailureFaceCount = InDifficulty - 2;

    if (InitialFailureFaceCount > 0)
    {
        NetProbability[1] +=
            static_cast<long double>(InitialFailureFaceCount) *
            OneFaceProbability;
    }

    if (!InWithTenReroll)
    {
        const int InitialSuccessFaceCount =
            GlobalSideNumber - InDifficulty + 1;

        NetProbability[2] +=
            static_cast<long double>(InitialSuccessFaceCount) *
            OneFaceProbability;
    }
    else
    {
        const int InitialNonTenSuccessFaceCount =
            GlobalSideNumber - InDifficulty;

        if (InitialNonTenSuccessFaceCount > 0)
        {
            NetProbability[2] +=
                static_cast<long double>(InitialNonTenSuccessFaceCount) *
                OneFaceProbability;
        }

        long double ChainProbability = OneFaceProbability;

        const long double TerminalFailureProbability =
            static_cast<long double>(InDifficulty - 1) /
            static_cast<long double>(GlobalSideNumber);

        const long double TerminalSuccessProbability =
            static_cast<long double>(GlobalSideNumber - InDifficulty) /
            static_cast<long double>(GlobalSideNumber);

        for (int TenCount = 1; TenCount < MaxTenReroll; ++TenCount)
        {
            NetProbability[TenCount + 1] +=
                ChainProbability * TerminalFailureProbability;

            NetProbability[TenCount + 2] +=
                ChainProbability * TerminalSuccessProbability;

            ChainProbability *= OneFaceProbability;
        }

        NetProbability[MaxTenReroll + 1] += ChainProbability;
    }

    std::vector<std::pair<int, long double>> Result;

    for (int Index = 0; Index < static_cast<int>(NetProbability.size()); ++Index)
    {
        if (NetProbability[Index] <= 0.0L)
        {
            continue;
        }

        const int NetValue = Index - 1;
        Result.emplace_back(NetValue, NetProbability[Index]);
    }

    return Result;
}

std::vector<long double> WodChanceCounter::GetNetDistribution(
    const int InDiceCount,
    const int InDifficulty,
    const bool InWithCancel,
    const bool InWithTenReroll,
    int& OutMinNet,
    int& OutMaxNet)
{
    const std::vector<std::pair<int, long double>> SingleDieNetDistribution =
        GetSingleDieNetDistribution(
            InDifficulty,
            InWithCancel,
            InWithTenReroll);

    const int MinDieNet = InWithCancel ? -1 : 0;
    const int MaxDieNet = GetMaxSuccessPerDie(InWithTenReroll);

    std::vector<long double> CurrentDistribution(1, 1.0L);
    int CurrentMinNet = 0;
    int CurrentMaxNet = 0;

    for (int DiceIndex = 0; DiceIndex < InDiceCount; ++DiceIndex)
    {
        const int NextMinNet = CurrentMinNet + MinDieNet;
        const int NextMaxNet = CurrentMaxNet + MaxDieNet;

        std::vector<long double> NextDistribution(
            NextMaxNet - NextMinNet + 1,
            0.0L);

        for (int CurrentNet = CurrentMinNet; CurrentNet <= CurrentMaxNet; ++CurrentNet)
        {
            const long double CurrentProbability =
                CurrentDistribution[CurrentNet - CurrentMinNet];

            if (CurrentProbability == 0.0L)
            {
                continue;
            }

            for (const std::pair<int, long double>& Entry : SingleDieNetDistribution)
            {
                const int NextNet = CurrentNet + Entry.first;

                NextDistribution[NextNet - NextMinNet] +=
                    CurrentProbability * Entry.second;
            }
        }

        CurrentDistribution.swap(NextDistribution);
        CurrentMinNet = NextMinNet;
        CurrentMaxNet = NextMaxNet;
    }

    OutMinNet = CurrentMinNet;
    OutMaxNet = CurrentMaxNet;

    return CurrentDistribution;
}

long double WodChanceCounter::GetTailProbability(
    const std::vector<long double>& InNetDistribution,
    const int InMinNet,
    const int InMaxNet,
    const int InTargetSuccessNum)
{
    if (InTargetSuccessNum > InMaxNet)
    {
        return 0.0L;
    }

    if (InTargetSuccessNum <= InMinNet)
    {
        return 1.0L;
    }

    long double Result = 0.0L;

    for (int NetValue = InTargetSuccessNum; NetValue <= InMaxNet; ++NetValue)
    {
        Result += InNetDistribution[NetValue - InMinNet];
    }

    return Result;
}

long double WodChanceCounter::GetBotchProbability(
    const std::vector<long double>& InNetDistribution,
    const int InMinNet)
{
    if (InMinNet >= 0)
    {
        return 0.0L;
    }

    long double Result = 0.0L;

    for (int NetValue = InMinNet; NetValue < 0; ++NetValue)
    {
        Result += InNetDistribution[NetValue - InMinNet];
    }

    return Result;
}

std::string WodChanceCounter::GetProbabilityList(
    const int InDiceCount,
    const int InDifficulty,
    const bool InWithCancel,
    const bool InWithTenReroll)
{
    if (!IsInputValid(InDiceCount, InDifficulty))
    {
        return "";
    }

    int MinNet = 0;
    int MaxNet = 0;

    const std::vector<long double> NetDistribution =
        GetNetDistribution(
            InDiceCount,
            InDifficulty,
            InWithCancel,
            InWithTenReroll,
            MinNet,
            MaxNet);

    std::vector<long double> SuccessAtLeast(MaxNet + 2, 0.0L);
    long double RunningProbability = 0.0L;

    for (int NetValue = MaxNet; NetValue >= 1; --NetValue)
    {
        RunningProbability += NetDistribution[NetValue - MinNet];
        SuccessAtLeast[NetValue] = RunningProbability;
    }

    std::vector<std::pair<int, long double>> Entries;

    if (InWithCancel)
    {
        Entries.emplace_back(-1, GetBotchProbability(NetDistribution, MinNet));
    }

    for (int TargetSuccessNum = 1;; ++TargetSuccessNum)
    {
        const long double SuccessChanceValue =
            TargetSuccessNum <= MaxNet ?
            SuccessAtLeast[TargetSuccessNum] :
            0.0L;

        if (SuccessChanceValue <= MinDisplayedProbability)
        {
            break;
        }

        Entries.emplace_back(TargetSuccessNum, SuccessChanceValue);
    }

    const int MaxDigits =
        static_cast<int>(std::to_string(Entries.back().first).length());

    std::ostringstream ResultStream;
    bool IsFirstLine = true;

    for (const std::pair<int, long double>& Entry : Entries)
    {
        if (!IsFirstLine)
        {
            ResultStream << '\n';
        }

        if (Entry.first < 0)
        {
            ResultStream << std::setw(MaxDigits) << "B";
        }
        else
        {
            ResultStream << std::setw(MaxDigits) << Entry.first;
        }

        ResultStream
            << ": "
            << std::fixed
            << std::setprecision(2)
            << static_cast<double>(Entry.second * 100.0L)
            << '%';

        IsFirstLine = false;
    }

    return ResultStream.str();
}