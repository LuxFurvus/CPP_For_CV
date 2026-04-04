
#include <cmath>
#include <random>
#include <array>
//
#include <SFML/Graphics.hpp>
//
#include "Utility.h"

std::pair<float, float> Utility::BuildHalfScreenWindow()
{
    const sf::VideoMode DesktopMode
        = sf::VideoMode::getDesktopMode();
    const float WindowWidth
        = DesktopMode.width * 0.75f;
    const float WindowHeight
        = DesktopMode.height * 0.75f;

    return {WindowWidth, WindowHeight};
}

sf::Vector2f Utility::GetMouseWorldPosition(
    const sf::RenderWindow &InWindow,
    const sf::View &InView)
{
    const sf::Vector2i MousePixel =
        sf::Mouse::getPosition(InWindow);

    const sf::Vector2f MouseWorld =
        InWindow.mapPixelToCoords(MousePixel, InView);

    return MouseWorld;
}

sf::Vector2f Utility::UnitVectorFromAngle(const float Radians)
{
    const sf::Vector2f Result
        = { std::cos(Radians), std::sin(Radians) };
    return Result;
}

float Utility::GetRandomFloatInRange(const float Min, const float Max)
{
    static thread_local std::mt19937 Generator
    {
        std::random_device{}()
    };

    std::uniform_real_distribution<float> Distribution(Min, Max);

    return Distribution(Generator);
}

sf::Color Utility::GetRandomColor()
{
    std::array<sf::Color, 8> PredefinedColors
    {
        sf::Color::Red,
        sf::Color::Green,
        sf::Color::Blue,
        sf::Color::Yellow,
        sf::Color::Magenta,
        sf::Color::Cyan,
        sf::Color(255, 165, 0), // Orange
        sf::Color(128, 0, 128)  // Purple
    };

    return PredefinedColors[GetRandomFloatInRange(0.0f, 1.0f) * PredefinedColors.size()];
}
