
#include "CommonLibJNI.h"

jintArray CommonLibJNI::MakeJIntArray(
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


