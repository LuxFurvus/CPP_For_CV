
#pragma once

#include <SFML/System/Vector2.hpp>

struct IInputConsumer
{
    virtual ~IInputConsumer() = default;

    virtual bool ConsumeMouseRelease() = 0;
};
