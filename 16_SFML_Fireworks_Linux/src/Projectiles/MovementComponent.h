#pragma once

#include <SFML/System/Vector2.hpp>

class MovementComponent
{
private:
    // Core kinematic state
    sf::Vector2f Velocity { 0.0f, 0.0f };
    sf::Vector2f Acceleration { 0.0f, 0.0f };

    // Physics parameters
    sf::Vector2f Gravity { 0.0f, 0.0f };
    float LinearDrag = 0.0f;   // 0 = no drag, typical: 0.1–2.0

public:

    void Reset()
    {
        Velocity = { 0.0f, 0.0f };
        Acceleration = { 0.0f, 0.0f };
        Gravity = { 0.0f, 0.0f };
        LinearDrag = 0.0f;
    }

    void SetVelocity(const sf::Vector2f& InVelocity)
    {
        Velocity = InVelocity;
    }

    void SetGravity(const sf::Vector2f& InGravity)
    {
        Gravity = InGravity;
    }

    void SetLinearDrag(float InDrag)
    {
        LinearDrag = InDrag;
    }

    const sf::Vector2f& GetVelocity() const
    {
        return Velocity;
    }

    void AddForce(const sf::Vector2f& Force)
    {
        Acceleration += Force;
    }

    void UpdatePosition(
        sf::Vector2f& InOutPosition,
        float DeltaTime)
    {
        // Apply gravity
        Acceleration += Gravity;

        // Integrate velocity
        Velocity += Acceleration * DeltaTime;

        // Apply linear drag (stable exponential decay)
        if (LinearDrag > 0.0f)
        {
            const float DragFactor =
                1.0f / (1.0f + LinearDrag * DeltaTime);

            Velocity *= DragFactor;
        }

        // Integrate position
        InOutPosition += Velocity * DeltaTime;

        // Clear forces (important!)
        Acceleration = { 0.0f, 0.0f };
    }
};
