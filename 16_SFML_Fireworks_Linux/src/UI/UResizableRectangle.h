
#pragma once

#include <SFML/Graphics.hpp>
//
#include "../Interfaces/IGetDrawable.h"
#include "../Interfaces/IResizable.h"
#include "../UI/UResizableShape.h"
#include "../Basics/WindowHolder.h"
#include "../Basics/Consts.h"


class UResizableRectangle final : public UResizableShape<sf::RectangleShape>
{
public:
    explicit UResizableRectangle(
        const sf::Vector2f& InSize,
        const sf::Color& InColor)
        :
        UResizableShape(InSize)
    {
        Shape.setFillColor(InColor);
        Shape.setOrigin(
            InSize.x * 0.5f,
            InSize.y * 0.5f
        );
    }

    void Resize() override
    {
        const sf::RenderWindow& Window = WindowHolder::Get();

        const float ScaleX =
            static_cast<float>(Window.getSize().x) / Consts::ReferenceWidth;
        const float ScaleY =
            static_cast<float>(Window.getSize().y) / Consts::ReferenceHeight;

        const float UniformScale = std::min(ScaleX, ScaleY);

        Shape.setScale(UniformScale, UniformScale);

        const sf::Vector2f ScaledHalfSize =
            Shape.getSize() * 0.5f * UniformScale;

        Shape.setPosition(
            Window.getSize().x - ScaledHalfSize.x,
            Window.getSize().y - ScaledHalfSize.y
        );
    }
};

