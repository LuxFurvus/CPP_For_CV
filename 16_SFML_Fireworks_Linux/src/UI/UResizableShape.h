
#pragma once

#include <concepts>
#include <type_traits>
#include <SFML/Graphics/Shape.hpp>

template <typename T>
concept CSfmlConcreteShape =
    std::derived_from<T, sf::Shape>;

template <CSfmlConcreteShape TShape>
class UResizableShape : public IResizable, public IGetDrawable
{
protected:
    TShape Shape;

public:
    template <typename... Args>
    explicit UResizableShape(Args&&... InArgs)
        : Shape(std::forward<Args>(InArgs)...)
    {}

    TShape& GetShape()
    {
        return Shape;
    }

    const TShape& GetShape() const
    {
        return Shape;
    }

    const sf::Drawable& GetDrawable() const override
    {
        return Shape;
    }
};
