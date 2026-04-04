#pragma once

namespace Utility
{

std::pair<float, float> BuildHalfScreenWindow();

sf::Vector2f GetMouseWorldPosition(
    const sf::RenderWindow& InWindow,
    const sf::View& InView);

sf::Vector2f UnitVectorFromAngle(
    const float Radians);

float GetRandomFloatInRange(
    const float Min,
    const float Max);

sf::Color GetRandomColor();

} // namespace Utility
