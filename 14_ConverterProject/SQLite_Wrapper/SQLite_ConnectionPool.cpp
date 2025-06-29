
#include "SQLite_ConnectionPool.h"
////////
#include "../ConditionChecker/ConditionChecker.h"


std::shared_ptr<SQLite_DbConnection> SQLite_ConnectionPool::TryGetCachedConnection(const std::string& DbPath)
{
    const auto It = Connections.find(DbPath);
    if (It != Connections.end())
    {
        return It->second;
    }
    return nullptr;
}

std::shared_ptr<SQLite_DbConnection> SQLite_ConnectionPool::RegisterNewConnection(const SQLite_ConnectionCreationKit& Kit)
{
    std::shared_ptr<SQLite_DbConnection> NewConnection = std::make_shared<SQLite_DbConnection>(Kit);

    CONFIRMS(NewConnection,
        "Connection {} is not valid.", Kit.DbPath);

    Connections.emplace(Kit.DbPath, NewConnection);

    return NewConnection;
}

std::shared_ptr<SQLite_DbConnection> SQLite_ConnectionPool::GetConnection(const SQLite_ConnectionCreationKit& Kit)
{
    std::lock_guard<std::mutex> Lock(Mutex);

    if (std::shared_ptr<SQLite_DbConnection> Existing = TryGetCachedConnection(Kit.DbPath))
    {
        return Existing;
    }

    return RegisterNewConnection(Kit);
}



void SQLite_ConnectionPool::CloseConnection(const std::string& DbPath)
{
    std::lock_guard<std::mutex> Lock(Mutex);
    auto It = Connections.find(DbPath);
    if (It == Connections.end()) return;
    Connections.erase(It);
}

void SQLite_ConnectionPool::CloseAll()
{
    std::lock_guard<std::mutex> Lock(Mutex);
    Connections.clear();
}
