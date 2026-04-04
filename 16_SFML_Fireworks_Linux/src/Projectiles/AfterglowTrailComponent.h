#pragma once

#include <array>
#include <SFML/Graphics.hpp>

class AfterglowTrailComponent
{
private:

    static constexpr int MaxTrailPoints = 64;

    struct TrailPoint
    {
        sf::CircleShape Shape;
        float Age = 0.0f;
        bool IsAlive = false;
    };

    std::array<TrailPoint, MaxTrailPoints> Points;

    int HeadIndex = 0;
    int ActiveCount = 0;

    float MaxLifetime = 1.0f;
    float SpawnInterval = 0.03f;
    float TimeAccumulator = 0.0f;

private:

    void SpawnPoint(const sf::CircleShape& SourceShape)
    {
        TrailPoint& P = Points[HeadIndex];

        P.Shape = SourceShape;
        P.Age = 0.0f;
        P.IsAlive = true;

        HeadIndex = (HeadIndex + 1) % MaxTrailPoints;

        if (ActiveCount < MaxTrailPoints)
        {
            ++ActiveCount;
        }
    }

public:

    void Configure(
        const float InLifetime,
        const float InSpawnInterval)
    {
        MaxLifetime = InLifetime;
        SpawnInterval = InSpawnInterval;
    }

    void Update(
        const sf::CircleShape& SourceShape,
        const float DeltaTime)
    {
        TimeAccumulator += DeltaTime;

        if (TimeAccumulator >= SpawnInterval)
        {
            TimeAccumulator -= SpawnInterval;
            SpawnPoint(SourceShape);
        }

        for (int i = 0; i < MaxTrailPoints; ++i)
        {
            TrailPoint& P = Points[i];
            if (!P.IsAlive) continue;

            P.Age += DeltaTime;

            const float Alpha =
                1.0f - (P.Age / MaxLifetime);

            if (Alpha <= 0.0f)
            {
                P.IsAlive = false;
                --ActiveCount;
                continue;
            }

            sf::Color C = P.Shape.getFillColor();
            C.a = static_cast<sf::Uint8>(255.0f * Alpha);
            P.Shape.setFillColor(C);
        }
    }

    void Draw(sf::RenderWindow& Window) const
    {
        for (const TrailPoint& P : Points)
        {
            if (!P.IsAlive) continue;
            Window.draw(P.Shape);
        }
    }

    void Clear()
    {
        for (TrailPoint& P : Points)
        {
            P.IsAlive = false;
            P.Age = 0.0f;
        }

        HeadIndex = 0;
        ActiveCount = 0;
        TimeAccumulator = 0.0f;
    }
};
