
#pragma once

#include <vector>
//
#include <SFML/Graphics.hpp>
//
#include "../Interfaces/IResizable.h"
#include "../Interfaces/ILayoutItem.h"

class UiVerticalLayout final : public IResizable
{
private:

    const sf::View& View;

    std::vector<ILayoutItem*> Items;

    float Spacing = 12.0f;

public:

    explicit UiVerticalLayout(
        const sf::View& InView,
        const float InSpacing)
        :
        View(InView),
        Spacing(InSpacing)
    {
    }

    void SetItems(const std::vector<ILayoutItem*>& InItems)
    {
        Items = InItems;
    }

    void Resize() override
    {
        if (Items.empty()) return;

        float TotalHeight = 0.0f;

        for (const ILayoutItem* Item : Items)
        {
            TotalHeight += Item->GetSize().y;
        }

        TotalHeight += Spacing * (Items.size() - 1);

        const sf::Vector2f Center = View.getCenter();
        float CurrentY = Center.y - TotalHeight * 0.5f;

        for (ILayoutItem* Item : Items)
        {
            const sf::Vector2f Size = Item->GetSize();

            Item->SetPosition({
                Center.x,
                CurrentY + Size.y * 0.5f
            });

            CurrentY += Size.y + Spacing;
        }
    }
};

