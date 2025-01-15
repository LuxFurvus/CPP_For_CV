
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
////////
#include <stdio.h>
#include <sstream>
////////
#include <sqlite3.h>
#include <tgbot/tgbot.h>

// Function to initialize the database and create table if not exists
void InitializeDatabase(sqlite3* &db) {
    int rc = sqlite3_open("events.db", &db);
    if (rc) {
        printf("Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }
    const char* createTableQuery = "CREATE TABLE IF NOT EXISTS events ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                   "details TEXT NOT NULL);";
    rc = sqlite3_exec(db, createTableQuery, 0, 0, 0);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Database initialized successfully.\n");
    }
}

// Function to add an event to the database
void AddEvent(sqlite3* db, const std::string& details) {
    const char* insertQuery = "INSERT INTO events (details) VALUES (?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, insertQuery, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, details.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Function to retrieve all events from the database
std::vector<std::string> GetEvents(sqlite3* db) {
    std::vector<std::string> events;
    const char* selectQuery = "SELECT id, details FROM events;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, selectQuery, -1, &stmt, 0);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* details = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::ostringstream oss;
        oss << id << ". " << details;
        events.push_back(oss.str());
    }
    sqlite3_finalize(stmt);
    return events;
}

// Function to delete a specific event by ID
void DeleteEvent(sqlite3* db, int eventId) {
    const char* deleteQuery = "DELETE FROM events WHERE id = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, deleteQuery, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, eventId);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Function to clear all events from the database
void ClearEvents(sqlite3* db) {
    const char* clearQuery = "DELETE FROM events;";
    sqlite3_exec(db, clearQuery, 0, 0, 0);
}

// Function to create the main inline keyboard menu
TgBot::InlineKeyboardMarkup::Ptr CreateMainMenu() {
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup());

    std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
    TgBot::InlineKeyboardButton::Ptr addEventButton(new TgBot::InlineKeyboardButton());
    addEventButton->text = "➕ Add Event";
    addEventButton->callbackData = "add_event";
    row1.push_back(addEventButton);

    TgBot::InlineKeyboardButton::Ptr viewEventsButton(new TgBot::InlineKeyboardButton());
    viewEventsButton->text = "📋 View Events";
    viewEventsButton->callbackData = "view_events";
    row1.push_back(viewEventsButton);

    std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
    TgBot::InlineKeyboardButton::Ptr clearEventsButton(new TgBot::InlineKeyboardButton());
    clearEventsButton->text = "🗑 Clear Events";
    clearEventsButton->callbackData = "clear_events";
    row2.push_back(clearEventsButton);

    keyboard->inlineKeyboard.push_back(row1);
    keyboard->inlineKeyboard.push_back(row2);

    return keyboard;
}

void BotLoop(const TgBot::Bot& bot)
{
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        printf("Error: %s\n", e.what());
    }
}

int main() {
    sqlite3* db;
    InitializeDatabase(db);

    TgBot::Bot bot("7587798634:AAFqGAZ_KrGnBQu21xoe2mBSMjRrvp72Mes");

    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id,
                                 "Welcome to the Event Planner Bot!\nUse the buttons below to manage events.",
                                 nullptr, 0, CreateMainMenu());
    });

    bot.getEvents().onCallbackQuery([&bot, &db](TgBot::CallbackQuery::Ptr query) {
        if (query->data == "add_event") {
            bot.getApi().sendMessage(query->message->chat->id, "Please type the event details:");
        } else if (query->data == "view_events") {
            std::vector<std::string> events = GetEvents(db);
            if (events.empty()) {
                bot.getApi().sendMessage(query->message->chat->id, "No events have been added yet.");
                return;
            }
            std::string response = "Planned Events:\n";
            for (const auto& event : events) {
                response += event + "\n";
            }
            bot.getApi().sendMessage(query->message->chat->id, response);
        } else if (query->data == "clear_events") {
            ClearEvents(db);
            bot.getApi().sendMessage(query->message->chat->id, "All events have been cleared.");
        }
    });

    bot.getEvents().onAnyMessage([&bot, &db](TgBot::Message::Ptr message) {
        if (message->text.empty()) {
            return;
        }

        // Assume any message after clicking "Add Event" is event details
        AddEvent(db, message->text);
        bot.getApi().sendMessage(message->chat->id, "Event added successfully!");
        bot.getApi().sendMessage(message->chat->id,
                                 "Use the buttons below to manage more events.",
                                 nullptr, 0, CreateMainMenu());
    });

    BotLoop(bot);

    sqlite3_close(db);
    return 0;
}
