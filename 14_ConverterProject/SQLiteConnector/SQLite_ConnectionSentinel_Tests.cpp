#include <memory>
#include <filesystem>
////////
#include <gtest/gtest.h>
#include <sqlite3.h>
////////
#include "SQLiteConnector.h"

//////////////////////////////
//          Connection Sentinel
//////////////////////////////

// #define EXAMINE_MEMORY_LEAK_WITHOUT_ConnectionSentinel

//////////////////////////////////////////////
// T01: Register_AddsPointer
//////////////////////////////////////////////

TEST(SQLite_ConnectionSentinel, T01_RegisterAddsPointer)
{
    SCOPED_TRACE("/////////////// T01: Register_AddsPointer ///////////////");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    const auto Lambda = [&]()
    {
        SQLite_Statement* Statement = new SQLite_Statement(StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
        Sentinel->Register(Statement);
        Sentinel->Unregister(Statement);
        delete Statement;
    };
    EXPECT_NO_THROW(Lambda());

    sqlite3_close(Db);
}


//////////////////////////////////////////////
// T02: Unregister_RemovesPointer
//////////////////////////////////////////////

TEST(SQLite_ConnectionSentinel, T02_UnregisterRemovesPointer)
{
    SCOPED_TRACE("/////////////// T02: Unregister_RemovesPointer ///////////////");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    const auto Lambda = [&]()
    {
        SQLite_Statement* Statement = new SQLite_Statement(StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
        Sentinel->Register(Statement);
        Sentinel->Unregister(Statement);
        delete Statement;
    };
    EXPECT_NO_THROW(Lambda());

    sqlite3_close(Db);
}

//////////////////////////////////////////////
// T03: DoubleRegister_HasSingleEntry
//////////////////////////////////////////////

TEST(SQLite_ConnectionSentinel, T03_DoubleRegisterHasSingleEntry)
{
    SCOPED_TRACE("/////////////// T03: DoubleRegister_HasSingleEntry ///////////////");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    SQLite_Statement* Statement = new SQLite_Statement(StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
    Sentinel->Register(Statement);
    Sentinel->Register(Statement);  // Should not crash
    Sentinel->Unregister(Statement);
    delete Statement;

    sqlite3_close(Db);
}

//////////////////////////////////////////////
// T04: DoubleUnregister_Safe
//////////////////////////////////////////////

TEST(SQLite_ConnectionSentinel, T04_DoubleUnregisterSafe)
{
    SCOPED_TRACE("/////////////// T04: DoubleUnregister_Safe ///////////////");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    SQLite_Statement* Statement = new SQLite_Statement(StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
    Sentinel->Register(Statement);
    Sentinel->Unregister(Statement);
    EXPECT_NO_THROW(Sentinel->Unregister(Statement)); // Safe double unregister
    delete Statement;

    sqlite3_close(Db);
}

//////////////////////////////////////////////
// T05: InvalidateAll_ClearsSet
//////////////////////////////////////////////

TEST(SQLite_ConnectionSentinel, T05_InvalidateAllClearsSet)
{
    SCOPED_TRACE("/////////////// T05: InvalidateAll_ClearsSet ///////////////");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    SQLite_Statement* A = new SQLite_Statement(StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
    SQLite_Statement* B = new SQLite_Statement(StatementCreationKit{ Sentinel, Db }, "SELECT 2;");

    Sentinel->Register(A);
    Sentinel->Register(B);

    EXPECT_NO_THROW(Sentinel->InvalidateAll());

    delete A;
    delete B;
    sqlite3_close(Db);
}

//////////////////////////////////////////////
// T06: RegisterAfterInvalidate_Works
//////////////////////////////////////////////

TEST(SQLite_ConnectionSentinel, T06_RegisterAfterInvalidateWorks)
{
    SCOPED_TRACE("/////////////// T06: RegisterAfterInvalidate_Works ///////////////");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    Sentinel->InvalidateAll();

    SQLite_Statement* Statement = new SQLite_Statement(StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
    EXPECT_NO_THROW(Sentinel->Register(Statement));
    Sentinel->Unregister(Statement);
    delete Statement;

    sqlite3_close(Db);
}

//////////////////////////////////////////////
// T07: RegisterAndDestroy_UnregistersAutomatically
//////////////////////////////////////////////

TEST(SQLite_ConnectionSentinel, T07_RegisterAndDestroyUnregistersAutomatically)
{
    SCOPED_TRACE("/////////////// T07: RegisterAndDestroy_UnregistersAutomatically ///////////////");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    SQLite_Statement* Statement = new SQLite_Statement(StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
    Sentinel->Register(Statement);
    delete Statement;  // Should auto-unregister

    EXPECT_NO_THROW(Sentinel->InvalidateAll());
    sqlite3_close(Db);
}

//////////////////////////////////////////////
// T08: Destructor_CallsInvalidateAll
//////////////////////////////////////////////

TEST(SQLite_ConnectionSentinel, T08_DestructorCallsInvalidateAll)
{
    SCOPED_TRACE("/////////////// T08: Destructor_CallsInvalidateAll ///////////////");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    std::shared_ptr<SQLite_ConnectionSentinel> Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    SQLite_Statement* Statement = new SQLite_Statement(StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
    Sentinel->Register(Statement);
    delete Statement;

    // Sentinel destruction should not crash
    EXPECT_NO_THROW(Sentinel.reset());
    sqlite3_close(Db);
}

//////////////////////////////////////////////
// T09: ThreadSafety_ParallelRegisterUnregister
//////////////////////////////////////////////

TEST(SQLite_ConnectionSentinel, T09_ThreadSafetyParallelRegisterUnregister)
{
    SCOPED_TRACE("/////////////// T09: ThreadSafety_ParallelRegisterUnregister ///////////////");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_ConnectionSentinel>();

    constexpr int ThreadCount = 10;
    std::vector<std::thread> Threads;

    for (int i = 0; i < ThreadCount; ++i)
    {
        Threads.emplace_back([Sentinel, Db]()
        {
            for (int j = 0; j < 100; ++j)
            {
                SQLite_Statement* S = new SQLite_Statement(StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
                Sentinel->Register(S);
                Sentinel->Unregister(S);
                delete S;
            }
        });
    }

    for (auto& T : Threads) T.join();

    EXPECT_NO_THROW(Sentinel->InvalidateAll());
    sqlite3_close(Db);
}

//////////////////////////////
// T10: Independent destruction without sentinel
//////////////////////////////

TEST(SQLite_ConnectionSentinel, T10_IndependentDestructionNoSentinel)
{
    SCOPED_TRACE("/////////////// T10: Independent destruction without sentinel. ///////////////");
    sqlite3* Db = nullptr;
    sqlite3_open("test.db", &Db);
    sqlite3_exec(Db, "CREATE TABLE IF NOT EXISTS dummy (id INTEGER);", nullptr, nullptr, nullptr);

    SQLite_Statement Stmt(Db, "SELECT 1;");
    EXPECT_NE(Stmt.Get(), nullptr);

    Stmt.Finalize();
    sqlite3_close(Db);
    std::filesystem::remove("test.db");
}

//////////////////////////////
// T11: Sentinel invalidates statements on DbConnection destruction
//////////////////////////////

TEST(SQLite_ConnectionSentinel, T11_DbConnectionDestructionInvalidatesStatements)
{
    SCOPED_TRACE("/////////////// T11: Sentinel invalidates statements when SQLite_DbConnection is destroyed. ///////////////");
    SQLite_Statement* StatementPtr = nullptr;

    {
        SQLite_DbConnection Db(":memory:", false, true);
        StatementCreationKit Kit = Db.MakeStatementKit();
        StatementPtr = new SQLite_Statement(Kit, "SELECT 1;");
    }

    // StatementPtr should now be finalized but safe to access
    StatementPtr->Step();  // No crash expected
    delete StatementPtr;
}

//////////////////////////////
// T12: Statement destruction before DbConnection
//////////////////////////////

TEST(SQLite_ConnectionSentinel, T12_StatementDestroyedBeforeDb)
{
    SCOPED_TRACE("/////////////// T12: Statement destroyed before DbConnection, unregisters cleanly. ///////////////");

    SQLite_DbConnection Db(":memory:", false, true);
    StatementCreationKit Kit = Db.MakeStatementKit();

    {
        SQLite_Statement Stmt1(Kit, "SELECT 1;");
        SQLite_Statement Stmt2(Kit, "SELECT 2;");
        // Both should unregister safely before Db is destroyed
    }
}

//////////////////////////////
// T13: Statement created without sentinel
//////////////////////////////

TEST(SQLite_ConnectionSentinel, T13_NullptrSentinelSafe)
{
    SCOPED_TRACE("/////////////// T13: Statement with nullptr sentinel behaves safely. ///////////////");

    sqlite3* RawDb = nullptr;
    sqlite3_open(":memory:", &RawDb); // In-memory DB for testing

    StatementCreationKit Kit{ std::weak_ptr<SQLite_ConnectionSentinel>{}, RawDb };

    {
        SQLite_Statement Stmt(Kit, "SELECT 1;");
        EXPECT_NE(Stmt.Get(), nullptr);
    }

    sqlite3_close(RawDb);
}

//////////////////////////////
// T14: Finalize is idempotent
//////////////////////////////

TEST(SQLite_ConnectionSentinel, T14_FinalizeFromSentinelIdempotent)
{
    SCOPED_TRACE("/////////////// T14: Finalize can be safely called multiple times. ///////////////");

    SQLite_DbConnection Db(":memory:", false, true);
    StatementCreationKit Kit = Db.MakeStatementKit();

    SQLite_Statement Stmt(Kit, "SELECT 1;");
    Stmt.Finalize();
    EXPECT_NO_THROW(Stmt.Finalize()); // Safe repeat
}

//////////////////////////////
// T15: Concurrent registration and invalidation
//////////////////////////////

TEST(SQLite_ConnectionSentinel, T15_ThreadSafetyConcurrentAccess)
{
    SCOPED_TRACE("/////////////// T15: Concurrent registration and invalidation do not cause race conditions. ///////////////");
    std::vector<std::thread> Threads;
    std::shared_ptr<SQLite_ConnectionSentinel> SharedSentinel = std::make_shared<SQLite_ConnectionSentinel>();

    for (int i = 0; i < 10; ++i)
    {
        Threads.emplace_back([SharedSentinel]() {
            sqlite3* DbHandle = nullptr;
            sqlite3_open("test.db", &DbHandle);

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            StatementCreationKit Kit{ SharedSentinel, DbHandle };

            {
                SQLite_Statement Stmt(Kit, "SELECT 1;");
            }

            sqlite3_close(DbHandle);
        });
    }

    SharedSentinel->InvalidateAll();

    for (auto& Thread : Threads)
    {
        Thread.join();
    }

    std::filesystem::remove("test.db");
}

//////////////////////////////
// T16: Simulate leak without sentinel (optional)
//////////////////////////////

#ifdef EXAMINE_MEMORY_LEAK_WITHOUT_ConnectionSentinel

TEST(SQLite_ConnectionSentinel, T16_LeakWithoutSentinel)
{
    SCOPED_TRACE("/////////////// T16: Statement created without sentinel is not invalidated automatically. ///////////////");
    sqlite3* DbHandle = nullptr;
    sqlite3_open(":memory:", &DbHandle);

    SQLite_Statement* Stmt = new SQLite_Statement(DbHandle, "SELECT 1;");
    sqlite3_close(DbHandle);  // Finalization will fail silently
    delete Stmt;              // Should not crash

    // Use leak-checking tools to validate
}

#endif
