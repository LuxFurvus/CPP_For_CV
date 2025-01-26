
// cd ~/tgbot

// g++ telegram_bot.cpp -o telegram_bot --std=c++20 -I/usr/local/include -lTgBot -lboost_system -lssl -lcrypto -lpthread -lsqlite3

// ./telegram_bot

////////
#include "TGBot_BotManager.h"
////////

int main()
{
    const BotManager GamePlannerBot;
    GamePlannerBot.BotLoop();
}
