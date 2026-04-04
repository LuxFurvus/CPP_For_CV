
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

void ConditionChecker::SetStacktraceEnabled(const bool IsEnabled)
{
    IncludeStacktrace.store(IsEnabled, std::memory_order_relaxed);
}

///////////////////

void ConditionChecker::ReportConditionWarning(
        const char* ExprText,
        std::string_view WarningMsg,
        const std::source_location& Loc)
{
    const std::string MsgToPrint = PrepareMassage(
        ExprText,
        Loc,
        WarningMsg,
        "⚠️ WARNING");
    std::cerr << MsgToPrint;
}

//=================

void ConditionChecker::ReportConditionFailure(
        const char* ExprText,
        std::string_view ErrorMsg,
        const std::source_location& Loc)
{
    const std::string MsgToThrow = PrepareMassage(
        ExprText,
        Loc,
        ErrorMsg,
        "❌ ERROR");
    throw std::runtime_error(MsgToThrow);
}

//\/\/\/\/\/\/\/\/\/

std::string ConditionChecker::PrepareMassage(
    const char* ExprText,
    const std::source_location& Loc,
    std::string_view ErrorMsg,
    std::string_view MessageType)
{
    std::stringstream Stream;

    const char* const File = Loc.file_name();
    const int Line = Loc.line();

    Stream << std::format("\n\n{}: condition check failed\n"
                          "Expression: {}\n"
                          "Location  : {} : {}\n"
                          "Message   : [ {} ]\n\n",
                          MessageType,
                          (ExprText ? ExprText : "(null)"),
                          (File ? File : "(unknown)"), Line,
                          ErrorMsg);

    if (IsStacktraceEnabled())
    {
        Stream << "\n\t🔍 Stack trace:\n";
        Stream << boost::stacktrace::stacktrace() << "\n";
    }

    return Stream.str();
}

//\/\/\/\/\/\/\/\/\/

bool ConditionChecker::IsStacktraceEnabled()
{
    return IncludeStacktrace.load(std::memory_order_relaxed);
}

// end of ConditionChecker.cpp
