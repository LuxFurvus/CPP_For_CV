
#include <gtest/gtest.h>
////////
#include "SQL_StatementSeparator.h"


// ================================
// ░ Test Case Descriptions ░
// ================================

// T01: correctly parses single statement terminated by semicolon
// T02: correctly parses multiple semicolon-terminated statements
// T03: correctly ignores semicolon inside single quotes
// T04: correctly ignores semicolon inside double quotes
// T05: correctly ignores semicolon inside line comment (--)
// T06: correctly ignores semicolon inside block comment (/* */)
// T07: correctly includes final statement even without semicolon
// T08: correctly returns empty output for input with only whitespace
// T09: correctly returns empty output for empty string
// T10: correctly handles mixture of comments, quotes, and statements
// T11: does not crash or split incorrectly with unclosed single quote
// T12: does not crash or split incorrectly with unclosed double quote
// T13: does not crash or split incorrectly with unclosed block comment
// T14: correctly parses statement between two block comments
// T15: correctly preserves line comment at end and parses statement



////////////////////////////////////////////////////////////////
// T01: correctly parses single statement terminated by semicolon
////////////////////////////////////////////////////////////////
TEST(SQL_StatementSeparator, T01_SingleStatement)
{
    SCOPED_TRACE("Single SQL statement with semicolon should be parsed correctly");
    std::string Input = "SELECT * FROM test;";
    auto Result = SQL_StatementSeparator(Input).SplitStatements();
    ASSERT_EQ(Result.size(), 1);
    ASSERT_EQ(Result[0], "SELECT * FROM test");
}

////////////////////////////////////////////////////////////////
// T02: correctly parses multiple semicolon-terminated statements
////////////////////////////////////////////////////////////////
TEST(SQL_StatementSeparator, T02_MultipleStatements)
{
    SCOPED_TRACE("Two SQL statements each with semicolon should be split");
    std::string Input = "SELECT * FROM test1;SELECT * FROM test2;";
    auto Result = SQL_StatementSeparator(Input).SplitStatements();
    ASSERT_EQ(Result.size(), 2);
    ASSERT_EQ(Result[0], "SELECT * FROM test1");
    ASSERT_EQ(Result[1], "SELECT * FROM test2");
}

////////////////////////////////////////////////////////////////
// T03: correctly ignores semicolon inside single quotes
////////////////////////////////////////////////////////////////
TEST(SQL_StatementSeparator, T03_SemicolonInsideSingleQuotes)
{
    SCOPED_TRACE("Semicolon inside single quotes should not split statement");
    std::string Input = "INSERT INTO test VALUES('abc;def');";
    auto Result = SQL_StatementSeparator(Input).SplitStatements();
    ASSERT_EQ(Result.size(), 1);
    ASSERT_EQ(Result[0], "INSERT INTO test VALUES('abc;def')");
}

////////////////////////////////////////////////////////////////
// T04: correctly ignores semicolon inside double quotes
////////////////////////////////////////////////////////////////
TEST(SQL_StatementSeparator, T04_SemicolonInsideDoubleQuotes)
{
    SCOPED_TRACE("Semicolon inside double quotes should not split statement");
    std::string Input = "SELECT \"ab;cd\" FROM test;";
    auto Result = SQL_StatementSeparator(Input).SplitStatements();
    ASSERT_EQ(Result.size(), 1);
    ASSERT_EQ(Result[0], "SELECT \"ab;cd\" FROM test");
}

////////////////////////////////////////////////////////////////
// T05: correctly skips single-line and block comments
////////////////////////////////////////////////////////////////
TEST(SQL_StatementSeparator, T05_CommentSkipping)
{
    SCOPED_TRACE("Comments should not affect statement detection");
    std::string Input = "SELECT 1; -- comment\n/* block */ SELECT 2;";
    auto Result = SQL_StatementSeparator(Input).SplitStatements();
    ASSERT_EQ(Result.size(), 2);
    ASSERT_EQ(Result[0], "SELECT 1");
    ASSERT_EQ(Result[1], "SELECT 2");
}

////////////////////////////////////////////////////////////////
// T06: correctly handles comment-like content inside quotes
////////////////////////////////////////////////////////////////
TEST(SQL_StatementSeparator, T06_CommentMarkersInsideQuotes)
{
    SCOPED_TRACE("Comment markers inside quotes must not be parsed as comments");
    std::string Input = "SELECT '-- not comment'; SELECT '/* still not comment */';";
    auto Result = SQL_StatementSeparator(Input).SplitStatements();
    ASSERT_EQ(Result.size(), 2);
    ASSERT_EQ(Result[0], "SELECT '-- not comment'");
    ASSERT_EQ(Result[1], "SELECT '/* still not comment */'");
}

////////////////////////////////////////////////////////////////
// T07: correctly includes final statement even without semicolon
////////////////////////////////////////////////////////////////
TEST(SQL_StatementSeparator, T07_StatementWithoutTerminator)
{
    SCOPED_TRACE("Final statement without semicolon should still be captured");
    std::string Input = "SELECT * FROM test";
    auto Result = SQL_StatementSeparator(Input).SplitStatements();
    ASSERT_EQ(Result.size(), 1);
    ASSERT_EQ(Result[0], "SELECT * FROM test");
}

////////////////////////////////////////////////////////////////
// T08: correctly returns empty output for input with only whitespace
////////////////////////////////////////////////////////////////
TEST(SQL_StatementSeparator, T08_WhitespaceOnly)
{
    SCOPED_TRACE("Whitespace-only input should result in empty output");
    std::string Input = "   \t \n ";
    auto Result = SQL_StatementSeparator(Input).SplitStatements();
    ASSERT_TRUE(Result.empty());
}

////////////////////////////////////////////////////////////////
// T09: correctly returns empty output for empty string
////////////////////////////////////////////////////////////////
TEST(SQL_StatementSeparator, T09_EmptyInput)
{
    SCOPED_TRACE("Empty input string should return empty output");
    std::string Input;
    auto Result = SQL_StatementSeparator(Input).SplitStatements();
    ASSERT_TRUE(Result.empty());
}

////////////////////////////////////////////////////////////////
// T10: correctly skips redundant semicolons
////////////////////////////////////////////////////////////////
TEST(SQL_StatementSeparator, T10_MultipleSemicolons)
{
    SCOPED_TRACE("Multiple semicolons in a row should not produce empty statements");
    std::string Input = "SELECT 1;;;SELECT 2;;";
    auto Result = SQL_StatementSeparator(Input).SplitStatements();
    ASSERT_EQ(Result.size(), 2);
    ASSERT_EQ(Result[0], "SELECT 1");
    ASSERT_EQ(Result[1], "SELECT 2");
}

////////////////////////////////////////////////////////////////
// T11: does not crash or split incorrectly with unclosed single quote
////////////////////////////////////////////////////////////////
TEST(SQL_StatementSeparator, T11_UnclosedSingleQuote)
{
    SCOPED_TRACE("Unclosed single quote should not crash and be treated as literal");
    std::string Input = "INSERT INTO test VALUES('abc;";
    auto Result = SQL_StatementSeparator(Input).SplitStatements();
    ASSERT_EQ(Result.size(), 1);
    ASSERT_EQ(Result[0], "INSERT INTO test VALUES('abc;");
}

////////////////////////////////////////////////////////////////
// T12: does not crash or split incorrectly with unclosed double quote
////////////////////////////////////////////////////////////////
TEST(SQL_StatementSeparator, T12_UnclosedDoubleQuote)
{
    SCOPED_TRACE("Unclosed double quote should not crash and be treated as literal");
    std::string Input = "SELECT \"abc;";
    auto Result = SQL_StatementSeparator(Input).SplitStatements();
    ASSERT_EQ(Result.size(), 1);
    ASSERT_EQ(Result[0], "SELECT \"abc;");
}

////////////////////////////////////////////////////////////////
// T13: correctly handles mixed line endings (LF, CR, CRLF)
////////////////////////////////////////////////////////////////
TEST(SQL_StatementSeparator, T13_MixedNewlines)
{
    SCOPED_TRACE("Mixed line endings must be handled correctly");
    std::string Input = "SELECT 1;\nSELECT 2;\rSELECT 3;\r\nSELECT 4;";
    auto Result = SQL_StatementSeparator(Input).SplitStatements();
    ASSERT_EQ(Result.size(), 4);
    ASSERT_EQ(Result[0], "SELECT 1");
    ASSERT_EQ(Result[1], "SELECT 2");
    ASSERT_EQ(Result[2], "SELECT 3");
    ASSERT_EQ(Result[3], "SELECT 4");
}


