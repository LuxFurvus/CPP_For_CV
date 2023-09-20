#include "pch.h"

#include "unrep_test_reformer.h"
#include "unrep_test_tokensearch.h"
#include<string>

// Test the IsSpace function
TEST(TextReformerTest, IsSpaceTest) {
	// Test regular space characters
	EXPECT_TRUE(TextReformer::getInstance().IsSpace(' '));
	EXPECT_TRUE(TextReformer::getInstance().IsSpace('\t'));
	EXPECT_TRUE(TextReformer::getInstance().IsSpace('\n'));
	EXPECT_TRUE(TextReformer::getInstance().IsSpace('\r'));
	EXPECT_TRUE(TextReformer::getInstance().IsSpace('\v'));
	EXPECT_TRUE(TextReformer::getInstance().IsSpace('\f'));

	// Test non-space characters
	EXPECT_FALSE(TextReformer::getInstance().IsSpace('A'));
	EXPECT_FALSE(TextReformer::getInstance().IsSpace('1'));
	EXPECT_FALSE(TextReformer::getInstance().IsSpace('@'));
}

// Test the IsSentenceEnd function
TEST(TextReformerTest, IsSentenceEndTest) {
	// Test sentence-ending characters
	EXPECT_TRUE(TextReformer::getInstance().IsSentenceEnd('.'));
	EXPECT_TRUE(TextReformer::getInstance().IsSentenceEnd('!'));
	EXPECT_TRUE(TextReformer::getInstance().IsSentenceEnd('?'));
	EXPECT_TRUE(TextReformer::getInstance().IsSentenceEnd(':'));
	EXPECT_TRUE(TextReformer::getInstance().IsSentenceEnd(';'));

	// Test non-sentence-ending characters
	EXPECT_FALSE(TextReformer::getInstance().IsSentenceEnd('A'));
	EXPECT_FALSE(TextReformer::getInstance().IsSentenceEnd('1'));
	EXPECT_FALSE(TextReformer::getInstance().IsSentenceEnd('@'));
}

TEST(TextReformerTest, RemoveOddCharsTest) {
	// Test with input containing odd characters
	std::wstring input = L"Hello\tworld!\nThis is a\vtest string.";
	std::wstring expectedOutput = L"Hello world! This is a test string.";
	std::wstring actualOutput = TextReformer::getInstance().RemoveOddChars(input);

	EXPECT_EQ(actualOutput, expectedOutput);
}

TEST(TextReformerTest, RemoveOddChars_NormalInput) {
	TextReformer reformer;
	std::wstring input = L"Hello\tworld! This is a test.";
	std::wstring expectedOutput = L"Hello world! This is a test.";
	ASSERT_EQ(reformer.RemoveOddChars(input), expectedOutput);
}

TEST(TextReformerTest, RemoveOddChars_ExtraSpacesAndNewlines) {
	TextReformer reformer;
	std::wstring input = L"  Multiple   spaces\nand\tnewlines.  ";
	std::wstring expectedOutput = L"Multiple spaces and newlines. ";
	ASSERT_EQ(reformer.RemoveOddChars(input), expectedOutput);
}

TEST(TextReformerTest, RemoveOddChars_Punctuation) {
	TextReformer reformer;
	std::wstring input = L"This, is - a... punctuated !string.";
	std::wstring expectedOutput = L"This, is - a. punctuated !string.";
	ASSERT_EQ(reformer.RemoveOddChars(input), expectedOutput);
}

TEST(TextReformerTest, RemoveOddChars_StartsWithSpace) {
	TextReformer reformer;
	std::wstring input = L"   Leading space.";
	std::wstring expectedOutput = L"Leading space.";
	ASSERT_EQ(reformer.RemoveOddChars(input), expectedOutput);
}

TEST(TextReformerTest, RemoveOddChars_SentenceStartsWithPunctuation) {
	TextReformer reformer;
	std::wstring input = L".Starts with punctuation.";
	std::wstring expectedOutput = L"Starts with punctuation.";
	ASSERT_EQ(reformer.RemoveOddChars(input), expectedOutput);
}

TEST(TextReformerTest, RemoveOddChars_EmptyInput) {
	TextReformer reformer;
	std::wstring input = L"";
	std::wstring expectedOutput = L"";
	ASSERT_EQ(reformer.RemoveOddChars(input), expectedOutput);
}

TEST(TextReformerTest, RemoveOddChars_SpecialCharacters) {
	TextReformer reformer;
	std::wstring input = L"Ô?W?";
	std::wstring expectedOutput = L"Ô?W?";
	ASSERT_EQ(reformer.RemoveOddChars(input), expectedOutput);
}

TEST(TextReformerTest, ReformTextTest) {
	// Test with input containing odd characters and newline handling
	std::wstring input = L"Hello\tworld!\nThis is a\vtest string.";
	std::wstring expectedOutput = L"Hello world!\r\nThis is a test string.\r\n";
	std::wstring actualOutput = TextReformer::getInstance().ReformText(input);

	EXPECT_EQ(actualOutput, expectedOutput);
}

TEST(TextReformerTest, ReformTextTest_SentenceEndsWithoutNewLine) {
	// Test with input where sentence ends without a newline
	std::wstring input = L"Hello world! This is a test string.";
	std::wstring expectedOutput = L"Hello world!\r\nThis is a test string.\r\n";
	std::wstring actualOutput = TextReformer::getInstance().ReformText(input);

	EXPECT_EQ(actualOutput, expectedOutput);
}

TEST(TextReformerTest, ReformTextTest_LeadingSpaces) {
	// Test with input containing leading spaces
	std::wstring input = L"    Leading spaces should not be preserved.";
	std::wstring expectedOutput = L"Leading spaces should not be preserved.\r\n";
	std::wstring actualOutput = TextReformer::getInstance().ReformText(input);

	EXPECT_EQ(actualOutput, expectedOutput);
}

TEST(TextReformerTest, ReformTextTest_SingleCharacter) {
	// Test with single character input
	std::wstring input = L"X";
	std::wstring expectedOutput = L"X";
	std::wstring actualOutput = TextReformer::getInstance().ReformText(input);

	EXPECT_EQ(actualOutput, expectedOutput);
}