
#include <jni.h>

#include "../CommonCpp/CommonLibJNI.h"

#include "../CommonCpp/SortedRollsMaker.h"
#include "DndRollRules.h"
#include "DndChancesCounter.h"
#include "DndRollButtonTextFormatter.h"

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_randogen_DndNativeBridge_GetDndRollResultsJNI(
    JNIEnv* Env,
    jobject /* ThisObj */,
    jintArray InDiceCounts,
    jintArray InSideNumbers,
    jint InRollModifier,
    jclass ResultClass)
{
    if (!Env)
    {
        return nullptr;
    }

    jmethodID ConstructorId = Env->GetMethodID(
        ResultClass,
        "<init>",
        "(Ljava/lang/String;I)V");

    if (!ConstructorId)
    {
        return nullptr;
    }

    jstring EmptyString
        = Env->NewStringUTF("");
    if (!EmptyString)
    {
        return nullptr;
    }

    if (!InDiceCounts || !InSideNumbers)
    {
        jobject ModOnlyObject = Env->NewObject(
            ResultClass,
            ConstructorId,
            EmptyString,
            static_cast<jint>(InRollModifier));
        return ModOnlyObject;
    }

    if (!ResultClass)
    {
        return nullptr;
    }

    const jsize DiceCountsLength
        = Env->GetArrayLength(InDiceCounts);
    const jsize SideNumbersLength
        = Env->GetArrayLength(InSideNumbers);

    if (DiceCountsLength <= 0 || DiceCountsLength != SideNumbersLength)
    {
        jobject ModOnlyObject = Env->NewObject(
            ResultClass,
            ConstructorId,
            EmptyString,
            InRollModifier);
        return ModOnlyObject;
    }

    std::vector<jint> DiceCounts(
        static_cast<std::size_t>(DiceCountsLength));
    std::vector<jint> SideNumbers(
        static_cast<std::size_t>(SideNumbersLength));

    Env->GetIntArrayRegion(
        InDiceCounts,
        0,
        DiceCountsLength,
        DiceCounts.data());

    if (Env->ExceptionCheck() == JNI_TRUE)
    {
        return nullptr;
    }

    Env->GetIntArrayRegion(
        InSideNumbers,
        0,
        SideNumbersLength,
        SideNumbers.data());

    if (Env->ExceptionCheck() == JNI_TRUE)
    {
        return nullptr;
    }

    std::vector<RollPair> RollData;
    RollData.reserve(static_cast<std::size_t>(DiceCountsLength));

    for (jsize Index = 0; Index < DiceCountsLength; ++Index)
    {
        RollPair CurrentRollData;
        CurrentRollData.DiceNum
            = static_cast<int>(DiceCounts[Index]);
        CurrentRollData.SideNum
            = static_cast<int>(SideNumbers[Index]);
        RollData.push_back(CurrentRollData);
    }

    const auto [ResultStdString, ResultSum]
        = DndRollRulesLib::GetDndRollResults(
            RollData,
            static_cast<int>(InRollModifier));

    if (ResultStdString.empty())
    {
        return nullptr;
    }

    jstring ResultString
        = Env->NewStringUTF(ResultStdString.c_str());

    if (!ResultString)
    {
        return nullptr;
    }

    jobject ResultObject = Env->NewObject(
        ResultClass,
        ConstructorId,
        ResultString,
        static_cast<jint>(ResultSum));

    Env->DeleteLocalRef(ResultString);

    return ResultObject;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_randogen_DndNativeBridge_GetDndMinAvgMaxValuesStringJNI(
    JNIEnv* Env,
    jobject /* ThisObj */,
    jintArray InDiceCounts,
    jintArray InSideNumbers,
    jint InRollModifier,
    jclass ResultClass)
{
    if (!Env)
    {
        return nullptr;
    }

    jstring ModOnlyString
        = Env->NewStringUTF(std::to_string(InRollModifier).c_str());
    if (!ModOnlyString)
    {
        return nullptr;
    }

    jmethodID ConstructorId = Env->GetMethodID(
        ResultClass,
        "<init>",
        "(Ljava/lang/String;I)V");

    if (!ConstructorId || !ResultClass || !InDiceCounts || !InSideNumbers)
    {
        return ModOnlyString;
    }

    const jsize DiceCountsLength
        = Env->GetArrayLength(InDiceCounts);
    const jsize SideNumbersLength
        = Env->GetArrayLength(InSideNumbers);

    if (DiceCountsLength <= 0 || DiceCountsLength != SideNumbersLength)
    {
        return ModOnlyString;
    }

    std::vector<jint> DiceCounts(
        static_cast<std::size_t>(DiceCountsLength));
    std::vector<jint> SideNumbers(
        static_cast<std::size_t>(SideNumbersLength));

    Env->GetIntArrayRegion(
        InDiceCounts,
        0,
        DiceCountsLength,
        DiceCounts.data());

    if (Env->ExceptionCheck() == JNI_TRUE)
    {
        return ModOnlyString;
    }

    Env->GetIntArrayRegion(
        InSideNumbers,
        0,
        SideNumbersLength,
        SideNumbers.data());

    if (Env->ExceptionCheck() == JNI_TRUE)
    {
        return ModOnlyString;
    }

    std::vector<RollPair> RollData;
    RollData.reserve(static_cast<std::size_t>(DiceCountsLength));

    for (jsize Index = 0; Index < DiceCountsLength; ++Index)
    {
        RollPair CurrentRollData;
        CurrentRollData.DiceNum
            = static_cast<int>(DiceCounts[Index]);
        CurrentRollData.SideNum
            = static_cast<int>(SideNumbers[Index]);
        RollData.push_back(CurrentRollData);
    }

    const std::string ResultStdString
        = DndChancesCounter::GetMinAvgMaxValuesString(
            RollData,
            static_cast<int>(InRollModifier));

    if (ResultStdString.empty())
    {
        return ModOnlyString;
    }
    Env->DeleteLocalRef(ModOnlyString);

    jstring ResultString
        = Env->NewStringUTF(ResultStdString.c_str());

    return ResultString;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_randogen_DndNativeBridge_GetDndRollButtonTextJNI(
    JNIEnv* Env,
    jobject /* ThisObj */,
    jintArray InDiceCounts,
    jintArray InSideNumbers,
    jint InRollModifier)
{
    if (!Env)
    {
        return nullptr;
    }

    if (!InDiceCounts || !InSideNumbers)
    {
        const std::string ResultStdString =
            DndRollButtonTextFormatter::GetRollButtonText(
                {},
                static_cast<int>(InRollModifier)
            );

        return Env->NewStringUTF(ResultStdString.c_str());
    }

    const jsize DiceCountsLength =
        Env->GetArrayLength(InDiceCounts);

    const jsize SideNumbersLength =
        Env->GetArrayLength(InSideNumbers);

    if (DiceCountsLength <= 0 || DiceCountsLength != SideNumbersLength)
    {
        const std::string ResultStdString =
            DndRollButtonTextFormatter::GetRollButtonText(
                {},
                static_cast<int>(InRollModifier)
            );

        return Env->NewStringUTF(ResultStdString.c_str());
    }

    std::vector<jint> DiceCounts(
        static_cast<std::size_t>(DiceCountsLength));

    std::vector<jint> SideNumbers(
        static_cast<std::size_t>(SideNumbersLength));

    Env->GetIntArrayRegion(
        InDiceCounts,
        0,
        DiceCountsLength,
        DiceCounts.data());

    if (Env->ExceptionCheck() == JNI_TRUE)
    {
        return nullptr;
    }

    Env->GetIntArrayRegion(
        InSideNumbers,
        0,
        SideNumbersLength,
        SideNumbers.data());

    if (Env->ExceptionCheck() == JNI_TRUE)
    {
        return nullptr;
    }

    std::vector<RollPair> RollData;
    RollData.reserve(static_cast<std::size_t>(DiceCountsLength));

    for (jsize Index = 0; Index < DiceCountsLength; ++Index)
    {
        RollPair CurrentRollData;

        CurrentRollData.DiceNum =
            static_cast<int>(DiceCounts[Index]);

        CurrentRollData.SideNum =
            static_cast<int>(SideNumbers[Index]);

        RollData.push_back(CurrentRollData);
    }

    const std::string ResultStdString =
        DndRollButtonTextFormatter::GetRollButtonText(
            RollData,
            static_cast<int>(InRollModifier)
        );

    return Env->NewStringUTF(ResultStdString.c_str());
}
