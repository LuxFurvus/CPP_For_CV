
#pragma once

#include <SFML/Graphics.hpp>
//
#include "../Interfaces/IGetDrawable.h"
#include "../Interfaces/IResizable.h"
#include "../UI/UResizableShape.h"
#include "../Basics/WindowHolder.h"
#include "../Basics/Consts.h"


class UResizableCircle final : public UResizableShape<sf::CircleShape>
{
public:
    explicit UResizableCircle(
        float InRadius,
        const sf::Color& InColor)
        :
        UResizableShape(InRadius)
    {
        Shape.setFillColor(InColor);
        Shape.setOrigin(InRadius, InRadius);
    }

    void Resize() override
    {
        const sf::RenderWindow& Window = WindowHolder::Get();

        const float ScaleX = Window.getSize().x / Consts::ReferenceWidth;
        const float ScaleY = Window.getSize().y / Consts::ReferenceHeight;
        const float UniformScale = std::min(ScaleX, ScaleY);

        Shape.setScale(UniformScale, UniformScale);

        const float ScaledRadius = Shape.getRadius() * UniformScale;
        Shape.setPosition(
            Window.getSize().x - ScaledRadius,
            Window.getSize().y - ScaledRadius
        );
    }
};

