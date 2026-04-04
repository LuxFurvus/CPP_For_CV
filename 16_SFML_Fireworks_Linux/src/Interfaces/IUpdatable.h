
#pragma once

struct IUpdatable
{
    virtual ~IUpdatable() = default;
    virtual void Update() = 0;
};
