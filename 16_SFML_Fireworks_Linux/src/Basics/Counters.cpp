
#include "../Basics/Counters.h"


void UFpsCounter::Update()
{
    const float DeltaTime = GFrameTimer::GetDeltaTime();

    AccumulatedTime += DeltaTime;
    ++FrameCounter;

    if (AccumulatedTime >= UpdateInterval)
    {
        CurrentFPS = FrameCounter / AccumulatedTime;

        AccumulatedTime = 0.0f;
        FrameCounter = 0;
    }
}

