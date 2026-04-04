
#pragma once

class GameState
{
private:
    static inline bool Paused = false;

    static inline bool SingleProjectile = false;

    static inline bool BigProjectiles = false;

    static inline bool HighVelocity = false;

public:
    static bool IsPaused()
    {
        return Paused;
    }

    static void Pause()
    {
        Paused = true;
    }

    static void Resume()
    {
        Paused = false;
    }

    static void SetSingleProjectile(const bool bEnabled)
    {
        SingleProjectile = bEnabled;
    }

    static void SetBigProjectiles(const bool bEnabled)
    {
        BigProjectiles = bEnabled;
    }

    static void SetHighVelocity(const bool bEnabled)
    {
        HighVelocity = bEnabled;
    }

    static bool IsSingleProjectile()
    {
        return SingleProjectile;
    }

    static bool IsBigProjectiles()
    {
        return BigProjectiles;
    }

    static bool IsHighVelocity()
    {
        return HighVelocity;
    }
};
