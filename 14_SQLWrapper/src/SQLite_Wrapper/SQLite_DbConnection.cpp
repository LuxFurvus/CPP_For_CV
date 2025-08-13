// SQLite_DbConnection.cpp

#include "SQLite_DbConnection.h"
////////
#include <filesystem>
////////
#include <sqlite3.h>
////////
#include "../ConditionChecker/ConditionChecker.h"
////////
#include "SQLite_Statement.h"


///////////////////


SQLite_DbConnection::SQLite_DbConnection(const SQLite_ConnectionCreationKit& Kit)
    : DbHandle(nullptr), Sentinel(nullptr)
{
    SetupConnection(Kit);
}

//\/\/\/\/\/\/\/\/\/

void SQLite_DbConnection::SetupConnection(const SQLite_ConnectionCreationKit& Kit)
{
    VaidateDbPath(Kit);

    const int OpenFlags = Kit.IsReadOnly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE;
    const int AllFlags = OpenFlags | SQLITE_OPEN_EXRESCODE | SQLITE_OPEN_FULLMUTEX;
    const int OpenResult = sqlite3_open_v2(Kit.DbPath.c_str(), &DbHandle, AllFlags, nullptr);

    CheckOpenResult(OpenResult);

    if (!Kit.EnableSentinel) return;
    Sentinel = std::make_shared<SQLite_StatementSentinel>();
}

//\/\/\/\/\/\/\/\/\/

void SQLite_DbConnection::VaidateDbPath(const SQLite_ConnectionCreationKit& Kit)
{
    const std::string& Path = Kit.DbPath;

    // Skip file existence check for in-memory or URI-based databases
    if (Path.contains(":memory:") || Path.contains("file:")) return;

    const bool Exists = std::filesystem::exists(Path);
    CONFIRMS(Exists, "Database file does not exist: {}", Path);
}

//=================

void SQLite_DbConnection::CheckOpenResult(const int OpenResult)
{
    if (OpenResult == SQLITE_OK) return;
    CloseConnection();
    CONFIRMS(!OpenResult,
        "Error {}: [{}]", OpenResult, sqlite3_errmsg(DbHandle));
}

///////////////////

SQLite_DbConnection::~SQLite_DbConnection() noexcept
{
    CloseConnection();
}

//\/\/\/\/\/\/\/\/\/

void SQLite_DbConnection::CloseConnection() noexcept
{
    if (Sentinel) Sentinel->InvalidateAll();
    const int CloseResult = sqlite3_close(DbHandle);
    
    WARNIFFALSE(CloseResult == SQLITE_OK,
        "CloseResult == {}: [{}]", CloseResult, sqlite3_errmsg(DbHandle));
}

///////////////////

sqlite3* SQLite_DbConnection::Get() const
{
    return DbHandle;
}

///////////////////

bool SQLite_DbConnection::IsValid() const
{
    return static_cast<bool>(DbHandle);
}

///////////////////

SQLite_StatementCreationKit SQLite_DbConnection::MakeStatementKit() const
{
    return SQLite_StatementCreationKit{ Sentinel, DbHandle };
}
