// SQL_StatementSeparator.cpp

#include "SQL_StatementSeparator.h"

SQL_StatementSeparator::SQL_StatementSeparator(const std::string& InScript)
    : Statements(), Script(InScript), Current(), State(), Index(0) {}

std::vector<std::string> SQL_StatementSeparator::SplitStatements()
{
    ParseAllCharacters();
    FinalizeStatements();
    return Statements;
}

void SQL_StatementSeparator::FinalizeStatements()
{
    if (!IsCurrentNonEmptyMeaningful()) return;
    Statements.push_back(Current);
}

void SQL_StatementSeparator::ParseAllCharacters()
{
    while (Index < Script.size())
    {
        ProcessNextCharacter();
    }
}

bool SQL_StatementSeparator::ProcessNextCharacter()
{
    char Ch = Script[Index];
    char Next = (Index + 1 < Script.size()) ? Script[Index + 1] : '\0';

    if (!State.InSingleQuote && !State.InDoubleQuote)
    {
        if (HandleLineCommentStart(Ch, Next)) return false;
        if (HandleBlockCommentStart(Ch, Next)) return false;
        if (HandleBlockCommentEnd(Ch, Next)) return false;
        if (HandleLineCommentEnd(Ch)) return false;
        if (HandleCommentBody()) return false;
    }

    UpdateQuoteState(Ch);
    HandleStatementSplit(Ch);

    ++Index;
    return false;
}

void SQL_StatementSeparator::TryAddCurrentStatement()
{
    if (!IsCurrentNonEmptyMeaningful()) return;

    std::string Trimmed = Trim(Current);
    if (!Trimmed.empty())
    {
        Statements.push_back(std::move(Trimmed));
    }

    Current.clear();
}

std::string SQL_StatementSeparator::Trim(const std::string& Str)
{
    const auto First = Str.find_first_not_of(" \t\r\n");
    if (First == std::string::npos) return "";

    const auto Last = Str.find_last_not_of(" \t\r\n");
    return Str.substr(First, Last - First + 1);
}

void SQL_StatementSeparator::HandleStatementSplit(char Ch)
{
    if (!ShouldSplitHere(Ch))
    {
        Current += Ch;
        return;
    }

    TryAddCurrentStatement();
}

bool SQL_StatementSeparator::IsLineCommentStart(char Ch, char Next)
{
    return Ch == '-' && Next == '-';
}

bool SQL_StatementSeparator::IsBlockCommentStart(char Ch, char Next)
{
    return Ch == '/' && Next == '*';
}

bool SQL_StatementSeparator::IsBlockCommentEnd(char Ch, char Next)
{
    return Ch == '*' && Next == '/';
}

void SQL_StatementSeparator::UpdateQuoteState(char Ch)
{
    if (Ch == '\'' && !State.InDoubleQuote)
    {
        State.InSingleQuote = !State.InSingleQuote;
    }
    else if (Ch == '"' && !State.InSingleQuote)
    {
        State.InDoubleQuote = !State.InDoubleQuote;
    }
}

bool SQL_StatementSeparator::ShouldSplitHere(char Ch) const
{
    return Ch == ';' && !State.InSingleQuote && !State.InDoubleQuote;
}

bool SQL_StatementSeparator::HandleLineCommentStart(char Ch, char Next)
{
    if (!State.InLineComment && !State.InBlockComment && IsLineCommentStart(Ch, Next))
    {
        State.InLineComment = true;
        Index += 2;
        return true;
    }
    return false;
}

bool SQL_StatementSeparator::HandleBlockCommentStart(char Ch, char Next)
{
    if (!State.InBlockComment && !State.InLineComment && IsBlockCommentStart(Ch, Next))
    {
        State.InBlockComment = true;
        Index += 2;
        return true;
    }
    return false;
}

bool SQL_StatementSeparator::HandleBlockCommentEnd(char Ch, char Next)
{
    if (State.InBlockComment && IsBlockCommentEnd(Ch, Next))
    {
        State.InBlockComment = false;
        Index += 2;
        return true;
    }
    return false;
}

bool SQL_StatementSeparator::HandleLineCommentEnd(char Ch)
{
    if (State.InLineComment && (Ch == '\n' || Ch == '\r'))
    {
        State.InLineComment = false;
        ++Index;
        return true;
    }
    return false;
}

bool SQL_StatementSeparator::HandleCommentBody()
{
    if (State.InLineComment || State.InBlockComment)
    {
        ++Index;
        return true;
    }
    return false;
}

bool SQL_StatementSeparator::IsCurrentNonEmptyMeaningful() const
{
    return std::any_of(Current.begin(), Current.end(),
        [](unsigned char Ch) {
            return !std::isspace(Ch);
    });
}

// SQL_StatementSeparator.cpp
