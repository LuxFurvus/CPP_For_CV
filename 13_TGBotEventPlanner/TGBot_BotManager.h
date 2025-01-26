#pragma once

#include <thread>
////////
#include "TGBot_UiKeeper.h"
#include "TGBot_Utilities.h"
#include "TGBot_SQLiteDBManager.h"

class BotManager
{
private:
    // For handling user tasks in parallel
    boost::asio::thread_pool ThreadPool;

    // The unique_ptr ownership ensures no dangling pointer for the bot
    std::unique_ptr<TgBot::Bot> BotPtr;

    // This reference is for convenience; it references *BotPtr
    TgBot::Bot& BotObj;

    // Instance of the DB manager
    SQLiteDBManager DbHandle;

    // Protected state
    mutable std::mutex StateMutex;

    std::unordered_map<int64_t, EUserStates> UserStates;

    // Action map
    std::unordered_map<std::string, std::function<void(const std::shared_ptr<TgBot::Message>&)>> CallbackOptions;

private:

    bool IsGuest(const int64_t TelegramId, std::string& ErrMsg) const
    {
        ErrMsg.clear();
        // Negative TelegramIds considered invalid
        if (TelegramId <= 0)
        {
            ErrMsg = "Invalid Telegram ID provided!\n";
            return true;
        }

        // getChatMember can throw, so be ready for that
        try
        {
            const auto ChatMember = BotObj.getApi().getChatMember(GlobalSettings::ManagedGroup, TelegramId);

            // A user is treated as not a guest,
            // if their status in the group is anything other than "left" or "kicked".
            if (ChatMember->status != "left" && ChatMember->status != "kicked")
            {
                return false; // Not a guest if they are in the group
            }
        }
        catch (const TgBot::TgException& Exp)
        {
            ErrMsg = Exp.what();
            // If an error occurs, we treat them as a guest to avoid blocking
            return true;
        }
        return true;
    }

    EUserRole GetUserRole(const int64_t TelegramId, std::string& ErrMsg)
    {
        if (DbHandle.IsAdmin(TelegramId, ErrMsg))
        {
            return EUserRole::Admin;
        }
        if (IsGuest(TelegramId, ErrMsg))
        {
            return EUserRole::Guest;
        }
        return EUserRole::User;
    }

    // Update current user state in a thread-safe manner
    void SetUserState(const int64_t UserId, const EUserStates NewUserSate)
    {
        std::lock_guard<std::mutex> Lock(StateMutex);

        const auto Pair = UserStates.find(UserId);
        if (Pair == UserStates.end()) {
            UserStates.emplace(UserId, NewUserSate);
        } else {
            Pair->second = NewUserSate;
        }
    }

    // Capture current user state in a thread-safe manner
    EUserStates GetUserState(const int64_t UserId) const
    {
        std::lock_guard<std::mutex> Lock(StateMutex);

        const auto Pair = UserStates.find(UserId);

        if (Pair == UserStates.end())
        {
            return EUserStates::None;
        }

        return Pair->second;
    }

    void RefreshBot(const int64_t UserId)
    {
        // Reset the user state
        SetUserState(UserId, EUserStates::None);

        (void) BotObj.getApi().sendMessage(UserId, "Выберите действие:", nullptr, nullptr, UiKeeper::GetMainMenu());
    }

    EUserRole ProcessStatefulMessages(const std::string& MsgText, const int64_t UserId)
    {
        std::string ErrMsg;

        switch (GetUserState(UserId))
        {
        case EUserStates::AddGame:
        {
            const bool AddResult = DbHandle.AddGame(MsgText, ErrMsg);
            const std::string AddMsg = AddResult
                ? "Игра успешно добавлена!"
                : ErrMsg;
            (void) BotObj.getApi().sendMessage(UserId, AddMsg);
            return EUserRole::User;
        }
        case EUserStates::DeleteGame:
        {
            const bool DeleteResult = DbHandle.DeleteGame(MsgText, ErrMsg);
            const std::string DeleteMsg = DeleteResult
                ? "Игра успешно удалена!"
                : ErrMsg;
            (void) BotObj.getApi().sendMessage(UserId, DeleteMsg);
            return EUserRole::User;
        }
        case EUserStates::AdminAdd:
            ProcessMessageAdminAdd(MsgText, UserId);
            return EUserRole::Admin;
        case EUserStates::AdminRemove:
            ProcessMessageAdminRemove(MsgText, UserId);
            return EUserRole::Admin;
        case EUserStates::None:
            // Nothing to do
            return EUserRole::Guest;
        }
        return EUserRole::Guest;
    }

    void ProcessMessageAdminRemove(const std::string& MsgText, const int64_t UserId)
    {
        std::string ErrMsg;

        // Do nothing, if not an admin does this request
        if (GetUserRole(UserId, ErrMsg) != EUserRole::Admin)
        {
            RefreshBot(UserId);
            return;
        }

        const int64_t BanishedAdminId = TgBotUtilities::StringToInt64(MsgText, ErrMsg);

        if (BanishedAdminId == 0 || !ErrMsg.empty())
        {
            (void) BotObj.getApi().sendMessage(UserId, ErrMsg);
            return;
        }

        // We can rescind admin status only from users with admin status
        if (GetUserRole(BanishedAdminId, ErrMsg) != EUserRole::Admin)
        {
            (void) BotObj.getApi().sendMessage(UserId, "Only users with admin status can lost it!\n");
            return;
        }

        const bool RemoveAdminResult = DbHandle.RemoveAdmin(BanishedAdminId, ErrMsg);
        const std::string RemoveAdminMsg = RemoveAdminResult
            ? "Админ успешно исключен!"
            : ErrMsg;
        (void) BotObj.getApi().sendMessage(UserId, RemoveAdminMsg);
    }

    void ProcessMessageAdminAdd(const std::string& MsgText, const int64_t UserId)
    {
        std::string ErrMsg;

        // Do nothing, if not an admin does this request
        if (GetUserRole(UserId, ErrMsg) != EUserRole::Admin)
        {
            RefreshBot(UserId);
            return;
        }

        const int64_t NewAdminId = TgBotUtilities::StringToInt64(MsgText, ErrMsg);

        if (NewAdminId == 0 || !ErrMsg.empty())
        {
            (void) BotObj.getApi().sendMessage(UserId, ErrMsg);
            return;
        }

        // We can grant admin status only to users of ManagedGroup without admin status
        if (GetUserRole(NewAdminId, ErrMsg) != EUserRole::User)
        {
            std::ostringstream Oss;
            Oss << "Only users of " << GlobalSettings::ManagedGroup << " without admin status can be granted admin status!\n";
            (void) BotObj.getApi().sendMessage(UserId, Oss.str());
            return;
        }

        const bool AddAdminResult = DbHandle.AddAdmin(NewAdminId, ErrMsg);
        const std::string AddAdminMsg = AddAdminResult
            ? "Админ успешно добавлен!"
            : ErrMsg;
        (void) BotObj.getApi().sendMessage(UserId, AddAdminMsg);
    }

    // Callback for the "/admin" command
    std::function<void(std::shared_ptr<TgBot::Message>)> AdminListener =
        [this](const std::shared_ptr<TgBot::Message>& Message)
    {
        boost::asio::post(ThreadPool, [this, Message]
        {
            const int64_t UserId = Message->chat->id;
            SetUserState(UserId, EUserStates::None);

            std::string ErrMsg;
            if (GetUserRole(UserId, ErrMsg) != EUserRole::Admin)
            {
                RefreshBot(UserId);
                return;
            }

            (void) BotObj.getApi().sendMessage(UserId, "Подключаем Админ-режим...", nullptr, nullptr, UiKeeper::GetAdminMenu());
        });
    };

    // Called when any message is sent (that is not a command)
    const std::function<void(std::shared_ptr<TgBot::Message>)> MessageListener =
        [this](const std::shared_ptr<TgBot::Message>& Message)
    {
        boost::asio::post(ThreadPool, [this, Message]
        {
            const bool bIsMessageInvalid =
                !Message
                || Message->text.empty()
                || Message->text.at(0) == '/';

            if (bIsMessageInvalid) return;

            const std::string MsgText = Message->text;
            const int64_t UserId = Message->chat->id;

            // If user input matches a menu callback
            if (CallbackOptions.contains(MsgText))
            {
                SetUserState(UserId, EUserStates::None);
                CallbackOptions.at(MsgText)(Message);
                return;
            }

            // Refresh the menu
            switch (ProcessStatefulMessages(MsgText, UserId))
            {
                case EUserRole::Admin:
                    AdminListener(Message);
                    break;
                case EUserRole::User:
                case EUserRole::Guest:
                case EUserRole::None:
                    RefreshBot(UserId);
                    break;
            }
        });
    };

    // Fills the callback map with the appropriate actions
    void FillCallbackOptions()
    {
        CallbackOptions.emplace(UiKeeper::AddGame, [this](const std::shared_ptr<TgBot::Message>& Message)
        {
            const int64_t UserId = Message->chat->id;
            //printf("User__Id = %ld", UserId);
            SetUserState(UserId, EUserStates::AddGame);
            (void) BotObj.getApi().sendMessage(UserId, "Введите данные игры:", nullptr, nullptr, UiKeeper::GetMenuRemover());
        });

        CallbackOptions.emplace(UiKeeper::ViewGames, [this](const std::shared_ptr<TgBot::Message>& Message)
        {
            const int64_t UserId = Message->chat->id;
            SetUserState(UserId, EUserStates::None);

            std::string ErrMsg;
            std::vector<std::string> Games = DbHandle.GetGames(ErrMsg);
            if (!ErrMsg.empty())
            {
                (void) BotObj.getApi().sendMessage(UserId, ErrMsg);
                return;
            }
            if (Games.empty())
            {
                (void) BotObj.getApi().sendMessage(UserId, "Пока нет игр в расписании...");
                return;
            }

            std::string Response("Расписание игр:\n\n");
            for (const std::string& Game : Games)
            {
                Response.append(Game).append("\n");
            }
            (void) BotObj.getApi().sendMessage(UserId, Response);
        });

        CallbackOptions.emplace(UiKeeper::DeleteGame, [this](const std::shared_ptr<TgBot::Message>& Message)
        {
            const int64_t UserId = Message->chat->id;
            SetUserState(UserId, EUserStates::DeleteGame);
            (void) BotObj.getApi().sendMessage(UserId, "Укажите ID игры для удаления:", nullptr, nullptr, UiKeeper::GetMenuRemover());
        });

        CallbackOptions.emplace(UiKeeper::AdminClose, [this](const std::shared_ptr<TgBot::Message>& Message)
        {
            const int64_t UserId = Message->chat->id;
            SetUserState(UserId, EUserStates::None);
            RefreshBot(UserId);
        });

        CallbackOptions.emplace(UiKeeper::AdminAdd, [this](const std::shared_ptr<TgBot::Message>& Message)
        {
            const int64_t UserId = Message->chat->id;

            std::string ErrMsg;
            // Do nothing, if not an admin does this request
            if (GetUserRole(UserId, ErrMsg) != EUserRole::Admin)
            {
                if (!ErrMsg.empty()) (void) BotObj.getApi().sendMessage(UserId, ErrMsg);
                RefreshBot(UserId);
                return;
            }

            SetUserState(UserId, EUserStates::AdminAdd);
            (void) BotObj.getApi().sendMessage(UserId, "Укажите ID пользователя, чтобы даровать ему права администратора:", nullptr, nullptr, UiKeeper::GetMenuRemover());
        });

        CallbackOptions.emplace(UiKeeper::AdminRemove, [this](const std::shared_ptr<TgBot::Message>& Message)
        {
            const int64_t UserId = Message->chat->id;
            std::string ErrMsg;
            // Do nothing, if not an admin does this request
            if (GetUserRole(UserId, ErrMsg) != EUserRole::Admin)
            {
                if (!ErrMsg.empty()) (void) BotObj.getApi().sendMessage(UserId, ErrMsg);
                RefreshBot(UserId);
                return;
            }

            const auto AdminViewIter = CallbackOptions.find(UiKeeper::AdminView);
            if (AdminViewIter != CallbackOptions.end())
            {
                AdminViewIter->second(Message);
            }

            SetUserState(UserId, EUserStates::AdminRemove);
            (void) BotObj.getApi().sendMessage(UserId, "Укажите ID пользователя, чтобы лишить его прав администратора:", nullptr, nullptr, UiKeeper::GetMenuRemover());
        });

        CallbackOptions.emplace(UiKeeper::AdminView, [this](const std::shared_ptr<TgBot::Message>& Message)
        {
            const int64_t UserId = Message->chat->id;
            SetUserState(UserId, EUserStates::None);

            std::string ErrMsg;
            std::vector<int64_t> DbAdminIds = DbHandle.GetAdminList(ErrMsg);

            // Attempt to get Telegram group admins
            std::vector<int64_t> AllAdminIds;
            try
            {
                const auto Admins = BotObj.getApi().getChatAdministrators(GlobalSettings::ManagedGroup);
                for (const std::shared_ptr<TgBot::ChatMember>& Admin : Admins)
                {
                    if (Admin->user->id == 0) continue;
                    AllAdminIds.push_back(Admin->user->id);
                }
            }
            catch (const TgBot::TgException& Exp)
            {
                ErrMsg.append("\n").append(Exp.what());
            }

            // Combine DB admins with group admins
            AllAdminIds.insert(AllAdminIds.end(), DbAdminIds.begin(), DbAdminIds.end());

            if (AllAdminIds.empty())
            {
                (void) BotObj.getApi().sendMessage(UserId, ErrMsg.append("\nНе найдены администраторы у группы ").append(GlobalSettings::ManagedGroup));
            }

            std::string AllAdminIdsMsg("Администраторы бота:\n\n");
            for (const int64_t AdminId : AllAdminIds)
            {
                const std::string OneAdminNick = GetTelegramNick(AdminId, ErrMsg);
                if (!OneAdminNick.empty())
                {
                    AllAdminIdsMsg
                    .append(OneAdminNick)
                    .append(" => ")
                    .append(TgBotUtilities::ToString(AdminId))
                    .append("\n");
                }
                if (!ErrMsg.empty())
                {
                    (void) BotObj.getApi().sendMessage(UserId, ErrMsg, nullptr);
                }
            }
            (void) BotObj.getApi().sendMessage(UserId, AllAdminIdsMsg, nullptr);
        });
    }

    std::string GetTelegramNick(int64_t UserId, std::string& ErrMsg) const
    {
        ErrMsg.clear();
        try
        {
            const std::shared_ptr<TgBot::Chat> Chat = BotObj.getApi().getChat(UserId);
            if (!Chat || Chat->username.empty()) return TgBotUtilities::GetUserLink(UserId);

            // Return @name, if it exists
            return std::string("@").append(Chat->username);
        }
        catch (const TgBot::TgException& Exp)
        {
            ErrMsg = Exp.what();
            return TgBotUtilities::GetUserLink(UserId); // Could not retrieve username
        }
    }

public:

    explicit BotManager()
        : ThreadPool {std::clamp(std::thread::hardware_concurrency(), 1u, 16u)}
        , BotPtr {std::make_unique<TgBot::Bot>(GlobalSettings::BotToken)}
        , BotObj {*BotPtr}
    {
        FillCallbackOptions();
        BotObj.getEvents().onCommand("admin", AdminListener);
        BotObj.getEvents().onAnyMessage(MessageListener);
    }

    ~BotManager()
    {
        // Wait for all pending tasks to finish
        ThreadPool.join();
    }

    BotManager(const BotManager& Man) = delete;
    BotManager(BotManager&& Man) noexcept = delete;
    BotManager operator=(const BotManager&) const = delete;
    BotManager operator=(BotManager&& Man) const = delete;

    void BotLoop() const
    {
        // Start a task in the thread pool to run the long poll
        try
        {
            printf("Bot username: %s\n", BotObj.getApi().getMe()->username.c_str());

            constexpr int32_t Limit = 100;
            constexpr int32_t Timeout = 1;
            const auto AllowUpdates = std::make_shared<std::vector<std::string>>(std::initializer_list<std::string>{"message", "poll_answer"});

            TgBot::TgLongPoll LongPoll(BotObj, Limit, Timeout, AllowUpdates);
            while (true)
            {
                printf("Long poll started\n");
                LongPoll.start();
            }
        }
        catch (TgBot::TgException& TgException)
        {
            printf("Error: %s\n", TgException.what());
        }
    }
};


