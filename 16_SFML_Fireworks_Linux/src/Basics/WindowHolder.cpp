#include "WindowHolder.h"


void WindowHolder::Initialize()
{
    const auto [WindowWidth, WindowHeight]
        = Utility::BuildHalfScreenWindow();

    Instance = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(WindowWidth, WindowHeight),
        WindowTitle,
        sf::Style::Default);

    Instance->setFramerateLimit(FramerateLimit);
}

sf::RenderWindow& WindowHolder::Get()
{
    if (!Instance)
    {
        Initialize();
    }
    return *Instance;
}
