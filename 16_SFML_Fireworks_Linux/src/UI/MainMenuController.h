#pragma once

#include <SFML/Graphics.hpp>

#include "../Interfaces/IUpdatable.h"
#include "../Interfaces/IDrawable.h"
#include "../Interfaces/IResizable.h"
#include "../Interfaces/IInputConsumer.h"

#include "../UI/Button.h"
#include "../UI/VerticalLayout.h"
#include "../UI/UResizableCircle.h"
#include "../UI/UResizableRectangle.h"
#include "../Basics/WindowHolder.h"
#include "../Basics/GameState.h"

class MainMenuController final :
    public IUpdatable,
    public IDrawable,
    public IResizable,
    public IInputConsumer
{
private:
    // --- Buttons ---
    UButton<UResizableCircle> MainMenuButton;
    UButton<UResizableRectangle> ResumeButton;
    UButton<UResizableRectangle> CloseButton;

    // --- Labels ---
    UiText MenuLabel;
    UiText ResumeLabel;
    UiText CloseLabel;

    // --- Layout ---
    UiVerticalLayout MenuLayout;

private:
    void BindActions()
    {
        MainMenuButton.OnReleased =
            Delegate<void()>::FromLambda([this]()
            {
                ShowMenu();
            });

        ResumeButton.OnReleased =
            Delegate<void()>::FromLambda([this]()
            {
                HideMenu();
            });

        CloseButton.OnReleased =
            Delegate<void()>::FromLambda([this]()
            {
                CloseApp();
            });
    }

    void ShowMenu()
    {
        GameState::Pause();
        MainMenuButton.SetState(EUiItemState::Hidden);
        ResumeButton.SetState(EUiItemState::Visible);
        CloseButton.SetState(EUiItemState::Visible);
    }

    void HideMenu()
    {
        GameState::Resume();
        ResumeButton.SetState(EUiItemState::Hidden);
        CloseButton.SetState(EUiItemState::Hidden);
        MainMenuButton.SetState(EUiItemState::Visible);
    }

    void CloseApp()
    {
        WindowHolder::Get().close();
    }

public:
    explicit MainMenuController(const sf::View& UiView)
        : MainMenuButton(
              ButtonTheme{
                  .Idle = sf::Color{255, 0, 0},
                  .Hovered = sf::Color{150, 0, 0},
                  .Pressed = sf::Color{200, 0, 0}},
              UiView,
              30.0f,
              sf::Color::Red),

          ResumeButton(
              ButtonTheme{
                  .Idle = sf::Color{0, 0, 255},
                  .Hovered = sf::Color{0, 0, 150},
                  .Pressed = sf::Color{0, 0, 200}},
              UiView,
              sf::Vector2f{80.0f, 40.0f},
              sf::Color::Blue),

          CloseButton(
              ButtonTheme{
                  .Idle = sf::Color{0, 255, 0},
                  .Hovered = sf::Color{0, 150, 0},
                  .Pressed = sf::Color{0, 200, 0}},
              UiView,
              sf::Vector2f{80.0f, 40.0f},
              sf::Color::Green),

          MenuLabel(Consts::ObliviousFont, 14.0f, sf::Color::Yellow),
          ResumeLabel(Consts::ObliviousFont, 14.0f, sf::Color::White),
          CloseLabel(Consts::ObliviousFont, 14.0f, sf::Color::White),

          MenuLayout(UiView, 16.0f)
    {
        MenuLabel.SetString("Menu");
        ResumeLabel.SetString("Resume");
        CloseLabel.SetString("Close");

        MainMenuButton.SetLabel(MenuLabel);
        ResumeButton.SetLabel(ResumeLabel);
        CloseButton.SetLabel(CloseLabel);

        ResumeButton.SetState(EUiItemState::Hidden);
        CloseButton.SetState(EUiItemState::Hidden);

        MenuLayout.SetItems({ &ResumeButton, &CloseButton });

        BindActions();
    }

    // --- IUpdatable ---
    void Update() override
    {
        MainMenuButton.Update();
        ResumeButton.Update();
        CloseButton.Update();
    }

    // --- IDrawable ---
    void Draw() const override
    {
        MainMenuButton.Draw();

        ResumeButton.Draw();
        ResumeLabel.Draw();

        CloseButton.Draw();
        CloseLabel.Draw();
    }

    // --- IResizable ---
    void Resize() override
    {
        MainMenuButton.Resize();
        ResumeButton.Resize();
        CloseButton.Resize();

        MenuLabel.Resize();
        ResumeLabel.Resize();
        CloseLabel.Resize();

        MenuLayout.Resize();
    }

    // --- IInputConsumer ---
    bool ConsumeMouseRelease() override
    {
        if (MainMenuButton.ConsumeMouseRelease())
            return true;

        if (ResumeButton.ConsumeMouseRelease())
            return true;

        if (CloseButton.ConsumeMouseRelease())
            return true;

        return false;
    }
};
