
#pragma once

#include <SFML/Graphics.hpp>


class GFrameTimer
{
private:

    static inline sf::Clock FrameCounter;

    static inline float CurrentDeltaTime = 0.0f;

public:

    static void Tick()
    {
        CurrentDeltaTime = FrameCounter.restart().asSeconds();
    }

    static float GetDeltaTime()
    {
        return CurrentDeltaTime;
    }
};
