#pragma once

#include <array>
#include <string>
#include <vector>

struct FValueParamsRnk
{
    const int InRollNum = 0;
    const int InKeepNum = 0;
    const int InBonus = 0;
};

class RollAndKeepChanceCounter
{
private:
    struct FBaseCaseCacheRnk
    {
        bool IsBuilt;
        int MaxRandomThreshold;
        std::vector<long double> SurvivalProbabilityList;
    };

private:
    static constexpr int SideCount = 10;
    static constexpr int MaxNormalizedDice = 10;
    static constexpr int ThresholdStep = 5;
    static constexpr int MinThreshold = 5;

    static constexpr long double ZeroProbability = 0.0L;
    static constexpr long double FullProbability = 1.0L;
    static constexpr long double SingleFaceProbability = 0.1L;
    static constexpr long double ProbabilityCutoff = 0.0001L;
    static constexpr long double PositiveProbabilityBelowCutoff = 0.000099L;
    static constexpr long double HundredPercent = 100.0L;
    static constexpr long double FormatterUpperBound = 99.9L;
    static constexpr long double FormatterLowerBound = 0.01L;

    static constexpr int UniversalExplodingTensBelowCutoffRandomThreshold = 511;
    static constexpr int UniversalExplodingTensExactRandomThresholdLimit = 510;

private:

    static long double ClampUnitProbability(long double InValue);

    static int GetMinTotal(const FValueParamsRnk& InValueParams);
    static int GetNeededRandomThreshold(int InTotalThreshold, int InBonus);
    static int GetMaxRandomTotalWithoutExplodingTens(int InKeepNum);

    static std::vector<long double> BuildExactSingleDieProbabilityList(
        int InMaxRandomThreshold,
        bool InExplodeTens,
        bool InExplodeOnes);

    static int GetBaseCaseRandomThresholdLimit(
        int InRollNum,
        int InKeepNum,
        bool InExplodeTens);

    static bool IsAboveFiniteMaximum(
        int InNeededRandomThreshold,
        int InKeepNum,
        bool InExplodeTens);

    static bool IsGuaranteedBelowCutoff(
        int InNeededRandomThreshold,
        bool InExplodeTens);

    static const std::array<
        std::array<long double, MaxNormalizedDice + 1>,
        MaxNormalizedDice + 1>&
    GetBinomialCoefficientTable();

    static long double GetBinomialProbability(
        int InTrialNum,
        int InSuccessNum,
        long double InSuccessProbability);

    static long double GetInitialAdjustedRollProbability(
        int InValue,
        bool InExplodeOnes);

    static long double GetExactSingleDieProbability(
        int InValue,
        bool InExplodeTens,
        bool InExplodeOnes);

    static void BuildBinomialProbabilityRow(
        int InTrialNum,
        long double InSuccessProbability,
        std::array<long double, MaxNormalizedDice + 1>& OutProbabilityList);

    static std::vector<long double> BuildSurvivalFromProbabilityList(
        const std::vector<long double>& InProbabilityList);

    static std::vector<long double> BuildZeroKeptSurvivalProbabilityList();

    static std::vector<long double> BuildSingleKeptSurvivalProbabilityList(
        int InRollNum,
        int InMaxRandomThreshold,
        bool InExplodeTens,
        bool InExplodeOnes);

    static std::vector<long double> BuildAllKeptSumProbabilityList(
        int InRollNum,
        int InMaxRandomThreshold,
        bool InExplodeTens,
        bool InExplodeOnes);

    static std::vector<long double> BuildAllKeptSurvivalProbabilityList(
        int InRollNum,
        int InMaxRandomThreshold,
        bool InExplodeTens,
        bool InExplodeOnes);

    static std::size_t GetStateIndex(
        int InRemainingRollNum,
        int InKeptDiceNum,
        int InBucketedKeptSum,
        int InKeepNum,
        int InMaxBucketIndex);

    static std::vector<long double> BuildGeneralTopKSumProbabilityList(
        int InRollNum,
        int InKeepNum,
        int InMaxRandomThreshold,
        bool InExplodeTens,
        bool InExplodeOnes);

    static std::vector<long double> BuildGeneralTopKSurvivalProbabilityList(
        int InRollNum,
        int InKeepNum,
        int InMaxRandomThreshold,
        bool InExplodeTens,
        bool InExplodeOnes);

    static std::vector<long double> BuildBaseSurvivalProbabilityList(
        int InRollNum,
        int InKeepNum,
        int InMaxRandomThreshold,
        bool InExplodeTens,
        bool InExplodeOnes);

    static std::array<
        std::array<
            std::array<
                std::array<FBaseCaseCacheRnk, 2>, 2>,
            MaxNormalizedDice + 1>,
        MaxNormalizedDice + 1>&
    GetBaseCaseCacheTable();

    static FBaseCaseCacheRnk& GetBaseCaseCache(
        int InRollNum,
        int InKeepNum,
        bool InExplodeTens,
        bool InExplodeOnes);

    static void BuildBaseCaseCache(
        int InRollNum,
        int InKeepNum,
        bool InExplodeTens,
        bool InExplodeOnes);

    static void EnsureBaseCaseCacheBuilt(
        int InRollNum,
        int InKeepNum,
        bool InExplodeTens,
        bool InExplodeOnes);

    static long double GetProbabilityAtOrAboveTotalThreshold(
        int InTotalThreshold,
        const FValueParamsRnk& InNormalizedValueParams,
        bool InExplodeTens,
        bool InExplodeOnes);

    static void AppendProbabilityLine(
        std::ostringstream& OutStream,
        int InThreshold,
        long double InProbability,
        bool InIsFirstLine);

    static bool ShouldStopPrinting(
        const long double InProbability);

    static std::string BuildProbabilityListText(
        const FValueParamsRnk& InNormalizedValueParams,
        bool InExplodeTens,
        bool InExplodeOnes);

public:

    static int GetFirstPrintedThreshold(int InMinTotal);

    static FValueParamsRnk ApplyTenDiceRule(const FValueParamsRnk &InValueParams);

    static std::string FormatChancePercent(long double InProbability);

    static std::string GetProbabilityList(
        int InRollNum,
        int InKeepNum,
        int InBonus,
        bool InExplodeTens,
        bool InExplodeOnes);
};
