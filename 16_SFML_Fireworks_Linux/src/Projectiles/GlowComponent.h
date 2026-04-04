#pragma once

#include <array>
//
#include <SFML/Graphics.hpp>

class GlowComponent
{
public:
    static constexpr int MaxLayers = 32;

private:
    std::array<sf::CircleShape, MaxLayers> Halos;
    int LayerCount = 0;

    float GlowSpread = 1.0f;
    float GlowStep = 1.0f;
    float MaxAlpha = 25.0f;
    float MinSize = 1.0f;

public:
    void Configure(
        float InGlowSpread,
        float InGlowStep,
        float InMaxAlpha,
        float InMinSize)
    {
        GlowSpread = InGlowSpread;
        GlowStep = InGlowStep;
        MaxAlpha = InMaxAlpha;
        MinSize = InMinSize;
    }

    void BuildFromCircle(
        const sf::CircleShape& Source)
    {
        const float BaseRadius = Source.getRadius();
        if (BaseRadius <= MinSize)
        {
            LayerCount = 0;
            return;
        }

        const sf::Color BaseColor = Source.getFillColor();
        const float GlowExtent = BaseRadius * GlowSpread;

        LayerCount = std::min(
            static_cast<int>(GlowExtent / GlowStep),
            MaxLayers
        );

        for (int i = 0; i < LayerCount; ++i)
        {
            const float Radius =
                BaseRadius + (i + 1) * GlowStep;

            sf::CircleShape& Halo = Halos[i];
            Halo.setRadius(Radius);
            Halo.setOrigin(Radius, Radius);

            const float T =
                static_cast<float>(i) / LayerCount;

            const float Falloff =
                (1.0f - T) * (1.0f - T);

            sf::Color C = BaseColor;
            C.a = static_cast<sf::Uint8>(MaxAlpha * Falloff);

            Halo.setFillColor(C);
        }
    }

    void SetCenter(const sf::Vector2f& Center)
    {
        for (int i = 0; i < LayerCount; ++i)
        {
            Halos[i].setPosition(Center);
        }
    }

    void Draw(sf::RenderWindow& Window) const
    {
        static const sf::RenderStates Additive(sf::BlendAdd);
        for (int i = 0; i < LayerCount; ++i)
        {
            Window.draw(Halos[i], Additive);
        }
    }
};
