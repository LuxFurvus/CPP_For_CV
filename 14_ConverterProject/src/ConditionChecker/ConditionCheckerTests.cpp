#include <limits>
#include <string>
#include <vector>
#include <map>
#include <gtest/gtest.h>
#include "ConditionChecker.h"

using namespace ::testing::internal;

///////////////////////////
// CORE FUNCTION TESTS
///////////////////////////

TEST(ConditionChecker, T01_ReportBasicInfo)
{
    SCOPED_TRACE("Standard failure report with valid condition and location");
    EXPECT_THROW({
        try {
            ConditionChecker::ReportConditionFailure("x > 0", "test.cpp", 42);
        } catch (const std::runtime_error& e) {
            EXPECT_NE(std::string(e.what()).find("x > 0"), std::string::npos);
            throw;
        }
    }, std::runtime_error);
}

TEST(ConditionChecker, T02_ReportNullCondition)
{
    SCOPED_TRACE("Null condition expression string");
    EXPECT_THROW({
        ConditionChecker::ReportConditionFailure(nullptr, "file.cpp", 7);
    }, std::runtime_error);
}

TEST(ConditionChecker, T03_ReportNullFile)
{
    SCOPED_TRACE("Null file name string");
    EXPECT_THROW({
        ConditionChecker::ReportConditionFailure("SomeCondition", nullptr, 99);
    }, std::runtime_error);
}

TEST(ConditionChecker, T04_ReportEmptyConditionAndFile)
{
    SCOPED_TRACE("Empty condition and empty file name");
    EXPECT_THROW({
        ConditionChecker::ReportConditionFailure("", "", 1);
    }, std::runtime_error);
}

TEST(ConditionChecker, T05_ReportLongCondition)
{
    SCOPED_TRACE("Long condition string");
    std::string LongCond(1000, 'X');
    EXPECT_THROW({
        ConditionChecker::ReportConditionFailure(LongCond.c_str(), "long.cpp", 17);
    }, std::runtime_error);
}

TEST(ConditionChecker, T06_ReportExtremeLineNumbers)
{
    SCOPED_TRACE("Extreme line number values");
    EXPECT_THROW({
        ConditionChecker::ReportConditionFailure("MinLine", "file.cpp", std::numeric_limits<int>::min());
    }, std::runtime_error);

    EXPECT_THROW({
        ConditionChecker::ReportConditionFailure("MaxLine", "file.cpp", std::numeric_limits<int>::max());
    }, std::runtime_error);
}

///////////////////////////
// STACKTRACE ENABLEMENT TESTS
///////////////////////////

TEST(ConditionChecker, T07_EnableStacktraceTrue)
{
    SCOPED_TRACE("Enable stacktrace and verify flag");
    ConditionChecker::SetStacktraceEnabled(true);
    EXPECT_TRUE(ConditionChecker::IsStacktraceEnabled());
}

TEST(ConditionChecker, T08_EnableStacktraceFalse)
{
    SCOPED_TRACE("Disable stacktrace and verify flag");
    ConditionChecker::SetStacktraceEnabled(false);
    EXPECT_FALSE(ConditionChecker::IsStacktraceEnabled());
}

TEST(ConditionChecker, T09_StacktraceIsPrintedWhenEnabled)
{
    SCOPED_TRACE("Capture output and expect stack trace when enabled");

    ConditionChecker::SetStacktraceEnabled(true);

    std::string Output;
    try
    {
        CaptureStderr();
        ConditionChecker::ReportConditionFailure("TraceEnabled", "trace.cpp", 777);
    }
    catch (const std::runtime_error& E)
    {
        Output = GetCapturedStderr();
        std::string What = E.what();
        Output += What;
    }

    EXPECT_NE(Output.find("Stack trace"), std::string::npos);
}

TEST(ConditionChecker, T10_StacktraceIsOmittedWhenDisabled)
{
    SCOPED_TRACE("Capture output and expect no stack trace when disabled");

    ConditionChecker::SetStacktraceEnabled(false);

    std::string Output;
    try
    {
        CaptureStderr();
        ConditionChecker::ReportConditionFailure("TraceDisabled", "trace.cpp", 888);
    }
    catch (const std::runtime_error& E)
    {
        Output = GetCapturedStderr();
        std::string What = E.what();
        Output += What;
    }

    EXPECT_EQ(Output.find("Stack trace"), std::string::npos);
}

///////////////////////////
// MACRO TESTS: CONFIRM
///////////////////////////

TEST(ConditionChecker, T11_ConfirmFalseLiteral)
{
    SCOPED_TRACE("CONFIRM(false)");
    EXPECT_THROW({
        CONFIRM(false);
    }, std::runtime_error);
}

TEST(ConditionChecker, T12_ConfirmNullPointer)
{
    SCOPED_TRACE("CONFIRM(nullptr != nullptr)");
    const char* Ptr = nullptr;
    EXPECT_THROW({
        CONFIRM(Ptr != nullptr);
    }, std::runtime_error);
}

TEST(ConditionChecker, T13_ConfirmZeroInt)
{
    SCOPED_TRACE("CONFIRM(0 != 0)");
    int Value = 0;
    EXPECT_THROW({
        CONFIRM(Value != 0);
    }, std::runtime_error);
}

TEST(ConditionChecker, T14_ConfirmEmptyString)
{
    SCOPED_TRACE("Empty std::string should not be empty");
    std::string Text;
    EXPECT_THROW({
        CONFIRM(!Text.empty());
    }, std::runtime_error);
}

TEST(ConditionChecker, T15_ConfirmVectorEmpty)
{
    SCOPED_TRACE("Empty vector expected to be non-empty");
    std::vector<int> V;
    EXPECT_THROW({
        CONFIRM(!V.empty());
    }, std::runtime_error);
}

TEST(ConditionChecker, T16_ConfirmMapMissingKey)
{
    SCOPED_TRACE("Missing key in std::map");
    std::map<std::string, int> M;
    EXPECT_THROW({
        CONFIRM(M.contains("missing"));
    }, std::runtime_error);
}

TEST(ConditionChecker, T17_ConfirmDoubleTooSmall)
{
    SCOPED_TRACE("Double comparison fails");
    double Val = 1e-15;
    EXPECT_THROW({
        CONFIRM(Val > 1e-10);
    }, std::runtime_error);
}

TEST(ConditionChecker, T18_ConfirmMaxDoubleTooLarge)
{
    SCOPED_TRACE("DBL_MAX < threshold fails");
    double Max = std::numeric_limits<double>::max();
    EXPECT_THROW({
        CONFIRM(Max < 1e308);
    }, std::runtime_error);
}

TEST(ConditionChecker, T19_ConfirmIntMinNegative)
{
    SCOPED_TRACE("INT_MIN > 0 check fails");
    int Min = std::numeric_limits<int>::min();
    EXPECT_THROW({
        CONFIRM(Min > 0);
    }, std::runtime_error);
}

TEST(ConditionChecker, T20_ConfirmUnsignedWraparound)
{
    SCOPED_TRACE("UINT_MAX + 1 > UINT_MAX always false");
    unsigned int Max = std::numeric_limits<unsigned int>::max();
    EXPECT_THROW({
        CONFIRM(Max + 1 > Max);
    }, std::runtime_error);
}

TEST(ConditionChecker, T21_ConfirmFalseViaNotTrue)
{
    SCOPED_TRACE("CONFIRM(!true)");
    EXPECT_THROW({
        CONFIRM(!true);
    }, std::runtime_error);
}

TEST(ConditionChecker, T22_ConfirmStringNotFound)
{
    SCOPED_TRACE("std::string::find returns npos");
    std::string Text = "Hello";
    EXPECT_THROW({
        CONFIRM(Text.find("world") != std::string::npos);
    }, std::runtime_error);
}
