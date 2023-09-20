#include "unrep_tokensearch.h"

// Definition of the static member variable Tokenizer
TokenSearcher TokenSearcher::Tokenizer;

TokenSearcher& TokenSearcher::getInstance() {
	return Tokenizer;
}

void TokenSearcher::setTokenMinSize(int MinSize) {
	if (MinSize < 1) {
		return;
	}
	TokenMinSize = MinSize;
}

void TokenSearcher::setTokenMinFrequency(int MinFrequency) {
	if (MinFrequency < 1) {
		return;
	}
	TokenMinFrequency = MinFrequency;
}

int TokenSearcher::getTokenMinSize() {
	return TokenMinSize;
}

int TokenSearcher::getTokenMinFrequency() {
	return TokenMinFrequency;
}

bool TokenSearcher::NoDigiPunct(unsigned char c) {
	return std::ispunct(c) || std::isdigit(c);
}
bool TokenSearcher::NoDoubleSpaces(char a, char b) {
	return a == ' ' && b == ' ';
}
int TokenSearcher::ToLowerEngRu(char r) {
	if (r >= 97 && r <= 122) {
		return r;
	}
	else if (r >= 65 && r <= 90) {
		return std::tolower(r);
	}

	switch (r) {
	case 'À': r = 'à'; break;
	case 'Á': r = 'á'; break;
	case 'Â': r = 'â'; break;
	case 'Ã': r = 'ã'; break;
	case 'Ä': r = 'ä'; break;
	case 'Å': r = 'å'; break;
	case '¨': r = '¸'; break;
	case 'Æ': r = 'æ'; break;
	case 'Ç': r = 'ç'; break;
	case 'È': r = 'è'; break;
	case 'É': r = 'é'; break;
	case 'Ê': r = 'ê'; break;
	case 'Ë': r = 'ë'; break;
	case 'Ì': r = 'ì'; break;
	case 'Í': r = 'í'; break;
	case 'Î': r = 'î'; break;
	case 'Ï': r = 'ï'; break;
	case 'Ð': r = 'ð'; break;
	case 'Ñ': r = 'ñ'; break;
	case 'Ò': r = 'ò'; break;
	case 'Ó': r = 'ó'; break;
	case 'Ô': r = 'ô'; break;
	case 'Õ': r = 'õ'; break;
	case 'Ö': r = 'ö'; break;
	case '×': r = '÷'; break;
	case 'Ø': r = 'ø'; break;
	case 'Ù': r = 'ù'; break;
	case 'Ú': r = 'ú'; break;
	case 'Û': r = 'û'; break;
	case 'Ü': r = 'ü'; break;
	case 'Ý': r = 'ý'; break;
	case 'Þ': r = 'þ'; break;
	case 'ß': r = 'ÿ'; break;
	}
	return (r);
}
//static bool ForSort(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
//	return a.second > b.second;
//}

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

int TokenSearcher::CountOneToken(const std::string& String, const std::string& Token) {
	int count = 0;
	std::string::size_type pos = 0;
	while ((pos = String.find(Token, pos)) != std::string::npos) {
		++count;
		pos += Token.size();
	}
	return count;
}

bool TokenSearcher::IsAlphaEngRu(char w) {
	if ((w >= 97 && w <= 122) || (w >= 65 && w <= 90) || (w >= -64 && w <= -1) ||
		w == '¨' || w == '¸') {
		return true;
	}
	return false;
}

std::string TokenSearcher::CleanTheText(const std::string& text) {
	std::string TextToClean = text;

	//Removes all digits and punctuation signs from the text for faster iteration
	TextToClean.erase(
		std::remove_if(TextToClean.begin(),
			TextToClean.end(),
			[this](unsigned char c) {
				return TokenSearcher::NoDigiPunct(c);
			}),
		TextToClean.end()
	);

	//Removes double spaces, left from the last operation, from the text for faster iteration
	TextToClean.erase(
		std::unique(TextToClean.begin(),
			TextToClean.end(),
			[this](char a, char b) {
				return TokenSearcher::NoDoubleSpaces(a, b);
			}),
		TextToClean.end()
	);

	//Converts all uppercase letters to lowercase for unified search
	std::transform(TextToClean.begin(), TextToClean.end(), TextToClean.begin(), [this](char r) {
		return TokenSearcher::ToLowerEngRu(r);
		});

	return TextToClean;
}

void TokenSearcher::FindLeaderPair(std::string& MaxStr, int& MaxNum, std::map<std::string, int>& WordMap) {
	for (const auto& entry : WordMap) {
		const std::string& word = entry.first;
		int frequency = entry.second;

		if (frequency > MaxNum ||
			(frequency == MaxNum && word.length() > MaxStr.length())) {
			MaxNum = frequency;
			MaxStr = word;
		}
	}
}

std::map<std::string, int> TokenSearcher::TokenizeText(const std::string& TextToDo) {
	std::map<std::string, int> TokenFrequency;
	std::map<std::string, int> AllVariansMap;
	std::unordered_set<std::string> LearnedTokens;

	const int TextSize = static_cast<int>(TextToDo.length());

	int TokenStart = 0;

	//This map contains words, we've already found and evaluated. So we do not need to waste time to check again
	//Iterate over text letter by letter
	for (int ii = 0; ii < TextSize; ++ii) {
		//////
		//////If we stubble upon a letter, that means we've found a word. So we start counting its length
		if (IsAlphaEngRu(TextToDo[ii])) {
			continue;
		}
		//////
		//////If we stumble upon anything but letter, we check, whether we've iterate through a word large enough to be estimated
		else if (ii - TokenStart >= TokenMinSize) {
			std::string SingleToken = TextToDo.substr(TokenStart, ii - TokenStart);
			AllVariansMap.clear();

			//Looking for the word occurrences in the text. After every iteration we cut the last char out of the word, till the word get the minimal size
			while (SingleToken.length() >= static_cast<size_t>(TokenMinSize)) {
				AllVariansMap.emplace_hint(begin(AllVariansMap), SingleToken, CountOneToken(TextToDo, SingleToken));
				//If we deal with a familiar word or word part, we skip this word to look for new ones
				if (LearnedTokens.count(SingleToken) != 0) {
					goto NextWord;
				}
				SingleToken.pop_back();
			}
			//Select the most frequent AND the longest word variant
			std::string LeaderStr;
			int LeaderNum = 0;
			FindLeaderPair(LeaderStr, LeaderNum, AllVariansMap);

			if (LeaderNum >= TokenMinFrequency) {
				//Check if the word we fount is unique in TokenFrequency map. If not, we merge similar words and their counters in one pair
				for (auto& token : TokenFrequency) {
					if (LeaderStr.find(token.first) != std::string::npos ||
						token.first.find(LeaderStr) != std::string::npos
						) {
						std::string Shorty = (LeaderStr.length() > token.first.length()) ? token.first : LeaderStr;
						LeaderNum = (LeaderNum > token.second) ? LeaderNum : token.second;
						TokenFrequency.erase(token.first);
						TokenFrequency.emplace(Shorty, LeaderNum);
						LearnedTokens.emplace(Shorty);
						goto NextWord;
					}
				}
				TokenFrequency.emplace(LeaderStr, LeaderNum);
				LearnedTokens.emplace(LeaderStr);
			}
		}
		////////If we stumble upon anything but letter, but the last word was less than the minimal size, we reset the word counter and keep looking for a bigger word
	NextWord:
		TokenStart = ii + 1;
	}
	return TokenFrequency;
}

//Function to analyse the given text and search for repeated tokens
std::string TokenSearcher::AnalyzeTokens(const std::string& text) {
	const std::string TextToDo = CleanTheText(text);

	std::map<std::string, int> Tokens = TokenizeText(TextToDo);

	if (Tokens.size() == 0) {
		return "The text has no repeated words and word parts!";
	}
	std::vector<std::pair<std::string, int>> vec(Tokens.begin(), Tokens.end());
	std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
		return a.second > b.second;
		});

	std::string OutPut;

	for (auto const& pair : vec) {
		OutPut.insert(OutPut.size(), pair.first);
		OutPut.push_back('(');
		OutPut.insert(OutPut.size(), std::to_string(pair.second));
		OutPut.push_back('\r');
		OutPut.push_back('\n');
	}

	return OutPut;
}