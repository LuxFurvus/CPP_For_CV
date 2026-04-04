#pragma once

#include <cstdint>
//
#include "../Basics/FrameTimer.h"
#include "../Delegates.h"
#include "../Interfaces/IUpdatable.h"

class UFpsCounter final : public IUpdatable
{
private:

    static constexpr float UpdateInterval = 0.5f;

    float AccumulatedTime = 0.0f;

    uint32_t CurrentFPS = 0;

    uint32_t FrameCounter = 0;

public:

    void Update() override;

    uint32_t GetValue() const
    {
        return CurrentFPS;
    }
};


class UDelegateCounter final : public IUpdatable
{
private:

    uint32_t ValueCounted = 0;

public:

    Delegate<uint32_t()> QueryValue;

public:

    void Update() override
    {
        if (!QueryValue) return;
        ValueCounted = QueryValue();
    }

    uint32_t GetValue() const
    {
        return ValueCounted;
    }
};


class UIncrementCounter final : public IUpdatable
{
private:
    uint64_t ValueCounted = 0;

public:
    void Update() override
    {
        ++ValueCounted;
    }

    uint64_t GetValue() const
    {
        return ValueCounted;
    }
};