// SQL_StatementSeparator.h

#pragma once
#ifndef SQL_STATEMENT_SEPARATOR_H
#define SQL_STATEMENT_SEPARATOR_H

#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

struct SQLParseState
{
    bool InSingleQuote = false;
    bool InDoubleQuote = false;
    bool InLineComment = false;
    bool InBlockComment = false;
};

class SQL_StatementSeparator
{
private:
    std::vector<std::string> Statements;
    std::string Script;
    std::string Current;
    SQLParseState State;
    size_t Index = 0;    

public:
    explicit SQL_StatementSeparator(const std::string& InScript);

    std::vector<std::string> SplitStatements();

private:
    void FinalizeStatements();
    void ParseAllCharacters();
    bool ProcessNextCharacter();
    void TryAddCurrentStatement();
    static std::string Trim(const std::string& Str);
    void HandleStatementSplit(char Ch);
    static bool IsLineCommentStart(char Ch, char Next);
    static bool IsBlockCommentStart(char Ch, char Next);
    static bool IsBlockCommentEnd(char Ch, char Next);
    void UpdateQuoteState(char Ch);
    bool ShouldSplitHere(char Ch) const;
    bool HandleLineCommentStart(char Ch, char Next);
    bool HandleBlockCommentStart(char Ch, char Next);
    bool HandleBlockCommentEnd(char Ch, char Next);
    bool HandleLineCommentEnd(char Ch);
    bool HandleCommentBody();
    bool IsCurrentNonEmptyMeaningful() const;
};


#endif // SQL_STATEMENT_SEPARATOR_H
