#pragma once

#include <cmath>
#include <numbers>
#include <optional>
//
#include <SFML/Graphics.hpp>
//
#include "../Basics/FrameTimer.h"
#include "../Basics/Counters.h"
#include "../Basics/GameState.h"
#include "../UI/StatsOverlay.h"
#include "../Projectiles/GlowComponent.h"
#include "../Projectiles/MovementComponent.h"
#include "../Projectiles/AfterglowTrailComponent.h"
#include "../Interfaces/IUpdatable.h"
#include "../Interfaces/IDrawable.h"


struct Projectile
{
    sf::CircleShape Shape;

    GlowComponent Glow;
    MovementComponent Movement;
    AfterglowTrailComponent Afterglow;

    float PixelsPerSecond = 300.0f;
    float MaxLifetimeSeconds = 3.0f;
    float ElapsedLifetimeSeconds = 0.0f;
    int Generation = 0;
    bool IsActive = false;
};


class UProjectileController : public IUpdatable, public IDrawable
{
private:

    static constexpr int ChildCount = 10;
    static constexpr int MaxParticleCount = 5000;
    static constexpr float GlowSpread = 5.5f; // >1 = wider glow
    static constexpr float MaxGlowAlpha = 25.0f; // lower peak
    static constexpr float MinSizeForGlow = 1.0f;
    static constexpr float GlowStep = 1.0f; // px per layer

    std::array<Projectile, MaxParticleCount> Pool;
    std::array<int, MaxParticleCount> ActiveIndices;
    std::array<int, MaxParticleCount> FreeIndices;

    int ActiveCount = 0;
    int FreeCount = MaxParticleCount;

    const sf::View& InView;

    UIncrementCounter& FireworksFiredCounter;

    std::array<sf::Vector2f, ChildCount> Directions;

private:

    void InitDirections()
    {
        for (int i = 0; i < ChildCount; ++i)
        {
            constexpr float AngleFactor =
                2.0f * std::numbers::pi / static_cast<float>(ChildCount);
            const float Angle =
                AngleFactor * i;
            Directions[i] = Utility::UnitVectorFromAngle(Angle);
        }
    }

    bool IsProjectileExpired(const Projectile& P) const
    {
        return P.ElapsedLifetimeSeconds >= P.MaxLifetimeSeconds;
    }

    sf::Vector2f GetCenter(const Projectile& P) const
    {
        const sf::Vector2f Center = P.Shape.getPosition()
            + sf::Vector2f(P.Shape.getRadius(), P.Shape.getRadius());
        return Center;
    }

    void ApplyEffectsToProjectile(Projectile& InProjectile)
    {
        InProjectile.Glow.Configure(
            GlowSpread,
            GlowStep,
            MaxGlowAlpha,
            MinSizeForGlow
        );

        InProjectile.Glow.BuildFromCircle(InProjectile.Shape);

        if (InProjectile.Generation <= 1)
        {
            InProjectile.Afterglow.Configure(
                1.0f,    // fade duration
                0.03f    // spawn rate
            );
        }
        else
        {
            InProjectile.Afterglow.Clear();
        }
    }

    void SpawnChildProjectile(
        const Projectile& Parent,
        const int DirectionIndex)
    {
        const float Radius =
            Parent.Shape.getRadius() * 0.5f;

        if (Radius <= 0.6f || Parent.Generation > 2)
        {
            return;
        }

        Projectile* Child = AcquireProjectile();
        if (!Child) return;

        const float Lifetime =
            Parent.MaxLifetimeSeconds <= 0.9f
            ? 7.5f
            : Parent.MaxLifetimeSeconds * 0.5f;

        const sf::Vector2f Center =
            GetCenter(Parent);

        const sf::Vector2f Direction =
            Directions[DirectionIndex];

        Child->PixelsPerSecond = Parent.PixelsPerSecond;
        Child->MaxLifetimeSeconds = Lifetime;
        Child->ElapsedLifetimeSeconds = 0.0f;
        Child->Generation = Parent.Generation + 1;

        Child->Movement.SetVelocity(
            Direction * Parent.PixelsPerSecond
        );
        Child->Movement.SetGravity({ 0.0f, 120.0f });
        Child->Movement.SetLinearDrag(0.4f);

        Child->Shape.setRadius(Radius);
        Child->Shape.setFillColor(Parent.Shape.getFillColor());
        Child->Shape.setPosition(
            Center.x - Radius,
            Center.y - Radius
        );

        ApplyEffectsToProjectile(*Child);
    }

    Projectile* AcquireProjectile()
    {
        if (FreeCount == 0)
        {
            return nullptr;
        }

        const int Index = FreeIndices[--FreeCount];

        Projectile& P = Pool[Index];
        P.IsActive = true;
        P.ElapsedLifetimeSeconds = 0.0f;

        ActiveIndices[ActiveCount++] = Index;
        return &P;
    }

    void ReleaseProjectile(const int Index)
    {
        Projectile& P = Pool[Index];
        P.IsActive = false;

        FreeIndices[FreeCount++] = Index;
    }

public:

    UProjectileController(const sf::View& InView, UIncrementCounter& InCounter)
        :
        ActiveCount(0),
        FreeCount(MaxParticleCount),
        InView(InView),
        FireworksFiredCounter(InCounter)
    {
        for (int i = 0; i < MaxParticleCount; ++i)
        {
            FreeIndices[i] = i;
        }

        InitDirections();
    }

    void Update() override
    {
        if (GameState::IsPaused())
        {
            return;
        }

        const float DeltaTime = GFrameTimer::GetDeltaTime();

        const float ScreenBottom =
            InView.getCenter().y + InView.getSize().y * 0.5f;

        for (int i = 0; i < ActiveCount;)
        {
            const int Index = ActiveIndices[i];
            Projectile& P = Pool[Index];

            const bool bIsChildBelowScreen =
                P.Generation > 0 &&
                P.Shape.getPosition().y > ScreenBottom;

            if (bIsChildBelowScreen)
            {
                ReleaseProjectile(Index);
                ActiveIndices[i] = ActiveIndices[--ActiveCount];
                continue;
            }

            P.ElapsedLifetimeSeconds += DeltaTime;

            if (IsProjectileExpired(P))
            {
                for (int d = 0; d < ChildCount; ++d)
                {
                    SpawnChildProjectile(P, d);
                }

                ReleaseProjectile(Index);
                ActiveIndices[i] = ActiveIndices[--ActiveCount];
                continue;
            }

            sf::Vector2f Position = P.Shape.getPosition();
            P.Movement.UpdatePosition(Position, DeltaTime);
            P.Shape.setPosition(Position);

            P.Glow.SetCenter(GetCenter(P));

            if (P.Generation <= 1)
            {
                P.Afterglow.Update(P.Shape, DeltaTime);
            }

            ++i;
        }
    }

    void Draw() const override
    {
        sf::RenderWindow& OwningWindow = WindowHolder::Get();

        for (int i = 0; i < ActiveCount; ++i)
        {
            const Projectile& P = Pool[ActiveIndices[i]];

            if (P.Generation <= 1)
            {
                P.Afterglow.Draw(OwningWindow);
            }

            P.Glow.Draw(OwningWindow);
            OwningWindow.draw(P.Shape);
        }
    }

    uint32_t GetActiveParticlesNum() const
    {
        return static_cast<uint32_t>(ActiveCount);
    }

    static sf::Vector2f GetRandomScreenButtomPosition(
        const sf::View& InView,
        const float InRadius)
    {
        const float RandomOffsetXBase =
            InView.getSize().x * 0.25f;
        const float RandomOffsetX =
            Utility::GetRandomFloatInRange(-RandomOffsetXBase, RandomOffsetXBase);

        const float X =
            InView.getCenter().x - InRadius + RandomOffsetX;
        const float Y =
            InView.getSize().y - InRadius * 2.0f - 10.0f;

        return { X, Y };
    }

    static sf::Vector2f GetRadiusAddjustedScreenPosition(
        const sf::Vector2f& WorldPosition,
        const float InRadius
    )
    {
        const sf::Vector2f Result {
            WorldPosition.x + InRadius,
            WorldPosition.y + InRadius
        };
        return Result;
    }

    void SpawnFireworksAt(
        const sf::Vector2f& InPosition,
        const float InRadius,
        const float InSpeed,
        const float InLifetime,
        const sf::Color& InColor)
    {
        if (GameState::IsPaused())
        {
            return;
        }

        Projectile* P = AcquireProjectile();
        if (!P) return;

        P->PixelsPerSecond = InSpeed;
        P->MaxLifetimeSeconds = InLifetime;
        P->ElapsedLifetimeSeconds = 0.0f;
        P->Generation = 0;

        const float RandomVelocityOffset
            = Utility::GetRandomFloatInRange(-0.5f, 0.5f);

        P->Movement.SetVelocity({ InSpeed * RandomVelocityOffset, -InSpeed });
        P->Movement.SetGravity({ 0.0f, 120.0f });
        P->Movement.SetLinearDrag(0.1f);

        P->Shape.setRadius(InRadius);
        P->Shape.setFillColor(InColor);
        P->Shape.setPosition(InPosition.x, InPosition.y);

        ApplyEffectsToProjectile(*P);
        FireworksFiredCounter.Update();
    }

    void SpawnRandomExplosionAt(
        const sf::Vector2f& WorldPosition)
    {
        const sf::Color InColor = Utility::GetRandomColor();

        const bool bIsSingleProjectile = GameState::IsSingleProjectile();
        const bool bIsBigProjectiles = GameState::IsBigProjectiles();
        const bool bIsHighVelocity = GameState::IsHighVelocity();

        const float MaxRadius = bIsBigProjectiles ? 20.0f : 6.0f;
        const float MaxSpeed = bIsHighVelocity ? 500.0f : 150.0f;

        const float InRadius = Utility::GetRandomFloatInRange(3.0f, MaxRadius);
        const float InSpeed = Utility::GetRandomFloatInRange(100.0f, MaxSpeed);

        const float InLifetime
            = bIsSingleProjectile
            ? Utility::GetRandomFloatInRange(2.0f, 4.0f)
            : 0.0f;

        const sf::Vector2f WorldPositionAdjusted
            = GetRadiusAddjustedScreenPosition(WorldPosition, InRadius);

        SpawnFireworksAt(
            WorldPositionAdjusted,
            InRadius,
            InSpeed,
            InLifetime,
            InColor
        );
    }

    void SpawnRandomProjectileAtScreenBottom()
    {
        const float InRadius = Utility::GetRandomFloatInRange(3.0f, 8.0f);
        const float InSpeed = Utility::GetRandomFloatInRange(250.0f, 500.0f);
        const float InLifetime = Utility::GetRandomFloatInRange(2.0f, 4.0f);
        const sf::Color InColor = Utility::GetRandomColor();

        const sf::Vector2f ScreenButtomPosition
            = GetRandomScreenButtomPosition(InView, InRadius);

        SpawnFireworksAt(
            ScreenButtomPosition,
            InRadius,
            InSpeed,
            InLifetime,
            InColor
        );
    }

};
