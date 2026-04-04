#include "leak_warning_suppression.h"
//
#include <SFML/Graphics.hpp>
//
#include "Basics/InputProcessor.h"
#include "Basics/Counters.h"
#include "Basics/RenderLoop.h"
#include "Basics/WindowHolder.h"
#include "Basics/UActiveView.h"
#include "UI/StatsOverlay.h"
#include "UI/MainMenuController.h"
#include "UI/BottomLeftTickBoxController.h"
#include "Projectiles/ProjectileController.h"

int main()
{
    // --------------------------------------------------
    // Stats
    // --------------------------------------------------
    UFpsCounter CurrentFpsCounter;
    UDelegateCounter ActiveParticlesCounter;
    UIncrementCounter FireworksFiredCounter;

    UiStatsOverlay Overlay{
        CurrentFpsCounter,
        ActiveParticlesCounter,
        FireworksFiredCounter
    };

    // --------------------------------------------------
    // Views
    // --------------------------------------------------
    UActiveView WorldView;
    UActiveView UiView;

    // --------------------------------------------------
    // Gameplay
    // --------------------------------------------------
    const std::unique_ptr<UProjectileController> ProjectileCtrl =
        std::make_unique<UProjectileController>(
            WorldView.GetView(),
            FireworksFiredCounter
        );

    ActiveParticlesCounter.QueryValue =
        Delegate<uint32_t()>::FromConstMethod<
            UProjectileController,
            &UProjectileController::GetActiveParticlesNum
        >(ProjectileCtrl.get());

    // --------------------------------------------------
    // UI
    // --------------------------------------------------
    MainMenuController MainMenu(UiView.GetView());
    BottomLeftTickBoxController TickBoxes(UiView.GetView());

    // --------------------------------------------------
    // Input
    // --------------------------------------------------
    UInputProcessor EventProcessor;
    EventProcessor.SetActiveView(WorldView.GetView());

    EventProcessor.SetInputConsumers({
        &MainMenu,
        &TickBoxes
    });

    EventProcessor.SetResizableElements({
        &UiView,
        &WorldView,
        &Overlay,
        &MainMenu,
        &TickBoxes
    });

    EventProcessor.OnLeftMouseReleasedAt[0] =
        Delegate<void(const sf::Vector2f&)>::FromLambda(
            [&](const sf::Vector2f& Pos)
            {
                ProjectileCtrl->SpawnRandomExplosionAt(Pos);
            }
        );

    // --------------------------------------------------
    // Main loop
    // --------------------------------------------------
    RenderLoop MainLoop{ EventProcessor };

    MainLoop.SetUpdatableElements({
        ProjectileCtrl.get(),
        &CurrentFpsCounter,
        &ActiveParticlesCounter,
        &Overlay,
        &MainMenu,
        &TickBoxes
    });

    MainLoop.SetDrawableElements({
        &UiView,
        ProjectileCtrl.get(),
        &Overlay,
        &MainMenu,
        &TickBoxes
    });

    MainLoop.Run();
}
