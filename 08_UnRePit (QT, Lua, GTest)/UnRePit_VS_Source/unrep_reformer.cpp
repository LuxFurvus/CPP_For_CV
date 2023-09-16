#include "unrep_reformer.h"

//////////////PRIVATE//////////////////

TextReformer TextReformer::Reformer;

TextReformer::TextReformer() {}

bool TextReformer::IsSpace(char16_t ch) {
	return ch == ' '
		|| ch == '\n'
		|| ch == '\t'
		|| ch == '\v'
		|| ch == '\r'
		|| ch == '\f';
}

bool TextReformer::IsSentenceEnd(char16_t ch) {
	return ch == '.'
		|| ch == '!'
		|| ch == '?'
		|| ch == '…'
		|| ch == ':'
		|| ch == ';';
}

std::wstring TextReformer::RemoveOddChars(const std::wstring& Input) {
	std::wstring result;

	// Flags to track previous characters
	bool prevSpace{ false };
	bool prevPunctuation{ false };

	// Preallocates memory for the result
	result.reserve(Input.length());

	// Checks for space or newline characters. Only adds the character if it's not of space-type
	for (char16_t ch : Input) {
		if (IsSpace(ch)) {
			// Replaces double spaces and newlines with a single space
			if (!prevSpace) {
				result.push_back(' ');
				prevSpace = true;
				prevPunctuation = false;
				continue;
			}
		}
		else if (IsSentenceEnd(ch)) {
			if (!prevPunctuation) {
				result.push_back(ch);
				prevPunctuation = true;
				prevSpace = false;
				continue;
			}
		}
		// Adds non-space characters to the result
		else {
			result.push_back(ch);
			prevSpace = false;
			prevPunctuation = false;
		}
	}
	// Remove the first char, if it's of a space-type
	if (result.size() > 0 &&
		(IsSpace(result[0]) || IsSentenceEnd(result[0]))
		) {
		return result.substr(1);
	}
	else {
		return result;
	}
}

//////////////PUBLIC//////////////////

TextReformer& TextReformer::getInstance() {
	return Reformer;
}

std::wstring TextReformer::ReformText(const std::wstring& Input) {
	const std::wstring TextToReform = RemoveOddChars(Input);
	std::wstring Result;

	bool WasNewLine = false;
	for (char16_t ch : TextToReform) {
		if (IsSentenceEnd(ch)) {
			Result.push_back(ch);
			Result.push_back('\r');
			Result.push_back('\n');
			WasNewLine = true;
			continue;
		}
		else if (ch == ' ' && WasNewLine) {
			WasNewLine = false;
			continue;
		}
		else {
			Result.push_back(ch);
			continue;
		}
	}

	return Result;
}