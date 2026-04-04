#pragma once

#include <SFML/Graphics.hpp>

#include "../Interfaces/IResizable.h"
#include "../Interfaces/IUpdatable.h"
#include "../Interfaces/IDrawable.h"
#include "../Interfaces/IInputConsumer.h"
#include "../Interfaces/ILayoutItem.h"

#include "../UI/UiText.h"
#include "../UI/UiItemState.h"
#include "../UI/UResizableRectangle.h"
#include "../Basics/Utility.h"
#include "../Basics/WindowHolder.h"
#include "../Delegates.h"

class UTickBox final :
    public IResizable,
    public IUpdatable,
    public IDrawable,
    public IInputConsumer,
    public ILayoutItem
{
private:

    UiText Label;

    UResizableRectangle Box;

    const sf::View& OwningView;

    static constexpr float Spacing = 8.0f;

    EUiItemState State = EUiItemState::Visible;

    bool bChecked = false;

public:

    Delegate<void(const bool)> OnChanged;

private:

    void UpdateVisual()
    {
        Box.GetShape().setFillColor(
            bChecked ? sf::Color::Green : sf::Color::Transparent
        );
    }

public:

    explicit UTickBox(
        const sf::View& InView,
        const std::string& InText,
        float FontSize = 14.0f)
        :
        Label(Consts::ObliviousFont, FontSize, sf::Color::White),
        Box(sf::Vector2f{ 18.0f, 18.0f }, sf::Color::Transparent),
        OwningView(InView)
    {
        Label.SetString(InText);
        Box.GetShape().setOutlineThickness(2.0f);
        Box.GetShape().setOutlineColor(sf::Color::White);

        UpdateVisual();
    }

    // ---------------- State ----------------

    void SetState(EUiItemState InState)
    {
        State = InState;
        Label.SetState(InState);
    }

    bool IsVisible() const
    {
        return State == EUiItemState::Visible;
    }

    // ---------------- Value ----------------

    bool IsChecked() const
    {
        return bChecked;
    }

    void SetChecked(bool InChecked)
    {
        bChecked = InChecked;
        UpdateVisual();
    }

    // ---------------- Layout ----------------

    void SetPosition(const sf::Vector2f& Position) override
    {
        Box.GetShape().setPosition(Position);

        const sf::Vector2f BoxSize =
            Box.GetShape().getGlobalBounds().getSize();

        const sf::Vector2f LabelSize =
            Label.GetSize();

        Label.SetPosition({
            Position.x + BoxSize.x * 0.5f + Spacing,
            Position.y - LabelSize.y * 0.5f
        });
    }

    sf::Vector2f GetSize() const override
    {
        const sf::Vector2f BoxSize =
            Box.GetShape().getGlobalBounds().getSize();

        const sf::Vector2f LabelSize =
            Label.GetSize();

        return {
            BoxSize.x + Spacing + LabelSize.x,
            std::max(BoxSize.y, LabelSize.y)
        };
    }

    // ---------------- Input ----------------

    bool ConsumeMouseRelease() override
    {
        if (State != EUiItemState::Visible)
            return false;

        const sf::Vector2f MouseWorld =
            Utility::GetMouseWorldPosition(
                WindowHolder::Get(),
                OwningView
            );

        if (!Box.GetShape().getGlobalBounds().contains(MouseWorld))
            return false;

        bChecked = !bChecked;
        UpdateVisual();

        if (OnChanged)
            OnChanged(bChecked);

        return true;
    }

    // ---------------- Update ----------------

    void Update() override
    {
        // no per-frame logic yet
    }

    // ---------------- Resize ----------------

    void Resize() override
    {
        Label.Resize();
        Box.Resize();
    }

    // ---------------- Draw ----------------

    void Draw() const override
    {
        if (State != EUiItemState::Visible)
            return;

        sf::RenderWindow& Window = WindowHolder::Get();

        Label.Draw();
        Window.draw(Box.GetDrawable());
    }
};
