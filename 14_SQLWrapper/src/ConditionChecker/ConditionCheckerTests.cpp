#include <limits>
#include <string>
#include <vector>
#include <map>
////////
#include <gtest/gtest.h>
////////
#include "ConditionChecker.h"


// ================================
// ░ Test Case Descriptions ░
// ================================

// T01: ReportConditionWarning prints all parts of a warning message including expression, file, and formatted message to std::cerr.
// T02: ReportConditionWarning handles empty custom message correctly and still prints expression and file.

// T03: ReportConditionFailure throws exception even with default (empty) error message.
// T04: ReportConditionFailure throws exception with detailed message when a custom formatted error is provided.

// T05: CONFIRM macro does nothing if the condition is true.
// T06: CONFIRM macro throws an exception with proper details when the condition is false.
// T07: CONFIRMS macro does nothing if the condition is true, even with formatting arguments.
// T08: CONFIRMS macro throws and includes the formatted message in the exception.
// T09: WARNIFFALSE does not print anything to std::cerr when condition is true.
// T10: WARNIFFALSE prints a structured warning with formatted message when condition is false.
// T11: CONFIRMS handles minimal formatted message gracefully (e.g., non-variadic call).

// T12: SetStacktraceEnabled can be toggled without crashing or throwing.
// T13: When stacktrace is enabled, ReportConditionFailure includes full stack trace in exception message.
// T14: When stacktrace is enabled, ReportConditionWarning includes full stack trace in std::cerr output.
// T15: When stacktrace is disabled, ReportConditionFailure does not include stack trace in exception message.
// T16: When stacktrace is disabled, ReportConditionWarning does not include stack trace in std::cerr output.


///////////////////////////

////////////////////////////////////////
// T01_WarningMessageIsStructuredCorrectly
////////////////////////////////////////
class ConditionChecker_T01_WarningMessageIsStructuredCorrectly : public ::testing::Test {};

TEST(ConditionChecker_T01_WarningMessageIsStructuredCorrectly, WarningOutputContainsExpectedParts)
{
    SCOPED_TRACE("== T01: ReportConditionWarning emits correctly structured warning to std::cerr ==");

    ::testing::internal::CaptureStderr();
    ::ConditionChecker::ReportConditionWarning("x > 0", "FakeFile.cpp", 123, "Non-critical failure");
    std::string Output = ::testing::internal::GetCapturedStderr();

    EXPECT_TRUE(Output.find("condition check failed") != std::string::npos);
    EXPECT_TRUE(Output.find("Expression: x > 0") != std::string::npos);
    EXPECT_TRUE(Output.find("Location  : FakeFile.cpp : 123") != std::string::npos);
    EXPECT_TRUE(Output.find("Message   : [ Non-critical failure ]") != std::string::npos);
}

////////////////////////////////////////
// T02_EmptyWarningMessage
////////////////////////////////////////
class ConditionChecker_T02_EmptyWarningMessage : public ::testing::Test {};

TEST(ConditionChecker_T02_EmptyWarningMessage, EmitsWarningWithEmptyMessageField)
{
    SCOPED_TRACE("== T02: ReportConditionWarning prints structure even with empty message ==");

    ::testing::internal::CaptureStderr();
    ::ConditionChecker::ReportConditionWarning("ptr != nullptr", "SomeFile.cpp", 45, "");
    std::string Output = ::testing::internal::GetCapturedStderr();

    EXPECT_TRUE(Output.find("Expression: ptr != nullptr") != std::string::npos);
    EXPECT_TRUE(Output.find("Message   : [  ]") != std::string::npos);
}

////////////////////////////////////////
// T03_ThrowsStructuredFailureMessage_DefaultError
////////////////////////////////////////
class ConditionChecker_T03_ThrowsStructuredFailureMessage_DefaultError : public ::testing::Test {};

TEST(ConditionChecker_T03_ThrowsStructuredFailureMessage_DefaultError, ThrowsWithEmptyMessageStructured)
{
    SCOPED_TRACE("== T03: ReportConditionFailure throws with structured message (empty custom text) ==");

    try
    {
        ::ConditionChecker::ReportConditionFailure("a == b", "Logic.cpp", 999);
        FAIL() << "Expected exception to be thrown";
    }
    catch (const std::exception& E)
    {
        std::string Msg = E.what();
        EXPECT_TRUE(Msg.find("condition check failed") != std::string::npos);
        EXPECT_TRUE(Msg.find("Expression: a == b") != std::string::npos);
        EXPECT_TRUE(Msg.find("Location  : Logic.cpp : 999") != std::string::npos);
        EXPECT_TRUE(Msg.find("Message   : [  ]") != std::string::npos);
    }
}

////////////////////////////////////////
// T04_ThrowsStructuredFailureMessage_WithFormat
////////////////////////////////////////
class ConditionChecker_T04_ThrowsStructuredFailureMessage_WithFormat : public ::testing::Test {};

TEST(ConditionChecker_T04_ThrowsStructuredFailureMessage_WithFormat, ThrowsWithCustomFormattedMessage)
{
    SCOPED_TRACE("== T04: CONFIRMS throws exception with formatted message ==");

    try
    {
        CONFIRMS(false, "Expected value: {}, Actual value: {}", 10, 20);
        FAIL() << "Expected exception to be thrown";
    }
    catch (const std::exception& E)
    {
        std::string Msg = E.what();
        EXPECT_TRUE(Msg.find("Expression: false") != std::string::npos);
        EXPECT_TRUE(Msg.find("Expected value: 10, Actual value: 20") != std::string::npos);
    }
}

////////////////////////////////////////
// T05_WarnIfFalse_FormattedOutput
////////////////////////////////////////
class ConditionChecker_T05_WarnIfFalse_FormattedOutput : public ::testing::Test {};

TEST(ConditionChecker_T05_WarnIfFalse_FormattedOutput, OutputsStructuredWarning)
{
    SCOPED_TRACE("== T05: WARNIFFALSE prints formatted warning ==");

    ::testing::internal::CaptureStderr();
    WARNIFFALSE(2 + 2 == 5, "Math is broken: got {}", 4);
    std::string Output = ::testing::internal::GetCapturedStderr();

    EXPECT_TRUE(Output.find("Expression: 2 + 2 == 5") != std::string::npos);
    EXPECT_TRUE(Output.find("Message   : [ Math is broken: got 4 ]") != std::string::npos);
}

////////////////////////////////////////
// T06_ConfirmMacro_NoThrow
////////////////////////////////////////
class ConditionChecker_T06_ConfirmMacro_NoThrow : public ::testing::Test {};

TEST(ConditionChecker_T06_ConfirmMacro_NoThrow, ConfirmTruePassesSilently)
{
    SCOPED_TRACE("== T06: CONFIRM does nothing if expression is true ==");

    EXPECT_NO_THROW({
        CONFIRM(1 + 1 == 2);
    });
}

////////////////////////////////////////
// T07_ConfirmMacro_ThrowsStructured
////////////////////////////////////////
class ConditionChecker_T07_ConfirmMacro_ThrowsStructured : public ::testing::Test {};

TEST(ConditionChecker_T07_ConfirmMacro_ThrowsStructured, ThrowsWithExprInfo)
{
    SCOPED_TRACE("== T07: CONFIRM throws with basic failure structure ==");

    try
    {
        CONFIRM(false);
        FAIL() << "Expected failure from CONFIRM";
    }
    catch (const std::exception& E)
    {
        std::string Msg = E.what();
        EXPECT_TRUE(Msg.find("Expression: false") != std::string::npos);
    }
}

////////////////////////////////////////
// T08_ConfirmsMacro_NoThrow
////////////////////////////////////////
class ConditionChecker_T08_ConfirmsMacro_NoThrow : public ::testing::Test {};

TEST(ConditionChecker_T08_ConfirmsMacro_NoThrow, ConfirmWithMessageDoesNothingWhenTrue)
{
    SCOPED_TRACE("== T08: CONFIRMS does not throw if expression is true ==");

    EXPECT_NO_THROW({
        CONFIRMS(10 == 10, "Should not fail, values: {} and {}", 10, 10);
    });
}

////////////////////////////////////////
// T09_WarnIfFalse_TrueCondition
////////////////////////////////////////
class ConditionChecker_T09_WarnIfFalse_TrueCondition : public ::testing::Test {};

TEST(ConditionChecker_T09_WarnIfFalse_TrueCondition, EmitsNothingOnSuccess)
{
    SCOPED_TRACE("== T09: WARNIFFALSE does not emit anything when condition is true ==");

    ::testing::internal::CaptureStderr();
    WARNIFFALSE(5 < 10, "Should not trigger warning");
    std::string Output = ::testing::internal::GetCapturedStderr();

    EXPECT_TRUE(Output.empty());
}

////////////////////////////////////////
// T10_WarnIfFalse_EmptyMessage
////////////////////////////////////////
class ConditionChecker_T10_WarnIfFalse_EmptyMessage : public ::testing::Test {};

TEST(ConditionChecker_T10_WarnIfFalse_EmptyMessage, HandlesEmptyFormatCorrectly)
{
    SCOPED_TRACE("== T10: WARNIFFALSE prints properly even with minimal format string ==");

    ::testing::internal::CaptureStderr();
    WARNIFFALSE(false, "");
    std::string Output = ::testing::internal::GetCapturedStderr();

    EXPECT_TRUE(Output.find("Expression: false") != std::string::npos);
    EXPECT_TRUE(Output.find("Message   : [  ]") != std::string::npos);
}

////////////////////////////////////////
// T11_ConfirmsMinimalFormattedMessage
////////////////////////////////////////
class ConditionChecker_T11_ConfirmsMinimalFormattedMessage : public ::testing::Test {};

TEST(ConditionChecker_T11_ConfirmsMinimalFormattedMessage, ThrowsWithMinimalFormattedMessage)
{
    SCOPED_TRACE("== T11: CONFIRMS throws properly with minimal message ==");

    try
    {
        CONFIRMS(false, "Failure.");
        FAIL() << "Expected exception to be thrown";
    }
    catch (const std::exception& E)
    {
        std::string Msg = E.what();
        EXPECT_TRUE(Msg.find("Expression: false") != std::string::npos);
        EXPECT_TRUE(Msg.find("Message   : [ Failure. ]") != std::string::npos);
    }
}

////////////////////////////////////////
// T12_SetStacktraceEnabled_Toggle
////////////////////////////////////////
class ConditionChecker_T12_SetStacktraceEnabled_Toggle : public ::testing::Test {};

TEST(ConditionChecker_T12_SetStacktraceEnabled_Toggle, CanBeToggledWithoutException)
{
    SCOPED_TRACE("== T12: SetStacktraceEnabled can be called with true/false without exception ==");

    EXPECT_NO_THROW({
        ::ConditionChecker::SetStacktraceEnabled(true);
        ::ConditionChecker::SetStacktraceEnabled(false);
    });
}


////////////////////////////////////////
// T13_ReportConditionFailure_StacktraceEnabled
////////////////////////////////////////
class ConditionChecker_T13_ReportConditionFailure_StacktraceEnabled : public ::testing::Test {};

TEST(ConditionChecker_T13_ReportConditionFailure_StacktraceEnabled, IncludesStacktraceWhenEnabled)
{
    SCOPED_TRACE("== T13: ReportConditionFailure includes stack trace when enabled ==");

    ConditionChecker::SetStacktraceEnabled(true);

    try
    {
        CONFIRMS(false, "Trigger failure for stack trace");
        FAIL() << "Expected exception";
    }
    catch (const std::exception& E)
    {
        std::string Msg = E.what();
        EXPECT_TRUE(Msg.contains("🔍 Stack trace:"));
    }
}

////////////////////////////////////////
// T14_ReportConditionWarning_StacktraceEnabled
////////////////////////////////////////
class ConditionChecker_T14_ReportConditionWarning_StacktraceEnabled : public ::testing::Test {};

TEST(ConditionChecker_T14_ReportConditionWarning_StacktraceEnabled, IncludesStacktraceInWarningOutput)
{
    SCOPED_TRACE("== T14: ReportConditionWarning includes stack trace when enabled ==");

    ConditionChecker::SetStacktraceEnabled(true);

    ::testing::internal::CaptureStderr();
    WARNIFFALSE(false, "Trigger warning for stack trace");
    std::string Output = ::testing::internal::GetCapturedStderr();

    EXPECT_TRUE(Output.contains("🔍 Stack trace:"));
}

////////////////////////////////////////
// T15_ReportConditionFailure_StacktraceDisabled
////////////////////////////////////////
class ConditionChecker_T15_ReportConditionFailure_StacktraceDisabled : public ::testing::Test {};

TEST(ConditionChecker_T15_ReportConditionFailure_StacktraceDisabled, NoStacktraceWhenDisabled)
{
    SCOPED_TRACE("== T15: ReportConditionFailure omits stack trace when disabled ==");

    ::ConditionChecker::SetStacktraceEnabled(false);

    try
    {
        CONFIRM(false);
        FAIL() << "Expected exception";
    }
    catch (const std::exception& E)
    {
        std::string Msg = E.what();
        EXPECT_TRUE(Msg.find("🔍 Stack trace:") == std::string::npos);
        EXPECT_TRUE(Msg.find("boost::stacktrace") == std::string::npos);
    }
}

////////////////////////////////////////
// T16_ReportConditionWarning_StacktraceDisabled
////////////////////////////////////////
class ConditionChecker_T16_ReportConditionWarning_StacktraceDisabled : public ::testing::Test {};

TEST(ConditionChecker_T16_ReportConditionWarning_StacktraceDisabled, NoStacktraceInWarningOutput)
{
    SCOPED_TRACE("== T16: ReportConditionWarning omits stack trace when disabled ==");

    ::ConditionChecker::SetStacktraceEnabled(false);

    ::testing::internal::CaptureStderr();
    WARNIFFALSE(false, "Trigger warning with stacktrace off");
    std::string Output = ::testing::internal::GetCapturedStderr();

    EXPECT_TRUE(Output.find("🔍 Stack trace:") == std::string::npos);
    EXPECT_TRUE(Output.find("boost::stacktrace") == std::string::npos);
}