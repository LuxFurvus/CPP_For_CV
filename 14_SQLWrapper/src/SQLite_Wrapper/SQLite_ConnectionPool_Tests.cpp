
#include <mutex>
#include <unordered_set>
////////
#include <gtest/gtest.h>
#include <sqlite3.h>
////////
#include "SQLite_ConnectionPool.h"

// ================================
// ░ Test Case Descriptions ░
// ================================

// T01: returns the same connection when called multiple times with the same path
// T02: returns different connections for different paths
// T03: removes only the specified connection from the pool
// T04: clears the entire connection pool
// T05: does nothing and throws nothing when closing a non-existent connection
// T06: throws std::runtime_error when called with an empty DbPath (invalid)
// T07: in concurrent threads, returns the same shared_ptr for the same DbPath
// T08: in concurrent threads, returns different shared_ptrs for different DbPaths
// T09: safely handles interleaved GetConnection and CloseConnection without crashes


///////////////////////////////////////
// T01: returns the same connection when called multiple times with the same path
///////////////////////////////////////

TEST(SQLite_ConnectionPool, T01_SamePointerReturnedForSamePath)
{
    SCOPED_TRACE("T01: returns the same connection when called multiple times with the same path");

    const std::string DbPath = ":memory:";

    auto Conn1 = SQLite_ConnectionPool::GetConnection({ DbPath });
    auto Conn2 = SQLite_ConnectionPool::GetConnection({ DbPath });

    EXPECT_EQ(Conn1.get(), Conn2.get());
}

///////////////////////////////////////
// T02: returns different connections for different paths
///////////////////////////////////////

TEST(SQLite_ConnectionPool, T02_DifferentPathsYieldDifferentConnections)
{
    SCOPED_TRACE("T02: returns different connections for different paths");

    auto Conn1 = SQLite_ConnectionPool::GetConnection({ ":memory:" });
    auto Conn2 = SQLite_ConnectionPool::GetConnection({ "file:other?mode=memory&cache=shared" });

    EXPECT_NE(Conn1.get(), Conn2.get());
}

///////////////////////////////////////
// T03: removes only the specified connection from the pool
///////////////////////////////////////

TEST(SQLite_ConnectionPool, T03_ErasesSingleTargetEntry)
{
    SCOPED_TRACE("T03: removes only the specified connection from the pool");

    const std::string Path1 = ":memory:";
    const std::string Path2 = "file:another?mode=memory&cache=shared";

    SQLite_ConnectionPool::GetConnection({ Path1 });
    SQLite_ConnectionPool::GetConnection({ Path2 });

    SQLite_ConnectionPool::CloseConnection(Path1);

    EXPECT_FALSE(SQLite_ConnectionPool::IsConnectionOpen(Path1));
    EXPECT_TRUE(SQLite_ConnectionPool::IsConnectionOpen(Path2));
}

///////////////////////////////////////
// T04: clears the entire connection pool
///////////////////////////////////////

TEST(SQLite_ConnectionPool, T04_ClearsAllConnections)
{
    SCOPED_TRACE("T04: clears the entire connection pool");

    SQLite_ConnectionPool::GetConnection({ ":memory:" });
    SQLite_ConnectionPool::GetConnection({ "file:temp2?mode=memory&cache=shared" });

    SQLite_ConnectionPool::CloseAll();

    EXPECT_TRUE(SQLite_ConnectionPool::GetOpenConnectionPaths().empty());
}

///////////////////////////////////////
// T05: does nothing and throws nothing when closing a non-existent connection
///////////////////////////////////////

TEST(SQLite_ConnectionPool, T05_NoThrowOnUnknownPath)
{
    SCOPED_TRACE("T05: does nothing and throws nothing when closing a non-existent connection");

    EXPECT_NO_THROW(SQLite_ConnectionPool::CloseConnection("nonexistent.db"));
}

///////////////////////////////////////
// T06: throws std::runtime_error when called with an empty DbPath
///////////////////////////////////////

TEST(SQLite_ConnectionPool, T06_ThrowsOnEmptyPath)
{
    SCOPED_TRACE("T06: throws std::runtime_error when called with an empty DbPath");

    EXPECT_THROW(SQLite_ConnectionPool::GetConnection({ "" }), std::runtime_error);
}

///////////////////////////////////////
// T07: in concurrent threads, returns the same shared_ptr for the same DbPath
///////////////////////////////////////

TEST(SQLite_ConnectionPool, T07_SameConnectionInAllThreads)
{
    SCOPED_TRACE("T07: in concurrent threads, returns the same shared_ptr for the same DbPath");

    const std::string Path = ":memory:";
    constexpr int ThreadCount = 8;
    std::vector<std::shared_ptr<SQLite_DbConnection>> Results(ThreadCount);
    std::vector<std::thread> Threads;

    for (int i = 0; i < ThreadCount; ++i)
    {
        Threads.emplace_back([&, i]() {
            Results[i] = SQLite_ConnectionPool::GetConnection({ Path });
        });
    }

    for (auto& T : Threads) T.join();

    for (int i = 1; i < ThreadCount; ++i)
    {
        EXPECT_EQ(Results[0].get(), Results[i].get());
    }
}

///////////////////////////////////////
// T08: in concurrent threads, returns different shared_ptrs for different DbPaths
///////////////////////////////////////

TEST(SQLite_ConnectionPool, T08_DifferentConnectionsFromDifferentPaths)
{
    SCOPED_TRACE("T08: in concurrent threads, returns different shared_ptrs for different DbPaths");

    constexpr int ThreadCount = 8;
    std::vector<std::shared_ptr<SQLite_DbConnection>> Results(ThreadCount);
    std::vector<std::thread> Threads;

    for (int i = 0; i < ThreadCount; ++i)
    {
        Threads.emplace_back([&, i]() {
            std::string Path = "file:thread_" + std::to_string(i) + "?mode=memory&cache=shared";
            Results[i] = SQLite_ConnectionPool::GetConnection({ Path });
        });
    }

    for (auto& T : Threads) T.join();

    std::unordered_set<void*> UniquePtrs;
    for (const auto& Conn : Results)
    {
        UniquePtrs.insert(Conn.get());
    }

    EXPECT_EQ(UniquePtrs.size(), ThreadCount);
}

///////////////////////////////////////
// T09: safely handles interleaved GetConnection and CloseConnection without crashes
///////////////////////////////////////

TEST(SQLite_ConnectionPool, T09_InterleavedAccessSafe)
{
    SCOPED_TRACE("T09: safely handles interleaved GetConnection and CloseConnection without crashes");

    constexpr int Iterations = 100;
    const std::string Path = ":memory:";

    std::thread Creator([&]() {
        for (int i = 0; i < Iterations; ++i)
        {
            SQLite_ConnectionPool::GetConnection({ Path });
        }
    });

    std::thread Destroyer([&]() {
        for (int i = 0; i < Iterations; ++i)
        {
            SQLite_ConnectionPool::CloseConnection(Path);
        }
    });

    Creator.join();
    Destroyer.join();

    SUCCEED(); // No crash or exception is considered success
}
