#pragma once

#include <SFML/Graphics.hpp>
//
#include "../Delegates.h"
#include "../Interfaces/IResizable.h"
#include "../Interfaces/IInputConsumer.h"
#include "../Basics/WindowHolder.h"

class UInputProcessor
{
private:

    std::array<IInputConsumer*, 10> InputConsumers;

    std::array<IResizable*, 10> ResizableElements;

    const sf::View* ActiveView = nullptr;

private:

    void ProcessEvent_Closed(
        const sf::Event& InEvent);

    void ProcessEvent_Resized(
        const sf::Event& InEvent);

    void ProcessEvent_LeftMouseReleased(
        const sf::Event& InEvent);

public:

    UInputProcessor();

    std::array<Delegate<void(const sf::Vector2f&)>, 10> OnLeftMouseReleasedAt;

    void SetActiveView(
        const sf::View& InView);

    void PollEvents();

    void SetResizableElements(
        const std::array<IResizable*, 10>& InElements);

    void SetInputConsumers(
        const std::array<IInputConsumer*, 10>& InConsumers);
};
