#pragma once

#ifdef NoUseAnymore


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