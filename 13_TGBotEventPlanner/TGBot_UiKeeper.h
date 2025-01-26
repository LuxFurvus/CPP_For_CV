#pragma once

#include <tgbot/tgbot.h>

class UiKeeper
{
private:
    std::shared_ptr<TgBot::ReplyKeyboardMarkup> MainMenu = nullptr;
    std::shared_ptr<TgBot::ReplyKeyboardMarkup> AdminMenu = nullptr;
    std::shared_ptr<TgBot::ReplyKeyboardRemove> MenuRemover = nullptr;

    // Private constructor to initialize menus once
    UiKeeper()
    {
        MainMenu = CreateMainMenu();
        AdminMenu = CreateAdminMenu();
        MenuRemover = CreateMenuRemover();
    }

    // Helper function to create the main menu
    static std::shared_ptr<TgBot::ReplyKeyboardMarkup> CreateMainMenu()
    {
        auto Keyboard = std::make_shared<TgBot::ReplyKeyboardMarkup>();
        Keyboard->resizeKeyboard = true;

        static const std::vector<std::shared_ptr<TgBot::KeyboardButton>> Row1{
            std::make_shared<TgBot::KeyboardButton>(AddGame),
            std::make_shared<TgBot::KeyboardButton>(ViewGames)};

        static const std::vector<std::shared_ptr<TgBot::KeyboardButton>> Row2{
            std::make_shared<TgBot::KeyboardButton>(DeleteGame)};

        Keyboard->keyboard.push_back(Row1);
        Keyboard->keyboard.push_back(Row2);

        return Keyboard;
    }

    // Helper function to create the admin menu
    static std::shared_ptr<TgBot::ReplyKeyboardMarkup> CreateAdminMenu()
    {
        auto Keyboard = std::make_shared<TgBot::ReplyKeyboardMarkup>();
        Keyboard->resizeKeyboard = true;

        static const std::vector<std::shared_ptr<TgBot::KeyboardButton>> Row1
        {
            std::make_shared<TgBot::KeyboardButton>(AdminAdd),
            std::make_shared<TgBot::KeyboardButton>(AdminRemove),
        };

        static const std::vector<std::shared_ptr<TgBot::KeyboardButton>> Row2
        {
            std::make_shared<TgBot::KeyboardButton>(AdminView),
            std::make_shared<TgBot::KeyboardButton>(AdminClose),
        };

        Keyboard->keyboard.push_back(Row1);
        Keyboard->keyboard.push_back(Row2);

        return Keyboard;
    }

    // Helper function to create the menu remover
    static std::shared_ptr<TgBot::ReplyKeyboardRemove> CreateMenuRemover()
    {
        auto RemoveKeyboard = std::make_shared<TgBot::ReplyKeyboardRemove>();
        RemoveKeyboard->removeKeyboard = true;
        return RemoveKeyboard;
    }

    // Accessor for singleton instance
    static UiKeeper& GetInstance()
    {
        static UiKeeper Instance;
        return Instance;
    }

public:

    static inline const std::string AddGame { "➕ Добавить Игру" };
    static inline const std::string ViewGames { "📋 Посмотреть расписание игр" };
    static inline const std::string DeleteGame { "🗑 Удалить игру" };
    static inline const std::string AdminAdd { "🗓 Добавить нового Админа" };
    static inline const std::string AdminRemove { "🗓 Удалить Админа" };
    static inline const std::string AdminView { "🗓 Посмотреть список Админов" };
    static inline const std::string AdminClose { "🚪 Покинуть Админ-режим" };

public:

    // Rule of five methods to enforce singleton
    UiKeeper(const UiKeeper&) = delete;
    UiKeeper& operator=(const UiKeeper&) = delete;
    UiKeeper(UiKeeper&&) = delete;
    UiKeeper& operator=(UiKeeper&&) = delete;
    ~UiKeeper() = default;

    // Public static methods to access ready menus
    static std::shared_ptr<TgBot::ReplyKeyboardMarkup> GetMainMenu()
    {
        return GetInstance().MainMenu;
    }

    static std::shared_ptr<TgBot::ReplyKeyboardMarkup> GetAdminMenu()
    {
        return GetInstance().AdminMenu;
    }

    static std::shared_ptr<TgBot::ReplyKeyboardRemove> GetMenuRemover()
    {
        return GetInstance().MenuRemover;
    }
};
