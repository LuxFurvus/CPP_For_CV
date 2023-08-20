#ifndef TEXTREFORMER_H
#define TEXTREFORMER_H

#include <string>

class TextReformer {
private:
	TextReformer();  // Private constructor to prevent instantiation
	static TextReformer Reformer;

	bool IsSpace(char16_t ch);
	bool IsSentenceEnd(char16_t ch);
	std::wstring RemoveOddChars(const std::wstring& Input);

public:
	static TextReformer& getInstance();
	std::wstring ReformText(const std::wstring& Input);
};

#endif // TEXTREFORMER_H