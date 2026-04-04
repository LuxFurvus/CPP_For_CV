
#include "InputProcessor.h"

UInputProcessor::UInputProcessor() = default;

void UInputProcessor::ProcessEvent_Closed(
    const sf::Event& InEvent)
{
    sf::RenderWindow& CurrentWindow = WindowHolder::Get();
    if (InEvent.type != sf::Event::Closed) return;
    CurrentWindow.close();
}

void UInputProcessor::ProcessEvent_Resized(
    const sf::Event& InEvent)
{
    if (InEvent.type != sf::Event::Resized) return;

    for (IResizable* Element : ResizableElements)
    {
        if (!Element) continue;
        Element->Resize();
    }
}

void UInputProcessor::ProcessEvent_LeftMouseReleased(
    const sf::Event& InEvent)
{
    if (InEvent.type != sf::Event::MouseButtonReleased ||
        InEvent.mouseButton.button != sf::Mouse::Left)
        return;

    for (IInputConsumer* Consumer : InputConsumers)
    {
        if (Consumer && Consumer->ConsumeMouseRelease())
            return;
    }

    sf::RenderWindow& Window = WindowHolder::Get();

    const sf::Vector2f MouseWorld =
        Utility::GetMouseWorldPosition(
            Window,
            ActiveView ? *ActiveView : Window.getDefaultView()
        );

    for (const auto& Handler : OnLeftMouseReleasedAt)
    {
        if (Handler)
            Handler(MouseWorld);
    }
}

void UInputProcessor::SetActiveView(
    const sf::View& InView)
{
    ActiveView = &InView;
}

void UInputProcessor::PollEvents()
{
    sf::Event Event;

    sf::RenderWindow& CurrentWindow = WindowHolder::Get();
    while (CurrentWindow.pollEvent(Event))
    {
        ProcessEvent_Closed(Event);
        ProcessEvent_Resized(Event);
        ProcessEvent_LeftMouseReleased(Event);
    }
}

void UInputProcessor::SetResizableElements(
    const std::array<IResizable*, 10>& InElements)
{
    ResizableElements = InElements;
}

void UInputProcessor::SetInputConsumers(
    const std::array<IInputConsumer*, 10>& InConsumers)
{
    InputConsumers = InConsumers;
}
