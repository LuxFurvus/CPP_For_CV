
#include <limits>
#include <string>
////////
#include <gtest/gtest.h>
////////
#include "ConditionChecker.h"

TEST(ConditionCheckerDeathTest, ReportFailure)
{
    {
        SCOPED_TRACE("Standard failure report with typical condition info");
        EXPECT_DEATH({
            InternalConfirm::ReportConditionFailure("x > 0", "test.cpp", 42);
        }, "Condition check failed");
    }

    {
        SCOPED_TRACE("Zero line number");
        EXPECT_DEATH({
            InternalConfirm::ReportConditionFailure("value == 0", "file.cpp", 0);
        }, "file\\.cpp:0");
    }

    {
        SCOPED_TRACE("Long condition expression");
        std::string LongCond(1000, 'C');
        EXPECT_DEATH({
            InternalConfirm::ReportConditionFailure(LongCond.c_str(), "long.cpp", 7);
        }, "long\\.cpp:7");
    }

    {
        SCOPED_TRACE("Null condition name");
        EXPECT_DEATH({
            InternalConfirm::ReportConditionFailure(nullptr, "main.cpp", 10);
        }, "main\\.cpp:10");
    }

    {
        SCOPED_TRACE("Null file name");
        EXPECT_DEATH({
            InternalConfirm::ReportConditionFailure("SomeCondition", nullptr, 99);
        }, "Location    : \\(unknown\\):99");
    }

    {
        SCOPED_TRACE("Empty condition expression and file name");
        EXPECT_DEATH({
            InternalConfirm::ReportConditionFailure("", "", 1);
        }, "Location    : :1");
    }

    {
        SCOPED_TRACE("Minimum and maximum line numbers");
        EXPECT_DEATH({
            InternalConfirm::ReportConditionFailure("MinLine", "file.cpp", std::numeric_limits<int>::min());
        }, "file\\.cpp");

        EXPECT_DEATH({
            InternalConfirm::ReportConditionFailure("MaxLine", "file.cpp", std::numeric_limits<int>::max());
        }, "file\\.cpp");
    }

    {
        SCOPED_TRACE("Check stack trace format");
        EXPECT_DEATH({
            InternalConfirm::ReportConditionFailure("TraceCheck", "trace.cpp", 888);
        }, "Stack trace");
    }
}


///////////////////////////


TEST(ConditionCheckerDeathTest, ConfirmMacro)
{
    ///////////////////////////////
    /// NORMAL CASES
    ///////////////////////////////

    // Condition fails: substring not found
    {
        SCOPED_TRACE("Substring not found");
        std::string Text = "Hello";
        EXPECT_DEATH({
            CONFIRM(Text.find("world") != std::string::npos);
        }, "Condition check failed");
    }

    // Condition fails: null pointer comparison
    {
        SCOPED_TRACE("Null pointer check fails");
        const char* Ptr = nullptr;
        EXPECT_DEATH({
            CONFIRM(Ptr != nullptr);
        }, "Condition check failed");
    }

    // Condition fails: boolean false literal
    {
        SCOPED_TRACE("Literal false condition");
        EXPECT_DEATH({
            CONFIRM(false);
        }, "Condition check failed");
    }

    ///////////////////////////////
    /// EDGE CASES
    ///////////////////////////////

    // Zero-like integer check
    {
        SCOPED_TRACE("Integer equals zero");
        int Value = 0;
        EXPECT_DEATH({
            CONFIRM(Value != 0);
        }, "Condition check failed");
    }

    // Floating point epsilon check
    {
        SCOPED_TRACE("Floating-point small value check");
        double Small = 1e-16;
        EXPECT_DEATH({
            CONFIRM(Small > 1e-10);
        }, "Condition check failed");
    }

    // Fails on empty string
    {
        SCOPED_TRACE("Empty string non-empty check fails");
        std::string S;
        EXPECT_DEATH({
            CONFIRM(!S.empty());
        }, "Condition check failed");
    }

    ///////////////////////////////
    /// INVALID INPUTS
    ///////////////////////////////

    // Simulated bad pointer dereference guard
    {
        SCOPED_TRACE("Simulated pointer invalidity");
        int* P = nullptr;
        EXPECT_DEATH({
            CONFIRM(P != nullptr);
        }, "Condition check failed");
    }

    // Invalid condition logic
    {
        SCOPED_TRACE("Division by zero is not attempted but check fails");
        int Denominator = 0;
        EXPECT_DEATH({
            CONFIRM(Denominator != 0);
        }, "Condition check failed");
    }

    ///////////////////////////////
    /// EMPTY STRUCTURES
    ///////////////////////////////

    // Empty vector check
    {
        SCOPED_TRACE("Empty vector should not be empty");
        std::vector<int> Data;
        EXPECT_DEATH({
            CONFIRM(!Data.empty());
        }, "Condition check failed");
    }

    // Empty map key check
    {
        SCOPED_TRACE("Empty map lookup failure");
        std::map<std::string, int> Map;
        EXPECT_DEATH({
            CONFIRM(Map.contains("key"));
        }, "Condition check failed");
    }

    ///////////////////////////////
    /// MAXIMUMS AND MINIMUMS
    ///////////////////////////////

    // Minimum integer comparison
    {
        SCOPED_TRACE("INT_MIN fails comparison");
        int Min = std::numeric_limits<int>::min();
        EXPECT_DEATH({
            CONFIRM(Min > 0);
        }, "Condition check failed");
    }

    // Integer overflow logic
    {
        SCOPED_TRACE("UINT_MAX fails check");
        uint Max = std::numeric_limits<uint>::max();
        EXPECT_DEATH({
            CONFIRM(Max + 1 > Max);
        }, "Condition check failed");
    }

    // Maximum floating-point check
    {
        SCOPED_TRACE("DBL_MAX fails upper bound check");
        double Max = std::numeric_limits<double>::max();
        EXPECT_DEATH({
            CONFIRM(Max < 1e308); // Always false
        }, "Condition check failed");
    }

    ///////////////////////////////
    /// SELF-CONTAINED FAILURES
    ///////////////////////////////

    // Confirm(false) hardcoded
    {
        SCOPED_TRACE("Direct confirm(false)");
        EXPECT_DEATH({
            CONFIRM(false);
        }, "Condition check failed");
    }

    // Negated true
    {
        SCOPED_TRACE("Negated true should fail");
        EXPECT_DEATH({
            CONFIRM(!true);
        }, "Condition check failed");
    }
}
