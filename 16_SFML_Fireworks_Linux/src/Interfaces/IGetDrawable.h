
#pragma once

class IGetDrawable
{
public:
    virtual ~IGetDrawable() = default;
    virtual const sf::Drawable& GetDrawable() const = 0;
};
