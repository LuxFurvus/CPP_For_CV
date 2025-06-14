#include <gtest/gtest.h>
////////
#include "TextCleaner.h"
//clear; g++ -std=c++26 -fsanitize=undefined -fsanitize=address -Wall -pedantic -Wextra *.cpp -lgtest -lgtest_main -pthread -o test && ./test; rm test

TEST(TextCleaner, RemoveSubstrings)
{
    // Removing a single occurrence of a substring
    {
        std::string Text = "Hello World";
        TextCleaner::RemoveSubstrings(Text, "World");
        EXPECT_EQ(Text, "Hello ");
    }

    // Removing multiple identical substrings
    {
        std::string Text = "abc abc abc";
        TextCleaner::RemoveSubstrings(Text, "abc");
        EXPECT_EQ(Text, "  ");
    }

    // Removing a single character from a string
    {
        std::string Text = "banana";
        TextCleaner::RemoveSubstrings(Text, "a");
        EXPECT_EQ(Text, "bnn");
    }

    // No match (substring not present)
    {
        std::string Text = "unchanged";
        TextCleaner::RemoveSubstrings(Text, "zzz");
        EXPECT_EQ(Text, "unchanged");
    }

    // Removing the entire string
    {
        std::string Text = "erase_me";
        TextCleaner::RemoveSubstrings(Text, "erase_me");
        EXPECT_EQ(Text, "");
    }

    // Removing from an empty string
    {
        std::string Text = "";
        TextCleaner::RemoveSubstrings(Text, "any");
        EXPECT_EQ(Text, "");
    }

    // Removing with an empty substring to remove
    {
        std::string Text = "abc";
        TextCleaner::RemoveSubstrings(Text, "");
        EXPECT_EQ(Text, "abc");
    }

    // Removing overlapping substrings
    {
        std::string Text = "aaaa";
        TextCleaner::RemoveSubstrings(Text, "aa");
        EXPECT_EQ(Text, "");
    }

    // Removing whitespace
    {
        std::string Text = "Hello World";
        TextCleaner::RemoveSubstrings(Text, " ");
        EXPECT_EQ(Text, "HelloWorld");
    }

    // Removing a substring that repeats with different spacing
    {
        std::string Text = "abc def abc def";
        TextCleaner::RemoveSubstrings(Text, "abc");
        EXPECT_EQ(Text, " def  def");
    }

    // Removing single-letter substrings
    {
        std::string Text = "testcase";
        TextCleaner::RemoveSubstrings(Text, "e");
        EXPECT_EQ(Text, "tstcas");
    }

    // Removing numeric substrings
    {
        std::string Text = "123-456-789";
        TextCleaner::RemoveSubstrings(Text, "-");
        EXPECT_EQ(Text, "123456789");
    }

    // Removal with substrings at the start, middle, and end
    {
        std::string Text = "abc123abc456abc";
        TextCleaner::RemoveSubstrings(Text, "abc");
        EXPECT_EQ(Text, "123456");
    }

    // Repeated removal of overlapping pairs
    {
        std::string Text = "aaaabaaa";
        TextCleaner::RemoveSubstrings(Text, "aa");
        EXPECT_EQ(Text, "ba");
    }

    // Removal of an entire repeating pattern
    {
        std::string Text = "xyxyxyxy";
        TextCleaner::RemoveSubstrings(Text, "xy");
        EXPECT_EQ(Text, "");
    }
}

////////////////////////////////////

TEST(TextCleaner, RemoveSubstringsFromSet)
{
    ///////////////////// Typical Scenarios /////////////////////

    // Removing multiple substrings in a sentence
    {
        std::string Text = "Hello, world! This is a test.";
        std::vector<std::string> Substrings = {"Hello", "test"};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, ", world! This is a .");
    }

    // Removing repeated substrings
    {
        std::string Text = "abc def abc def";
        std::vector<std::string> Substrings = {"abc", "def"};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "   ");
    }

    // Removing single-character substrings
    {
        std::string Text = "banana";
        std::vector<std::string> Substrings = {"a", "n"};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "b");
    }

    // Removing whitespace characters
    {
        std::string Text = "remove all spaces";
        std::vector<std::string> Substrings = {" "};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "removeallspaces");
    }

    // Removing numeric substrings
    {
        std::string Text = "123-456-789";
        std::vector<std::string> Substrings = {"-", "123"};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "456789");
    }

    ///////////////////// Boundary Scenarios /////////////////////

    // Removing when input is empty
    {
        std::string Text = "";
        std::vector<std::string> Substrings = {"a", "b"};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "");
    }

    // Removing when substrings to remove are empty
    {
        std::string Text = "nothing changes";
        std::vector<std::string> Substrings = {};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "nothing changes");
    }

    // Removing entire string
    {
        std::string Text = "erase_me";
        std::vector<std::string> Substrings = {"erase_me"};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "");
    }

    // Removing with substrings that overlap partially
    {
        std::string Text = "aaaa";
        std::vector<std::string> Substrings = {"aa"};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "");
    }

    // Removing substring at start, middle, and end
    {
        std::string Text = "abc123abc456abc";
        std::vector<std::string> Substrings = {"abc"};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "123456");
    }

    ///////////////////// Edge Scenarios /////////////////////

    // Removing with empty search substrings
    {
        std::string Text = "abc";
        std::vector<std::string> Substrings = {""};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "abc");
    }

    // Removing repeated overlapping patterns
    {
        std::string Text = "aaaabaaa";
        std::vector<std::string> Substrings = {"ba", "aa"};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "");
    }

    // Removing entire repeating pattern
    {
        std::string Text = "xyxyxyxy";
        std::vector<std::string> Substrings = {"xy"};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "");
    }

    // Removing single-letter substrings across entire text
    {
        std::string Text = "example";
        std::vector<std::string> Substrings = {"e"};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "xampl");
    }

    // Removing a substring that does not exist
    {
        std::string Text = "nothing changes";
        std::vector<std::string> Substrings = {"zzz"};
        TextCleaner::RemoveSubstringsFromSet(Text, Substrings);
        EXPECT_EQ(Text, "nothing changes");
    }
}

/////////////////////////////////////////////

TEST(TextCleaner, ExtractSubstringBetweenMarkers)
{
    /////////////////
    // Normal Cases
    /////////////////

    {
        SCOPED_TRACE("Normal Case: Extracts 'world' from between <marker> and </marker>");
        std::string Text = "Hello <marker>world</marker>!";
        std::string StartMarker = "<marker>";
        std::string EndMarker = "</marker>";
        EXPECT_EQ(TextCleaner::ExtractSubstringBetweenMarkers(Text, StartMarker, EndMarker), "world");
    }

    {
        SCOPED_TRACE("Normal Case: Extracts 'test' from between <data> and </data>");
        std::string Text = "This is a <data>test</data> case.";
        std::string StartMarker = "<data>";
        std::string EndMarker = "</data>";
        EXPECT_EQ(TextCleaner::ExtractSubstringBetweenMarkers(Text, StartMarker, EndMarker), "test");
    }

    /////////////////
    // Edge Cases
    /////////////////

    {
        SCOPED_TRACE("Edge Case: Start and end markers are the same");
        std::string Text = "<marker>content<marker> extra";
        std::string StartMarker = "<marker>";
        std::string EndMarker = "<marker>";
        EXPECT_EQ(TextCleaner::ExtractSubstringBetweenMarkers(Text, StartMarker, EndMarker), "content");
    }

    {
        SCOPED_TRACE("Edge Case: Substring at the end of the string");
        std::string Text = "<start>data<end>";
        std::string StartMarker = "<start>";
        std::string EndMarker = "<end>";
        EXPECT_EQ(TextCleaner::ExtractSubstringBetweenMarkers(Text, StartMarker, EndMarker), "data");
    }

    /////////////////
    // Invalid Inputs
    /////////////////

    {
        SCOPED_TRACE("Invalid Input: Start marker not found");
        std::string Text = "Hello <marker>world</marker>";
        std::string StartMarker = "<notfound>";
        std::string EndMarker = "</marker>";
        EXPECT_EQ(TextCleaner::ExtractSubstringBetweenMarkers(Text, StartMarker, EndMarker), "");
    }

    {
        SCOPED_TRACE("Invalid Input: End marker not found");
        std::string Text = "Hello <marker>world</marker>";
        std::string StartMarker = "<marker>";
        std::string EndMarker = "<missing>";
        EXPECT_EQ(TextCleaner::ExtractSubstringBetweenMarkers(Text, StartMarker, EndMarker), "");
    }

    /////////////////
    // Empty Structures
    /////////////////

    {
        SCOPED_TRACE("Empty Structure: Text is empty");
        std::string Text = "";
        std::string StartMarker = "<start>";
        std::string EndMarker = "<end>";
        EXPECT_EQ(TextCleaner::ExtractSubstringBetweenMarkers(Text, StartMarker, EndMarker), "");
    }

    {
        SCOPED_TRACE("Empty Structure: Both markers are empty");
        std::string Text = "Some random text";
        std::string StartMarker = "";
        std::string EndMarker = "";
        EXPECT_EQ(TextCleaner::ExtractSubstringBetweenMarkers(Text, StartMarker, EndMarker), "");
    }

    /////////////////
    // Maximums and Minimums
    /////////////////

    {
        SCOPED_TRACE("Maximum Input: Large string with embedded markers");
        std::string Text(1000000, 'A');
        std::string StartMarker = "<start>";
        std::string EndMarker = "</end>";
        Text.insert(500000, StartMarker);
        Text.append(EndMarker);
        EXPECT_EQ(TextCleaner::ExtractSubstringBetweenMarkers(Text, StartMarker, EndMarker), std::string(500000, 'A'));
    }

    {
        SCOPED_TRACE("Minimum Input: Single character text");
        std::string Text = "A";
        std::string StartMarker = "<start>";
        std::string EndMarker = "</end>";
        EXPECT_EQ(TextCleaner::ExtractSubstringBetweenMarkers(Text, StartMarker, EndMarker), "");
    }

    /////////////////
    // Substring Not Found
    /////////////////

    {
        SCOPED_TRACE("Substring Not Found: Markers not in text");
        std::string Text = "No markers here!";
        std::string StartMarker = "<start>";
        std::string EndMarker = "</end>";
        EXPECT_EQ(TextCleaner::ExtractSubstringBetweenMarkers(Text, StartMarker, EndMarker), "");
    }
}

TEST(TextCleaner, ReplaceSubstrings)
{
    {
        std::string Text = "abc abc abc";
        TextCleaner::ReplaceSubstrings(Text, "abc", "x");
        EXPECT_EQ(Text, "x x x");
    }

    {
        std::string Text = "aaa aaa aaa";
        TextCleaner::ReplaceSubstrings(Text, "a", "b");
        EXPECT_EQ(Text, "bbb bbb bbb");
    }

    {
        std::string Text = "Hello, World!";
        TextCleaner::ReplaceSubstrings(Text, "World", "Universe");
        EXPECT_EQ(Text, "Hello, Universe!");
    }

    {
        std::string Text = "Nothing to replace here";
        TextCleaner::ReplaceSubstrings(Text, "zzz", "abc");
        EXPECT_EQ(Text, "Nothing to replace here");
    }

    {
        std::string Text = "Replace me entirely";
        TextCleaner::ReplaceSubstrings(Text, "Replace me entirely", "Done");
        EXPECT_EQ(Text, "Done");
    }

    {
        std::string Text = "";
        TextCleaner::ReplaceSubstrings(Text, "any", "thing");
        EXPECT_EQ(Text, "");
    }

    {
        std::string Text = "abc";
        TextCleaner::ReplaceSubstrings(Text, "", "x");
        EXPECT_EQ(Text, "abc");
    }

    {
        std::string Text = "abcabcabc";
        TextCleaner::ReplaceSubstrings(Text, "abc", "abc");
        EXPECT_EQ(Text, "abcabcabc");
    }

    {
        std::string Text = "aaaa";
        TextCleaner::ReplaceSubstrings(Text, "aa", "b");
        EXPECT_EQ(Text, "bb");
    }

    {
        std::string Text = "aaaa";
        TextCleaner::ReplaceSubstrings(Text, "a", "");
        EXPECT_EQ(Text, "");
    }

    {
        std::string Text = "abcabc";
        TextCleaner::ReplaceSubstrings(Text, "abc", "abcd");
        EXPECT_EQ(Text, "abcdabcd");
    }

    {
        std::string Text = "ababab";
        TextCleaner::ReplaceSubstrings(Text, "ab", "ba");
        EXPECT_EQ(Text, "bababa");
    }

    {
        std::string Text = "This is a test.";
        TextCleaner::ReplaceSubstrings(Text, " ", "_");
        EXPECT_EQ(Text, "This_is_a_test.");
    }

    {
        std::string Text = "aaaabaaa";
        TextCleaner::ReplaceSubstrings(Text, "aa", "x");
        EXPECT_EQ(Text, "xxbxa");
    }

    {
        std::string Text = "Hello";
        TextCleaner::ReplaceSubstrings(Text, "Hello", "");
        EXPECT_EQ(Text, "");
    }
}

//////////////////////////////////


TEST(TextCleaner, ReplaceSubstringsFromSet)
{
    {
        std::string Text = "abc def ghi";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {"abc", "x"},
            {"def", "y"}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "x y ghi");
    }

    {
        std::string Text = "aaaa bbbb cccc";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {"a", "b"},
            {"b", "c"}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "cccc cccc cccc");
    }

    {
        std::string Text = "Hello World";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {"World", "Universe"},
            {"Hello", "Hi"}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "Hi Universe");
    }

    {
        std::string Text = "No replacements here.";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {"zzz", "xxx"}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "No replacements here.");
    }

    {
        std::string Text = "Same replacement";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {"Same replacement", "Done"}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "Done");
    }

    {
        std::string Text = "";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {"a", "b"}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "");
    }

    {
        std::string Text = "abc";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {"", "x"}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "abc");
    }

    {
        std::string Text = "abc abc";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {"abc", "abc"}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "abc abc");
    }

    {
        std::string Text = "aaaa";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {"aa", "b"}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "bb");
    }

    {
        std::string Text = "aaaa";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {"a", ""}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "");
    }

    {
        std::string Text = "abcabc";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {"abc", "abcd"}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "abcdabcd");
    }

    {
        std::string Text = "abab";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {"ab", "ba"}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "baba");
    }

    {
        std::string Text = "This is a test.";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {" ", "_"}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "This_is_a_test.");
    }

    {
        std::string Text = "aabbcc";
        std::vector<std::pair<std::string, std::string>> Replacements = {
            {"aa", "x"},
            {"bb", "y"},
            {"cc", "z"}
        };
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "xyz");
    }

    {
        std::string Text = "abc abc";
        std::vector<std::pair<std::string, std::string>> Replacements = {};
        TextCleaner::ReplaceSubstringsFromSet(Text, Replacements);
        EXPECT_EQ(Text, "abc abc");
    }
}

/////////////////////////////////////

TEST(TextCleaner, ReplaceConsecutiveCharsWithSingle)
{
    ////////////////////////////////////////////////////////////////////////////////
    // Typical scenarios
    ////////////////////////////////////////////////////////////////////////////////

    // Deduplicating consecutive spaces
    {
        std::string Text = "This  is   a test";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, ' ');
        EXPECT_EQ(Text, "This is a test");
    }

    // Deduplicating consecutive dashes
    {
        std::string Text = "word---word--word";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, '-');
        EXPECT_EQ(Text, "word-word-word");
    }

    // Mixed characters with duplicated target characters
    {
        std::string Text = "a..b....c.d.";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, '.');
        EXPECT_EQ(Text, "a.b.c.d.");
    }

    // Deduplicating tabs
    {
        std::string Text = "a\t\tb\t\t\tc";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, '\t');
        EXPECT_EQ(Text, "a\tb\tc");
    }

    // Internal and edge duplicates
    {
        std::string Text = "---a--b---";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, '-');
        EXPECT_EQ(Text, "-a-b-");
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Boundary scenarios
    ////////////////////////////////////////////////////////////////////////////////

    // Empty string
    {
        std::string Text = "";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, ' ');
        EXPECT_EQ(Text, "");
    }

    // String without any matching characters
    {
        std::string Text = "abcdef";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, 'x');
        EXPECT_EQ(Text, "abcdef");
    }

    // Single-character string (non-target)
    {
        std::string Text = "A";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, ' ');
        EXPECT_EQ(Text, "A");
    }

    // Single-character string (target)
    {
        std::string Text = " ";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, ' ');
        EXPECT_EQ(Text, " ");
    }

    // All characters are the same as target
    {
        std::string Text = "     ";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, ' ');
        EXPECT_EQ(Text, " ");
    }

    // Target character only at the beginning
    {
        std::string Text = "   abc";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, ' ');
        EXPECT_EQ(Text, " abc");
    }

    // Target character only at the end
    {
        std::string Text = "abc   ";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, ' ');
        EXPECT_EQ(Text, "abc ");
    }

    // Target character in alternation with others (no duplicates)
    {
        std::string Text = "-a-b-c-";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, '-');
        EXPECT_EQ(Text, "-a-b-c-");
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Edge scenarios
    ////////////////////////////////////////////////////////////////////////////////

    // Alternating target and non-target in dense sequence
    {
        std::string Text = "-a--b---c----d-----";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, '-');
        EXPECT_EQ(Text, "-a-b-c-d-");
    }

    // Multiple different characters, only some repeated target characters
    {
        std::string Text = "X==X===Y====Z";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, '=');
        EXPECT_EQ(Text, "X=X=Y=Z");
    }

    // Target character appears as every second character
    {
        std::string Text = "a-a-a-a-";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, '-');
        EXPECT_EQ(Text, "a-a-a-a-");
    }

    // Only two target characters
    {
        std::string Text = "--";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, '-');
        EXPECT_EQ(Text, "-");
    }

    // Target character inside numeric input
    {
        std::string Text = "1,,2,,,3";
        TextCleaner::ReplaceConsecutiveCharsWithSingle(Text, ',');
        EXPECT_EQ(Text, "1,2,3");
    }
}

//////////////////////////////////////


TEST(TextCleaner, RemoveRange)
{
    // Test 1: Erase the entire text
    {
        std::string Text = "Erase me completely";
        TextCleaner::RemoveRange(Text, 0, Text.size());
        EXPECT_EQ(Text, "");
    }

    // Test 2: Erase from the start to a specific middle position
    {
        std::string Text = "Hello, this is a sample text for testing.";
        size_t Middle = 13; // up to "Hello, this is "
        std::string Expected = Text.substr(Middle);
        TextCleaner::RemoveRange(Text, 0, Middle);
        EXPECT_EQ(Text, Expected);
    }

    // Test 3: Erase from a middle position to the end
    {
        std::string Text = "A short example of text content.";
        size_t Middle = 10;
        std::string Expected = Text.substr(0, Middle);
        TextCleaner::RemoveRange(Text, Middle, Text.size());
        EXPECT_EQ(Text, Expected);
    }

    // Test 4: Erase a single character in the middle
    {
        std::string Text = "This text has a single character to remove.";
        size_t Middle = 10;
        std::string Expected = Text.substr(0, Middle) + Text.substr(Middle + 1);
        TextCleaner::RemoveRange(Text, Middle, Middle + 1);
        EXPECT_EQ(Text, Expected);
    }

    // Test 5: Erase a specific range (10 characters in the middle)
    {
        std::string Text = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        size_t Start = 10;
        size_t End = 20;
        std::string Expected = Text.substr(0, Start) + Text.substr(End);
        TextCleaner::RemoveRange(Text, Start, End);
        EXPECT_EQ(Text, Expected);
    }

    // Test 6: Erase a single character at the start
    {
        std::string Text = "First character removed.";
        std::string Expected = Text.substr(1);
        TextCleaner::RemoveRange(Text, 0, 1);
        EXPECT_EQ(Text, Expected);
    }

    // Test 7: Erase a single character at the end
    {
        std::string Text = "Remove last.";
        std::string Expected = Text.substr(0, Text.size() - 1);
        TextCleaner::RemoveRange(Text, Text.size() - 1, Text.size());
        EXPECT_EQ(Text, Expected);
    }

    // Test 8: Erase zero-length range (no-op)
    {
        std::string Text = "No change expected here.";
        std::string Expected = Text;
        TextCleaner::RemoveRange(Text, 10, 10);
        EXPECT_EQ(Text, Expected);
    }
}


//////////////////////////////////////
TEST(TextCleaner, FindFirstSubstringInRange)
{
    // Test 1: Simple exact match at the beginning
    {
        std::string Text = "\nBegin something here";
        std::vector<std::string> Substrings = { "Begin" };
        auto Range = std::make_pair(0, Text.size());
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first != std::string::npos, true);
        EXPECT_EQ(Result.first, 1);
        EXPECT_EQ(Result.second, 5);
    }

    // Test 2: Match in the middle
    {
        std::string Text = "This text has a FunctionReference marker";
        std::vector<std::string> Substrings = { "FunctionReference" };
        auto Range = std::make_pair(0, Text.size());
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first != std::string::npos, true);
        EXPECT_EQ(Result.second, std::string("FunctionReference").length());
    }

    // Test 3: Match at the end
    {
        std::string Text = "This ends with End Object";
        std::vector<std::string> Substrings = { "End Object" };
        auto Range = std::make_pair(0, Text.size());
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first != std::string::npos, true);
        EXPECT_EQ(Result.second, 10);
    }

    // Test 4: Multiple substrings; closest match first
    {
        std::string Text = "\nBegin here and then FunctionReference there and finally End Object";
        std::vector<std::string> Substrings = { "Begin", "FunctionReference", "End Object" };
        auto Range = std::make_pair(0, Text.size());
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first, 1);
        EXPECT_EQ(Result.second, 5);
    }

    // Test 5: No substring match
    {
        std::string Text = "Completely different text.";
        std::vector<std::string> Substrings = { "NonExistentSubstring" };
        auto Range = std::make_pair(0, Text.size());
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first, std::string::npos);
        EXPECT_EQ(Result.second, 0);
    }

    // Test 6: Empty substring list
    {
        std::string Text = "Any text here";
        std::vector<std::string> Substrings = { };
        auto Range = std::make_pair(0, Text.size());
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first, std::string::npos);
        EXPECT_EQ(Result.second, 0);
    }

    // Test 7: Empty text
    {
        std::string Text = "";
        std::vector<std::string> Substrings = { "Begin" };
        auto Range = std::make_pair(0, Text.size());
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first, std::string::npos);
        EXPECT_EQ(Result.second, 0);
    }

    // Test 8: Range out of bounds (start index too large)
    {
        std::string Text = "Some text";
        std::vector<std::string> Substrings = { "Begin" };
        auto Range = std::make_pair(Text.size() + 10, Text.size() + 20);
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first, std::string::npos);
        EXPECT_EQ(Result.second, 0);
    }

    // Test 9: Range reversed (invalid range)
    {
        std::string Text = "Some text";
        std::vector<std::string> Substrings = { "Begin" };
        auto Range = std::make_pair(10, 5);
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first, std::string::npos);
        EXPECT_EQ(Result.second, 0);
    }

    // Test 10: Valid range within a segment of text
    {
        std::string Text = "Here is a PinName followed by other words.";
        std::vector<std::string> Substrings = { "PinName" };
        auto Range = std::make_pair(0, 500);
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first != std::string::npos, true);
        EXPECT_EQ(Result.second, std::string("PinName").length());
    }

    // Test 11: Substring ends outside range
    {
        std::string Text = "FunctionReference at position 25";
        std::vector<std::string> Substrings = { "FunctionReference" };
        auto Range = std::make_pair(0, 10);
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first, 0);
        EXPECT_EQ(Result.second, std::string("FunctionReference").length());
    }

    // Test 12: Multiple substrings, second match is closer
    {
        std::string Text = "Here is a FunctionReference example";
        std::vector<std::string> Substrings = { "Nonexistent", "FunctionReference" };
        auto Range = std::make_pair(0, Text.size());
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first != std::string::npos, true);
        EXPECT_EQ(Result.second, std::string("FunctionReference").length());
    }

    // Test 13: Range is a single character (should find)
    {
        std::string Text = "Begin here";
        std::vector<std::string> Substrings = { "Begin" };
        auto Range = std::make_pair(0, 1);
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first, 0);
        EXPECT_EQ(Result.second, std::string("Begin").length());
    }

    // Test 14: Substring within narrow range
    {
        std::string Text = "Here is a PersistentGuid example";
        size_t Pos = Text.find("PersistentGuid");
        std::vector<std::string> Substrings = { "PersistentGuid" };
        auto Range = std::make_pair(Pos, Pos + 20);
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first, Pos);
        EXPECT_EQ(Result.second, std::string("PersistentGuid").length());
    }

    // Test 15: Range ends exactly at substring boundary
    {
        std::string Text = "The final section: End Object";
        size_t Pos = Text.find("End Object");
        std::vector<std::string> Substrings = { "End Object" };
        auto Range = std::make_pair(Pos, Pos + std::string("End Object").length());
        auto Result = TextCleaner::FindFirstSubstringInRange(Text, Substrings, Range);
        EXPECT_EQ(Result.first, Pos);
        EXPECT_EQ(Result.second, std::string("End Object").length());
    }
}

///////////////////////////////////////////

TEST(TextCleaner, RemoveSubstringStartingFromEndingWithBeforeDelimiter)
{
    // Common delimiter: space and comma
    const std::vector<std::string> Delimiters = { " ", "," };

    // Test 1: Empty marker → no change
    {
        std::string Text = "Sample text";
        std::string Marker = "";
        std::vector<std::string> Endings = { "END" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "Sample text");
    }

    // Test 2: Empty endings list → no change
    {
        std::string Text = "Sample text";
        std::string Marker = "MARK";
        std::vector<std::string> Endings = { };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "Sample text");
    }

    // Test 3: Empty text → no change
    {
        std::string Text = "";
        std::string Marker = "MARK";
        std::vector<std::string> Endings = { "END" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "");
    }

    // Test 4: No marker present → no change
    {
        std::string Text = "Nothing to remove here.";
        std::string Marker = "MARKER";
        std::vector<std::string> Endings = { "END" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "Nothing to remove here.");
    }

    // Test 5: Simple removal without delimiter
    {
        std::string Text = "prefixMARKERfooENDsuffix";
        std::string Marker = "MARKER";
        std::vector<std::string> Endings = { "END" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "prefixsuffix");
    }

    // Test 6: Multiple markers → both segments removed
    {
        std::string Text = "A_MARKERxEND_and_MARKERyEND_B";
        std::string Marker = "_MARKER";
        std::vector<std::string> Endings = { "END" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "A_and_B");
    }

    // Test 7: Delimiter after ending → removal still occurs
    {
        std::string Text = "xMARKERfooEND,y";
        std::string Marker = "MARKER";
        std::vector<std::string> Endings = { "END" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "x,y");
    }

    // Test 8: Ending only after delimiter → no removal
    {
        std::string Text = "A_MARKERfoo,yEND";
        std::string Marker = "_MARKER";
        std::vector<std::string> Endings = { "END" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "A_MARKERfoo,yEND");
    }

    // Test 9: Marker at end with no ending → no change
    {
        std::string Text = "justMARKER";
        std::string Marker = "MARKER";
        std::vector<std::string> Endings = { "END" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "justMARKER");
    }

    // Test 10: Ending equals marker → only marker is removed
    {
        std::string Text = "fooMARKERbar";
        std::string Marker = "MARKER";
        std::vector<std::string> Endings = { "MARKER" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "foobar");
    }

    // Test 11: Two different endings, two segments removed
    {
        std::string Text = "xMARKERaEND2yMARKERbEND1z";
        std::string Marker = "MARKER";
        std::vector<std::string> Endings = { "END1", "END2" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "xyz");
    }

    // Test 12: Back-to-back removals → result is empty
    {
        std::string Text = "MARKERfooENDMARKERbarEND";
        std::string Marker = "MARKER";
        std::vector<std::string> Endings = { "END" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "");
    }

    // Test 13: Prefix and suffix around one marker → correct removal
    {
        std::string Text = "helloMARKER123ENDworld";
        std::string Marker = "MARKER";
        std::vector<std::string> Endings = { "END" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "helloworld");
    }

    // Test 14: Removal with delimiter as space
    {
        std::string Text = "startMARKERfooEND suffix";
        std::string Marker = "MARKER";
        std::vector<std::string> Endings = { "END" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "start suffix");
    }

    // Test 15: Nested markers, only outer segment removed
    {
        std::string Text = "MARKERaMARKERbENDEND";
        std::string Marker = "MARKER";
        std::vector<std::string> Endings = { "END" };
        TextCleaner::RemoveSubstringStartingFromEndingWithBeforeDelimiter(Text, Marker, Endings, Delimiters);
        EXPECT_EQ(Text, "END");
    }
}

///////////////////////////////////////////


TEST(TextCleaner, RemoveSubstringInRange)
{
    // Test normal case - removal in the middle
    {
        std::string Text = "Hello world";
        size_t StartPos = 3;
        size_t StopPos = 8;
        TextCleaner::RemoveSubstringInRange(Text, { StartPos, StopPos });
        EXPECT_EQ(Text, "Helrld");
    }

    // Test removal from start of string
    {
        std::string Text = "abcdef";
        size_t StartPos = 0;
        size_t StopPos = 3;
        TextCleaner::RemoveSubstringInRange(Text, { StartPos, StopPos });
        EXPECT_EQ(Text, "def");
    }

    // Test removal to end of string (StopPos at last character)
    {
        std::string Text = "abcdef";
        size_t StartPos = 2;
        size_t StopPos = 5;
        TextCleaner::RemoveSubstringInRange(Text, { StartPos, StopPos });
        EXPECT_EQ(Text, "abf");
    }

    // Test StartPos == StopPos (no removal)
    {
        std::string Text = "abcdef";
        size_t StartPos = 3;
        size_t StopPos = 3;
        TextCleaner::RemoveSubstringInRange(Text, { StartPos, StopPos });
        EXPECT_EQ(Text, "abcdef");
    }

    // Test StartPos > StopPos (invalid range, no removal)
    {
        std::string Text = "abcdef";
        size_t StartPos = 5;
        size_t StopPos = 3;
        TextCleaner::RemoveSubstringInRange(Text, { StartPos, StopPos });
        EXPECT_EQ(Text, "abcdef");
    }

    // Test StopPos beyond text size (no removal)
    {
        std::string Text = "abcdef";
        size_t StartPos = 2;
        size_t StopPos = 10;
        TextCleaner::RemoveSubstringInRange(Text, { StartPos, StopPos });
        EXPECT_EQ(Text, "abcdef");
    }

    // Test complete removal (entire string)
    {
        std::string Text = "abc";
        size_t StartPos = 0;
        size_t StopPos = 2;
        TextCleaner::RemoveSubstringInRange(Text, { StartPos, StopPos });
        EXPECT_EQ(Text, "c");
    }

    // Test empty string
    {
        std::string Text = "";
        size_t StartPos = 0;
        size_t StopPos = 0;
        TextCleaner::RemoveSubstringInRange(Text, { StartPos, StopPos });
        EXPECT_EQ(Text, "");
    }
}

TEST(TextCleaner, RemoveSubstringFromMarkerTillDelimiter)
{
    // Helper lambda to define delimiters
    const std::vector<std::string> IsStopChar = { ",", " " };

    // Test normal removal from marker to first delimiter
    {
        std::string Text = "This is a test: remove this part, and leave this.";
        size_t StartPos = 0;
        std::string Marker = "test:";
        size_t Result = TextCleaner::RemoveSubstringFromMarkerTillDelimiter(Text, StartPos, Marker, { "," });
        EXPECT_EQ(Result, 10);
        EXPECT_EQ(Text, "This is a , and leave this.");
    }

    // Test marker not found
    {
        std::string Text = "No marker here.";
        size_t StartPos = 0;
        std::string Marker = "missing";
        size_t Result = TextCleaner::RemoveSubstringFromMarkerTillDelimiter(Text, StartPos, Marker, IsStopChar);
        EXPECT_EQ(Result, std::string::npos);
        EXPECT_EQ(Text, "No marker here.");
    }

    // Test marker at the end of the string
    {
        std::string Text = "End marker here: marker";
        size_t StartPos = 0;
        std::string Marker = ": marker";
        size_t Result = TextCleaner::RemoveSubstringFromMarkerTillDelimiter(Text, StartPos, Marker, IsStopChar);
        EXPECT_EQ(Result, Text.size());
        EXPECT_EQ(Text, "End marker here");
    }

    // Test multiple markers - ensure correct marker is removed
    {
        std::string Text = "First marker remove, then marker remove again";
        size_t StartPos = 10;
        std::string Marker = "marker";
        size_t Result = TextCleaner::RemoveSubstringFromMarkerTillDelimiter(Text, StartPos, Marker, IsStopChar);
        EXPECT_EQ(Result, 26);
        EXPECT_EQ(Text, "First marker remove, then  remove again");
    }

    // Test removal when delimiter immediately follows marker
    {
        std::string Text = "Text before marker, next.";
        size_t StartPos = 0;
        std::string Marker = "marker";
        size_t Result = TextCleaner::RemoveSubstringFromMarkerTillDelimiter(Text, StartPos, Marker, IsStopChar);
        EXPECT_EQ(Result, 12);
        EXPECT_EQ(Text, "Text before , next.");
    }

    // Test empty string (no removal)
    {
        std::string Text = "";
        size_t StartPos = 0;
        std::string Marker = "marker";
        size_t Result = TextCleaner::RemoveSubstringFromMarkerTillDelimiter(Text, StartPos, Marker, IsStopChar);
        EXPECT_EQ(Result, std::string::npos);
        EXPECT_EQ(Text, "");
    }

    // Test marker at the very beginning of the string
    {
        std::string Text = "marker content to remove and more";
        size_t StartPos = 0;
        std::string Marker = "marker";
        size_t Result = TextCleaner::RemoveSubstringFromMarkerTillDelimiter(Text, StartPos, Marker, {"a"});
        EXPECT_EQ(Result, 0);
        EXPECT_EQ(Text, "and more");
    }

    // Test no delimiter found after marker (remove to end)
    {
        std::string Text = "Text marker removalToEnd";
        size_t StartPos = 0;
        std::string Marker = "marker";
        // auto NoStopChar = [](char) { return false; }; // No delimiter
        size_t Result = TextCleaner::RemoveSubstringFromMarkerTillDelimiter(Text, StartPos, Marker, { });
        EXPECT_EQ(Result, 5);
        EXPECT_EQ(Text, "Text ");
    }
}

TEST(TextCleaner, RemoveAllSubstringsFromMarkerTillDelimiter)
{
    // Test removing multiple substrings in a text
    {
        std::string Text = "abc START1234 STOP def START5678 STOP ghi";
        TextCleaner::RemoveAllSubstringsFromMarkerTillDelimiter(Text, "START", {" "});
        EXPECT_EQ(Text, "abc  STOP def  STOP ghi");
    }

    // Test no marker found in text
    {
        std::string Text = "abc def ghi";
        TextCleaner::RemoveAllSubstringsFromMarkerTillDelimiter(Text, "MARKER", {" "});
        EXPECT_EQ(Text, "abc def ghi");
    }

    // Test marker at end of string
    {
        std::string Text = "abc def START";
        TextCleaner::RemoveAllSubstringsFromMarkerTillDelimiter(Text, "START", {" "});
        EXPECT_EQ(Text, "abc def ");
    }

    // Test overlapping markers
    {
        std::string Text = "abc START1 START2 STOP def";
        TextCleaner::RemoveAllSubstringsFromMarkerTillDelimiter(Text, "START", {" "});
        EXPECT_EQ(Text, "abc   STOP def");
    }

    // Test delimiter not found after marker (remove till end)
    {
        std::string Text = "abc START123";
        TextCleaner::RemoveAllSubstringsFromMarkerTillDelimiter(Text, "START", {" "});
        EXPECT_EQ(Text, "abc ");
    }

    // Test multiple markers without delimiters
    {
        std::string Text = "abc START1 def START2 ghi";
        TextCleaner::RemoveAllSubstringsFromMarkerTillDelimiter(Text, "START", {","});
        EXPECT_EQ(Text, "abc ");
    }

    // Test whitespace delimiters
    {
        std::string Text = "START1 abc START2 def";
        TextCleaner::RemoveAllSubstringsFromMarkerTillDelimiter(Text, "START", {" "});
        EXPECT_EQ(Text, " abc  def");
    }

    // Test only marker present, no delimiter (remove marker only)
    {
        std::string Text = "abc START";
        TextCleaner::RemoveAllSubstringsFromMarkerTillDelimiter(Text, "START", {" "});
        EXPECT_EQ(Text, "abc ");
    }

    // Test empty string
    {
        std::string Text = "";
        TextCleaner::RemoveAllSubstringsFromMarkerTillDelimiter(Text, "START", {" "});
        EXPECT_EQ(Text, "");
    }

    // Test marker at start of string
    {
        std::string Text = "START123 abc";
        TextCleaner::RemoveAllSubstringsFromMarkerTillDelimiter(Text, "START", {" "});
        EXPECT_EQ(Text, " abc");
    }
}

TEST(TextCleaner, RemoveAllSubstringsFromAnyMarkerTillDelimiter)
{
    // Test multiple markers with distinct appearances
    {
        std::string Text = "abc START1 123 STOP def START2 456 STOP ghi";
        std::vector<std::string> Markers = { "START1", "START2" };
        TextCleaner::RemoveAllSubstringsFromAnyMarkerTillDelimiter(Text, Markers, {" "});
        EXPECT_EQ(Text, "abc  123 STOP def  456 STOP ghi");
    }

    // Test markers with partial overlaps in text
    {
        std::string Text = "abc M1 123 M2 456 def";
        std::vector<std::string> Markers = { "M1", "M2" };
        TextCleaner::RemoveAllSubstringsFromAnyMarkerTillDelimiter(Text, Markers, {" "});
        EXPECT_EQ(Text, "abc  123  456 def");
    }

    // Test no markers in the text
    {
        std::string Text = "abc def ghi";
        std::vector<std::string> Markers = { "X", "Y" };
        TextCleaner::RemoveAllSubstringsFromAnyMarkerTillDelimiter(Text, Markers, {" "});
        EXPECT_EQ(Text, "abc def ghi");
    }

    // Test marker at the very beginning
    {
        std::string Text = "START-data-here abc";
        std::vector<std::string> Markers = { "START" };
        TextCleaner::RemoveAllSubstringsFromAnyMarkerTillDelimiter(Text, Markers, {" "});
        EXPECT_EQ(Text, " abc");
    }

    // Test multiple markers, some overlapping
    {
        std::string Text = "abc marker1 def marker2 ghi marker1 xyz";
        std::vector<std::string> Markers = { "marker1", "marker2" };
        TextCleaner::RemoveAllSubstringsFromAnyMarkerTillDelimiter(Text, Markers, {" "});
        EXPECT_EQ(Text, "abc  def  ghi  xyz");
    }

    // Test markers with no delimiter found (remove to end)
    {
        std::string Text = "abc marker1 content marker2";
        std::vector<std::string> Markers = { "marker1", "marker2" };
        TextCleaner::RemoveAllSubstringsFromAnyMarkerTillDelimiter(Text, Markers, { }); // no delimiter
        EXPECT_EQ(Text, "abc ");
    }

    // Test markers at the end of text
    {
        std::string Text = "abc def marker";
        std::vector<std::string> Markers = { "marker" };
        TextCleaner::RemoveAllSubstringsFromAnyMarkerTillDelimiter(Text, Markers, {" "});
        EXPECT_EQ(Text, "abc def ");
    }

    // Test overlapping marker names
    {
        std::string Text = "abc MARKMARKER rest";
        std::vector<std::string> Markers = { "MARK", "MARKER" };
        TextCleaner::RemoveAllSubstringsFromAnyMarkerTillDelimiter(Text, Markers, {" "});
        EXPECT_EQ(Text, "abc  rest");
    }

    // Test empty text
    {
        std::string Text = "";
        std::vector<std::string> Markers = { "MARK" };
        TextCleaner::RemoveAllSubstringsFromAnyMarkerTillDelimiter(Text, Markers, {" "});
        EXPECT_EQ(Text, "");
    }

    // Test empty marker set
    {
        std::string Text = "abc START def";
        std::vector<std::string> Markers;
        TextCleaner::RemoveAllSubstringsFromAnyMarkerTillDelimiter(Text, Markers, {" "});
        EXPECT_EQ(Text, "abc START def");
    }
}
