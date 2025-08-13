
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

void ConditionChecker::ReportConditionWarning(const char *ExprText, const char *File, int Line, std::string_view WarningMsg)
{
    const std::string MsgToPrint = PrepareMassage(ExprText, File, Line, WarningMsg, "⚠️ WARNING");
    std::cerr << MsgToPrint;
}

//=================

void ConditionChecker::ReportConditionFailure(const char* ExprText, const char* File, int Line, std::string_view ErrorMsg)
{
    const std::string MsgToThrow = PrepareMassage(ExprText, File, Line, ErrorMsg, "❌ ERROR");
    throw std::runtime_error(MsgToThrow);
}

//\/\/\/\/\/\/\/\/\/

std::string ConditionChecker::PrepareMassage(const char* ExprText, const char* File, int Line, std::string_view ErrorMsg, std::string_view MessageType)
{
    std::stringstream Stream;

    Stream << std::format("\n\n{}: condition check failed\n"
                          "Expression: {}\n"
                          "Location  : {} : {}\n"
                          "Message   : [ {} ]\n",
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
