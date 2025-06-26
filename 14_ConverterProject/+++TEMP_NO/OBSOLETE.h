#pragma once

#ifdef NoUseAnymore



class SQLite_DbChecker
{
    friend class SQLiteConnector_DbChecker_Test;

    SQLite_DbChecker() = delete;

public:
    static void CheckResult(const int ResultCode, const char *OperatonType);
    static void CheckResult(sqlite3 *DbHandle, const int ResultCode, const char *OperatonType);
    static void WarnIfFalse(const bool Condition, const char* WarningMsg);
};


////////////////////

void SQLite_DbChecker::CheckResult(const int ResultCode, const char* OperationType)
{
    if (ResultCode == SQLITE_OK) return;
    CONFIRMS(false, "ErrorCode: {}, Operation: [{}]", ResultCode, OperationType);
}

void SQLite_DbChecker::CheckResult(sqlite3* DbHandle, const int ResultCode, const char* OperationType)
{
    if (ResultCode == SQLITE_OK) return;
    CONFIRMS(OperationType, "OperationType message is null");

    const int ErrorCode = sqlite3_extended_errcode(DbHandle);
    CONFIRMS(false,
        "ErrorCode: {}, Operation: [{}]: {}", ErrorCode, OperationType, sqlite3_errmsg(DbHandle));
}

void SQLite_DbChecker::WarnIfFalse(const bool Condition, const char* WarningMsg)
{
    if (Condition) return;
    CONFIRM(WarningMsg);
    std::print(std::cerr, "Warning: {}\n", WarningMsg);
}







TEST(SQLite_DbChecker, CheckResult_OK) {
    sqlite3* db = nullptr;
    EXPECT_NO_THROW(SQLite_DbChecker::CheckResult(db, SQLITE_OK, "test"));
}

TEST(SQLite_DbChecker, WarnIfFalse_False) {
    testing::internal::CaptureStderr();
    SQLite_DbChecker::WarnIfFalse(false, "test");
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_NE(output.find("Warning: test"), std::string::npos);
}

TEST(SQLite_DbChecker, WarnIfFalse_True) {
    EXPECT_NO_THROW(SQLite_DbChecker::WarnIfFalse(true, "test"));
}

TEST(SQLiteConnector, SQLite_DbChecker)
{
    const auto CreateInMemoryDb = []()
    {
        sqlite3* Db = nullptr;
        EXPECT_EQ(sqlite3_open(":memory:", &Db), SQLITE_OK);
        sqlite3_extended_result_codes(Db, 1); // Enable extended error codes
        return Db;
    };

    //////////////////////////////////////////////
    //              Normal Cases
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with SQLITE_OK should succeed");

        sqlite3* Db = CreateInMemoryDb();
        ASSERT_NO_FATAL_FAILURE(SQLite_DbChecker::CheckResult(Db, SQLITE_OK, "no-op"));
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //              Edge Cases
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with long operation name (format + utf8)");

        sqlite3* Db = CreateInMemoryDb();

        // Force an error
        const char* InvalidSQL = "INVALID SQL!";
        sqlite3_exec(Db, InvalidSQL, nullptr, nullptr, nullptr);

        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, SQLITE_ERROR, "execute_invalid_sql_with_long_name"), std::runtime_error);
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //             Invalid Inputs
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with nullptr as operation type");

        sqlite3* Db = CreateInMemoryDb();

        sqlite3_exec(Db, "BAD;", nullptr, nullptr, nullptr);

        const char* Op = nullptr;
        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, SQLITE_ERROR, Op), std::runtime_error);
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //             Empty Structures
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with empty string as operation type");

        sqlite3* Db = CreateInMemoryDb();

        sqlite3_exec(Db, "INVALID!", nullptr, nullptr, nullptr);

        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, SQLITE_ERROR, ""), std::runtime_error);
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //        Maximums and Minimums
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with INT_MAX as result code");

        sqlite3* Db = CreateInMemoryDb();

        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, INT_MAX, "overflow"), std::runtime_error);
        sqlite3_close(Db);
    }

    {
        SCOPED_TRACE("CheckResult with INT_MIN as result code");

        sqlite3* Db = CreateInMemoryDb();

        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, INT_MIN, "underflow"), std::runtime_error);
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //         Extended Result Code Cases
    //////////////////////////////////////////////
    {
        SCOPED_TRACE("CheckResult with extended error code (e.g. SQLITE_IOERR)");

        sqlite3* Db = nullptr;
        // Force failure: opening invalid path
        int rc = sqlite3_open("/invalid/path/to.db", &Db);
        ASSERT_NE(rc, SQLITE_OK);
        sqlite3_extended_result_codes(Db, 1);

        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, rc, "open_invalid_path"), std::runtime_error);
        sqlite3_close(Db);
    }

    {
        SCOPED_TRACE("CheckResult with SQLITE_CONSTRAINT_NOTNULL (extended)");

        sqlite3* Db = CreateInMemoryDb();

        const char* Schema = "CREATE TABLE T (Id INTEGER PRIMARY KEY, Name TEXT NOT NULL);";
        const char* InsertFail = "INSERT INTO T (Id) VALUES (1);";

        sqlite3_exec(Db, Schema, nullptr, nullptr, nullptr);
        sqlite3_exec(Db, InsertFail, nullptr, nullptr, nullptr); // Should fail on NOT NULL

        EXPECT_THROW(SQLite_DbChecker::CheckResult(Db, SQLITE_CONSTRAINT, "insert_violate_notnull"), std::runtime_error);
        sqlite3_close(Db);
    }

    //////////////////////////////////////////////
    //              WarnIfFalse Cases
    //////////////////////////////////////////////

    {
        SCOPED_TRACE("WarnIfFalse: Condition is true (no output expected)");
        std::stringstream buffer;
        auto* oldBuf = std::cerr.rdbuf(buffer.rdbuf());

        SQLite_DbChecker::WarnIfFalse(true, "Should not warn");

        std::cerr.rdbuf(oldBuf);
        const std::string output = buffer.str();
        EXPECT_EQ(output, "");
    }

    {
        SCOPED_TRACE("WarnIfFalse: Condition is false with valid message");
        std::stringstream buffer;
        auto* oldBuf = std::cerr.rdbuf(buffer.rdbuf());

        SQLite_DbChecker::WarnIfFalse(false, "Expected warning output");

        std::cerr.rdbuf(oldBuf);
        const std::string output = buffer.str();
        EXPECT_EQ(output, "Warning: Expected warning output\n");
    }

    {
        SCOPED_TRACE("WarnIfFalse: Empty string as warning message");
        std::stringstream buffer;
        auto* oldBuf = std::cerr.rdbuf(buffer.rdbuf());

        SQLite_DbChecker::WarnIfFalse(false, "");

        std::cerr.rdbuf(oldBuf);
        const std::string output = buffer.str();
        EXPECT_EQ(output, "Warning: \n");
    }

    {
        SCOPED_TRACE("WarnIfFalse: Warning message is nullptr (must crash due to CONFIRM)");
        const char* msg = nullptr;
        EXPECT_THROW({
            SQLite_DbChecker::WarnIfFalse(false, msg);
        }, std::runtime_error);
    }
}






    static void BindParamsByIndex(sqlite3_stmt* Statement, const std::vector<BindableValue>& BindingValues)
    {
        CONFIRM(Statement != nullptr);
        CONFIRM(IsCorrectBindableParamNumber(Statement, BindingValues));

        PrepareStatementForBinding(Statement);

        int ParamIndex = 1;
        for (const BindableValue& Value : BindingValues)
        {
            const int BindResult = BindOneParam(Statement, ParamIndex++, Value);
            SQLite_DbChecker::CheckResult(BindResult, "BindOneParam");
        }
    }

    static bool IsCorrectBindableParamNumber(sqlite3_stmt* Statement, const std::vector<BindableValue>& BindingValues)
    {
        const size_t BindableParamNumber = GetBindableParamNumber(Statement);
        return BindingValues.size() == BindableParamNumber;
    }
    
    static bool IsStopCharacter(char SomeChar);

bool TextCleaner::IsStopCharacter(char SomeChar)
{
    return isspace(static_cast<unsigned char>(SomeChar)) || SomeChar == ',';
}

TEST(TextCleaner, IsStopCharacter)
{
    // Test whitespace characters
    EXPECT_TRUE(TextCleaner::IsStopCharacter(' '));
    EXPECT_TRUE(TextCleaner::IsStopCharacter('\t'));
    EXPECT_TRUE(TextCleaner::IsStopCharacter('\n'));
    EXPECT_TRUE(TextCleaner::IsStopCharacter('\r'));
    EXPECT_TRUE(TextCleaner::IsStopCharacter('\f'));
    EXPECT_TRUE(TextCleaner::IsStopCharacter('\v'));

    // Test comma character
    EXPECT_TRUE(TextCleaner::IsStopCharacter(','));

    // Test alphanumeric characters
    EXPECT_FALSE(TextCleaner::IsStopCharacter('a'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('Z'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('0'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('9'));

    // Test common punctuation characters that are not commas
    EXPECT_FALSE(TextCleaner::IsStopCharacter('.'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('!'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('?'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter(';'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter(':'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('\''));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('\"'));

    // Test miscellaneous symbols
    EXPECT_FALSE(TextCleaner::IsStopCharacter('@'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('#'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('$'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('%'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('^'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('&'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('*'));
    EXPECT_FALSE(TextCleaner::IsStopCharacter('('));
    EXPECT_FALSE(TextCleaner::IsStopCharacter(')'));

    // Test extended ASCII and negative values (casting to unsigned char to ensure correctness)
    EXPECT_FALSE(TextCleaner::IsStopCharacter(static_cast<char>(-1)));
    EXPECT_FALSE(TextCleaner::IsStopCharacter(static_cast<char>(128)));
    EXPECT_FALSE(TextCleaner::IsStopCharacter(static_cast<char>(255)));
}

/////////////////////////////////////////////


    static size_t FindNearestDelimiter(const std::string &Text, size_t StartPos, std::function<bool(char)> IsDelimiter);

size_t TextCleaner::FindNearestDelimiter(const std::string &Text, size_t StartPos, std::function<bool(char)> IsDelimiter)
{
    if (Text.empty() || StartPos >= Text.size()) return std::string::npos;

    while (StartPos < Text.size())
    {
        if (IsDelimiter(Text[StartPos])) return StartPos;
        ++StartPos;
    }
    return StartPos;
}

    friend class TextCleaner_FindNearestDelimiter_Test;
    
TEST(TextCleaner, FindNearestDelimiter)
{
    // Test empty string - should return StartPos unchanged
    {
        const std::string Text = "";
        size_t StartPos = 0;
        auto IsStop = [](char) { return true; }; // Should not matter
        EXPECT_EQ(TextCleaner::FindNearestDelimiter(Text, StartPos, IsStop), std::string::npos);
    }

    // Test no stop character found in the text
    {
        const std::string Text = "abcdefg";
        size_t StartPos = 0;
        auto IsStop = [](char c) { return c == 'x'; };
        EXPECT_EQ(TextCleaner::FindNearestDelimiter(Text, StartPos, IsStop), Text.size());
    }

    // Test first character is a stop character
    {
        const std::string Text = "abc";
        size_t StartPos = 0;
        auto IsStop = [](char c) { return c == 'a'; };
        EXPECT_EQ(TextCleaner::FindNearestDelimiter(Text, StartPos, IsStop), 0);
    }

    // Test stop character in the middle of the text
    {
        const std::string Text = "hello world";
        size_t StartPos = 2;
        auto IsStop = [](char c) { return c == 'o'; };
        EXPECT_EQ(TextCleaner::FindNearestDelimiter(Text, StartPos, IsStop), 4);
    }

    // Test stop character at the end
    {
        const std::string Text = "test.";
        size_t StartPos = 0;
        auto IsStop = [](char c) { return c == '.'; };
        EXPECT_EQ(TextCleaner::FindNearestDelimiter(Text, StartPos, IsStop), 4);
    }

    // Test multiple stop characters, but should return the first occurrence
    {
        const std::string Text = "abc def,ghi";
        size_t StartPos = 0;
        auto IsStop = [](char c) { return c == ' ' || c == ','; };
        EXPECT_EQ(TextCleaner::FindNearestDelimiter(Text, StartPos, IsStop), 3);
    }

    // Test StartPos beyond the text size
    {
        const std::string Text = "abc";
        size_t StartPos = 10;
        auto IsStop = [](char c) { return c == 'a'; };
        EXPECT_EQ(TextCleaner::FindNearestDelimiter(Text, StartPos, IsStop), std::string::npos);
    }

    // Test using std::isspace as the stop condition
    {
        const std::string Text = "abc\tdef\nghi";
        size_t StartPos = 0;
        auto IsStop = [](char c) { return std::isspace(static_cast<unsigned char>(c)); };
        EXPECT_EQ(TextCleaner::FindNearestDelimiter(Text, StartPos, IsStop), 3); // Finds tab character
    }
}


/////////////////////////////////////////////


#endif // #ifdef NoUseAnymore