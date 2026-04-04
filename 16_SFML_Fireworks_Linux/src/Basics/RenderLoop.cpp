#include "RenderLoop.h"

void RenderLoop::ExecuteOneCycle()
{
    GFrameTimer::Tick();

    EventProcessor.PollEvents();

    for (IUpdatable* Element : ElementsToUpdate)
    {
        if (!Element) continue;
        Element->Update();
    }

    sf::RenderWindow& CurrentWindow = WindowHolder::Get();
    CurrentWindow.clear(sf::Color::Black);

    for (IDrawable* Element : ElementsToDraw)
    {
        if (!Element) continue;
        Element->Draw();
    }

    CurrentWindow.display();
}

RenderLoop::RenderLoop(
    UInputProcessor& InEventProcessor)
    :
        EventProcessor(InEventProcessor)
{
}

void RenderLoop::SetUpdatableElements(
    std::array<IUpdatable*, 10> InElements)
{
    ElementsToUpdate = InElements;
}

void RenderLoop::SetDrawableElements(
    std::array<IDrawable*, 10> InElements)
{
    ElementsToDraw = InElements;
}

void RenderLoop::Run()
{
    sf::RenderWindow& CurrentWindow = WindowHolder::Get();
    while (CurrentWindow.isOpen())
    {
        ExecuteOneCycle();
    }
}
