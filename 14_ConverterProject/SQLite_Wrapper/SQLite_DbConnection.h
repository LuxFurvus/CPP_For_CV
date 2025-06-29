#pragma once
#ifndef SQLITE_DB_CONNECTION_H
#define SQLITE_DB_CONNECTION_H

#include <memory>
////////


struct sqlite3;
////////
class SQLite_StatementSentinel;
struct SQLite_StatementCreationKit;


///////////////////////////////


struct SQLite_ConnectionCreationKit
{
    std::string DbPath;
    bool IsReadOnly = false;
    bool EnableSentinel = true;
};


///////////////////////////////


class SQLite_DbConnection
{
    #include "SQLite_TestFriends.inl"

    SQLite_DbConnection() = delete;
    SQLite_DbConnection(const SQLite_DbConnection&) = delete;
    SQLite_DbConnection& operator=(const SQLite_DbConnection&) = delete;

    SQLite_DbConnection(SQLite_DbConnection&&) = delete;
    SQLite_DbConnection& operator=(SQLite_DbConnection&&) = delete;

private:
    sqlite3* DbHandle = nullptr;
    std::shared_ptr<SQLite_StatementSentinel> Sentinel;

private:
    void SetupConnection(const SQLite_ConnectionCreationKit& Kit);

    void CheckOpenResult(const int OpenResult);

    void CloseConnection() noexcept;

    void VaidateDbPath(const SQLite_ConnectionCreationKit& Kit);

public:
    SQLite_DbConnection(const SQLite_ConnectionCreationKit& Kit);
    ~SQLite_DbConnection() noexcept;

    sqlite3* Get() const;
    bool IsValid() const;
    SQLite_StatementCreationKit MakeStatementKit() const;
};


#endif // SQLITE_DB_CONNECTION_H
