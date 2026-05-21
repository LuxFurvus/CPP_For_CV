#include <jni.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <exception>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

#include "RollAndKeepChanceCounter.h"

namespace
{
    JavaVM* GlobalJavaVm = nullptr;

    struct FAsyncChanceRequest
    {
        jlong RequestId = 0;
        int RollNum = 0;
        int KeepNum = 0;
        int Bonus = 0;
        bool ExplodeTens = false;
        bool ExplodeOnes = false;
        jobject CallbackGlobalRef = nullptr;
    };

    class FScopedThreadEnv
    {
    private:
        JNIEnv* Env = nullptr;
        bool AttachedHere = false;

    public:
        FScopedThreadEnv()
        {
            if (!GlobalJavaVm)
            {
                return;
            }

            if (const jint GetEnvResult = GlobalJavaVm->GetEnv(
                    reinterpret_cast<void**>(&Env),
                    JNI_VERSION_1_6);
                GetEnvResult == JNI_OK)
            {
                return;
            }

            if (const jint AttachResult
                    = GlobalJavaVm->AttachCurrentThread(&Env, nullptr);
                AttachResult != JNI_OK)
            {
                Env = nullptr;
                return;
            }

            AttachedHere = true;
        }

        ~FScopedThreadEnv()
        {
            if (!AttachedHere || !GlobalJavaVm)
            {
                return;
            }

            GlobalJavaVm->DetachCurrentThread();
        }

        JNIEnv* GetEnv() const
        {
            return Env;
        }

        bool IsValid() const
        {
            return Env != nullptr;
        }
    };

    class FRollAndKeepChanceDispatcher
    {
    private:

        std::mutex Mutex;
        std::condition_variable Condition;
        std::thread WorkerThread;
        std::atomic<jlong> LatestRequestId = 0;

        FAsyncChanceRequest PendingRequest;

        bool StopRequested = false;
        bool HasPendingRequest = false;

    private:

        static void DeleteGlobalRefSafe(jobject InGlobalRef)
        {
            if (InGlobalRef == nullptr)
            {
                return;
            }

            FScopedThreadEnv ScopedEnv;
            if (!ScopedEnv.IsValid())
            {
                return;
            }

            ScopedEnv.GetEnv()->DeleteGlobalRef(InGlobalRef);
        }

        static void ClearPendingException(JNIEnv* Env)
        {
            if (!Env || Env->ExceptionCheck() != JNI_TRUE)
            {
                return;
            }

            Env->ExceptionDescribe();
            Env->ExceptionClear();
        }

        static void NotifyCallback(
            const FAsyncChanceRequest& InRequest,
            const char* InMethodName,
            const std::string& InText)
        {
            if (InRequest.CallbackGlobalRef == nullptr)
            {
                return;
            }

            FScopedThreadEnv ScopedEnv;
            if (!ScopedEnv.IsValid())
            {
                return;
            }

            JNIEnv* Env = ScopedEnv.GetEnv();

            jclass CallbackClass = Env->GetObjectClass(InRequest.CallbackGlobalRef);
            if (CallbackClass == nullptr)
            {
                ClearPendingException(Env);
                return;
            }

            jmethodID MethodId = Env->GetMethodID(
                CallbackClass,
                InMethodName,
                "(JLjava/lang/String;)V");

            if (MethodId == nullptr)
            {
                Env->DeleteLocalRef(CallbackClass);
                ClearPendingException(Env);
                return;
            }

            jstring ResultString = Env->NewStringUTF(InText.c_str());
            if (ResultString == nullptr)
            {
                Env->DeleteLocalRef(CallbackClass);
                ClearPendingException(Env);
                return;
            }

            Env->CallVoidMethod(
                InRequest.CallbackGlobalRef,
                MethodId,
                InRequest.RequestId,
                ResultString);

            ClearPendingException(Env);

            Env->DeleteLocalRef(ResultString);
            Env->DeleteLocalRef(CallbackClass);
        }

        void WorkerLoop()
        {
            for (;;)
            {
                FAsyncChanceRequest CurrentRequest;

                {
                    std::unique_lock<std::mutex> Lock(Mutex);

                    Condition.wait(
                        Lock,
                        [this]()
                        {
                            return StopRequested || HasPendingRequest;
                        });

                    if (StopRequested && !HasPendingRequest)
                    {
                        break;
                    }

                    CurrentRequest = PendingRequest;
                    PendingRequest = FAsyncChanceRequest{};
                    HasPendingRequest = false;
                }

                try
                {
                    const std::string ResultText =
                        RollAndKeepChanceCounter::GetProbabilityList(
                            CurrentRequest.RollNum,
                            CurrentRequest.KeepNum,
                            CurrentRequest.Bonus,
                            CurrentRequest.ExplodeTens,
                            CurrentRequest.ExplodeOnes);

                    if (CurrentRequest.RequestId == LatestRequestId.load())
                    {
                        NotifyCallback(
                            CurrentRequest,
                            "OnRollAndKeepChancesReady",
                            ResultText);
                    }
                }
                catch (const std::exception& ExceptionObj)
                {
                    if (CurrentRequest.RequestId == LatestRequestId.load())
                    {
                        NotifyCallback(
                            CurrentRequest,
                            "OnRollAndKeepChancesFailed",
                            ExceptionObj.what());
                    }
                }
                catch (...)
                {
                    if (CurrentRequest.RequestId == LatestRequestId.load())
                    {
                        NotifyCallback(
                            CurrentRequest,
                            "OnRollAndKeepChancesFailed",
                            "Native calculation failed.");
                    }
                }

                DeleteGlobalRefSafe(CurrentRequest.CallbackGlobalRef);
                CurrentRequest.CallbackGlobalRef = nullptr;
            }

            if (PendingRequest.CallbackGlobalRef != nullptr)
            {
                DeleteGlobalRefSafe(PendingRequest.CallbackGlobalRef);
                PendingRequest.CallbackGlobalRef = nullptr;
            }
        }

        FRollAndKeepChanceDispatcher()
        {
            WorkerThread = std::thread(&FRollAndKeepChanceDispatcher::WorkerLoop, this);
        }

        ~FRollAndKeepChanceDispatcher()
        {
            Shutdown();
        }

    public:
        FRollAndKeepChanceDispatcher(const FRollAndKeepChanceDispatcher&) = delete;
        FRollAndKeepChanceDispatcher& operator=(const FRollAndKeepChanceDispatcher&) = delete;

        static FRollAndKeepChanceDispatcher& Get()
        {
            static FRollAndKeepChanceDispatcher Instance;
            return Instance;
        }

        void Enqueue(
            JNIEnv* Env,
            jlong InRequestId,
            jint InRollNum,
            jint InKeepNum,
            jint InBonus,
            jboolean InExplodeTens,
            jboolean InExplodeOnes,
            jobject InCallback)
        {
            if (Env == nullptr || InCallback == nullptr)
            {
                return;
            }

            FAsyncChanceRequest NewRequest;
            NewRequest.RequestId = InRequestId;
            NewRequest.RollNum = static_cast<int>(InRollNum);
            NewRequest.KeepNum = static_cast<int>(InKeepNum);
            NewRequest.Bonus = static_cast<int>(InBonus);
            NewRequest.ExplodeTens = (InExplodeTens == JNI_TRUE);
            NewRequest.ExplodeOnes = (InExplodeOnes == JNI_TRUE);
            NewRequest.CallbackGlobalRef = Env->NewGlobalRef(InCallback);

            if (NewRequest.CallbackGlobalRef == nullptr)
            {
                ClearPendingException(Env);
                return;
            }

            LatestRequestId.store(InRequestId);

            jobject OldPendingCallback = nullptr;

            {
                std::lock_guard<std::mutex> Lock(Mutex);

                if (HasPendingRequest && PendingRequest.CallbackGlobalRef != nullptr)
                {
                    OldPendingCallback = PendingRequest.CallbackGlobalRef;
                }

                PendingRequest = NewRequest;
                HasPendingRequest = true;
            }

            if (OldPendingCallback != nullptr)
            {
                DeleteGlobalRefSafe(OldPendingCallback);
            }

            Condition.notify_one();
        }

        void Shutdown()
        {
            {
                std::lock_guard<std::mutex> Lock(Mutex);
                StopRequested = true;
            }

            Condition.notify_one();

            if (WorkerThread.joinable())
            {
                WorkerThread.join();
            }
        }
    };
}

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* InVm, void* /* Reserved */)
{
    GlobalJavaVm = InVm;
    FRollAndKeepChanceDispatcher::Get();
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM* /* InVm */, void* /* Reserved */)
{
    FRollAndKeepChanceDispatcher::Get().Shutdown();
    GlobalJavaVm = nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_randogen_RollAndKeepNativeBridge_StartRollAndKeepChancesAsyncJNI(
    JNIEnv* Env,
    jobject /* ThisObj */,
    jlong InRequestId,
    jint InRollNum,
    jint InKeepNum,
    jint InBonus,
    jboolean InExplodeTens,
    jboolean InExplodeOnes,
    jobject InCallback)
{
    FRollAndKeepChanceDispatcher::Get().Enqueue(
        Env,
        InRequestId,
        InRollNum,
        InKeepNum,
        InBonus,
        InExplodeTens,
        InExplodeOnes,
        InCallback);
}
