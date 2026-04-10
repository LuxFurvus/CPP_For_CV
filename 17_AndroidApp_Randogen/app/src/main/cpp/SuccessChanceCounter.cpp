#include "SuccessChanceCounter.h"

#include <cmath>
#include <iomanip>
#include <sstream>

long double FSuccessChanceCounter::BinomialCoefficient(
    int N,
    int K)
{
    if (K < 0 || K > N)
    {
        return 0.0L;
    }

    if (K == 0 || K == N)
    {
        return 1.0L;
    }

    if (K > N - K)
    {
        K = N - K;
    }

    long double Result = 1.0L;

    for (int I = 1; I <= K; ++I)
    {
        Result *= static_cast<long double>(N - K + I);
        Result /= static_cast<long double>(I);
    }

    return Result;
}

long double FSuccessChanceCounter::MultinomialCoefficient(
    int N,
    int S,
    int C)
{
    if (N < 0 || S < 0 || C < 0 || S + C > N)
    {
        return 0.0L;
    }

    return
        BinomialCoefficient(N, S) *
        BinomialCoefficient(N - S, C);
}

long double FSuccessChanceCounter::GetProbabilityChanceWithoutCancel(
    int InDiceCount,
    long double InSuccessProbability,
    int InTargetSuccessNum)
{
    if (InDiceCount < 0 ||
        InSuccessProbability < 0.0L ||
        InSuccessProbability > 1.0L ||
        InTargetSuccessNum < 0)
    {
        return -1.0L;
    }

    if (InTargetSuccessNum > InDiceCount)
    {
        return 0.0L;
    }

    if (InTargetSuccessNum == 0)
    {
        return 1.0L;
    }

    const long double FailureProbability = 1.0L - InSuccessProbability;

    long double TotalProbability = 0.0L;

    for (int SuccessCount = InTargetSuccessNum;
         SuccessCount <= InDiceCount;
         ++SuccessCount)
    {
        const int FailureCount = InDiceCount - SuccessCount;

        const long double Term =
            BinomialCoefficient(InDiceCount, SuccessCount) *
            std::pow(InSuccessProbability, SuccessCount) *
            std::pow(FailureProbability, FailureCount);

        TotalProbability += Term;
    }

    return TotalProbability;
}

long double FSuccessChanceCounter::GetBotchChance(
    const int InDiceCount,
    const int InSideNumber,
    const int InDifficulty)
{
    const bool bIsInputValid =
        InDiceCount >= 0 &&
        InSideNumber >= 2 &&
        InDifficulty >= 2 &&
        InDifficulty <= InSideNumber;

    if (!bIsInputValid)
    {
        return -1.0L;
    }

    if (InDiceCount == 0)
    {
        return 0.0L;
    }

    const long double SideCount
        = static_cast<long double>(InSideNumber);

    const long double NoSuccessAtAllProbability =
        static_cast<long double>(InDifficulty - 1) / SideCount;

    const long double NoSuccessAndNoOnesProbability =
        static_cast<long double>(InDifficulty - 2) / SideCount;

    return
        std::pow(NoSuccessAtAllProbability, InDiceCount) -
        std::pow(NoSuccessAndNoOnesProbability, InDiceCount);
}

long double FSuccessChanceCounter::GetProbabilityChance(
    const int InDiceCount,
    const int InSideNumber,
    const int InDifficulty,
    const int InTargetSuccessNum,
    const bool InWithCancel)
{
    const bool bIsInputValid =
        InDiceCount >= 0 &&
        InSideNumber >= 2 &&
        InDifficulty >= 2 &&
        InDifficulty <= InSideNumber;

    if (!bIsInputValid)
    {
        return -1.0L;
    }

    if (InTargetSuccessNum < 0)
    {
        return GetBotchChance(
            InDiceCount,
            InSideNumber,
            InDifficulty);
    }

    if (InTargetSuccessNum > InDiceCount)
    {
        return 0.0L;
    }

    const int N = InDiceCount;
    const long double SideCount
        = static_cast<long double>(InSideNumber);

    const long double SuccessProbability =
        static_cast<long double>(InSideNumber - InDifficulty + 1) / SideCount;

    if (!InWithCancel)
    {
        if (InTargetSuccessNum == 0)
        {
            return std::pow(1.0L - SuccessProbability, N);
        }

        return GetProbabilityChanceWithoutCancel(
            InDiceCount,
            SuccessProbability,
            InTargetSuccessNum);
    }

    const long double CancelProbability = 1.0L / SideCount;
    const long double OtherProbability =
            static_cast<long double>(InDifficulty - 2) / SideCount;

    long double TotalProbability = 0.0L;

    for (int SuccessCount = 0; SuccessCount <= N; ++SuccessCount)
    {
        for (int CancelCount = 0; CancelCount <= N - SuccessCount; ++CancelCount)
        {
            const int OtherCount = N - SuccessCount - CancelCount;
            const int NetSuccessCount = SuccessCount - CancelCount;

            bool bMatchesTarget = false;

            if (InTargetSuccessNum == 0)
            {
                bMatchesTarget = NetSuccessCount <= 0;
            }
            else
            {
                bMatchesTarget = NetSuccessCount >= InTargetSuccessNum;
            }

            if (!bMatchesTarget)
            {
                continue;
            }

            const long double Term =
                MultinomialCoefficient(N, SuccessCount, CancelCount) *
                std::pow(SuccessProbability, SuccessCount) *
                std::pow(CancelProbability, CancelCount) *
                std::pow(OtherProbability, OtherCount);

            TotalProbability += Term;
        }
    }

    return TotalProbability;
}


std::string FSuccessChanceCounter::GetProbabilityList(
    const int InDiceCount,
    const int InSideNumber,
    const int InDifficulty,
    const bool InWithCancel)
{
    const bool bIsInputCorrect
        = InDiceCount > 0
        || InSideNumber > 2
        || InDifficulty <= InSideNumber
        || InDifficulty >= 2;
    if (!bIsInputCorrect)
    {
        return "";
    }

    const int MaxDigits =
        static_cast<int>(std::to_string(InDiceCount).length());

    std::ostringstream ResultStream;
    bool bIsFirstLine = true;

    for (int TargetSuccessNum = -1; TargetSuccessNum <= InDiceCount; ++TargetSuccessNum)
    {
        if (TargetSuccessNum == 0)
        {
            continue;
        }

        if (TargetSuccessNum == -1 && !InWithCancel)
        {
            continue;
        }

        if (!bIsFirstLine)
        {
            ResultStream << '\n';
        }

        std::ostringstream LeftPartStream;

        if (TargetSuccessNum == -1)
        {
            LeftPartStream << std::setw(MaxDigits) << "B";
        }
        else
        {
            LeftPartStream << std::setw(MaxDigits) << TargetSuccessNum;
        }

        ResultStream << LeftPartStream.str() << ": ";

        const bool bIsDiceValueValid =
                InDiceCount >= TargetSuccessNum;

        if (!bIsDiceValueValid)
        {
            ResultStream << "--";
            bIsFirstLine = false;
            continue;
        }

        const long double SuccessChanceValue = GetProbabilityChance(
            InDiceCount,
            InSideNumber,
            InDifficulty,
            TargetSuccessNum,
            InWithCancel);

        if (SuccessChanceValue < 0.0L)
        {
            ResultStream << "--";
            bIsFirstLine = false;
            continue;
        }

        ResultStream
            << std::fixed
            << std::setprecision(2)
            << static_cast<double>(SuccessChanceValue * 100.0L)
            << '%';

        bIsFirstLine = false;
    }

    return ResultStream.str();
}
