#include <jni.h>

#include "SuccessChanceCounter.h"
#include "SortedRollsMaker.h"

static jintArray MakeJIntArray(
    JNIEnv* Env,
    const std::vector<int>& Values)
{
    if (Env == nullptr)
    {
        return nullptr;
    }

    jintArray ResultArray =
        Env->NewIntArray(static_cast<jsize>(Values.size()));

    if (ResultArray == nullptr)
    {
        return nullptr;
    }

    if (Values.empty())
    {
        return ResultArray;
    }

    std::vector<jint> JniValues;
    JniValues.reserve(Values.size());

    for (const int Value : Values)
    {
        JniValues.push_back(static_cast<jint>(Value));
    }

    Env->SetIntArrayRegion(
        ResultArray,
        0,
        static_cast<jsize>(JniValues.size()),
        JniValues.data());

    if (Env->ExceptionCheck() == JNI_TRUE)
    {
        Env->DeleteLocalRef(ResultArray);
        return nullptr;
    }

    return ResultArray;
}



extern "C" jstring
Java_com_example_randogen_DiceNativeBridge_GetSuccessChancesJNI(
    JNIEnv* Env,
    jobject /* ThisObj */,
    const jint DiceCount,
    const jint SideNumber,
    const jint DifficultyValue,
    const jboolean WithCancel)
{
    const std::string ResultStdString
        = FSuccessChanceCounter::GetProbabilityList(
            DiceCount,
            SideNumber,
            DifficultyValue,
            WithCancel);

    jstring ResultString
        = Env->NewStringUTF(ResultStdString.c_str());;

    return ResultString;
}


extern "C" JNIEXPORT jobject JNICALL
Java_com_example_randogen_DiceNativeBridge_GetRollResultsJNI(
        JNIEnv* Env,
        jobject /* ThisObj */,
        const jint DiceCount,
        const jint SideNumber,
        const jint DifficultyValue,
        const jboolean WithCancel,
        jclass ResultClass)
{
    const std::vector<int> ResultRoll = SortedRollsMaker::GetSortedRoll(
        static_cast<int>(DiceCount),
        static_cast<int>(SideNumber));

    if (ResultRoll.empty())
    {
        return nullptr;
    }

    jintArray ReturnRoll = MakeJIntArray(Env, ResultRoll);

    if (ReturnRoll == nullptr)
    {
        return nullptr;
    }

    jmethodID ConstructorId = Env->GetMethodID(
        ResultClass,
        "<init>",
        "(II[I)V");

    if (ConstructorId == nullptr)
    {
        Env->DeleteLocalRef(ReturnRoll);
        return nullptr;
    }

    const int RollSum = SortedRollsMaker::GetRollSum(ResultRoll);

    const int SuccessNum = SortedRollsMaker::GetSuccessesNum(
        ResultRoll,
        static_cast<int>(DifficultyValue),
        WithCancel == JNI_TRUE);

    jobject ResultObject = Env->NewObject(
        ResultClass,
        ConstructorId,
        static_cast<jint>(RollSum),
        static_cast<jint>(SuccessNum),
        ReturnRoll);

    Env->DeleteLocalRef(ReturnRoll);

    return ResultObject;
}
