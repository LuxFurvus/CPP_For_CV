#ifndef TEXTREFORMER_H
#define TEXTREFORMER_H

#include <string>

class TextReformer {
private:
	TextReformer();  // Private constructor to prevent instantiation
	static TextReformer Reformer;

	bool IsSpace(wchar_t ch);
	std::wstring RemoveDoubleSpaces(const std::wstring& Input);
	bool IsSentenceEnd(wchar_t ch);

public:
	static TextReformer& getInstance();
	std::wstring ReformText(const std::wstring& Input);
};

#endif // TEXTREFORMER_H