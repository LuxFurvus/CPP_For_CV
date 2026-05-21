
#include "WodChanceCounter.h"

#include <cmath>
#include <iomanip>
#include <sstream>

#include "../CommonCpp/CommonMathLib.h"

bool WodChanceCounter::IsInputValid(
    const int InDiceCount,
    const int InSideNumber,
    const int InDifficulty)
{
    return
        InDiceCount >= MinDiceCount &&
        InDiceCount <= MaxDiceCount &&
        InSideNumber >= MinSideNumber &&
        InSideNumber <= MaxSideNumber &&
        InDifficulty >= MinSideNumber &&
        InDifficulty <= InSideNumber;
}

long double WodChanceCounter::MultinomialCoefficient(
    const int N, const int S, const int C)
{
    if (N < 0 || S < 0 || C < 0 || S + C > N)
    {
        return 0.0L;
    }

    const long double BinomialCoefficientProduct =
        CommonMathLib::BinomialCoefficient(N, S) *
        CommonMathLib::BinomialCoefficient(N - S, C);

    return BinomialCoefficientProduct;
}

long double WodChanceCounter::GetProbabilityChanceWithoutCancel(
    const int InDiceCount,
    const long double InSuccessProbability,
    const int InTargetNumber)
{
    if (InDiceCount < 0 ||
        InSuccessProbability < 0.0L ||
        InSuccessProbability > 1.0L ||
        InTargetNumber < 0)
    {
        return -1.0L;
    }

    if (InTargetNumber > InDiceCount)
    {
        return 0.0L;
    }

    if (InTargetNumber == 0)
    {
        return 1.0L;
    }

    const long double FailureProbability = 1.0L - InSuccessProbability;

    long double TotalProbability = 0.0L;

    for (int SuccessCount = InTargetNumber;
         SuccessCount <= InDiceCount;
         ++SuccessCount)
    {
        const int FailureCount = InDiceCount - SuccessCount;

        const long double Term =
            CommonMathLib::BinomialCoefficient(InDiceCount, SuccessCount) *
            std::pow(InSuccessProbability, SuccessCount) *
            std::pow(FailureProbability, FailureCount);

        TotalProbability += Term;
    }

    return TotalProbability;
}

long double WodChanceCounter::GetBotchChance(
    const int InDiceCount,
    const int InSideNumber,
    const int InDifficulty)
{
    if (!IsInputValid(InDiceCount, InSideNumber, InDifficulty))
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

long double WodChanceCounter::GetProbabilityChance(
    const int InDiceCount,
    const int InSideNumber,
    const int InDifficulty,
    const int InTargetNumber,
    const bool InWithCancel)
{
    if (!IsInputValid(InDiceCount, InSideNumber, InDifficulty))
    {
        return -1.0L;
    }

    if (InTargetNumber < 0)
    {
        return GetBotchChance(
            InDiceCount,
            InSideNumber,
            InDifficulty);
    }

    if (InTargetNumber > InDiceCount)
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
        if (InTargetNumber == 0)
        {
            return std::pow(1.0L - SuccessProbability, N);
        }

        return GetProbabilityChanceWithoutCancel(
            InDiceCount,
            SuccessProbability,
            InTargetNumber);
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

            if (InTargetNumber == 0)
            {
                bMatchesTarget = NetSuccessCount <= 0;
            }
            else
            {
                bMatchesTarget = NetSuccessCount >= InTargetNumber;
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

std::string WodChanceCounter::GetProbabilityList(
    const int InDiceCount,
    const int InSideNumber,
    const int InDifficulty,
    const bool InWithCancel)
{
    if (!IsInputValid(InDiceCount, InSideNumber, InDifficulty))
    {
        return "";
    }

    const int MaxDigits =
        static_cast<int>(std::to_string(InDiceCount).length());

    std::ostringstream ResultStream;
    bool bIsFirstLine = true;

    for (int TargetNumber = -1; TargetNumber <= InDiceCount; ++TargetNumber)
    {
        if (TargetNumber == 0)
        {
            continue;
        }

        if (TargetNumber == -1 && !InWithCancel)
        {
            continue;
        }

        if (!bIsFirstLine)
        {
            ResultStream << '\n';
        }

        std::ostringstream LeftPartStream;

        if (TargetNumber == -1)
        {
            LeftPartStream << std::setw(MaxDigits) << "B";
        }
        else
        {
            LeftPartStream << std::setw(MaxDigits) << TargetNumber;
        }

        ResultStream << LeftPartStream.str() << ": ";

        const bool bIsDiceValueValid =
            InDiceCount >= TargetNumber;

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
            TargetNumber,
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
