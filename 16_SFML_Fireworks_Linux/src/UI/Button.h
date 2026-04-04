#pragma once

#include <SFML/Graphics.hpp>
//
#include "../Basics/Utility.h"
#include "../Interfaces/IResizable.h"
#include "../Interfaces/IUpdatable.h"
#include "../Interfaces/IDrawable.h"
#include "../Interfaces/IGetDrawable.h"
#include "../Interfaces/IInputConsumer.h"
#include "../Interfaces/ILayoutItem.h"
#include "../UI/UiItemState.h"
#include "../UI/UiText.h"
#include "../UI/UResizableShape.h"
#include "../Basics/WindowHolder.h"
#include "../Delegates.h"
enum class ButtonState
{
    Idle,
    Hovered,
    Pressed
};

struct ButtonStyle
{
    const sf::Color FillColor;
};

struct ButtonTheme
{
    const ButtonStyle Idle;
    const ButtonStyle Hovered;
    const ButtonStyle Pressed;
};



template <typename TResizable>
class UButton final :
    public IResizable,
    public IUpdatable,
    public IDrawable,
    public IInputConsumer,
    public ILayoutItem
{
private:

    ButtonTheme Theme;
    const sf::View& OwningView;
    TResizable ButtonShape;

    ButtonState PreviousState = ButtonState::Idle;
    ButtonState CurrentState  = ButtonState::Idle;

    EUiItemState State = EUiItemState::Visible;

    UiText* Label = nullptr;

private:

    ButtonState ResolveState(
        const sf::Vector2f& MouseWorld) const
    {
        const bool bIsHovering =
            ButtonShape.GetShape()
                .getGlobalBounds()
                .contains(MouseWorld);

        if (!bIsHovering)
        {
            return ButtonState::Idle;
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            return ButtonState::Pressed;
        }

        return ButtonState::Hovered;
    }

    void ApplyStyle()
    {
        sf::Color TargetColor;

        switch (CurrentState)
        {
            case ButtonState::Idle:
                TargetColor = Theme.Idle.FillColor;
                break;

            case ButtonState::Hovered:
                TargetColor = Theme.Hovered.FillColor;
                break;

            case ButtonState::Pressed:
                TargetColor = Theme.Pressed.FillColor;
                break;
        }

        ButtonShape.GetShape().setFillColor(TargetColor);
    }

    void UpdateLabelPosition()
    {
        if (!Label)
        {
            return;
        }

        const sf::Vector2f ButtonPos =
            ButtonShape.GetShape().getPosition();

        const sf::Vector2f LabelSize  = Label->GetSize();

        Label->SetPosition({
            ButtonPos.x - LabelSize.x * 0.5f,
            ButtonPos.y - LabelSize.y * 0.5f
        });
    }

public:

    Delegate<void()> OnReleased;

public:

    template <typename... ShapeArgs>
    explicit UButton(
        const ButtonTheme& InTheme,
        const sf::View& InView,
        ShapeArgs&&... InShapeArgs)
        :
        Theme(InTheme),
        OwningView(InView),
        ButtonShape(std::forward<ShapeArgs>(InShapeArgs)...)
    {
        ButtonShape.GetShape().setFillColor(Theme.Idle.FillColor);
    }

    bool ConsumeMouseRelease() override
    {
        if (State != EUiItemState::Visible)
        {
            return false;
        }

        const sf::Vector2f NewMouseWorld =
            Utility::GetMouseWorldPosition(
                WindowHolder::Get(),
                OwningView);

        const bool bInside =
            ButtonShape.GetShape()
                .getGlobalBounds()
                .contains(NewMouseWorld);

        if (!bInside)
        {
            return false;
        }

        const bool bIsButtonReleased = true
            && PreviousState == ButtonState::Pressed
            && CurrentState  == ButtonState::Hovered
            && OnReleased;

        if (bIsButtonReleased)
        {
            OnReleased();
        }

        return true; // input consumed
    }

    void SetState(const EUiItemState InState)
    {
        State = InState;

        if (Label)
        {
            Label->SetState(InState);
        }
    }

    bool IsVisible() const
    {
        return State == EUiItemState::Visible;
    }

    void SetPosition(const sf::Vector2f& Position) override
    {
        ButtonShape.GetShape().setPosition(Position);
        UpdateLabelPosition();
    }

    void SetLabel(UiText& InLabel)
    {
        Label = &InLabel;

        Label->SetState(State);

        UpdateLabelPosition();
    }

    sf::Vector2f GetSize() const override
    {
        return ButtonShape.GetShape().getGlobalBounds().getSize();
    }

    void Update() override
    {
        if (State != EUiItemState::Visible)
        {
            PreviousState = ButtonState::Idle;
            CurrentState  = ButtonState::Idle;
            return;
        }

        const sf::RenderWindow& Window
            = WindowHolder::Get();

        const sf::Vector2f MouseWorld =
            Utility::GetMouseWorldPosition(Window, OwningView);

        PreviousState = CurrentState;

        CurrentState = ResolveState(MouseWorld);
        ApplyStyle();

        const bool bIsButtonReleased =
            PreviousState == ButtonState::Pressed &&
            CurrentState  == ButtonState::Hovered;
        if (bIsButtonReleased && OnReleased)
        {
            OnReleased();
        }
    }

    void Resize() override
    {
        ButtonShape.Resize();

        if (Label)
        {
            Label->Resize();
            UpdateLabelPosition();
        }
    }

    void Draw() const override
    {
        if (State != EUiItemState::Visible)
        {
            return;
        }

        sf::RenderWindow& Window = WindowHolder::Get();
        Window.draw(ButtonShape.GetDrawable());

        if (Label)
        {
            Label->Draw();
        }
    }
};
