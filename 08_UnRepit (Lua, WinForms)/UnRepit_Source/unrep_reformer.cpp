#include "unrep_reformer.h"

//////////////PRIVATE//////////////////

TextReformer TextReformer::Reformer;

TextReformer::TextReformer() {}

bool TextReformer::IsSpace(wchar_t ch) {
	if (ch == ' ') return true;
	else if (ch == '\n') return true;
	else if (ch == '\t') return true;
	else if (ch == '\v') return true;
	else if (ch == '\r') return true;
	else if (ch == '\f') return true;
	else if (ch == '•') return true;
	else if (static_cast<int>(ch) > 377 && static_cast<int>(ch) < 1040) return true;
	else if (static_cast<int>(ch) > 1103) return true;

	return false;
}

std::wstring TextReformer::RemoveDoubleSpaces(const std::wstring& Input) {
	const std::wstring input = Input;
	std::wstring result;

	// Preallocates memory for the result
	result.reserve(input.length());

	// Flag to track the previous character being a space
	bool prevSpace = false;

	// Checks for space or newline characters. Only adds the character if it's not of space-type
	for (wchar_t ch : input) {
		if (IsSpace(ch)) {
			// Replaces double spaces and newlines with a single space
			if (!prevSpace) {
				result.push_back(' ');
				prevSpace = true;
				continue;
			}
		}
		// Adds non-space characters to the result
		else {
			result.push_back(ch);
			prevSpace = false;
		}
	}
	// Remove the first char, if it's of a space-type
	if (IsSpace(result[0])) {
		result.erase(0, 1);
	}

	return result;
}

bool TextReformer::IsSentenceEnd(wchar_t ch) {
	if (ch == '.') return true;
	else if (ch == '!') return true;
	else if (ch == '?') return true;
	else if (ch == '…') return true;
	else if (ch == ';') return true;

	return false;
}

//////////////PUBLIC//////////////////

TextReformer& TextReformer::getInstance() {
	return Reformer;
}

std::wstring TextReformer::ReformText(const std::wstring& Input) {
	const std::wstring TextToReform = RemoveDoubleSpaces(Input);
	std::wstring Result;

	bool WasNewLine = false;
	for (wchar_t ch : TextToReform) {
		if (IsSentenceEnd(ch)) {
			Result.push_back(ch);
			Result.push_back('\r');
			Result.push_back('\n');
			WasNewLine = true;
			continue;
		}
		else {
			if (ch == ' ' && WasNewLine) {
				WasNewLine = false;
				continue;
			}
			Result.push_back(ch);
			continue;
		}
	}

	return Result;
}