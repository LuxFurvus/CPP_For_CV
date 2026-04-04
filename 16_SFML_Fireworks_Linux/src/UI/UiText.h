
#pragma once

#include <string>
#include <assert.h>
#include <SFML/Graphics.hpp>
//
#include "../Interfaces/IResizable.h"
#include "../Interfaces/IDrawable.h"
#include "../Interfaces/ILayoutItem.h"
#include "../UI/UiItemState.h"
#include "../Basics/WindowHolder.h"
#include "../Basics/Consts.h"

class UiText final :
    public IResizable,
    public IDrawable,
    public ILayoutItem
{
private:
    sf::Font Font;
    sf::Text Text;

    EUiItemState State = EUiItemState::Visible;

    float BaseFontSize = 10.0f;

public:
    explicit UiText(
        const std::string& FontPath,
        float InBaseFontSize,
        const sf::Color& Color = sf::Color::White)
        :
        BaseFontSize(InBaseFontSize)
    {
        const bool Loaded = Font.loadFromFile(FontPath);
        assert(Loaded);

        Text.setFont(Font);
        Text.setFillColor(Color);
        Text.setCharacterSize(static_cast<unsigned>(BaseFontSize));
    }

    // ---------------- State ----------------

    void SetState(EUiItemState InState)
    {
        State = InState;
    }

    bool IsVisible() const
    {
        return State == EUiItemState::Visible;
    }

    // ---------------- Text API ----------------

    void SetString(const std::string& InText)
    {
        Text.setString(InText);
    }

    void SetColor(const sf::Color& Color)
    {
        Text.setFillColor(Color);
    }

    // ---------------- Layout ----------------

    void SetPosition(const sf::Vector2f& Position) override
    {
        Text.setPosition(Position);
    }

    sf::Vector2f GetSize() const override
    {
        const auto Bounds = Text.getGlobalBounds();
        return { Bounds.width, Bounds.height };
    }

    // ---------------- Resize ----------------

    void Resize() override
    {
        const sf::RenderWindow& Window = WindowHolder::Get();

        const float ScaleX =
            static_cast<float>(Window.getSize().x) / Consts::ReferenceWidth;
        const float ScaleY =
            static_cast<float>(Window.getSize().y) / Consts::ReferenceHeight;

        const float UniformScale = std::min(ScaleX, ScaleY);

        Text.setCharacterSize(
            static_cast<unsigned>(BaseFontSize * UniformScale));
    }

    // ---------------- Draw ----------------

    void Draw() const override
    {
        if (State != EUiItemState::Visible)
        {
            return;
        }

        sf::RenderWindow& Window = WindowHolder::Get();
        Window.draw(Text);
    }
};

