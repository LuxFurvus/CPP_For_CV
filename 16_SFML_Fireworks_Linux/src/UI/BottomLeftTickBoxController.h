
#pragma once

#include <array>
#include <SFML/Graphics.hpp>

#include "../Interfaces/IResizable.h"
#include "../Interfaces/IUpdatable.h"
#include "../Interfaces/IDrawable.h"
#include "../Interfaces/IInputConsumer.h"
#include "../UI/UTickBox.h"
#include "../Basics/WindowHolder.h"
#include "../Basics/GameState.h"

class BottomLeftTickBoxController final :
    public IResizable,
    public IUpdatable,
    public IDrawable,
    public IInputConsumer
{
private:

    static constexpr float Padding = 25.0f;
    static constexpr float Spacing = 10.0f;

    std::array<UTickBox, 2> Boxes;

private:

    float GetMaxWidth() const
    {
        float MaxWidth = 0.0f;

        for (const UTickBox& Box : Boxes)
        {
            MaxWidth = std::max(MaxWidth, Box.GetSize().x);
        }

        return MaxWidth;
    }

public:

    explicit BottomLeftTickBoxController(const sf::View& UiView)
        : Boxes{
            UTickBox(UiView, "Big Projectiles"),
            UTickBox(UiView, "High Velocity"),
            // UTickBox(UiView, "Single Projectile")
        }
    {
        Boxes[0].OnChanged = Delegate<void(bool)>::FromLambda(
            [](const bool bChecked) {
                GameState::SetBigProjectiles(bChecked);
        });
        Boxes[1].OnChanged = Delegate<void(bool)>::FromLambda(
            [](const bool bChecked) {
                GameState::SetHighVelocity(bChecked);
        });
        // Boxes[2].OnChanged = Delegate<void(bool)>::FromLambda(
        //     [](const bool bChecked) {
        //         GameState::SetSingleProjectile(bChecked);
        // });
    }

    // ---------------- Resize (layout) ----------------

    void Resize() override
    {
        const sf::RenderWindow& Window = WindowHolder::Get();

        float CurrentY =
            static_cast<float>(Window.getSize().y) - Padding;

        for (UTickBox& Box : Boxes)
        {
            const sf::Vector2f Size = Box.GetSize();

            const float LeftX = Padding;

            Box.SetPosition({
                LeftX,
                CurrentY - Size.y * 0.5f
            });

            CurrentY -= Size.y + Spacing;
        }
    }

    // ---------------- Update ----------------

    void Update() override
    {
        for (UTickBox& Box : Boxes)
        {
            Box.Update();
        }
    }

    // ---------------- Draw ----------------

    void Draw() const override
    {
        for (const UTickBox& Box : Boxes)
        {
            Box.Draw();
        }
    }

    // ---------------- Input ----------------

    bool ConsumeMouseRelease() override
    {
        for (UTickBox& Box : Boxes)
        {
            if (Box.ConsumeMouseRelease())
                return true;
        }

        return false;
    }
};
