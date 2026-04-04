#pragma once

#include <SFML/Graphics.hpp>
//
#include "../Basics/InputProcessor.h"
#include "../Basics/FrameTimer.h"
#include "../Interfaces/IUpdatable.h"
#include "../Interfaces/IDrawable.h"

class RenderLoop
{

private:

    UInputProcessor& EventProcessor;

    std::array<IUpdatable*, 10> ElementsToUpdate;

    std::array<IDrawable*, 10> ElementsToDraw;

private:

    void ExecuteOneCycle();

public:

    explicit RenderLoop(
        UInputProcessor& InEventProcessor);

    void SetUpdatableElements(
        std::array<IUpdatable*, 10> InElements);

    void SetDrawableElements(
        std::array<IDrawable*, 10> InElements);

    void Run();
};
