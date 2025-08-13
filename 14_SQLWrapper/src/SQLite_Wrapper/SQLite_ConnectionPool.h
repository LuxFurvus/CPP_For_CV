#pragma once
#ifndef SQLITE_CONNECTIONPOOL_H
#define SQLITE_CONNECTIONPOOL_H

#include <mutex>
#include <unordered_map>
#include <memory>
////////
#include "SQLite_DbConnection.h"

///////////////////

class SQLite_ConnectionPool
{
    SQLite_ConnectionPool() = delete;
    SQLite_ConnectionPool(const SQLite_ConnectionPool&) = delete;
    SQLite_ConnectionPool& operator=(const SQLite_ConnectionPool&) = delete;
    SQLite_ConnectionPool(SQLite_ConnectionPool&&) = delete;
    SQLite_ConnectionPool& operator=(SQLite_ConnectionPool&&) = delete;

private:
    static inline std::unordered_map<std::string, std::shared_ptr<SQLite_DbConnection>> Connections;
    static inline std::mutex Mutex;

private:
    static std::shared_ptr<SQLite_DbConnection> TryGetCachedConnection(const std::string& DbPath);
    static std::shared_ptr<SQLite_DbConnection> RegisterNewConnection(const SQLite_ConnectionCreationKit& Kit);

public:
    static std::shared_ptr<SQLite_DbConnection> GetConnection(const SQLite_ConnectionCreationKit& Kit);
    static void CloseConnection(const std::string& DbPath);
    static void CloseAll();
    static bool IsConnectionOpen(const std::string& DbPath);
    static std::vector<std::string> GetOpenConnectionPaths();
};


#endif
