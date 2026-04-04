#pragma once

#include <atomic>
#include <format>
#include <string_view>
#include <source_location>

///////////////////

class ConditionChecker
{
private:
    static inline std::atomic<bool> IncludeStacktrace = false;

    static std::string PrepareMassage(
        const char *ExprText, const std::source_location& Loc,
        std::string_view ErrorMsg, std::string_view MessageType);

    static bool IsStacktraceEnabled();

public:

    static void SetStacktraceEnabled(const bool IsEnabled);

    static void ReportConditionWarning(
        const char* ExprText,
        std::string_view WarningMsg,
        const std::source_location& Loc = std::source_location::current());

    [[noreturn]] static void ReportConditionFailure(
        const char* ExprText,
        std::string_view ErrorMsg = "",
        const std::source_location& Loc = std::source_location::current());
};

///////////////////

#define CONFIRM(Expr) \
    do { \
        if (!(Expr)) \
        { \
            ::ConditionChecker::ReportConditionFailure(#Expr); \
        } \
    } while (false)

#define CONFIRMS(Expr, ...) \
    do { \
        if (!(Expr)) \
        { \
            ::ConditionChecker::ReportConditionFailure(#Expr, std::format(__VA_ARGS__)); \
        } \
    } while (false)

#define WARNIFFALSE(Expr, ...) \
    do { \
        if (!(Expr)) \
        { \
            ::ConditionChecker::ReportConditionWarning(#Expr, std::format(__VA_ARGS__)); \
        } \
    } while (false)

/// end: ConditionChecker.h
