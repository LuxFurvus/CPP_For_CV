#pragma once

#include <memory>
//
#include <SFML/Graphics.hpp>
//
#include "Utility.h"


class WindowHolder
{
private:

    static inline std::unique_ptr<sf::RenderWindow> Instance = nullptr;

    static constexpr int FramerateLimit = 121;

    static constexpr char const* WindowTitle = "SFML Fireworks Project";

public:
    static void Initialize();

    static sf::RenderWindow& Get();
};
