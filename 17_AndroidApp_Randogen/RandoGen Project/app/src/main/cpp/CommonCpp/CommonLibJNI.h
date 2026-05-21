
#pragma once

#include <jni.h>

#include <vector>

struct CommonLibJNI
{
    static jintArray MakeJIntArray(
        JNIEnv* Env,
        const std::vector<int>& Values);
};
