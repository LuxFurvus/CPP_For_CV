
#include "ConditionChecker.h"
////////
#include <string_view>
#include <iostream>
#include <sstream>
#include <format>
////////
#define BOOST_STACKTRACE_USE_ADDR2LINE
#include <boost/stacktrace.hpp>
////////

void ConditionChecker::SetStacktraceEnabled(bool IsEnabled)
{
    IncludeStacktrace.store(IsEnabled, std::memory_order_relaxed);
}

///////////////////

bool ConditionChecker::IsStacktraceEnabled()
{
    return IncludeStacktrace.load(std::memory_order_relaxed);
}

///////////////////

void ConditionChecker::ReportConditionFailure(const char* ExprText, const char* File, int Line, std::string_view ErrorMsg)
{
    std::stringstream Stream;

    Stream << std::format("\n\n❌ Condition check failed\n"
                          "Expression  : {}\n"
                          "Location    : {} : {}\n"
                          "ErrorMsg    : {}\n",
                          (ExprText ? ExprText : "(null)"),
                          (File ? File : "(unknown)"), Line,
                          ErrorMsg);

    if (IsStacktraceEnabled())
    {
        Stream << "🔍 Stack trace:\n";
        Stream << boost::stacktrace::stacktrace() << "\n";
    }

    throw std::runtime_error(Stream.str());
}

// end of ConditionChecker.cpp
