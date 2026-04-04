
#include "../CppFunctionBuilder/CppBuilder_FunctionBody.h"
////////
#include <gtest/gtest.h>
////////
#include "../TestWrapper.h"


auto CallBuildBody = [](const FFuncBody& BodyObject)
{
    return CppBuilder_FunctionBody::BuildBody(BodyObject);
};


TEST(CppBuilder_FunctionBody, T01_EmptyExecAndNoReturn)
{
    SCOPED_TRACE("T01: No execution blocks and no return value yields nullopt");
    const FFuncBody Body{
        .ExecBlocks = {}
    };
    std::optional<std::string> Result = CallBuildBody(Body);
    EXPECT_FALSE(Result.has_value());
}

TEST(CppBuilder_FunctionBody, T02_OnlyReturnArgument)
{
    SCOPED_TRACE("T02: Only return value present");
    const FFuncBody Body{
        .ExecBlocks = {},
        .ReturnArguments = {"ResultValue"}
    };
    std::optional<std::string> Result = CallBuildBody(Body);
    std::string Expected =
        "{\n"
        "    return ResultValue;"
        "\n}";
    EXPECT_EQ(Result, Expected);
}

TEST(CppBuilder_FunctionBody, T03_SingleExecWithoutResultOrScope)
{
    SCOPED_TRACE("T03: One execution block with no result or scope");
    const FFuncBody::FExecBlock Block{
        .ResultVariable = std::nullopt,
        .FunctionScope = std::nullopt,
        .FunctionName = "DoSomething",
        .Arguments = {"1", "2"}
    };
    const FFuncBody Body{
        .ExecBlocks = {Block}
    };
    std::optional<std::string> Result = CallBuildBody(Body);
    ASSERT_TRUE(Result.has_value());

    std::string Expected =
        "{\n"
        "    DoSomething(1, 2);"
        "\n}";
    EXPECT_EQ(Result, Expected);
}

TEST(CppBuilder_FunctionBody, T04_ExecWithResultAndScope)
{
    SCOPED_TRACE("T04: Execution block with result and scoped function");
    const FFuncBody::FExecBlock::FExecResult ExecResult{
        .ResultType = EDataType::Bool,
        .ResultName = "bSuccess"
    };
    const FFuncBody::FExecBlock Block{
        .ResultVariable = std::make_optional(ExecResult),
        .FunctionScope = std::make_optional<std::string>("SomeLib"),
        .FunctionName = "CheckCondition",
        .Arguments = {"X", "Y"}
    };
    const FFuncBody Body{
        .ExecBlocks = {Block}
    };
    std::optional<std::string> Result = CallBuildBody(Body);
    std::string Expected =
        "{\n"
        "    bool bSuccess = SomeLib::CheckCondition(X, Y);"
        "\n}";
    EXPECT_EQ(Result, Expected);
}

TEST(CppBuilder_FunctionBody, T05_ExecAndReturnTogether)
{
    SCOPED_TRACE("T05: Both execution block and return argument present");
    const FFuncBody::FExecBlock Block{
        .ResultVariable = std::nullopt,
        .FunctionScope = std::nullopt,
        .FunctionName = "Compute",
        .Arguments = {"5"}
    };
    const FFuncBody Body{
        .ExecBlocks = {Block},
        .ReturnArguments = {"5"}
    };
    std::optional<std::string> Result = CallBuildBody(Body);
    std::string Expected =
        "{\n"
        "    Compute(5);\n\n"
        "    return 5;"
        "\n}";
    EXPECT_EQ(Result, Expected);
}

TEST(CppBuilder_FunctionBody, T06_ExecWithMultipleBlocks)
{
    SCOPED_TRACE("T06: Multiple valid execution blocks");
    const FFuncBody::FExecBlock Block1{
        .ResultVariable = std::nullopt,
        .FunctionScope = std::nullopt,
        .FunctionName = "Start",
        .Arguments = {}
    };
    const FFuncBody::FExecBlock::FExecResult Result2{
        .ResultType = EDataType::Int,
        .ResultName = "Total"
    };
    const FFuncBody::FExecBlock Block2{
        .ResultVariable = std::make_optional(Result2),
        .FunctionScope = std::make_optional<std::string>("Math"),
        .FunctionName = "Sum",
        .Arguments = {"A", "B"}
    };
    const FFuncBody Body{
        .ExecBlocks = {Block1, Block2},
        .ReturnArguments = {"Total"}
    };
    std::optional<std::string> Result = CallBuildBody(Body);
    ASSERT_TRUE(Result.has_value());
    std::string Expected =
        "{\n"
        "    Start();\n"
        "    int Total = Math::Sum(A, B);\n\n"
        "    return Total;"
        "\n}";
    EXPECT_EQ(*Result, Expected);
}
TEST(CppBuilder_FunctionBody, T07_OnlyMultipleReturnArguments)
{
    SCOPED_TRACE("T07: Only multiple return arguments present, expect MakeTuple");
    const FFuncBody Body{
        .ExecBlocks = {},
        .ReturnArguments = {"A", "B", "C"}
    };
    std::optional<std::string> Result = CallBuildBody(Body);
    ASSERT_TRUE(Result.has_value());

    std::string Expected =
        "{\n"
        "    return MakeTuple(A, B, C);"
        "\n}";
    EXPECT_EQ(*Result, Expected);
}

TEST(CppBuilder_FunctionBody, T08_ExecAndMultipleReturnArguments)
{
    SCOPED_TRACE("T08: Execution block plus multiple return arguments");
    const FFuncBody::FExecBlock Block{
        .ResultVariable = std::nullopt,
        .FunctionScope = std::make_optional<std::string>("Lib"),
        .FunctionName = "Generate",
        .Arguments = {"X"}
    };
    const FFuncBody Body{
        .ExecBlocks = {Block},
        .ReturnArguments = {"Res1", "Res2"}
    };
    std::optional<std::string> Result = CallBuildBody(Body);
    ASSERT_TRUE(Result.has_value());

    std::string Expected =
        "{\n"
        "    Lib::Generate(X);\n\n"
        "    return MakeTuple(Res1, Res2);"
        "\n}";
    EXPECT_EQ(*Result, Expected);
}

TEST(CppBuilder_FunctionBody, T09_MultipleExecBlocksAndMultipleReturnArguments)
{
    SCOPED_TRACE("T09: Multiple exec blocks with result assignments and multiple return args");
    const FFuncBody::FExecBlock::FExecResult Result1{
        .ResultType = EDataType::Int,
        .ResultName = "Sum"
    };
    const FFuncBody::FExecBlock Block1{
        .ResultVariable = std::make_optional(Result1),
        .FunctionScope = std::make_optional<std::string>("Math"),
        .FunctionName = "Add",
        .Arguments = {"A", "B"}
    };
    const FFuncBody::FExecBlock::FExecResult Result2{
        .ResultType = EDataType::Bool,
        .ResultName = "bValid"
    };
    const FFuncBody::FExecBlock Block2{
        .ResultVariable = std::make_optional(Result2),
        .FunctionScope = std::make_optional<std::string>("Validator"),
        .FunctionName = "Check",
        .Arguments = {"Sum"}
    };
    const FFuncBody Body{
        .ExecBlocks = {Block1, Block2},
        .ReturnArguments = {"Sum", "bValid"}
    };
    std::optional<std::string> Result = CallBuildBody(Body);
    ASSERT_TRUE(Result.has_value());

    std::string Expected =
        "{\n"
        "    int Sum = Math::Add(A, B);\n"
        "    bool bValid = Validator::Check(Sum);\n\n"
        "    return MakeTuple(Sum, bValid);"
        "\n}";
    EXPECT_EQ(*Result, Expected);
}
