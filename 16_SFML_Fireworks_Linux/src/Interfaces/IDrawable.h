
#pragma once

struct IDrawable
{
    virtual ~IDrawable() = default;
    virtual void Draw() const = 0;
};
