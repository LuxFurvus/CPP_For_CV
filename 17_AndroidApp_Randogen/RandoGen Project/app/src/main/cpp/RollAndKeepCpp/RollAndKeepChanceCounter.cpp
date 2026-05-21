#include "RollAndKeepChanceCounter.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

long double RollAndKeepChanceCounter::ClampUnitProbability(
    const long double InValue)
{
    if (InValue <= ZeroProbability)
    {
        return ZeroProbability;
    }

    if (InValue >= FullProbability)
    {
        return FullProbability;
    }

    return InValue;
}

int RollAndKeepChanceCounter::GetMinTotal(
    const FValueParamsRnk& InValueParams)
{
    return InValueParams.InBonus + InValueParams.InKeepNum;
}

int RollAndKeepChanceCounter::GetNeededRandomThreshold(
    const int InTotalThreshold,
    const int InBonus)
{
    return InTotalThreshold - InBonus;
}

int RollAndKeepChanceCounter::GetMaxRandomTotalWithoutExplodingTens(
    const int InKeepNum)
{
    return InKeepNum * SideCount;
}

int RollAndKeepChanceCounter::GetBaseCaseRandomThresholdLimit(
    const int InRollNum,
    const int InKeepNum,
    const bool InExplodeTens)
{
    if (InRollNum <= 0 || InKeepNum <= 0)
    {
        return 0;
    }

    if (!InExplodeTens)
    {
        return GetMaxRandomTotalWithoutExplodingTens(InKeepNum);
    }

    return UniversalExplodingTensExactRandomThresholdLimit;
}

bool RollAndKeepChanceCounter::IsAboveFiniteMaximum(
    const int InNeededRandomThreshold,
    const int InKeepNum,
    const bool InExplodeTens)
{
    if (InExplodeTens)
    {
        return false;
    }

    return InNeededRandomThreshold >
        GetMaxRandomTotalWithoutExplodingTens(InKeepNum);
}

bool RollAndKeepChanceCounter::IsGuaranteedBelowCutoff(
    const int InNeededRandomThreshold,
    const bool InExplodeTens)
{
    if (!InExplodeTens)
    {
        return false;
    }

    return InNeededRandomThreshold >=
        UniversalExplodingTensBelowCutoffRandomThreshold;
}

std::vector<long double>
RollAndKeepChanceCounter::BuildExactSingleDieProbabilityList(
        const int InMaxRandomThreshold,
        const bool InExplodeTens,
        const bool InExplodeOnes)
{
    std::vector<long double> Result(
            static_cast<std::size_t>(InMaxRandomThreshold + 1),
            ZeroProbability
    );

    if (InMaxRandomThreshold <= 0)
    {
        Result[0] = FullProbability;
        return Result;
    }

    const int FillLimit = InMaxRandomThreshold - 1;

    const long double BaseOneProbability =
            InExplodeOnes
            ? SingleFaceProbability * SingleFaceProbability
            : SingleFaceProbability;

    const long double BaseNonOneProbability =
            InExplodeOnes
            ? SingleFaceProbability
              + SingleFaceProbability * SingleFaceProbability
            : SingleFaceProbability;

    long double Sum = ZeroProbability;

    if (!InExplodeTens)
    {
        const int MaxFaceValue =
                std::min(FillLimit, SideCount);

        if (MaxFaceValue >= 1)
        {
            Result[1] = BaseOneProbability;
            Sum += BaseOneProbability;
        }

        for (int Value = 2; Value <= MaxFaceValue; ++Value)
        {
            Result[static_cast<std::size_t>(Value)] =
                    BaseNonOneProbability;
            Sum += BaseNonOneProbability;
        }

        Result[static_cast<std::size_t>(InMaxRandomThreshold)] =
                ClampUnitProbability(FullProbability - Sum);

        return Result;
    }

    const int MaxBaseValue =
            std::min(FillLimit, SideCount - 1);

    if (MaxBaseValue >= 1)
    {
        Result[1] = BaseOneProbability;
        Sum += BaseOneProbability;
    }

    for (int Value = 2; Value <= MaxBaseValue; ++Value)
    {
        Result[static_cast<std::size_t>(Value)] =
                BaseNonOneProbability;
        Sum += BaseNonOneProbability;
    }

    long double DecadeProbability =
            BaseNonOneProbability * SingleFaceProbability;

    for (int DecadeStart = SideCount;
         DecadeStart < FillLimit;
         DecadeStart += SideCount)
    {
        const int MaxValueInDecade =
                std::min(FillLimit, DecadeStart + SideCount - 1);

        for (int Value = DecadeStart + 1;
             Value <= MaxValueInDecade;
             ++Value)
        {
            Result[static_cast<std::size_t>(Value)] =
                    DecadeProbability;
            Sum += DecadeProbability;
        }

        DecadeProbability *= SingleFaceProbability;
    }

    Result[static_cast<std::size_t>(InMaxRandomThreshold)] =
            ClampUnitProbability(FullProbability - Sum);

    return Result;
}

const std::array<std::array<long double,
                            RollAndKeepChanceCounter::MaxNormalizedDice + 1>,
                 RollAndKeepChanceCounter::MaxNormalizedDice + 1>&
RollAndKeepChanceCounter::GetBinomialCoefficientTable()
{
    static const std::array<std::array<long double, MaxNormalizedDice + 1>,
                            MaxNormalizedDice + 1> Table =
        []()
        {
            std::array<std::array<long double, MaxNormalizedDice + 1>,
                       MaxNormalizedDice + 1> Result{};

            for (int N = 0; N <= MaxNormalizedDice; ++N)
            {
                Result[N][0] = FullProbability;
                Result[N][N] = FullProbability;

                for (int K = 1; K < N; ++K)
                {
                    Result[N][K] =
                        Result[N - 1][K - 1] +
                        Result[N - 1][K];
                }
            }

            return Result;
        }();

    return Table;
}

long double RollAndKeepChanceCounter::GetBinomialProbability(
    const int InTrialNum,
    const int InSuccessNum,
    const long double InSuccessProbability)
{
    if (InSuccessNum < 0 || InSuccessNum > InTrialNum)
    {
        return ZeroProbability;
    }

    if (InSuccessProbability <= ZeroProbability)
    {
        return InSuccessNum == 0
            ? FullProbability
            : ZeroProbability;
    }

    if (InSuccessProbability >= FullProbability)
    {
        return InSuccessNum == InTrialNum
            ? FullProbability
            : ZeroProbability;
    }

    const auto& Table = GetBinomialCoefficientTable();

    return Table[InTrialNum][InSuccessNum]
        * std::pow(InSuccessProbability, InSuccessNum)
        * std::pow(
            FullProbability - InSuccessProbability,
            InTrialNum - InSuccessNum
        );
}

long double RollAndKeepChanceCounter::GetInitialAdjustedRollProbability(
    const int InValue,
    const bool InExplodeOnes)
{
    if (InValue < 1 || InValue > SideCount)
    {
        return ZeroProbability;
    }

    if (!InExplodeOnes)
    {
        return SingleFaceProbability;
    }

    if (InValue == 1)
    {
        return SingleFaceProbability * SingleFaceProbability;
    }

    return SingleFaceProbability
        + SingleFaceProbability * SingleFaceProbability;
}

long double RollAndKeepChanceCounter::GetExactSingleDieProbability(
    const int InValue,
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    if (InValue <= 0)
    {
        return ZeroProbability;
    }

    if (!InExplodeTens)
    {
        return GetInitialAdjustedRollProbability(
            InValue,
            InExplodeOnes
        );
    }

    if (InValue < SideCount)
    {
        return GetInitialAdjustedRollProbability(
            InValue,
            InExplodeOnes
        );
    }

    if (InValue == SideCount)
    {
        return ZeroProbability;
    }

    const int Remainder = InValue % SideCount;

    if (Remainder == 0)
    {
        return ZeroProbability;
    }

    const int TensCount = InValue / SideCount;
    const long double InitialTenProbability =
        GetInitialAdjustedRollProbability(
            SideCount,
            InExplodeOnes
        );

    return InitialTenProbability
        * std::pow(SingleFaceProbability, TensCount);
}

void RollAndKeepChanceCounter::BuildBinomialProbabilityRow(
    const int InTrialNum,
    const long double InSuccessProbability,
    std::array<long double, MaxNormalizedDice + 1>& OutProbabilityList)
{
    OutProbabilityList.fill(ZeroProbability);

    if (InTrialNum <= 0)
    {
        OutProbabilityList[0] = FullProbability;
        return;
    }

    if (InSuccessProbability <= ZeroProbability)
    {
        OutProbabilityList[0] = FullProbability;
        return;
    }

    if (InSuccessProbability >= FullProbability)
    {
        OutProbabilityList[InTrialNum] = FullProbability;
        return;
    }

    const long double FailureProbability =
            FullProbability - InSuccessProbability;

    long double Probability =
            std::pow(FailureProbability, InTrialNum);

    OutProbabilityList[0] = Probability;

    const long double Ratio =
            InSuccessProbability / FailureProbability;

    for (int Count = 0; Count < InTrialNum; ++Count)
    {
        Probability *=
                (static_cast<long double>(InTrialNum - Count)
                 / static_cast<long double>(Count + 1))
                * Ratio;

        OutProbabilityList[Count + 1] = Probability;
    }
}

std::vector<long double>
RollAndKeepChanceCounter::BuildSurvivalFromProbabilityList(
    const std::vector<long double>& InProbabilityList)
{
    std::vector<long double> Result(
        InProbabilityList.size(),
        ZeroProbability
    );

    long double Tail = ZeroProbability;

    for (int Index = static_cast<int>(InProbabilityList.size()) - 1;
         Index >= 1;
         --Index)
    {
        Tail += InProbabilityList[static_cast<std::size_t>(Index)];
        Result[static_cast<std::size_t>(Index)] =
            ClampUnitProbability(Tail);
    }

    Result[0] = FullProbability;
    return Result;
}

std::vector<long double>
RollAndKeepChanceCounter::BuildZeroKeptSurvivalProbabilityList()
{
    return std::vector<long double>{FullProbability};
}

std::vector<long double>
RollAndKeepChanceCounter::BuildSingleKeptSurvivalProbabilityList(
        const int InRollNum,
        const int InMaxRandomThreshold,
        const bool InExplodeTens,
        const bool InExplodeOnes)
{
    const std::vector<long double> SingleDieProbabilityList =
            BuildExactSingleDieProbabilityList(
                    InMaxRandomThreshold,
                    InExplodeTens,
                    InExplodeOnes
            );

    const std::vector<long double> SingleDieSurvivalProbabilityList =
            BuildSurvivalFromProbabilityList(
                    SingleDieProbabilityList
            );

    std::vector<long double> Result(
            SingleDieSurvivalProbabilityList.size(),
            ZeroProbability
    );

    Result[0] = FullProbability;

    for (std::size_t Value = 1;
         Value < SingleDieSurvivalProbabilityList.size();
         ++Value)
    {
        const long double SingleDieFailure =
                FullProbability -
                SingleDieSurvivalProbabilityList[Value];

        Result[Value] = ClampUnitProbability(
                FullProbability -
                std::pow(SingleDieFailure, InRollNum)
        );
    }

    return Result;
}

std::vector<long double>
RollAndKeepChanceCounter::BuildAllKeptSumProbabilityList(
    const int InRollNum,
    const int InMaxRandomThreshold,
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    const std::vector<long double> SingleDieProbabilityList =
        BuildExactSingleDieProbabilityList(
            InMaxRandomThreshold,
            InExplodeTens,
            InExplodeOnes
        );

    std::vector<std::pair<int, long double>> NonZeroOutcomeList;
    NonZeroOutcomeList.reserve(
        static_cast<std::size_t>(InMaxRandomThreshold)
    );

    for (int DieValue = 1; DieValue <= InMaxRandomThreshold; ++DieValue)
    {
        const long double DieProbability =
            SingleDieProbabilityList[static_cast<std::size_t>(DieValue)];

        if (DieProbability <= ZeroProbability)
        {
            continue;
        }

        NonZeroOutcomeList.emplace_back(DieValue, DieProbability);
    }

    std::vector<long double> Current(
            static_cast<std::size_t>(InMaxRandomThreshold + 1),
            ZeroProbability
    );

    std::vector<long double> Next(
            static_cast<std::size_t>(InMaxRandomThreshold + 1),
            ZeroProbability
    );

    Current[0] = FullProbability;

    for (int DieIndex = 0; DieIndex < InRollNum; ++DieIndex)
    {
        std::fill(Next.begin(), Next.end(), ZeroProbability);

        for (int Sum = 0; Sum <= InMaxRandomThreshold; ++Sum)
        {
            const long double CurrentProbability =
                    Current[static_cast<std::size_t>(Sum)];

            if (CurrentProbability <= ZeroProbability)
            {
                continue;
            }

            for (const std::pair<int, long double>& Outcome
                    : NonZeroOutcomeList)
            {
                const int DieValue = Outcome.first;
                const long double DieProbability = Outcome.second;

                const int NewSum =
                        std::min(InMaxRandomThreshold, Sum + DieValue);

                Next[static_cast<std::size_t>(NewSum)] +=
                        CurrentProbability * DieProbability;
            }
        }

        Current.swap(Next);
    }

    return Current;
}

std::vector<long double>
RollAndKeepChanceCounter::BuildAllKeptSurvivalProbabilityList(
    const int InRollNum,
    const int InMaxRandomThreshold,
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    const std::vector<long double> ProbabilityList =
        BuildAllKeptSumProbabilityList(
            InRollNum,
            InMaxRandomThreshold,
            InExplodeTens,
            InExplodeOnes
        );

    return BuildSurvivalFromProbabilityList(ProbabilityList);
}

std::size_t RollAndKeepChanceCounter::GetStateIndex(
    const int InRemainingRollNum,
    const int InKeptDiceNum,
    const int InClampedKeptSum,
    const int InKeepNum,
    const int InMaxRandomThreshold)
{
    return (
        static_cast<std::size_t>(InRemainingRollNum)
        * static_cast<std::size_t>(InKeepNum + 1)
        + static_cast<std::size_t>(InKeptDiceNum)
    ) * static_cast<std::size_t>(InMaxRandomThreshold + 1)
        + static_cast<std::size_t>(InClampedKeptSum);
}

std::vector<long double>
RollAndKeepChanceCounter::BuildGeneralTopKSumProbabilityList(
    const int InRollNum,
    const int InKeepNum,
    const int InMaxRandomThreshold,
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    const std::vector<long double> SingleDieProbabilityList =
        BuildExactSingleDieProbabilityList(
            InMaxRandomThreshold,
            InExplodeTens,
            InExplodeOnes
        );

    const std::size_t SumStride =
        static_cast<std::size_t>(InMaxRandomThreshold + 1);

    const std::size_t RemainingStride =
        static_cast<std::size_t>(InKeepNum + 1) * SumStride;

    const std::size_t StateCount =
        static_cast<std::size_t>(InRollNum + 1) * RemainingStride;

    std::vector<long double> Current(StateCount, ZeroProbability);
    std::vector<long double> Next(StateCount, ZeroProbability);

    struct FActiveStateRnk
    {
        int RemainingRollNum;
        int KeptDiceNum;
        int ClampedKeptSum;
        std::size_t Index;
    };

    const auto MakeStateIndex =
        [RemainingStride, SumStride](
            const int InRemainingRollNum,
            const int InKeptDiceNum,
            const int InClampedKeptSum) -> std::size_t
        {
            return
                static_cast<std::size_t>(InRemainingRollNum)
                    * RemainingStride
                + static_cast<std::size_t>(InKeptDiceNum)
                    * SumStride
                + static_cast<std::size_t>(InClampedKeptSum);
        };

    const std::size_t StartIndex =
        MakeStateIndex(InRollNum, 0, 0);

    Current[StartIndex] = FullProbability;

    std::vector<FActiveStateRnk> CurrentActiveStateList;
    std::vector<FActiveStateRnk> NextActiveStateList;

    CurrentActiveStateList.reserve(256);
    NextActiveStateList.reserve(256);

    CurrentActiveStateList.push_back(
        FActiveStateRnk
        {
            InRollNum,
            0,
            0,
            StartIndex
        }
    );

    long double RemainingMass = FullProbability;

    for (int Value = InMaxRandomThreshold; Value >= 1; --Value)
    {
        const long double ExactMass =
            SingleDieProbabilityList[static_cast<std::size_t>(Value)];

        if (ExactMass <= ZeroProbability)
        {
            continue;
        }

        const long double ConditionalProbability =
            RemainingMass <= ZeroProbability
                ? FullProbability
                : ClampUnitProbability(ExactMass / RemainingMass);

        std::fill(Next.begin(), Next.end(), ZeroProbability);
        NextActiveStateList.clear();

        std::array<
            std::array<long double, MaxNormalizedDice + 1>,
            MaxNormalizedDice + 1> BinomialProbabilityTable{};

        std::array<bool, MaxNormalizedDice + 1> HasBinomialRow{};
        HasBinomialRow.fill(false);

        for (const FActiveStateRnk& State : CurrentActiveStateList)
        {
            const long double StateProbability =
                Current[State.Index];

            if (StateProbability <= ZeroProbability)
            {
                continue;
            }

            const int MaxKeepAdd =
                InKeepNum - State.KeptDiceNum;

            if (ConditionalProbability >= FullProbability)
            {
                const int Count = State.RemainingRollNum;
                const int KeepAdd =
                    std::min(Count, MaxKeepAdd);

                const int NewRemainingRollNum =
                    State.RemainingRollNum - Count;

                const int NewKeptDiceNum =
                    State.KeptDiceNum + KeepAdd;

                const int NewClampedKeptSum =
                    std::min(
                        InMaxRandomThreshold,
                        State.ClampedKeptSum + KeepAdd * Value
                    );

                const std::size_t NewIndex =
                    MakeStateIndex(
                        NewRemainingRollNum,
                        NewKeptDiceNum,
                        NewClampedKeptSum
                    );

                long double& NextProbability = Next[NewIndex];

                if (NextProbability <= ZeroProbability)
                {
                    NextActiveStateList.push_back(
                        FActiveStateRnk
                        {
                            NewRemainingRollNum,
                            NewKeptDiceNum,
                            NewClampedKeptSum,
                            NewIndex
                        }
                    );
                }

                NextProbability += StateProbability;
                continue;
            }

            const int RemainingRollNum =
                State.RemainingRollNum;

            if (!HasBinomialRow[RemainingRollNum])
            {
                BuildBinomialProbabilityRow(
                    RemainingRollNum,
                    ConditionalProbability,
                    BinomialProbabilityTable[RemainingRollNum]
                );

                HasBinomialRow[RemainingRollNum] = true;
            }

            const std::array<long double, MaxNormalizedDice + 1>&
            BinomialRow =
                BinomialProbabilityTable[RemainingRollNum];

            for (int Count = 0;
                 Count <= RemainingRollNum;
                 ++Count)
            {
                const long double TransitionProbability =
                    BinomialRow[Count];

                if (TransitionProbability <= ZeroProbability)
                {
                    continue;
                }

                const int KeepAdd =
                    std::min(Count, MaxKeepAdd);

                const int NewRemainingRollNum =
                    RemainingRollNum - Count;

                const int NewKeptDiceNum =
                    State.KeptDiceNum + KeepAdd;

                const int NewClampedKeptSum =
                    std::min(
                        InMaxRandomThreshold,
                        State.ClampedKeptSum + KeepAdd * Value
                    );

                const std::size_t NewIndex =
                    MakeStateIndex(
                        NewRemainingRollNum,
                        NewKeptDiceNum,
                        NewClampedKeptSum
                    );

                long double& NextProbability = Next[NewIndex];

                if (NextProbability <= ZeroProbability)
                {
                    NextActiveStateList.push_back(
                        FActiveStateRnk
                        {
                            NewRemainingRollNum,
                            NewKeptDiceNum,
                            NewClampedKeptSum,
                            NewIndex
                        }
                    );
                }

                NextProbability +=
                    StateProbability * TransitionProbability;
            }
        }

        Current.swap(Next);
        CurrentActiveStateList.swap(NextActiveStateList);

        RemainingMass =
            ClampUnitProbability(RemainingMass - ExactMass);

        if (CurrentActiveStateList.empty())
        {
            break;
        }
    }

    std::vector<long double> Result(
        static_cast<std::size_t>(InMaxRandomThreshold + 1),
        ZeroProbability
    );

    for (int ClampedKeptSum = 0;
         ClampedKeptSum <= InMaxRandomThreshold;
         ++ClampedKeptSum)
    {
        Result[static_cast<std::size_t>(ClampedKeptSum)] =
            Current[
                MakeStateIndex(
                    0,
                    InKeepNum,
                    ClampedKeptSum
                )
            ];
    }

    return Result;
}

std::vector<long double>
RollAndKeepChanceCounter::BuildGeneralTopKSurvivalProbabilityList(
    const int InRollNum,
    const int InKeepNum,
    const int InMaxRandomThreshold,
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    const std::vector<long double> ProbabilityList =
        BuildGeneralTopKSumProbabilityList(
            InRollNum,
            InKeepNum,
            InMaxRandomThreshold,
            InExplodeTens,
            InExplodeOnes
        );

    return BuildSurvivalFromProbabilityList(ProbabilityList);
}

std::vector<long double>
RollAndKeepChanceCounter::BuildBaseSurvivalProbabilityList(
    const int InRollNum,
    const int InKeepNum,
    const int InMaxRandomThreshold,
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    if (InRollNum <= 0 || InKeepNum <= 0)
    {
        return BuildZeroKeptSurvivalProbabilityList();
    }

    if (InKeepNum == 1)
    {
        return BuildSingleKeptSurvivalProbabilityList(
            InRollNum,
            InMaxRandomThreshold,
            InExplodeTens,
            InExplodeOnes
        );
    }

    if (InKeepNum == InRollNum)
    {
        return BuildAllKeptSurvivalProbabilityList(
            InRollNum,
            InMaxRandomThreshold,
            InExplodeTens,
            InExplodeOnes
        );
    }

    return BuildGeneralTopKSurvivalProbabilityList(
        InRollNum,
        InKeepNum,
        InMaxRandomThreshold,
        InExplodeTens,
        InExplodeOnes
    );
}

std::array<std::array<std::array<std::array<
               RollAndKeepChanceCounter::FBaseCaseCacheRnk, 2>, 2>,
                      RollAndKeepChanceCounter::MaxNormalizedDice + 1>,
           RollAndKeepChanceCounter::MaxNormalizedDice + 1>&
RollAndKeepChanceCounter::GetBaseCaseCacheTable()
{
    static std::array<std::array<std::array<std::array<FBaseCaseCacheRnk, 2>, 2>,
                                 MaxNormalizedDice + 1>,
                      MaxNormalizedDice + 1> CacheTable{};

    return CacheTable;
}

RollAndKeepChanceCounter::FBaseCaseCacheRnk&
RollAndKeepChanceCounter::GetBaseCaseCache(
    const int InRollNum,
    const int InKeepNum,
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    auto& CacheTable = GetBaseCaseCacheTable();

    return CacheTable
        [InRollNum]
        [InKeepNum]
        [InExplodeTens ? 1 : 0]
        [InExplodeOnes ? 1 : 0];
}

void RollAndKeepChanceCounter::BuildBaseCaseCache(
    const int InRollNum,
    const int InKeepNum,
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    FBaseCaseCacheRnk& Cache =
        GetBaseCaseCache(
            InRollNum,
            InKeepNum,
            InExplodeTens,
            InExplodeOnes
        );

    if (Cache.IsBuilt)
    {
        return;
    }

    Cache.MaxRandomThreshold =
        GetBaseCaseRandomThresholdLimit(
            InRollNum,
            InKeepNum,
            InExplodeTens
        );

    Cache.SurvivalProbabilityList =
        BuildBaseSurvivalProbabilityList(
            InRollNum,
            InKeepNum,
            Cache.MaxRandomThreshold,
            InExplodeTens,
            InExplodeOnes
        );

    Cache.IsBuilt = true;
}

void RollAndKeepChanceCounter::EnsureBaseCaseCacheBuilt(
    const int InRollNum,
    const int InKeepNum,
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    BuildBaseCaseCache(
        InRollNum,
        InKeepNum,
        InExplodeTens,
        InExplodeOnes
    );
}

long double RollAndKeepChanceCounter::GetProbabilityAtOrAboveTotalThreshold(
    const int InTotalThreshold,
    const FValueParamsRnk& InNormalizedValueParams,
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    const int NeededRandomThreshold =
        GetNeededRandomThreshold(
            InTotalThreshold,
            InNormalizedValueParams.InBonus
        );

    if (NeededRandomThreshold <= 0)
    {
        return FullProbability;
    }

    if (InNormalizedValueParams.InKeepNum <= 0)
    {
        return ZeroProbability;
    }

    if (IsAboveFiniteMaximum(
            NeededRandomThreshold,
            InNormalizedValueParams.InKeepNum,
            InExplodeTens))
    {
        return ZeroProbability;
    }

    if (IsGuaranteedBelowCutoff(
            NeededRandomThreshold,
            InExplodeTens))
    {
        return PositiveProbabilityBelowCutoff;
    }

    EnsureBaseCaseCacheBuilt(
        InNormalizedValueParams.InRollNum,
        InNormalizedValueParams.InKeepNum,
        InExplodeTens,
        InExplodeOnes
    );

    const FBaseCaseCacheRnk& Cache =
        GetBaseCaseCache(
            InNormalizedValueParams.InRollNum,
            InNormalizedValueParams.InKeepNum,
            InExplodeTens,
            InExplodeOnes
        );

    if (NeededRandomThreshold > Cache.MaxRandomThreshold)
    {
        return InExplodeTens
            ? PositiveProbabilityBelowCutoff
            : ZeroProbability;
    }

    return ClampUnitProbability(
        Cache.SurvivalProbabilityList[
            static_cast<std::size_t>(NeededRandomThreshold)
        ]
    );
}

void RollAndKeepChanceCounter::AppendProbabilityLine(
    std::ostringstream& OutStream,
    const int InThreshold,
    const long double InProbability,
    const bool InIsFirstLine)
{
    if (!InIsFirstLine)
    {
        OutStream << '\n';
    }

    OutStream
        << std::setw(2)
        << InThreshold
        << ": "
        << FormatChancePercent(InProbability);
}

bool RollAndKeepChanceCounter::ShouldStopPrinting(
    const long double InProbability)
{
    return InProbability < ProbabilityCutoff;
}

std::string RollAndKeepChanceCounter::BuildProbabilityListText(
    const FValueParamsRnk& InNormalizedValueParams,
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    const int MinTotal = GetMinTotal(InNormalizedValueParams);

    const int FirstPrintedThreshold =
        GetFirstPrintedThreshold(MinTotal);

    std::ostringstream Stream;
    bool IsFirstLine = true;

    bool HasPendingHundred = false;
    int PendingHundredThreshold = 0;
    long double PendingHundredProbability = FullProbability;

    for (int Threshold = FirstPrintedThreshold;
         ;
         Threshold += ThresholdStep)
    {
        const long double Probability =
            GetProbabilityAtOrAboveTotalThreshold(
                Threshold,
                InNormalizedValueParams,
                InExplodeTens,
                InExplodeOnes
            );

        if (Probability >= FullProbability)
        {
            HasPendingHundred = true;
            PendingHundredThreshold = Threshold;
            PendingHundredProbability = Probability;
        }
        else
        {
            if (HasPendingHundred)
            {
                AppendProbabilityLine(
                    Stream,
                    PendingHundredThreshold,
                    PendingHundredProbability,
                    IsFirstLine
                );

                IsFirstLine = false;
                HasPendingHundred = false;
            }

            AppendProbabilityLine(
                Stream,
                Threshold,
                Probability,
                IsFirstLine
            );

            IsFirstLine = false;
        }

        if (ShouldStopPrinting(Probability))
        {
            if (HasPendingHundred)
            {
                AppendProbabilityLine(
                    Stream,
                    PendingHundredThreshold,
                    PendingHundredProbability,
                    IsFirstLine
                );
            }

            break;
        }
    }

    return Stream.str();
}

int RollAndKeepChanceCounter::GetFirstPrintedThreshold(
    const int InMinTotal)
{
    if (InMinTotal < MinThreshold)
    {
        return MinThreshold;
    }

    return (InMinTotal / ThresholdStep) * ThresholdStep;
}

FValueParamsRnk RollAndKeepChanceCounter::ApplyTenDiceRule(
    const FValueParamsRnk& InValueParams)
{
    const int RawRollNum
        = std::max(0, InValueParams.InRollNum);
    const int RawKeepNum
        = std::max(0, InValueParams.InKeepNum);
    const int RawBonus
        = InValueParams.InBonus;

    const int KeepAfterRollClamp
        = std::min(RawKeepNum, RawRollNum);
    const int BonusAfterRollClamp
        = RawBonus + 2 * std::max(RawKeepNum - RawRollNum, 0);

    const int ExcessRollNum
        = std::max(RawRollNum - 10, 0);
    const int ExcessKeepNum
        = std::max(KeepAfterRollClamp - 10, 0);

    const int ConvertibleOne
        = std::max(10 - KeepAfterRollClamp, 0);
    const int ConvertibleTwo
        = ExcessRollNum / 2;
    const int ConvertiblePairs
        = std::min(ConvertibleOne, ConvertibleTwo);

    const int NormalizedRollNum
        = std::min(RawRollNum, 10);
    const int NormalizedKeepNum
        = std::min(
            10,
            KeepAfterRollClamp + ConvertiblePairs
        );

    const int NormalizedBonus
        = BonusAfterRollClamp
        + 2 * ExcessKeepNum
        + 2 * (ExcessRollNum - 2 * ConvertiblePairs);

    return FValueParamsRnk
    {
        NormalizedRollNum,
        NormalizedKeepNum,
        NormalizedBonus
    };
}

std::string RollAndKeepChanceCounter::FormatChancePercent(
    const long double InProbability)
{
    if (InProbability >= FullProbability)
    {
        return "100.0%";
    }

    if (InProbability <= ZeroProbability)
    {
        return "00.00%";
    }

    const long double PercentValue =
        InProbability * HundredPercent;

    if (PercentValue > FormatterUpperBound)
    {
        return ">99.9%";
    }

    if (PercentValue < FormatterLowerBound)
    {
        return "<0.01%";
    }

    std::ostringstream Stream;
    Stream
        << std::fixed
        << std::setprecision(2)
        << static_cast<double>(PercentValue)
        << '%';

    return Stream.str();
}

std::string RollAndKeepChanceCounter::GetProbabilityList(
    const int InRollNum,
    const int InKeepNum,
    const int InBonus,
    const bool InExplodeTens,
    const bool InExplodeOnes)
{
    const FValueParamsRnk NormalizedValueParams =
        ApplyTenDiceRule(
            FValueParamsRnk
            {
                InRollNum,
                InKeepNum,
                InBonus
            }
        );

    return BuildProbabilityListText(
        NormalizedValueParams,
        InExplodeTens,
        InExplodeOnes
    );
}
