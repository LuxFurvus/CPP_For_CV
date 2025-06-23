#pragma once

#include <atomic>
#include <string_view>

///////////////////

class ConditionChecker
{
private:
    static inline std::atomic<bool> IncludeStacktrace = false;

public:
    static void SetStacktraceEnabled(bool IsEnabled);
    static bool IsStacktraceEnabled();

    [[noreturn]] static void 
        ReportConditionFailure(const char* ExprText, const char* File, int Line, std::string_view ErrorMsg = "");
};

///////////////////

#define CONFIRM(Expr) \
    do { \
        if (!(Expr)) \
        { \
            ::ConditionChecker::ReportConditionFailure(#Expr, __FILE__, __LINE__); \
        } \
    } while (false)

    
#define CONFIRMF(Expr, ErrorMsg) \
    do { \
        if (!(Expr)) \
        { \
            ::ConditionChecker::ReportConditionFailure(#Expr, __FILE__, __LINE__, ErrorMsg); \
        } \
    } while (false)
    
#define CONFIRMS(Expr, ...) \
    do { \
        if (!(Expr)) \
        { \
            ::ConditionChecker::ReportConditionFailure(#Expr, __FILE__, __LINE__, std::format(__VA_ARGS__)); \
        } \
    } while (false)

/// end: ConditionChecker.h