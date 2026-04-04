
#pragma once

#include <SFML/System/Vector2.hpp>

struct ILayoutItem
{
    virtual ~ILayoutItem() = default;
    virtual void SetPosition(const sf::Vector2f& Position) = 0;
    virtual sf::Vector2f GetSize() const = 0;
};

