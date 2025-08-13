#pragma once

#include <atomic>
#include <format>
#include <string_view>

///////////////////

class ConditionChecker
{
private:
    static inline std::atomic<bool> IncludeStacktrace = false;

    static std::string PrepareMassage(
        const char *ExprText, const char *File, int Line,
        std::string_view ErrorMsg, std::string_view MessageType);

    static bool IsStacktraceEnabled();

public:

    static void SetStacktraceEnabled(bool IsEnabled);

    static void ReportConditionWarning(
        const char* ExprText, const char* File, int Line, std::string_view WarningMsg);

    [[noreturn]] static void ReportConditionFailure(
        const char* ExprText, const char* File, int Line, std::string_view ErrorMsg = "");
};

///////////////////

#define CONFIRM(Expr) \
    do { \
        if (!(Expr)) \
        { \
            ::ConditionChecker::ReportConditionFailure(#Expr, __FILE__, __LINE__); \
        } \
    } while (false)

#define CONFIRMS(Expr, ...) \
    do { \
        if (!(Expr)) \
        { \
            ::ConditionChecker::ReportConditionFailure(#Expr, __FILE__, __LINE__, std::format(__VA_ARGS__)); \
        } \
    } while (false)

#define WARNIFFALSE(Expr, ...) \
    do { \
        if (!(Expr)) \
        { \
            ::ConditionChecker::ReportConditionWarning(#Expr, __FILE__, __LINE__, std::format(__VA_ARGS__)); \
        } \
    } while (false)

/// end: ConditionChecker.h
