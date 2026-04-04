#include "UActiveView.h"

UActiveView::UActiveView()
{
    const auto [WindowWidth, WindowHeight]
        = Utility::BuildHalfScreenWindow();

    TargetView = sf::View{
        sf::FloatRect{
            0.0f,
            0.0f,
            WindowWidth,
            WindowHeight
        }
    };

    TargetView.setCenter(
        static_cast<float>(WindowWidth) * 0.5f,
        static_cast<float>(WindowHeight) * 0.5f
    );
}

sf::View& UActiveView::GetView()
{
    return TargetView;
}

void UActiveView::Resize()
{
    const sf::RenderWindow& OwningWindow = WindowHolder::Get();

    const float BaseWidth  = OwningWindow.getSize().x;
    const float BaseHeight = OwningWindow.getSize().y;

    const float BaseRatio
        = static_cast<float>(BaseWidth)
        / static_cast<float>(BaseHeight);

    TargetView.setSize(
        BaseWidth,
        BaseWidth / BaseRatio
    );

    TargetView.setCenter(
        BaseWidth * 0.5f,
        BaseHeight * 0.5f
    );
}

void UActiveView::AddDrawableElements(
    const std::array<const sf::Drawable*, 3>& InDrawables)
{
    Drawables = InDrawables;
}

void UActiveView::Draw() const
{
    sf::RenderWindow& OwningWindow = WindowHolder::Get();

    OwningWindow.setView(TargetView);

    for (const sf::Drawable* DrawableObj : Drawables)
    {
        if (!DrawableObj)
        {
            continue;
        }

        OwningWindow.draw(*DrawableObj);
    }
}
