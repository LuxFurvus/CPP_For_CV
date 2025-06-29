#include <string>
#include <unordered_set>
#include <sstream>

class TextAdapter
{
private:

    // Function to convert all uppercase letters to lowercase
    void ToLowercase(std::string& text)
    {
        for (char& c : text)
        {
            if (c >= 'A' && c <= 'Z')
            {
                c = c - 'A' + 'a';
            }
        }
    }

    // Function to replace punctuation and escape characters with a single space
    void ReplacePunctuationWithSpace(std::string& text)
    {
        for (char& c : text)
        {
            if (isalnum(static_cast<unsigned char>(c)) || c == ' ') continue;
            c = ' ';
        }
    }

    // Function to collapse multiple consecutive spaces into a single space
    void CollapseMultipleSpaces(std::string& text)
    {
        std::string result;
        bool previousSpace = false;
        for (char c : text)
        {
            if (c != ' ')
            {
                result += c;
                previousSpace = false;
                continue;
            }
            if (!previousSpace)
            {
                result += c;
                previousSpace = true;
            }
        }
        text.swap(result);
    }

    // Function to remove characters outside ASCII range [23, 126]
    void RemoveOutOfRangeChars(std::string& text)
    {
        std::string result;
        for (char c : text)
        {
            if (c >= 23 && c <= 126)
            {
                result.push_back(c);
            }
        }
        text.swap(result);
    }

    // Function to remove stopwords
    void RemoveStopwords(std::string& text)
    {
        static const std::unordered_set<std::string> stopwords = {
            "the", "a", "an", "of", "in", "on", "at", "is", "are", "was", "were", "and", "or", "but", "if", "then", "else", "for", "to", "with", "as", "by", "this", "that"
        };
        std::istringstream iss(text);
        std::ostringstream oss;
        std::string word;
        while (iss >> word)
        {
            if (stopwords.find(word) == stopwords.end())
            {
                oss << word << ' ';
            }
        }
        text = oss.str();
        if (!text.empty() && text.back() == ' ')
        {
            text.pop_back();
        }
    }

public:

    void AdaptText(std::string& text, const bool UseRemoveStopwords) {
        ToLowercase(text);
        ReplacePunctuationWithSpace(text);
        CollapseMultipleSpaces(text);
        RemoveOutOfRangeChars(text);
        
        if (UseRemoveStopwords) return;
        RemoveStopwords(text);
    }

};
