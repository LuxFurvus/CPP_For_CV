#pragma once

#include <vector>
#include <sstream>
#include <cassert>
#include <sqlite3.h>
#include <bits/stl_algo.h>
////////

class IDbTableManager
{
    
};

class SQLiteDBManager
{
private:

    mutable std::mutex PoolMutex;
    std::vector<sqlite3*> ConnectionPool;
    std::size_t PoolSize;

    constexpr static int DbInitFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
    constexpr static const char* DatabasePath = "games.db";

private:

    static bool CreateGameTable(sqlite3* Conn)
    {
        const char* CreateGameTableQuery = 
            "CREATE TABLE IF NOT EXISTS games ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "details TEXT NOT NULL);";

        char* Err = nullptr;
        const int Resp = sqlite3_exec(Conn, CreateGameTableQuery, nullptr, nullptr, &Err);
        if (Resp != SQLITE_OK)
        {
            printf("SQL error on games creation: %s\n", (Err != nullptr) ? Err : "");
            sqlite3_free(Err);
            return false;
        }
        return true;
    }

    // Function to initialize the database and create table if not exists
    void InitializeDatabase() const
    {
        assert(!ConnectionPool.empty());
        sqlite3* Conn = ConnectionPool[0];

        // Enable WAL mode
        int Resp = sqlite3_exec(Conn, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
        assert(Resp == SQLITE_OK);

        Resp = sqlite3_exec(Conn, "PRAGMA wal_autocheckpoint=1000;", nullptr, nullptr, nullptr);
        assert(Resp == SQLITE_OK);

        // Make sure we compiled SQLite with -DSQLITE_THREADSAFE=1 (or 2), so FULLMUTEX actually works.
        printf("SQLite threading mode: %s\n", (sqlite3_threadsafe() == 0) ? "Disabled" : "Enabled");

        assert(CreateGameTable(Conn));
        printf("Database initialized successfully.\n");
    }

public:

    explicit SQLiteDBManager()
    : PoolSize(std::clamp(std::thread::hardware_concurrency(), 2u, 8u))
    {
        // Fill the pool
        for (std::size_t Idx = 0; Idx < PoolSize; ++Idx)
        {
            sqlite3* Conn = nullptr;

            if (sqlite3_open_v2(DatabasePath, &Conn, DbInitFlags, nullptr) == SQLITE_OK)
            {
                ConnectionPool.push_back(Conn);
            }
        }
        assert(!ConnectionPool.empty());

        InitializeDatabase();
    }

    ~SQLiteDBManager()
    {
        for (sqlite3* Conn : ConnectionPool) {
            if (Conn == nullptr) continue;
            sqlite3_close(Conn);
        }
    }

private:

    class SqlConnection
    {
    private:

        sqlite3* LocalDb = nullptr;
        SQLiteDBManager& DbManager;

    public:

        SqlConnection(const SqlConnection& Man) = delete;
        SqlConnection(SqlConnection&& Man) noexcept = delete;
        SqlConnection operator=(const SqlConnection&) const = delete;
        SqlConnection operator=(SqlConnection&& Man) const = delete;

        explicit SqlConnection(SQLiteDBManager& DbManagerParam) : DbManager(DbManagerParam)
        {
            std::lock_guard<std::mutex> Lock(DbManager.PoolMutex);

            // If there are free connections, return one of them
            if (!DbManager.ConnectionPool.empty())
            {
                LocalDb = DbManager.ConnectionPool.back();
                DbManager.ConnectionPool.pop_back();
                return;
            }

            // Instead of returning nullptr, you are trying to create a new connection
            sqlite3* NewConn = nullptr;
            // Try to create another connection if we haven't reached the max
            if (sqlite3_open_v2(SQLiteDBManager::DatabasePath, &NewConn, SQLiteDBManager::DbInitFlags,nullptr) == SQLITE_OK)
            {
                LocalDb = NewConn;
            }
        }

        ~SqlConnection()
        {
            if (LocalDb == nullptr) return;
            std::lock_guard<std::mutex> Lock(DbManager.PoolMutex);
            DbManager.ConnectionPool.push_back(LocalDb);
        }

        [[nodiscard]] sqlite3* Get() const
        {
            return LocalDb;
        }

    };

    class ScopedStatement {
    private:

        sqlite3_stmt* Stmt = nullptr;

    public:

        ScopedStatement(const ScopedStatement&) = delete;
        ScopedStatement(ScopedStatement&&) noexcept = delete;
        ScopedStatement operator=(const ScopedStatement&) const = delete;
        ScopedStatement operator=(ScopedStatement&&) const = delete;

        ScopedStatement(sqlite3* LocalDb, const char* Query, std::string& ErrMsg) {
            ErrMsg.clear();
            if (sqlite3_prepare_v2(LocalDb, Query, -1, &Stmt, nullptr) != SQLITE_OK) {
                ErrMsg = sqlite3_errmsg(LocalDb);
            }
        }

        ~ScopedStatement() {
            if (Stmt == nullptr) return;
            sqlite3_finalize(Stmt);
        }

        [[nodiscard]] sqlite3_stmt* Get() const { return Stmt; }
    };

    public:

    // Function to add an event to the database
    bool AddGame(const std::string& Details, std::string& ErrMsg)
    {
        const SqlConnection ConnectObj{*this};
        sqlite3* LocalDb = ConnectObj.Get();
        if (LocalDb == nullptr)
        {
            ErrMsg = std::string("AcquireConnection failed in ").append(__func__);
            return false;
        }

        const char* InsertQuery = "INSERT INTO games (Details) VALUES (?);";

        const ScopedStatement Stmt(LocalDb, InsertQuery, ErrMsg);
        if (!ErrMsg.empty()) return {};

        sqlite3_bind_text(Stmt.Get(), 1, Details.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(Stmt.Get()) != SQLITE_DONE)
        {
            ErrMsg = sqlite3_errmsg(LocalDb);
            return false;
        }

        ErrMsg.clear();

        return true;
    }

    // Function to retrieve all games from the database
    std::vector<std::string> GetGames(std::string& ErrMsg)
    {
        const SqlConnection ConnectObj{*this};
        sqlite3* LocalDb = ConnectObj.Get();
        if (LocalDb == nullptr)
        {
            ErrMsg = std::string("AcquireConnection failed in ").append(__func__);
            return {};
        }

        const char* SelectQuery = "SELECT id, details FROM games;";

        std::vector<std::string> Games;

        const ScopedStatement Stmt(LocalDb, SelectQuery, ErrMsg);
        if (!ErrMsg.empty()) return {};

        while (sqlite3_step(Stmt.Get()) == SQLITE_ROW)
        {
            const int GameId = sqlite3_column_int(Stmt.Get(), 0);
            const unsigned char* Details = sqlite3_column_text(Stmt.Get(), 1);
            std::ostringstream Oss;
            Oss << GameId << ". " << Details;
            Games.push_back(Oss.str());
        }

        ErrMsg.clear();

        return Games;
    }

    // Function to delete a specific event by ID
    bool DeleteGame(const std::string& IdToDelete, std::string& ErrMsg)
    {
        const SqlConnection ConnectObj{*this};
        sqlite3* LocalDb = ConnectObj.Get();
        if (LocalDb == nullptr)
        {
            ErrMsg = std::string("AcquireConnection failed in ").append(__func__);
            return {};
        }

        uint64_t GameId = 0;
        try
        {
            GameId = std::stoul(IdToDelete);
        }
        catch (const std::exception&)
        {
            ErrMsg = "Wrong Game Id! It must be a positive integer!";
            return false;
        }

        const char* DeleteQuery = "DELETE FROM games WHERE id = ?;";

        const ScopedStatement Stmt(LocalDb, DeleteQuery, ErrMsg);
        if (!ErrMsg.empty()) return {};

        sqlite3_bind_int(Stmt.Get(), 1, static_cast<int>(GameId));

        if (sqlite3_step(Stmt.Get()) != SQLITE_DONE)
        {
            ErrMsg = sqlite3_errmsg(LocalDb);
            return false;
        }

        ErrMsg.clear();

        return true;
    }

    // Function to clear all games and reset IDs in the database
    bool ClearAllGames(std::string& ErrMsg)
    {
        const SqlConnection ConnectObj{*this};
        sqlite3* LocalDb = ConnectObj.Get();
        if (LocalDb == nullptr)
        {
            ErrMsg = std::string("AcquireConnection failed in ").append(__func__);
            return {};
        }

        if (sqlite3_exec(LocalDb, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK)
        {
            ErrMsg = "Failed to begin transaction.";
            return false;
        }

        const char* ClearQuery = "DELETE FROM games;";
        if (sqlite3_exec(LocalDb, ClearQuery, nullptr, nullptr, nullptr) != SQLITE_OK)
        {
            ErrMsg = "Failed to clear games.";
            sqlite3_exec(LocalDb, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }

        const char* ResetIdQuery = "DELETE FROM sqlite_sequence WHERE name='games';";
        if (sqlite3_exec(LocalDb, ResetIdQuery, nullptr, nullptr, nullptr) != SQLITE_OK)
        {
            ErrMsg = "Failed to reset ID counter.";
            sqlite3_exec(LocalDb, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }

        if (sqlite3_exec(LocalDb, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK)
        {
            ErrMsg = "Failed to commit transaction.";
            return false;
        }

        ErrMsg.clear();
        return true;
    }

    bool CheckOrCreateAdminsTable(std::string& ErrMsg)
    {
        const SqlConnection ConnectObj{*this};
        sqlite3* LocalDb = ConnectObj.Get();
        if (LocalDb == nullptr)
        {
            ErrMsg = std::string("AcquireConnection failed in ").append(__func__);
            return false;
        }

        try
        {
            // Check if the table exists by querying sqlite_master
            const char* CheckTableQuery =
                "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='admins';";

            sqlite3_stmt* Statement = nullptr;
            int Resp = sqlite3_prepare_v2(LocalDb, CheckTableQuery, -1, &Statement, nullptr);
            if (Resp != SQLITE_OK)
            {
                ErrMsg = std::string("Failed to prepare statement: ").append(sqlite3_errmsg(LocalDb));
                return false;
            }

            // Execute the query to check if the table exists
            int TableExists = 0;
            if (sqlite3_step(Statement) == SQLITE_ROW)
            {
                TableExists = sqlite3_column_int(Statement, 0);
            }
            sqlite3_finalize(Statement);

            // If the table exists, return true
            if (TableExists > 0)
            {
                ErrMsg.clear();
                return true;
            }

            // If the table doesn't exist, create it
            const char* CreateTableQuery =
                "CREATE TABLE IF NOT EXISTS admins ("
                "telegram_id INTEGER PRIMARY KEY);";

            Resp = sqlite3_exec(LocalDb, CreateTableQuery, nullptr, nullptr, nullptr);
            if (Resp != SQLITE_OK)
            {
                ErrMsg = std::string("Failed to create table: ").append(sqlite3_errmsg(LocalDb));
                return false;
            }

            // Table creation succeeded
            ErrMsg.clear();
            return true;
        }
        catch (const std::exception& Ex)
        {
            ErrMsg = std::string("Exception in CreateAdminsTable: ").append(Ex.what());
            return false;
        }
    }

    bool AddAdmin(const int64_t TelegramId, std::string& ErrMsg)
    {
        const SqlConnection ConnectObj{*this};
        sqlite3* LocalDb = ConnectObj.Get();
        if (LocalDb == nullptr)
        {
            ErrMsg = std::string("AcquireConnection failed in ").append(__func__);
            return {};
        }

        // Return, if there is no 'admins' table, and we could not create it
        if (!CheckOrCreateAdminsTable(ErrMsg)) return {};

        const char* InsertAdminQuery = "INSERT OR IGNORE INTO admins (telegram_id) VALUES (?);";

        const ScopedStatement Stmt(LocalDb, InsertAdminQuery, ErrMsg);
        if (!ErrMsg.empty()) return {};

        sqlite3_bind_int64(Stmt.Get(), 1, TelegramId);

        if (sqlite3_step(Stmt.Get()) != SQLITE_DONE)
        {
            ErrMsg = "Failed to add admin.";
            return false;
        }

        ErrMsg.clear();
        return true;
    }

    bool RemoveAdmin(const int64_t TelegramId, std::string& ErrMsg)
    {
        const SqlConnection ConnectObj{*this};
        sqlite3* LocalDb = ConnectObj.Get();
        if (LocalDb == nullptr)
        {
            ErrMsg = std::string("AcquireConnection failed in ").append(__func__);
            return {};
        }

        // Return, if there is no 'admins' table, and we could not create it
        if (!CheckOrCreateAdminsTable(ErrMsg)) return {};

        const char* RemoveQuery = "DELETE FROM admins WHERE telegram_id = ?;";

        const ScopedStatement Stmt(LocalDb, RemoveQuery, ErrMsg);
        if (!ErrMsg.empty()) return {};

        sqlite3_bind_int64(Stmt.Get(), 1, TelegramId);

        if (sqlite3_step(Stmt.Get()) != SQLITE_DONE)
        {
            ErrMsg = "Failed to remove admin.";
            return false;
        }

        ErrMsg.clear();

        return true;
    }

    std::vector<int64_t> GetAdminList(std::string& ErrMsg)
    {
        const SqlConnection ConnectObj{*this};
        sqlite3* LocalDb = ConnectObj.Get();
        if (LocalDb == nullptr)
        {
            ErrMsg = std::string("AcquireConnection failed in ").append(__func__);
            return {};
        }

        const char* GetAdminQuery = "SELECT telegram_id FROM admins;";

        std::vector<int64_t> Admins;

        const ScopedStatement Stmt(LocalDb, GetAdminQuery, ErrMsg);
        if (!ErrMsg.empty()) return {};

        while (sqlite3_step(Stmt.Get()) == SQLITE_ROW)
        {
            const int64_t TelegramId = sqlite3_column_int64(Stmt.Get(), 0);
            Admins.push_back(TelegramId);
        }

        ErrMsg.clear();

        return Admins;
    }

    bool IsAdmin(const int64_t TelegramId, std::string& ErrMsg) 
    {
        const SqlConnection ConnectObj{*this};
        sqlite3* LocalDb = ConnectObj.Get();
        if (LocalDb == nullptr)
        {
            ErrMsg = std::string("AcquireConnection failed in ").append(__func__);
            return {};
        }

        const char* IsAdminQuery = "SELECT 1 FROM admins WHERE telegram_id = ? LIMIT 1;";

        const ScopedStatement Stmt(LocalDb, IsAdminQuery, ErrMsg);
        if (!ErrMsg.empty()) return {};

        sqlite3_bind_int64(Stmt.Get(), 1, TelegramId);

        const bool IsAdminUser = (sqlite3_step(Stmt.Get()) == SQLITE_ROW);

        return IsAdminUser;
    }
};
