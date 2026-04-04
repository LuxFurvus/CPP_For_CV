#pragma once

#include <string>
#include <assert.h>
//
#include <SFML/Graphics.hpp>
//
#include "../Basics/Consts.h"
#include "../Basics/Counters.h"
#include "../UI/UiText.h"
#include "../Interfaces/IUpdatable.h"
#include "../Interfaces/IResizable.h"

class UiStatsOverlay :
    public IResizable,
    public IUpdatable,
    public IDrawable
{
private:

    UiText OverlayText{
        Consts::ObliviousFont,
        10.0f,
        sf::Color::White
    };

    const UFpsCounter& CurrentFpsCounter;
    const UDelegateCounter& ActiveParticlesCounter;
    const UIncrementCounter& FireworksFiredCounter;

    static constexpr float BasePadding = 10.0f;
    static constexpr unsigned int FontSize = 10u;

    static inline const std::string FPSLabel {"FPS: "};
    static inline const std::string ParticlesLabel {"Particles: "};
    static inline const std::string FireworksLabel {"Fireworks: "};

public:

    UiStatsOverlay(
        const UFpsCounter& InFpsCounter,
        const UDelegateCounter& InActiveParticlesCounter,
        const UIncrementCounter& InFireworksFiredCounter
    ) :
        CurrentFpsCounter(InFpsCounter),
        ActiveParticlesCounter(InActiveParticlesCounter),
        FireworksFiredCounter(InFireworksFiredCounter)
    {
        // const bool Loaded
        //     = Font.loadFromFile("assets/fonts/ObliviousFont.ttf");
        // assert(Loaded);

        // Text.setFont(Font);
        // Text.setCharacterSize(FontSize);
        // Text.setFillColor(sf::Color::White);
        // Text.setPosition(BasePadding, BasePadding);
    }

    void Update() override
    {
        std::string OverlayTextString;

        OverlayTextString.append(FPSLabel);
        OverlayTextString.append(std::to_string(CurrentFpsCounter.GetValue()));
        OverlayTextString.append("\n");

        OverlayTextString.append(FireworksLabel);
        OverlayTextString.append(std::to_string(FireworksFiredCounter.GetValue()));
        OverlayTextString.append("\n");

        OverlayTextString.append(ParticlesLabel);
        OverlayTextString.append(std::to_string(ActiveParticlesCounter.GetValue()));

        // Text.setString(OverlayText);
        OverlayText.SetString(OverlayTextString);
    }

    void Resize() override
    {
        OverlayText.Resize();

        const sf::RenderWindow& Window = WindowHolder::Get();

        const float ScaleX =
            static_cast<float>(Window.getSize().x) / Consts::ReferenceWidth;
        const float ScaleY =
            static_cast<float>(Window.getSize().y) / Consts::ReferenceHeight;

        const float UniformScale = std::min(ScaleX, ScaleY);

        const float ScaledPadding = BasePadding * UniformScale;

        OverlayText.SetPosition({
            ScaledPadding,
            ScaledPadding
        });
    }

    void Draw() const override
    {
        OverlayText.Draw();
    }
};
