
#include "../FieldParser/FieldParser.h"
////////
#include "../StringNormalizer/StringNormalizer.h"
#include "../SubstringSearcher/SubstringSearcher.h"
////////
#include "../FieldParser/BlueprintGuid.h"

//     //     //     //     //     //

std::optional<size_t> FieldParser::FindKeyEndPos(
    const FParseFieldParams& ParseParams)
{
    if (!ParseParams.IsValid()) return std::nullopt;

    const FSubstringLookupTask FindKeyTask {
        .Text = ParseParams.SourceBlock,
        .Candidates = { ParseParams.FieldKey },
        .Rules = ParseParams.IgnoredRules,
        .StartLookupAt = 0,
        .FindMatchOrFailAt = std::string_view::npos,
    };
    const auto [FieldRange, _]
        = SubstringSearcher::FindSubtringRangeByRules(FindKeyTask);

    const size_t KeyEndPos = FieldRange.End;

    if (KeyEndPos >= ParseParams.SourceBlock.size()) return std::nullopt;

    return KeyEndPos;
}

//     //     //     //     //     //

std::optional<std::string_view> FieldParser::GetValuePart(
    const FParseFieldParams& ParseParams)
{
    const std::optional<size_t> KeyEndPos
        = FindKeyEndPos(ParseParams);
    if (!KeyEndPos.has_value()) return std::nullopt;

    const std::string_view EffectiveText = ParseParams.SourceBlock.substr(*KeyEndPos);

    return EffectiveText;
}

//     //     //     //     //     //

std::string FieldParser::CollectDigitsInString(
    const std::string_view InString,
    const std::function<bool(char)> IgnoredChars)
{
    std::string Result;
    for (size_t i = 0; i < InString.size(); ++i)
    {
        const char Current = InString[i];
        if (IgnoredChars(Current)) continue;
        if (!std::isdigit(static_cast<unsigned char>(Current))) break;
        Result.push_back(Current);
    }
    return Result;
}

//     //     //     //     //     //

std::optional<uint64_t> FieldParser::ConvertStringToNumber(
    const std::string_view NumberString)
{
    if (NumberString.empty()) return std::nullopt;

    constexpr uint64_t MaxAllowed = std::numeric_limits<uint64_t>::max();
    uint64_t ParsedValue = 0;

    for (const char DigitChar : NumberString)
    {
        const uint64_t Digit = static_cast<uint64_t>(DigitChar - '0');

        if (ParsedValue > (MaxAllowed - Digit) / 10)
        {
            return std::nullopt;
        }

        ParsedValue = ParsedValue * 10 + Digit;
    }

    return ParsedValue;
}

//     //     //     //     //     //

std::optional<std::string_view> FieldParser::GetFirstEnclosedSubstring(
    const std::string_view ValueBlock,
    const std::pair<char, char>& Delimiters) {
    if (ValueBlock.empty()) {
        return std::nullopt;
    }

    size_t start = 0;
    if (Delimiters.first != 0) {
        start = ValueBlock.find(Delimiters.first);
        if (start == std::string_view::npos) {
            return std::nullopt;
        }
        ++start;
    }

    if (Delimiters.second == 0) {
        return ValueBlock.substr(start);
    }

    if (Delimiters.first == 0 || Delimiters.first == Delimiters.second) {
        size_t closePos = ValueBlock.find(Delimiters.second, start);
        if (closePos == std::string_view::npos) {
            return std::nullopt;
        }
        return ValueBlock.substr(start, closePos - start);
    }

    size_t current = start;
    int count = 1;
    while (current < ValueBlock.size() && count > 0) {
        if (ValueBlock[current] == Delimiters.first) {
            ++count;
        } else if (ValueBlock[current] == Delimiters.second) {
            --count;
        }
        ++current;
    }

    if (count != 0) {
        return std::nullopt;
    }

    size_t end = current - 1;
    return ValueBlock.substr(start, end - start);
}


//     //     //     //     //     //

std::vector<std::string_view> FieldParser::CollectEnclosedStrings(
    const std::string_view ValueBlock,
    const std::vector<std::pair<char, char>>& Delimiters)
{
    std::vector<std::string_view> Result;
    std::string_view CurrentView = ValueBlock;
    size_t RelativeOffset = 0;

    if (Delimiters.empty()) return { ValueBlock };

    for (size_t i = 0; i < Delimiters.size(); ++i)
    {
        const std::pair<char, char>& Delim = Delimiters[i];

        // Compute effective substring to search
        std::string_view SearchView = CurrentView;

        if (Delim.first == 0)
        {
            // First pair: search from the beginning of full ValueBlock
            if (i == 0)
            {
                SearchView = ValueBlock;
                RelativeOffset = 0;
            }
            else
            {
                const char PrevClose = Delimiters[i - 1].second;
                const size_t PrevClosePos = CurrentView.find(PrevClose);
                if (PrevClosePos == std::string_view::npos) return Result;

                // Move forward from previous closing delimiter
                RelativeOffset += PrevClosePos + 1;
                SearchView = ValueBlock.substr(RelativeOffset);
            }
        }

        const std::optional<std::string_view> Extracted = GetFirstEnclosedSubstring(SearchView, Delim);
        if (!Extracted.has_value()) return Result;

        Result.push_back(*Extracted);

        // Update CurrentView for next iteration
        RelativeOffset = Extracted->data() + Extracted->size() - ValueBlock.data();
        CurrentView = ValueBlock.substr(RelativeOffset);
    }

    return Result;
}


//     //     //     //     //     //

//     //     //     //     //     //

//     //     //     //     //     //


std::optional<bool> FieldParser::ParseFieldBool(
    const FParseFieldParams& ParseParams)
{
    const std::optional<std::string_view> ValuePart = GetValuePart(ParseParams);
    if (!ValuePart.has_value()) return std::nullopt;

    const FIgnoredRangesRules IgnoredRules {
        .IgnoredChar = [](char c) 
        {
            const bool IsAlpha = (c >= 'A' && c <= 'z');
            return !IsAlpha;
        },
        .IgnoredRangesBetween = {{'(', ')'}, {'\'', '\''}, {'"', '"'} },
        .bCaseInsensitive = true,
    };

    const std::string NormalizedValue
        = StringNormalizer::NormalizeStringByRules(*ValuePart, IgnoredRules);

    constexpr std::string_view TrueValue = "true";
    constexpr std::string_view FalseValue = "false";

    constexpr size_t MaxLength
        = std::max(TrueValue.size(), FalseValue.size());

    const std::string TargetRange = NormalizedValue.substr(0, MaxLength);

    if (TargetRange.contains(TrueValue)) return true;
    if (TargetRange.contains(FalseValue)) return false;
    return std::nullopt;
}

//     //     //     //     //     //

std::optional<uint64_t> FieldParser::ParseNumericField(
    const FParseFieldParams& ParseParams)
{
    const std::optional<std::string_view> ValuePart = GetValuePart(ParseParams);
    if (!ValuePart.has_value()) return std::nullopt;

    const std::string NumberString
        = CollectDigitsInString(*ValuePart, ParseParams.IgnoredRules.IgnoredChar);

    const std::optional<uint64_t> ParsedValue = ConvertStringToNumber(NumberString);

    return ParsedValue;
}

//     //     //     //     //     //

std::optional<FBlueprintGuid> FieldParser::ParseGuidField(
    const FParseFieldParams& ParseParams)
{
    const std::optional<std::string_view> ValuePart = GetValuePart(ParseParams);
    if (!ValuePart.has_value()) return std::nullopt;

    const std::string_view GuidText = (ValuePart->size() > 32)
        ? ValuePart->substr(0, 32)
        : *ValuePart;

    const std::optional<FBlueprintGuid> Guid
        = FBlueprintGuid::MakeGuidFromString(GuidText);
    if (!Guid.has_value()) return std::nullopt;

    return Guid;
}

//     //     //     //     //     //

std::vector<std::string_view> FieldParser::ParseFieldNestedStrings(
    const FParseFieldParams& ParseParams,
    const std::vector<std::pair<char, char>>& Delimiters)
{
    const std::optional<std::string_view> ValuePart = GetValuePart(ParseParams);
    if (!ValuePart.has_value()) return {};

    const std::vector<std::string_view> Extracted
        = CollectEnclosedStrings(*ValuePart, Delimiters);

    return Extracted;
}


//     //     //     //     //     //


std::optional<bool> FieldParser::ParseFieldBoolv0(
    const FParseFieldParams& ParseParams)
{
    const std::optional<std::string_view> ValuePart = GetValuePart(ParseParams);
    if (!ValuePart.has_value()) return std::nullopt;

    constexpr std::string_view TrueValue = "True";
    constexpr std::string_view FalseValue = "False";

    const FSubstringLookupTask FindValueTask {
        .Text = *ValuePart,
        .Candidates = { TrueValue, FalseValue },
        .Rules = ParseParams.IgnoredRules,
        .StartLookupAt = 0,
        .FindMatchOrFailAt = 0,
    };
    const auto [ValueRange, Value]
        = SubstringSearcher::FindSubtringRangeByRules(FindValueTask);

    if (!ValueRange.IsValid()) return std::nullopt;
    if (Value == TrueValue) return std::make_optional(true);
    if (Value == FalseValue) return std::make_optional(false);

    return std::nullopt;
}
