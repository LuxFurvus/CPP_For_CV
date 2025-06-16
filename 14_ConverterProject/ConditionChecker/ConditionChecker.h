#pragma once

#include <iostream>
#include <format>
#include <cstdlib>
////////
#define BOOST_STACKTRACE_USE_ADDR2LINE
#include <boost/stacktrace.hpp>

namespace InternalConfirm
{
    [[noreturn]] inline void 
    ReportConditionFailure(const char* ExprText, const char* File, int Line)
    {
        std::cerr << std::format("\n\n❌ Condition check failed\n"
                                 "Expression  : {}\n"
                                 "Location    : {}:{}\n",
                                 ExprText ? ExprText : "(null)",
                                 File ? File : "(unknown)",
                                 Line);
        std::cerr << "🔍 Stack trace:\n";
        std::cerr << boost::stacktrace::stacktrace() << "\n";

        std::abort();
    }
}

#define CONFIRM(Expr) \
    do { \
        if (!(Expr)) \
        { \
            ::InternalConfirm::ReportConditionFailure(#Expr, __FILE__, __LINE__); \
        } \
    } while (false)
