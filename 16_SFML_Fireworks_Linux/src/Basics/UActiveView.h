#pragma once

#include <SFML/Graphics.hpp>
//
#include "../Basics/WindowHolder.h"
#include "../Interfaces/IResizable.h"
#include "../Interfaces/IDrawable.h"


class UActiveView : public IResizable, public IDrawable
{
private:

    sf::View TargetView;

    std::array<const sf::Drawable*, 3> Drawables;

public:

    explicit UActiveView();

    sf::View& GetView();

    void Resize() override;

    void AddDrawableElements(const std::array<const sf::Drawable*, 3>& InDrawables);

    void Draw() const override;
};
