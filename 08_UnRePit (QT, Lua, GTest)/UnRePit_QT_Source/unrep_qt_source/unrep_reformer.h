#ifndef TEXTREFORMER_H
#define TEXTREFORMER_H

#include <string>

class TextReformer {
private:
	TextReformer();  // Private constructor to prevent instantiation
	static TextReformer Reformer;

    static bool IsSpace(char16_t ch);
    static bool IsSentenceEnd(char16_t ch);
    static std::wstring RemoveOddChars(const std::wstring& Input);

    //static TextReformer& getInstance();

public:
    static std::wstring ReformText(const std::wstring& Input);
};

#endif // TEXTREFORMER_H
