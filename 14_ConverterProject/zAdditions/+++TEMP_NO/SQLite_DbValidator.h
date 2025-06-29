#pragma once
#ifndef SQLITE_DB_VERIFIER_H
#define SQLITE_DB_VERIFIER_H

#include <string>
#include <fstream>
#include <string_view>
////////
#include <sqlite3.h>
#include <iostream>
#include <cstring>


//////////////


//////////////

class SQLite_DbValidator
{
private:
    sqlite3* DbHandle;
    std::string_view DbPath;

public:
    SQLite_DbValidator(
        sqlite3* InDbHandle, std::string_view InDbPath)
        : DbHandle(InDbHandle), DbPath(InDbPath) {}

    bool operator()() const
    {
        if (DbPath.contains(":memory:")) return DbHandle != nullptr;

        return DbHandle && ValidateFileHeader() && ValidateHeaderStructure();
    }

private:

    bool ValidateFileHeader() const
    {
        std::ifstream File(std::string(DbPath), std::ios::binary);
        if (!File) return false;

        char Header[16] = {};
        File.read(Header, sizeof(Header));
        if (File.gcount() < 16) return false;

        static constexpr char ExpectedHeader[16] = {
            'S', 'Q', 'L', 'i', 't', 'e', ' ', 'f', 'o', 'r', 'm', 'a', 't', ' ', '3', '\0'
        };

        return std::memcmp(Header, ExpectedHeader, 16) == 0;
    }

    bool ValidateHeaderStructure() const
    {
        std::ifstream File(std::string(DbPath), std::ios::binary);
        if (!File) return false;

        std::array<unsigned char, 100> Header = {};
        File.read(reinterpret_cast<char*>(Header.data()), Header.size());
        if (File.gcount() < 100) return false;

        const int PageSize = (Header[16] << 8) | Header[17];
        const bool PageSizeOk = ((PageSize >= 512 && PageSize <= 32768 && (PageSize & (PageSize - 1)) == 0) || PageSize == 1);

        return PageSizeOk &&
               (Header[18] == 1 || Header[18] == 2) &&
               (Header[19] == 1 || Header[19] == 2) &&
               Header[21] == 64 &&
               Header[22] == 32 &&
               Header[23] == 32;
    }
};

#endif // SQLITE_DB_VERIFIER_H

#ifdef BUBABABUKA


    bool operator()() const
    {
        if (!DbHandle)
            return false;
        if (Flags.CheckFileHeader 
            && !ValidateFileHeader()) return false;
        if (Flags.CheckHeaderStructure 
            && !ValidateHeaderStructure()) return false;
        if (Flags.CheckSqliteMaster 
            && !ValidateSqliteMasterAccess()) return false;
        if (Flags.CheckHandleValid 
            && !ValidateBasicStatement()) return false;
        if (Flags.CheckPragmaIntegrity 
            && !ValidatePragma("PRAGMA integrity_check;", "ok")) return false;
        if (Flags.CheckPragmaQuick 
            && !ValidatePragma("PRAGMA quick_check;", "ok")) return false;
        if (Flags.CheckForeignKey 
            && !ValidatePragma("PRAGMA foreign_key_check;", "")) return false;

        return true;
    }




        if (Flags.CheckSqliteMaster && !ValidateSqliteMasterAccess()) {
            std::cerr << "[FAIL] CheckSqliteMaster\n";
            return false;
        }
        if (Flags.CheckPragmaIntegrity && !ValidatePragma("PRAGMA integrity_check;", "ok")) {
            std::cerr << "[FAIL] CheckPragmaIntegrity\n";
            return false;
        }
        if (Flags.CheckPragmaQuick && !ValidatePragma("PRAGMA quick_check;", "ok")) {
            std::cerr << "[FAIL] CheckPragmaQuick\n";
            return false;
        }
        if (Flags.CheckForeignKey && !ValidatePragma("PRAGMA foreign_key_check;", "")) {
            std::cerr << "[FAIL] CheckForeignKey\n";
            return false;
        }




bool ValidateSqliteMasterAccess() const
{
    sqlite3_stmt* Statement = nullptr;
    const char* Sql = "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table';";
    if (sqlite3_prepare_v2(DbHandle, Sql, -1, &Statement, nullptr) != SQLITE_OK)
        return false;

    bool Success = false;
    if (sqlite3_step(Statement) == SQLITE_ROW)
    {
        const int Count = sqlite3_column_int(Statement, 0);
        Success = (Count > 0);
    }

    sqlite3_finalize(Statement);
    return Success;
}


bool ValidatePragma(const char* PragmaSql, const char* Expected) const
{
    sqlite3_stmt* Statement = nullptr;
    const int Prepare = sqlite3_prepare_v2(DbHandle, PragmaSql, -1, &Statement, nullptr);
    if (Prepare != SQLITE_OK || Statement == nullptr)
    {
        return false;
    }

    bool Success = false;

    if (std::strcmp(Expected, "") == 0)
    {
        // Fail if any row is returned (used for foreign_key_check)
        while (true)
        {
            const int Step = sqlite3_step(Statement);
            if (Step == SQLITE_DONE)
            {
                Success = true;
                break;
            }
            if (Step == SQLITE_ROW)
            {
                Success = false;
                break;
            }
            if (Step != SQLITE_ROW && Step != SQLITE_DONE)
            {
                Success = false;
                break;
            }
        }
    }
    else
    {
        // Expect exactly one row with matching result string, and no more
        const int Step1 = sqlite3_step(Statement);
        if (Step1 == SQLITE_ROW)
        {
            const unsigned char* Text = sqlite3_column_text(Statement, 0);
            if (Text != nullptr && std::string(reinterpret_cast<const char*>(Text)) == Expected)
            {
                const int Step2 = sqlite3_step(Statement);
                Success = (Step2 == SQLITE_DONE);
            }
        }
    }

    sqlite3_finalize(Statement);
    return Success;
}



#endif // BUBABABUKA