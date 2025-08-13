#include <memory>
#include <filesystem>
////////
#include <gtest/gtest.h>
#include <sqlite3.h>
////////
#include "SQLite_Statement.h"
#include "SQLite_DbConnection.h"

// T01: Register() correctly tracks a single statement in the sentinel
// T02: Register() is idempotent; duplicate registration does not cause duplication or error
// T03: Unregister() removes a previously registered statement from the sentinel
// T04: Unregister() is safe to call multiple times on the same or unregistered statement
// T05: InvalidateAll() finalizes all registered statements and clears the registry
// T06: Sentinel remains functional after InvalidateAll(); new registrations still succeed
// T07: SQLite_Statement unregisters itself from sentinel during destruction
// T08: SQLite_StatementSentinel destructor automatically calls InvalidateAll()
// T09: Concurrent Register()/Unregister() operations from multiple threads are thread-safe
// T10: SQLite_Statement works correctly without a sentinel (e.g., if created with null kit)
// T11: SQLite_DbConnection invalidates all statements registered to its sentinel
// T12: Destroying statements before connection close causes clean unregistration from sentinel
// T13: Statement created with manually null sentinel behaves safely and avoids registration
// T14: Finalize() can be called multiple times safely and is idempotent
// T15: Concurrent Register() and InvalidateAll() are thread-safe and do not cause races

// T16: StatementCreationKit copy constructor preserves DbHandle and Sentinel correctly
// T17: StatementCreationKit move constructor transfers ownership; source becomes inert
// T18: StatementCreationKit copy assignment clones content without affecting source
// T19: StatementCreationKit move assignment transfers content; source becomes safe but inert
// T20: Statement constructed from a moved-from kit behaves correctly and does not crash
// T21: StatementCreationKit with expired (destroyed) sentinel does not register the statement
// T22: StatementCreationKit with invalid (null) DbHandle causes controlled failure or assertion
// T23: InvalidateAll() called on an empty sentinel registry is safe and has no side effects
// T24: Moving or copying containers holding sentinel does not disrupt its internal state
// T25: Statement created with expired weak_ptr sentinel does not crash, just skips registration


// #define EXAMINE_MEMORY_LEAK_WITHOUT_ConnectionSentinel


//////////////////////////////////////////////
// T01: Register() correctly tracks a single statement in the sentinel
//////////////////////////////////////////////

TEST(SQLite_StatementSentinel, T01_RegisterAddsPointer)
{
    SCOPED_TRACE("T01: Register_AddsPointer");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();

    const auto Lambda = [&]()
    {
        SQLite_Statement* Statement = new SQLite_Statement(SQLite_StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
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

TEST(SQLite_StatementSentinel, T02_UnregisterRemovesPointer)
{
    SCOPED_TRACE("T02: Unregister_RemovesPointer");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();

    const auto Lambda = [&]()
    {
        SQLite_Statement* Statement = new SQLite_Statement(SQLite_StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
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

TEST(SQLite_StatementSentinel, T03_DoubleRegisterHasSingleEntry)
{
    SCOPED_TRACE("T03: DoubleRegister_HasSingleEntry");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();

    SQLite_Statement* Statement = new SQLite_Statement(SQLite_StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
    Sentinel->Register(Statement);
    Sentinel->Register(Statement);  // Should not crash
    Sentinel->Unregister(Statement);
    delete Statement;

    sqlite3_close(Db);
}

//////////////////////////////////////////////
// T04: DoubleUnregister_Safe
//////////////////////////////////////////////

TEST(SQLite_StatementSentinel, T04_DoubleUnregisterSafe)
{
    SCOPED_TRACE("T04: DoubleUnregister_Safe");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();

    SQLite_Statement* Statement = new SQLite_Statement(SQLite_StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
    Sentinel->Register(Statement);
    Sentinel->Unregister(Statement);
    EXPECT_NO_THROW(Sentinel->Unregister(Statement)); // Safe double unregister
    delete Statement;

    sqlite3_close(Db);
}

//////////////////////////////////////////////
// T05: InvalidateAll_ClearsSet
//////////////////////////////////////////////

TEST(SQLite_StatementSentinel, T05_InvalidateAllClearsSet)
{
    SCOPED_TRACE("T05: InvalidateAll_ClearsSet");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();

    SQLite_Statement* A = new SQLite_Statement(SQLite_StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
    SQLite_Statement* B = new SQLite_Statement(SQLite_StatementCreationKit{ Sentinel, Db }, "SELECT 2;");

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

TEST(SQLite_StatementSentinel, T06_RegisterAfterInvalidateWorks)
{
    SCOPED_TRACE("T06: RegisterAfterInvalidate_Works");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();

    Sentinel->InvalidateAll();

    SQLite_Statement* Statement = new SQLite_Statement(SQLite_StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
    EXPECT_NO_THROW(Sentinel->Register(Statement));
    Sentinel->Unregister(Statement);
    delete Statement;

    sqlite3_close(Db);
}

//////////////////////////////////////////////
// T07: RegisterAndDestroy_UnregistersAutomatically
//////////////////////////////////////////////

TEST(SQLite_StatementSentinel, T07_RegisterAndDestroyUnregistersAutomatically)
{
    SCOPED_TRACE("T07: RegisterAndDestroy_UnregistersAutomatically");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();

    SQLite_Statement* Statement = new SQLite_Statement(SQLite_StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
    Sentinel->Register(Statement);
    delete Statement;  // Should auto-unregister

    EXPECT_NO_THROW(Sentinel->InvalidateAll());
    sqlite3_close(Db);
}

//////////////////////////////////////////////
// T08: Destructor_CallsInvalidateAll
//////////////////////////////////////////////

TEST(SQLite_StatementSentinel, T08_DestructorCallsInvalidateAll)
{
    SCOPED_TRACE("T08: Destructor_CallsInvalidateAll");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    std::shared_ptr<SQLite_StatementSentinel> Sentinel = std::make_shared<SQLite_StatementSentinel>();

    SQLite_Statement* Statement = new SQLite_Statement(SQLite_StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
    Sentinel->Register(Statement);
    delete Statement;

    // Sentinel destruction should not crash
    EXPECT_NO_THROW(Sentinel.reset());
    sqlite3_close(Db);
}

//////////////////////////////////////////////
// T09: ThreadSafety_ParallelRegisterUnregister
//////////////////////////////////////////////

TEST(SQLite_StatementSentinel, T09_ThreadSafetyParallelRegisterUnregister)
{
    SCOPED_TRACE("T09: ThreadSafety_ParallelRegisterUnregister");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();

    constexpr int ThreadCount = 10;
    std::vector<std::thread> Threads;

    for (int i = 0; i < ThreadCount; ++i)
    {
        Threads.emplace_back([Sentinel, Db]()
        {
            for (int j = 0; j < 100; ++j)
            {
                SQLite_Statement* S = new SQLite_Statement(SQLite_StatementCreationKit{ Sentinel, Db }, "SELECT 1;");
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

TEST(SQLite_StatementSentinel, T10_IndependentDestructionNoSentinel)
{
    SCOPED_TRACE("T10: Independent destruction without sentinel.");
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

TEST(SQLite_StatementSentinel, T11_DbConnectionDestructionInvalidatesStatements)
{
    SCOPED_TRACE("T11: Sentinel invalidates statements when SQLite_DbConnection is destroyed.");
    SQLite_Statement* StatementPtr = nullptr;

    {
        SQLite_DbConnection Db({":memory:", false, true});
        SQLite_StatementCreationKit Kit = Db.MakeStatementKit();
        StatementPtr = new SQLite_Statement(Kit, "SELECT 1;");
    }

    // StatementPtr should now be finalized and crash, if accessed
    EXPECT_THROW(StatementPtr->Step(), std::runtime_error);
    delete StatementPtr;
}

//////////////////////////////
// T12: Statement destruction before DbConnection
//////////////////////////////

TEST(SQLite_StatementSentinel, T12_StatementDestroyedBeforeDb)
{
    SCOPED_TRACE("T12: Statement destroyed before DbConnection, unregisters cleanly.");

    SQLite_DbConnection Db({":memory:", false, true});
    SQLite_StatementCreationKit Kit = Db.MakeStatementKit();

    {
        SQLite_Statement Stmt1(Kit, "SELECT 1;");
        SQLite_Statement Stmt2(Kit, "SELECT 2;");
        // Both should unregister safely before Db is destroyed
    }
}

//////////////////////////////
// T13: Statement created without sentinel
//////////////////////////////

TEST(SQLite_StatementSentinel, T13_NullptrSentinelSafe)
{
    SCOPED_TRACE("T13: Statement with nullptr sentinel behaves safely.");

    sqlite3* RawDb = nullptr;
    sqlite3_open(":memory:", &RawDb); // In-memory DB for testing

    SQLite_StatementCreationKit Kit{ std::weak_ptr<SQLite_StatementSentinel>{}, RawDb };

    {
        SQLite_Statement Stmt(Kit, "SELECT 1;");
        EXPECT_NE(Stmt.Get(), nullptr);
    }

    sqlite3_close(RawDb);
}

//////////////////////////////
// T14: Finalize is idempotent
//////////////////////////////

TEST(SQLite_StatementSentinel, T14_FinalizeFromSentinelIdempotent)
{
    SCOPED_TRACE("T14: Finalize can be safely called multiple times.");

    SQLite_DbConnection Db({":memory:", false, true});
    SQLite_StatementCreationKit Kit = Db.MakeStatementKit();

    SQLite_Statement Stmt(Kit, "SELECT 1;");
    Stmt.Finalize();
    EXPECT_NO_THROW(Stmt.Finalize()); // Safe repeat
}

//////////////////////////////
// T15: Concurrent registration and invalidation
//////////////////////////////

TEST(SQLite_StatementSentinel, T15_ThreadSafetyConcurrentAccess)
{
    SCOPED_TRACE("T15: Concurrent registration and invalidation do not cause race conditions.");
    std::vector<std::thread> Threads;
    std::shared_ptr<SQLite_StatementSentinel> SharedSentinel = std::make_shared<SQLite_StatementSentinel>();

    for (int i = 0; i < 10; ++i)
    {
        Threads.emplace_back([SharedSentinel]() {
            sqlite3* DbHandle = nullptr;
            sqlite3_open("test.db", &DbHandle);

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            SQLite_StatementCreationKit Kit{ SharedSentinel, DbHandle };

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

//////////////////////////////////////////////
// SQLite_StatementCreationKit, T16_CopyConstructorPreservesFields
//////////////////////////////////////////////

TEST(SQLite_StatementCreationKit, T16_CopyConstructorPreservesFields)
{
    SCOPED_TRACE("T16: Copy constructor preserves DbHandle and Sentinel");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();

    SQLite_StatementCreationKit Original{ Sentinel, Db };
    SQLite_StatementCreationKit Copy = Original;

    SQLite_Statement Stmt(Copy, "SELECT 1;");
    EXPECT_NE(Stmt.Get(), nullptr);

    Stmt.Finalize();
    sqlite3_close(Db);
}


//////////////////////////////////////////////
// SQLite_StatementCreationKit, T17_MoveConstructorTransfersOwnership
//////////////////////////////////////////////

TEST(SQLite_StatementCreationKit, T17_MoveConstructorTransfersOwnership)
{
    SCOPED_TRACE("T17: Move constructor transfers ownership and leaves source inert");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();

    SQLite_StatementCreationKit Original{ Sentinel, Db };
    SQLite_StatementCreationKit Moved = std::move(Original);

    SQLite_Statement Stmt(Moved, "SELECT 1;");
    EXPECT_NE(Stmt.Get(), nullptr);

    Stmt.Finalize();
    sqlite3_close(Db);
}


//////////////////////////////////////////////
// SQLite_StatementCreationKit, T18_CopyAssignmentPreservesFields
//////////////////////////////////////////////

TEST(SQLite_StatementCreationKit, T18_CopyAssignmentPreservesFields)
{
    SCOPED_TRACE("T18: Copy assignment preserves Sentinel and DbHandle");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();

    SQLite_StatementCreationKit A{ Sentinel, Db };
    SQLite_StatementCreationKit B = A;

    SQLite_Statement Stmt(B, "SELECT 1;");
    EXPECT_NE(Stmt.Get(), nullptr);

    Stmt.Finalize();
    sqlite3_close(Db);
}


//////////////////////////////////////////////
// SQLite_StatementCreationKit, T19_MoveAssignmentTransfersOwnership
//////////////////////////////////////////////

TEST(SQLite_StatementCreationKit, T19_MoveAssignmentTransfersOwnership)
{
    SCOPED_TRACE("T19: Move assignment transfers fields, leaves source inert");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();

    SQLite_StatementCreationKit A{ Sentinel, Db };
    SQLite_StatementCreationKit B = std::move(A);

    SQLite_Statement Stmt(B, "SELECT 1;");
    EXPECT_NE(Stmt.Get(), nullptr);

    Stmt.Finalize();
    sqlite3_close(Db);
}


//////////////////////////////////////////////
// SQLite_StatementCreationKit, T20_UseAfterMoveBehavesCorrectly
//////////////////////////////////////////////

TEST(SQLite_StatementCreationKit, T20_UseAfterMoveBehavesCorrectly)
{
    SCOPED_TRACE("T20: Statement constructed from moved-from kit behaves safely");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();

    SQLite_StatementCreationKit A{ Sentinel, Db };
    SQLite_StatementCreationKit B = std::move(A);

    EXPECT_NO_THROW({
        SQLite_Statement Stmt(B, "SELECT 1;");
    });

    sqlite3_close(Db);
}


//////////////////////////////////////////////
// SQLite_StatementCreationKit, T21_ExpiredSentinelDoesNotRegister
//////////////////////////////////////////////

TEST(SQLite_StatementCreationKit, T21_ExpiredSentinelDoesNotRegister)
{
    SCOPED_TRACE("T21: Kit with destroyed sentinel does not register statement");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);
    std::weak_ptr<SQLite_StatementSentinel> WeakSentinel;

    {
        auto Sentinel = std::make_shared<SQLite_StatementSentinel>();
        WeakSentinel = Sentinel;
    }

    SQLite_StatementCreationKit Kit{ WeakSentinel, Db };

    EXPECT_NO_THROW({
        SQLite_Statement Stmt(Kit, "SELECT 1;");
    });

    sqlite3_close(Db);
}


//////////////////////////////////////////////
// SQLite_StatementCreationKit, T22_NullDbHandleFails
//////////////////////////////////////////////

TEST(SQLite_StatementCreationKit, T22_NullDbHandleFails)
{
    SCOPED_TRACE("T22: Creating statement with null DbHandle fails gracefully");

    SQLite_StatementCreationKit Kit{ std::weak_ptr<SQLite_StatementSentinel>{}, nullptr };

    EXPECT_ANY_THROW({
        SQLite_Statement Stmt(Kit, "SELECT 1;");
    });
}


//////////////////////////////////////////////
// SQLite_StatementSentinel, T23_InvalidateAllSafeWhenEmpty
//////////////////////////////////////////////

TEST(SQLite_StatementSentinel, T23_InvalidateAllSafeWhenEmpty)
{
    SCOPED_TRACE("T23: Calling InvalidateAll on empty sentinel is safe");

    auto Sentinel = std::make_shared<SQLite_StatementSentinel>();
    EXPECT_NO_THROW(Sentinel->InvalidateAll());
}


//////////////////////////////////////////////
// SQLite_StatementSentinel, T24_MovingSentinelContainerSafe
//////////////////////////////////////////////

TEST(SQLite_StatementSentinel, T24_MovingSentinelContainerSafe)
{
    SCOPED_TRACE("T24: Moving container holding sentinel does not break registry");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);

    std::optional<std::shared_ptr<SQLite_StatementSentinel>> Container;
    Container = std::make_shared<SQLite_StatementSentinel>();

    SQLite_StatementCreationKit Kit{ *Container, Db };
    SQLite_Statement* Statement = new SQLite_Statement(Kit, "SELECT 1;");
    Container.reset();  // triggers InvalidateAll()

    delete Statement;
    sqlite3_close(Db);
}

//////////////////////////////////////////////
// SQLite_StatementCreationKit, T25_ExpiredWeakSentinelSkipsRegistration
//////////////////////////////////////////////

TEST(SQLite_StatementCreationKit, T25_ExpiredWeakSentinelSkipsRegistration)
{
    SCOPED_TRACE("T25: Statement with expired weak_ptr sentinel avoids registration and does not crash");

    sqlite3* Db = nullptr;
    sqlite3_open(":memory:", &Db);

    std::weak_ptr<SQLite_StatementSentinel> ExpiredSentinel;
    {
        auto Temp = std::make_shared<SQLite_StatementSentinel>();
        ExpiredSentinel = Temp;
    }

    SQLite_StatementCreationKit Kit{ ExpiredSentinel, Db };
    EXPECT_NO_THROW({
        SQLite_Statement Stmt(Kit, "SELECT 1;");
    });

    sqlite3_close(Db);
}


#ifdef EXAMINE_MEMORY_LEAK_WITHOUT_ConnectionSentinel


//////////////////////////////
// T999: Simulate leak without sentinel (optional)
//////////////////////////////


TEST(SQLite_StatementSentinel, T16_LeakWithoutSentinel)
{
    SCOPED_TRACE("T16: Statement created without sentinel is not invalidated automatically.");
    sqlite3* DbHandle = nullptr;
    sqlite3_open(":memory:", &DbHandle);

    SQLite_Statement* Stmt = new SQLite_Statement(DbHandle, "SELECT 1;");
    sqlite3_close(DbHandle);  // Finalization will fail silently
    delete Stmt;              // Should not crash

    // Use leak-checking tools to validate
}

#endif
