
#include "../CppFunctionBuilder/CppBuilder_Function.h"
////////
#include <gtest/gtest.h>


auto CallBuildFunction = [](const FFunction& FunctionObject)
{
    return CppBuilder_Function::BuildFunction(FunctionObject);
};


TEST(CppBuilder_Function, T01_EmptyFunctionInvalidHeaderAndBody)
{
    SCOPED_TRACE("T01: Function with invalid header and invalid body yields nullopt");
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::MAX},
            .FuncName = "",
            .Params = {}
        },
        .Body = {
            .ExecBlocks = {}
        }
    };
    std::optional<std::string> Result = CallBuildFunction(Function);
    EXPECT_FALSE(Result.has_value());
}

TEST(CppBuilder_Function, T02_ValidHeaderInvalidBody)
{
    SCOPED_TRACE("T02: Valid header with invalid body yields nullopt");
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Void},
            .FuncName = "Reset",
            .Params = {}
        },
        .Body = {
            .ExecBlocks = {}
        }
    };
    std::optional<std::string> Result = CallBuildFunction(Function);
    EXPECT_FALSE(Result.has_value());
}

TEST(CppBuilder_Function, T03_InvalidHeaderValidBody)
{
    SCOPED_TRACE("T03: Invalid header with valid body yields void return type and UnknownBlueprintFunction");
    const FFuncBody::FExecBlock Block {
        .ResultVariable = std::nullopt,
        .FunctionScope = std::nullopt,
        .FunctionName = "Clear",
        .Arguments = {}
    };
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::MAX},
            .FuncName = "",
            .Params = {}
        },
        .Body = {
            .ExecBlocks = {Block}
        }
    };
    const std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    const std::string Expected =
        "auto UnknownBlueprintFunction()\n"
        "{\n"
        "    Clear();\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}


TEST(CppBuilder_Function, T04_ValidHeaderAndBodyWithExecOnly)
{
    SCOPED_TRACE("T04: Function with valid header and exec-only body");
    const FFuncBody::FExecBlock Block {
        .ResultVariable = std::nullopt,
        .FunctionScope = std::nullopt,
        .FunctionName = "Init",
        .Arguments = {}
    };
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Void},
            .FuncName = "Initialize",
            .Params = {}
        },
        .Body = {
            .ExecBlocks = {Block}
        }
    };
    std::optional<std::string> Result = CallBuildFunction(Function);
    std::string Expected =
        "void Initialize()\n"
        "{\n"
        "    Init();\n"
        "}";
    EXPECT_EQ(Result, Expected);
}

TEST(CppBuilder_Function, T05_ValidHeaderAndBodyWithExecAndReturn)
{
    SCOPED_TRACE("T05: Function with valid header and body including return");
    const FFuncBody::FExecBlock Block {
        .ResultVariable = std::nullopt,
        .FunctionScope = std::nullopt,
        .FunctionName = "Prepare",
        .Arguments = {"Data"}
    };
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Bool},
            .FuncName = "Run",
            .Params = {{
                .ParamName = "Data",
                .ParamType = EDataType::String,
                .DefaultValue = std::nullopt
            }}
        },
        .Body = {
            .ExecBlocks = {Block},
            .ReturnArguments = {"bReady"}
        }
    };
    std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    std::string Expected =
        "bool Run(FString Data)\n"
        "{\n"
        "    Prepare(Data);\n\n"
        "    return bReady;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}

TEST(CppBuilder_Function, T06_FullFunctionWithScopedCallAndReturnVar)
{
    SCOPED_TRACE("T06: Function with result assignment, scope, and return");
    const FFuncBody::FExecBlock::FExecResult ResultVar {
        .ResultType = EDataType::Int,
        .ResultName = "Sum"
    };
    const FFuncBody::FExecBlock Block {
        .ResultVariable = std::make_optional(ResultVar),
        .FunctionScope = std::make_optional<std::string>("Math"),
        .FunctionName = "Add",
        .Arguments = {"A", "B"}
    };
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Int},
            .FuncName = "AddTwo",
            .Params = {
                {
                    .ParamName = "A",
                    .ParamType = EDataType::Int,
                    .DefaultValue = std::nullopt
                },
                {
                    .ParamName = "B",
                    .ParamType = EDataType::Int,
                    .DefaultValue = std::nullopt
                }
            }
        },
        .Body = {
            .ExecBlocks = {Block},
            .ReturnArguments = {"Sum"}
        }
    };
    std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    std::string Expected =
        "int AddTwo(int A, int B)\n"
        "{\n"
        "    int Sum = Math::Add(A, B);\n\n"
        "    return Sum;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}

TEST(CppBuilder_Function, T07_TupleReturn_TwoPrimitives)
{
    SCOPED_TRACE("T07: Function returns TTuple<int, float>");
    const FFuncBody::FExecBlock Block {
        .ResultVariable = std::nullopt,
        .FunctionScope = std::nullopt,
        .FunctionName = "GetData",
        .Arguments = {}
    };
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Int, EDataType::Float},
            .FuncName = "FetchStats",
            .Params = {}
        },
        .Body = {
            .ExecBlocks = {Block},
            .ReturnArguments = {"Result"}
        }
    };
    const std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    const std::string Expected =
        "TTuple<int, float> FetchStats()\n"
        "{\n"
        "    GetData();\n\n"
        "    return Result;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}


TEST(CppBuilder_Function, T08_TupleReturn_PrimitiveAndCustom)
{
    SCOPED_TRACE("T08: Function returns TTuple<float, FVector>");
    const FFuncBody::FExecBlock Block {
        .ResultVariable = std::nullopt,
        .FunctionScope = std::make_optional<std::string>("Physics"),
        .FunctionName = "Compute",
        .Arguments = {"Mass", "Velocity"}
    };
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Float, EDataType::Vector},
            .FuncName = "CalculateImpact",
            .Params = {
                {
                    .ParamName = "Mass",
                    .ParamType = EDataType::Float,
                    .DefaultValue = std::nullopt
                },
                {
                    .ParamName = "Velocity",
                    .ParamType = EDataType::Vector,
                    .DefaultValue = std::nullopt
                }
            }
        },
        .Body = {
            .ExecBlocks = {Block},
            .ReturnArguments = {"Impact"}
        }
    };
    const std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    const std::string Expected =
        "TTuple<float, FVector> CalculateImpact(float Mass, FVector Velocity)\n"
        "{\n"
        "    Physics::Compute(Mass, Velocity);\n\n"
        "    return Impact;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}


TEST(CppBuilder_Function, T09_TupleReturn_TripleCustomTypes)
{
    SCOPED_TRACE("T09: Function returns TTuple<FVector, FRotator, FTransform>");
    const FFuncBody::FExecBlock Block {
        .ResultVariable = std::nullopt,
        .FunctionScope = std::nullopt,
        .FunctionName = "SplitTransform",
        .Arguments = {"InputTransform"}
    };
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Vector, EDataType::Rotator, EDataType::Transform},
            .FuncName = "Decompose",
            .Params = {{
                .ParamName = "InputTransform",
                .ParamType = EDataType::Transform,
                .DefaultValue = std::nullopt
            }}
        },
        .Body = {
            .ExecBlocks = {Block},
            .ReturnArguments = {"Decomposed"}
        }
    };
    const std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    const std::string Expected =
        "TTuple<FVector, FRotator, FTransform> Decompose(FTransform InputTransform)\n"
        "{\n"
        "    SplitTransform(InputTransform);\n\n"
        "    return Decomposed;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}


TEST(CppBuilder_Function, T10_TupleReturn_AssignedResultAndReturn)
{
    SCOPED_TRACE("T10: Tuple return with assigned variable and scope");
    const FFuncBody::FExecBlock::FExecResult ResultVar {
        .ResultType = EDataType::Bool,
        .ResultName = "bIsValid"
    };
    const FFuncBody::FExecBlock Block {
        .ResultVariable = std::make_optional(ResultVar),
        .FunctionScope = std::make_optional<std::string>("Validator"),
        .FunctionName = "Check",
        .Arguments = {"Token"}
    };
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Bool, EDataType::String},
            .FuncName = "Validate",
            .Params = {{
                    .ParamName = "Token",
                    .ParamType = EDataType::String,
                    .DefaultValue = std::nullopt
            }}
        },
        .Body = {
            .ExecBlocks = {Block},
            .ReturnArguments = {"ResultTuple"}
        }
    };
    const std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    const std::string Expected =
        "TTuple<bool, FString> Validate(FString Token)\n"
        "{\n"
        "    bool bIsValid = Validator::Check(Token);\n\n"
        "    return ResultTuple;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}

TEST(CppBuilder_Function, T11_ParamWithIntDefaultValue)
{
    SCOPED_TRACE("T11: Function with int parameter having default value");
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Int},
            .FuncName = "Square",
            .Params = {{
                .ParamName = "X",
                .ParamType = EDataType::Int,
                .DefaultValue = std::make_optional<std::string>("1")
            }}
        },
        .Body = {
            .ExecBlocks = {},
            .ReturnArguments = {"X * X"}
        }
    };
    const std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    const std::string Expected =
        "int Square(int X = 1)\n"
        "{\n"
        "    return X * X;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}


TEST(CppBuilder_Function, T12_ParamWithBoolDefaultValue)
{
    SCOPED_TRACE("T12: Function with bool parameter having default value");
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Bool},
            .FuncName = "IsEnabled",
            .Params = {{
                .ParamName = "bFlag",
                .ParamType = EDataType::Bool,
                .DefaultValue = std::make_optional<std::string>("true")
            }}
        },
        .Body = {
            .ExecBlocks = {},
            .ReturnArguments = {"bFlag"}
        }
    };
    const std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    const std::string Expected =
        "bool IsEnabled(bool bFlag = true)\n"
        "{\n"
        "    return bFlag;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}


TEST(CppBuilder_Function, T13_ParamWithStringDefaultValue)
{
    SCOPED_TRACE("T13: Function with string parameter having default value");
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::String},
            .FuncName = "Echo",
            .Params = {{
                .ParamName = "Msg",
                .ParamType = EDataType::String,
                .DefaultValue = std::make_optional<std::string>("\"Hello\"")
            }}
        },
        .Body = {
            .ExecBlocks = {},
            .ReturnArguments = {"Msg"}
        }
    };
    const std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    const std::string Expected =
        "FString Echo(FString Msg = \"Hello\")\n"
        "{\n"
        "    return Msg;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}


TEST(CppBuilder_Function, T14_ParamWithCustomTypeDefaultValue)
{
    SCOPED_TRACE("T14: Function with FVector parameter having default value");
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Vector},
            .FuncName = "NormalizeOrDefault",
            .Params = {{
                .ParamName = "Input",
                .ParamType = EDataType::Vector,
                .DefaultValue = std::make_optional<std::string>("FVector::ZeroVector")
            }}
        },
        .Body = {
            .ExecBlocks = {},
            .ReturnArguments = {"Input"}
        }
    };
    const std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    const std::string Expected =
        "FVector NormalizeOrDefault(FVector Input = FVector::ZeroVector)\n"
        "{\n"
        "    return Input;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}


TEST(CppBuilder_Function, T15_MultipleParamsWithDefaults)
{
    SCOPED_TRACE("T15: Function with multiple parameters, some with default values");
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Float},
            .FuncName = "Mix",
            .Params = {
                {
                    .ParamName = "A",
                    .ParamType = EDataType::Float,
                    .DefaultValue = std::nullopt
                },
                {
                    .ParamName = "B",
                    .ParamType = EDataType::Float,
                    .DefaultValue = std::make_optional<std::string>("0.5f")
                },
                {
                    .ParamName = "Alpha",
                    .ParamType = EDataType::Float,
                    .DefaultValue = std::make_optional<std::string>("1.0f")
                }
            }
        },
        .Body = {
            .ExecBlocks = {},
            .ReturnArguments = {"A * (1 - Alpha) + B * Alpha"}
        }
    };
    const std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    const std::string Expected =
        "float Mix(float A, float B = 0.5f, float Alpha = 1.0f)\n"
        "{\n"
        "    return A * (1 - Alpha) + B * Alpha;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}

TEST(CppBuilder_Function, T16_ParamWithIntDefaultValue)
{
    SCOPED_TRACE("T16: Function with int parameter having default value");
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Int},
            .FuncName = "Square",
            .Params = {{
                .ParamName = "X",
                .ParamType = EDataType::Int,
                .DefaultValue = std::make_optional<std::string>("1")
            }}
        },
        .Body = {
            .ExecBlocks = {},
            .ReturnArguments = {"X * X"}
        }
    };
    const std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    const std::string Expected =
        "int Square(int X = 1)\n"
        "{\n"
        "    return X * X;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}


TEST(CppBuilder_Function, T17_ParamWithBoolDefaultValue)
{
    SCOPED_TRACE("T17: Function with bool parameter having default value");
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Bool},
            .FuncName = "IsEnabled",
            .Params = {{
                .ParamName = "bFlag",
                .ParamType = EDataType::Bool,
                .DefaultValue = std::make_optional<std::string>("true")
            }}
        },
        .Body = {
            .ExecBlocks = {},
            .ReturnArguments = {"bFlag"}
        }
    };
    const std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    const std::string Expected =
        "bool IsEnabled(bool bFlag = true)\n"
        "{\n"
        "    return bFlag;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}


TEST(CppBuilder_Function, T18_TupleReturnWithDefaultParams)
{
    SCOPED_TRACE("T18: Function with tuple return type and default parameters");
    const FFunction Function {
        .Header = {
            .ReturnType = {EDataType::Int, EDataType::Bool},
            .FuncName = "Analyze",
            .Params = {
                {
                    .ParamName = "Data",
                    .ParamType = EDataType::Int,
                    .DefaultValue = std::make_optional<std::string>("0")
                },
                {
                    .ParamName = "bVerbose",
                    .ParamType = EDataType::Bool,
                    .DefaultValue = std::make_optional<std::string>("false")
                }
            }
        },
        .Body = {
            .ExecBlocks = {},
            .ReturnArguments = {"ResultTuple"}
        }
    };
    const std::optional<std::string> Result = CallBuildFunction(Function);
    ASSERT_TRUE(Result.has_value());

    const std::string Expected =
        "TTuple<int, bool> Analyze(int Data = 0, bool bVerbose = false)\n"
        "{\n"
        "    return ResultTuple;\n"
        "}";
    EXPECT_EQ(*Result, Expected);
}
