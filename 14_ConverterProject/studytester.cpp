
#include <cstdio>
#include <cstring>
#include <sqlite3.h>

//-----------------------------------------
// Utility: Open in-memory database
//-----------------------------------------
sqlite3* OpenMemoryDB()
{
    sqlite3* Db = nullptr;
    if (sqlite3_open(":memory:", &Db) != SQLITE_OK) {
        puts("[ERROR] Failed to open in-memory database.");
        return nullptr;
    }
    return Db;
}

//-----------------------------------------
// Utility: Prepare Statement
//-----------------------------------------
sqlite3_stmt* PrepareStatement(sqlite3* Db, const char* Sql)
{
    sqlite3_stmt* Stmt = nullptr;
    if (sqlite3_prepare_v2(Db, Sql, -1, &Stmt, nullptr) != SQLITE_OK) {
        puts("[ERROR] Failed to prepare statement.");
        return nullptr;
    }
    return Stmt;
}

//-----------------------------------------
// Utility: Execute Step and Reset
//-----------------------------------------
void StepAndReset(sqlite3_stmt* Stmt)
{
    while (sqlite3_step(Stmt) == SQLITE_ROW) {
        // just stepping through
    }
    sqlite3_reset(Stmt);
}

//=============================================================================
// Example 01: Access Column Count and Names
//=============================================================================
void Ex01_ColumnMetadata()
{
    puts("========== Example 01: Column Count and Names ==========");

    sqlite3* Db = OpenMemoryDB();
    sqlite3_exec(Db, "CREATE TABLE Users(Id INTEGER, Name TEXT);", nullptr, nullptr, nullptr);
    sqlite3_exec(Db, "INSERT INTO Users VALUES (1, 'Alice'), (2, 'Bob');", nullptr, nullptr, nullptr);

    sqlite3_stmt* Stmt = PrepareStatement(Db, "SELECT Id, Name FROM Users;");
    if (!Stmt) return;

    int ColCount = sqlite3_column_count(Stmt);
    printf("Column Count: %d\n", ColCount);

    for (int i = 0; i < ColCount; ++i) {
        printf("Column %d Name: %s\n", i, sqlite3_column_name(Stmt, i));
    }

    sqlite3_finalize(Stmt);
    sqlite3_close(Db);
}

//=============================================================================
// Example 02: Access Declared Type vs Runtime Type
//=============================================================================
void Ex02_DeclaredVsRuntimeType()
{
    puts("\n========== Example 02: Declared Type vs Runtime Type ==========");

    sqlite3* Db = OpenMemoryDB();
    sqlite3_exec(Db, "CREATE TABLE Data(Value REAL);", nullptr, nullptr, nullptr);
    sqlite3_exec(Db, "INSERT INTO Data VALUES (42.5);", nullptr, nullptr, nullptr);

    sqlite3_stmt* Stmt = PrepareStatement(Db, "SELECT Value FROM Data;");
    if (!Stmt) return;

    if (sqlite3_step(Stmt) == SQLITE_ROW) {
        const char* DeclaredType = sqlite3_column_decltype(Stmt, 0);
        int RuntimeType = sqlite3_column_type(Stmt, 0);

        printf("Declared Type: %s\n", DeclaredType ? DeclaredType : "NULL");
        printf("Runtime Type Code: %d\n", RuntimeType);
    }

    sqlite3_finalize(Stmt);
    sqlite3_close(Db);
}

//=============================================================================
// Example 03: Access Result Values With Type Conversion
//=============================================================================
void Ex03_TypeConversionDemo()
{
    puts("\n========== Example 03: Access Result Values With Type Conversion ==========");

    sqlite3* Db = OpenMemoryDB();
    sqlite3_exec(Db, "CREATE TABLE Items(Value INTEGER);", nullptr, nullptr, nullptr);
    sqlite3_exec(Db, "INSERT INTO Items VALUES (123);", nullptr, nullptr, nullptr);

    sqlite3_stmt* Stmt = PrepareStatement(Db, "SELECT Value FROM Items;");
    if (!Stmt) return;

    if (sqlite3_step(Stmt) == SQLITE_ROW) {
        int IntVal = sqlite3_column_int(Stmt, 0);
        double FloatVal = sqlite3_column_double(Stmt, 0);
        const unsigned char* TextVal = sqlite3_column_text(Stmt, 0);

        printf("As Integer: %d\n", IntVal);
        printf("As Float  : %f\n", FloatVal);
        printf("As Text   : %s\n", TextVal);
    }

    sqlite3_finalize(Stmt);
    sqlite3_close(Db);
}

//=============================================================================
// Example 04: Column Bytes and Memory Safety
//=============================================================================
void Ex04_TextLengthAndMemory()
{
    puts("\n========== Example 04: Column Bytes and Memory Safety ==========");

    sqlite3* Db = OpenMemoryDB();
    sqlite3_exec(Db, "CREATE TABLE Notes(Content TEXT);", nullptr, nullptr, nullptr);
    sqlite3_exec(Db, "INSERT INTO Notes VALUES ('hello 🌍');", nullptr, nullptr, nullptr);

    sqlite3_stmt* Stmt = PrepareStatement(Db, "SELECT Content FROM Notes;");
    if (!Stmt) return;

    if (sqlite3_step(Stmt) == SQLITE_ROW) {
        const unsigned char* TextVal = sqlite3_column_text(Stmt, 0);
        int ByteSize = sqlite3_column_bytes(Stmt, 0); // Must be after _text()

        printf("Text: %s\n", TextVal);
        printf("UTF-8 Byte Length: %d\n", ByteSize);
    }

    sqlite3_finalize(Stmt);
    sqlite3_close(Db);
}

//=============================================================================
// Example 05: Column Source Metadata (Requires SQLITE_ENABLE_COLUMN_METADATA)
//=============================================================================
void Ex05_ColumnSource()
{
    puts("\n========== Example 05: Column Source Metadata ==========");

    sqlite3* Db = OpenMemoryDB();
    sqlite3_exec(Db, "CREATE TABLE Log(Id INTEGER PRIMARY KEY, Message TEXT);", nullptr, nullptr, nullptr);
    sqlite3_exec(Db, "INSERT INTO Log(Message) VALUES ('Startup complete');", nullptr, nullptr, nullptr);

    sqlite3_stmt* Stmt = PrepareStatement(Db, "SELECT Id AS EntryId, Message FROM Log;");
    if (!Stmt) return;

#ifdef SQLITE_ENABLE_COLUMN_METADATA
    int ColCount = sqlite3_column_count(Stmt);
    for (int i = 0; i < ColCount; ++i) {
        const char* DbName = sqlite3_column_database_name(Stmt, i);
        const char* TableName = sqlite3_column_table_name(Stmt, i);
        const char* OriginName = sqlite3_column_origin_name(Stmt, i);

        printf("Column %d\n", i);
        printf(" - Database : %s\n", DbName ? DbName : "NULL");
        printf(" - Table    : %s\n", TableName ? TableName : "NULL");
        printf(" - Origin   : %s\n", OriginName ? OriginName : "NULL");
    }
#else
    puts("Metadata interfaces require SQLITE_ENABLE_COLUMN_METADATA.");
#endif

    sqlite3_finalize(Stmt);
    sqlite3_close(Db);
}

//=============================================================================
// Example 06: Handle NULL and Error Scenarios
//=============================================================================
void Ex06_HandleNullsAndErrors()
{
    puts("\n========== Example 06: Handle NULL and Error Scenarios ==========");

    sqlite3* Db = OpenMemoryDB();
    sqlite3_exec(Db, "CREATE TABLE Config(Key TEXT, Value TEXT);", nullptr, nullptr, nullptr);
    sqlite3_exec(Db, "INSERT INTO Config(Key, Value) VALUES ('theme', NULL);", nullptr, nullptr, nullptr);

    sqlite3_stmt* Stmt = PrepareStatement(Db, "SELECT Value FROM Config WHERE Key = 'theme';");
    if (!Stmt) return;

    if (sqlite3_step(Stmt) == SQLITE_ROW) {
        const unsigned char* TextVal = sqlite3_column_text(Stmt, 0);
        int ErrorCode = sqlite3_errcode(Db);

        if (!TextVal) {
            printf("Value is NULL or OOM. SQLite error code: %d\n", ErrorCode);
        } else {
            printf("Value: %s\n", TextVal);
        }
    }

    sqlite3_finalize(Stmt);
    sqlite3_close(Db);
}

//=============================================================================
// Main
//=============================================================================
int main()
{
    Ex01_ColumnMetadata();
    Ex02_DeclaredVsRuntimeType();
    Ex03_TypeConversionDemo();
    Ex04_TextLengthAndMemory();
    Ex05_ColumnSource();
    Ex06_HandleNullsAndErrors();
}