
#pragma once

class IResizable
{
public:
    virtual void Resize() = 0;
    virtual ~IResizable() = default;
};
