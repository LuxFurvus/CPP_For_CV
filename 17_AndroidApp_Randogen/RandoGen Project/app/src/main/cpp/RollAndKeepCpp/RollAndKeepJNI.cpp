
#include <jni.h>

#include "../CommonCpp/CommonLibJNI.h"

#include "RollAndKeepRulesLib.h"
#include "RollAndKeepChanceCounter.h"

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_randogen_RollAndKeepNativeBridge_GetRollAndKeepResultsJNI(
    JNIEnv* Env,
    jobject /* ThisObj */,
    jint InRollNum,
    jint InKeepNum,
    jint InBonus,
    jboolean InExplodeTens,
    jboolean InExplodeOnes,
    jclass ResultClass)
{
    const auto [SequenceRnK, SumRnK]
        = RollAndKeepRulesLib::GetRollAndKeepResults(
            static_cast<int>(InRollNum),
            static_cast<int>(InKeepNum),
            static_cast<int>(InBonus),
            InExplodeTens == JNI_TRUE,
            InExplodeOnes == JNI_TRUE);

    if (SequenceRnK.empty())
    {
        return nullptr;
    }

    jintArray ReturnRoll = CommonLibJNI::MakeJIntArray(Env, SequenceRnK);

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

    constexpr jint PlaceholderForSuccessNum = 0;

    jobject ResultObject = Env->NewObject(
        ResultClass,
        ConstructorId,
        static_cast<jint>(SumRnK),
        PlaceholderForSuccessNum,
        ReturnRoll);

    Env->DeleteLocalRef(ReturnRoll);

    return ResultObject;
}
