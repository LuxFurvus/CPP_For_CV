#ifndef TOKENSEARCHER_H
#define TOKENSEARCHER_H

#include <vector>
#include <unordered_set>
#include <cctype>
#include <algorithm>
#include <map>
#include <string>

class TokenSearcher {
private:
	int TokenMinSize;
	int TokenMinFrequency;
	static TokenSearcher Tokenizer;

private:
	TokenSearcher() noexcept : TokenMinSize(4), TokenMinFrequency(3) {}

private:
	bool NoDigiPunct(unsigned char c);
	bool NoDoubleSpaces(char a, char b);
	int ToLowerEngRu(char r);

private:
	//bool ForSort(const std::pair<std::string, int>& a,
	//	const std::pair<std::string, int>& b);

private:
	int CountOneToken(const std::string& String, const std::string& Token);
	bool IsAlphaEngRu(char w);
	std::string CleanTheText(const std::string& text);
	void FindLeaderPair(std::string& MaxStr, int& MaxNum,
		std::map<std::string, int>& WordMap);
	std::map<std::string, int> TokenizeText(const std::string& TextToDo);

public:
	static TokenSearcher& getInstance();
	void setTokenMinSize(int MinSize);
	void setTokenMinFrequency(int MinFrequency);
	int getTokenMinSize();
	int getTokenMinFrequency();
	std::string AnalyzeTokens(const std::string& text);
};

#endif // TOKENSEARCHER_H