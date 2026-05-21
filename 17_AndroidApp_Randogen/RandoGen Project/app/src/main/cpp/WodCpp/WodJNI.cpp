
#include <jni.h>

#include "../CommonCpp/CommonLibJNI.h"
#include "../CommonCpp/SortedRollsMaker.h"

#include "WodSuccessCounter.h"
#include "WodChanceCounter.h"

extern "C" jstring
Java_com_example_randogen_WoD_WodNativeBridge_GetSuccessChancesJNI(
    JNIEnv* Env,
    jobject /* ThisObj */,
    const jint DiceCount,
    const jint DifficultyValue,
    const jboolean WithCancel,
    const jboolean InWithTenReroll)
{
    const std::string ResultStdString
        = WodChanceCounter::GetProbabilityList(
            DiceCount,
            DifficultyValue,
            WithCancel,
            InWithTenReroll);

    jstring ResultString
        = Env->NewStringUTF(ResultStdString.c_str());

    return ResultString;
}


extern "C" JNIEXPORT jobject JNICALL
Java_com_example_randogen_WoD_WodNativeBridge_GetRollResultsJNI(
    JNIEnv* Env,
    jobject /* ThisObj */,
    const jint DiceCount,
    const jint DifficultyValue,
    const jboolean WithCancel,
    const jboolean InWithTenReroll,
    jclass ResultClass)
{
    const auto [FinalSortedRoll, SuccessNum]
        = WodSuccessCounter::GetSuccessesNum(
            static_cast<int>(DiceCount),
            static_cast<int>(DifficultyValue),
            WithCancel == JNI_TRUE,
            InWithTenReroll == JNI_TRUE);

    if (FinalSortedRoll.empty())
    {
        return nullptr;
    }

    jintArray ReturnRoll
        = CommonLibJNI::MakeJIntArray(Env, FinalSortedRoll);

    if (!ReturnRoll)
    {
        return nullptr;
    }

    jmethodID ConstructorId = Env->GetMethodID(
        ResultClass,
        "<init>",
        "(II[I)V");

    if (!ConstructorId)
    {
        Env->DeleteLocalRef(ReturnRoll);
        return nullptr;
    }

    jobject ResultObject = Env->NewObject(
        ResultClass,
        ConstructorId,
        0,
        static_cast<jint>(SuccessNum),
        ReturnRoll);

    Env->DeleteLocalRef(ReturnRoll);

    return ResultObject;
}
