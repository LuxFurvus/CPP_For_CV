
#include "../CppFunctionBuilder/CppBuilder_FunctionHeader.h"
////////
#include <gtest/gtest.h>


struct BuildHeaderWrapper : public CppBuilder_FunctionHeader
{
    const FFuncHeader HeaderMember;
    BuildHeaderWrapper(const FFuncHeader& Header) : HeaderMember(Header) {};
    operator auto()
    {
        return CppBuilder_FunctionHeader::BuildHeader(HeaderMember);
    }
};


TEST(CppBuilder_FunctionHeader, T01_SingleVoidFunctionNoParams)
{
    SCOPED_TRACE("T01: Function with void return type and no parameters");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Void},
        .FuncName = "DoNothing",
        .Params = {}
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected = "void DoNothing()\n";
    EXPECT_EQ(Result, Expected);
}


TEST(CppBuilder_FunctionHeader, T02_FunctionWithSingleIntParam)
{
    SCOPED_TRACE("T02: Function with one integer parameter");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Int},
        .FuncName = "GetValue",
        .Params = {
            { .ParamName = "Index", .ParamType = EDataType::Int, .DefaultValue = std::nullopt }
        }
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected = "int GetValue(int Index)\n";
    EXPECT_EQ(Result, Expected);
}


TEST(CppBuilder_FunctionHeader, T03_FunctionWithMultipleParams)
{
    SCOPED_TRACE("T03: Function with multiple typed parameters");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Float},
        .FuncName = "Lerp",
        .Params = {
            { .ParamName = "A", .ParamType = EDataType::Float, .DefaultValue = std::nullopt },
            { .ParamName = "B", .ParamType = EDataType::Float, .DefaultValue = std::nullopt },
            { .ParamName = "Alpha", .ParamType = EDataType::Float, .DefaultValue = std::nullopt }
        }
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected
        = "float Lerp(float A, float B, float Alpha)\n";
    EXPECT_EQ(Result, Expected);
}


TEST(CppBuilder_FunctionHeader, T04_CustomTypeAsReturnType)
{
    SCOPED_TRACE("T04: Function with custom struct as return type");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Vector},
        .FuncName = "ComputeDirection",
        .Params = {
            { .ParamName = "From", .ParamType = EDataType::Vector, .DefaultValue = std::nullopt },
            { .ParamName = "To", .ParamType = EDataType::Vector, .DefaultValue = std::nullopt }
        }
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected
        = "FVector ComputeDirection(FVector From, FVector To)\n";
    EXPECT_EQ(Result, Expected);
}


TEST(CppBuilder_FunctionHeader, T05_EmptyFunctionName)
{
    SCOPED_TRACE("T05: Empty Function Name -> fallback to UnknownBlueprintFunction");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Bool},
        .FuncName = "",
        .Params = {
            { .ParamName = "bFlag", .ParamType = EDataType::Bool, .DefaultValue = std::nullopt }
        }
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    ASSERT_TRUE(Result.has_value());

    const std::optional<std::string> Expected =
        "bool UnknownBlueprintFunction(bool bFlag)\n";
    EXPECT_EQ(Result, Expected);
}


TEST(CppBuilder_FunctionHeader, T06_ParamNameEdgeCases)
{
    SCOPED_TRACE("T06: Unusual parameter names with underscores and digits");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Int},
        .FuncName = "Compute",
        .Params = {
            { .ParamName = "_hidden", .ParamType = EDataType::Int, .DefaultValue = std::nullopt },
            { .ParamName = "value2", .ParamType = EDataType::Float, .DefaultValue = std::nullopt }
        }
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected
        = "int Compute(int _hidden, float value2)\n";
    EXPECT_EQ(Result, Expected);
}

TEST(CppBuilder_FunctionHeader, T07_MultiplePrimitiveReturns)
{
    SCOPED_TRACE("T07: Function returning a tuple of int and float");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Int, EDataType::Float},
        .FuncName = "GetIndexAndWeight",
        .Params = {}
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected = "TTuple<int, float> GetIndexAndWeight()\n";
    EXPECT_EQ(Result, Expected);
}

TEST(CppBuilder_FunctionHeader, T08_PrimitiveAndCustomReturn)
{
    SCOPED_TRACE("T08: Function returning a tuple of float and FVector");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Float, EDataType::Vector},
        .FuncName = "CalculateImpact",
        .Params = {
            { .ParamName = "Mass", .ParamType = EDataType::Float, .DefaultValue = std::nullopt },
            { .ParamName = "Velocity", .ParamType = EDataType::Vector, .DefaultValue = std::nullopt }
        }
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected
        = "TTuple<float, FVector> CalculateImpact(float Mass, FVector Velocity)\n";
    EXPECT_EQ(Result, Expected);
}

TEST(CppBuilder_FunctionHeader, T09_TripleCustomReturns)
{
    SCOPED_TRACE("T09: Function returning a tuple of FVector, FRotator, and FTransform");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Vector, EDataType::Rotator, EDataType::Transform},
        .FuncName = "DecomposeTransform",
        .Params = {
            { .ParamName = "Input", .ParamType = EDataType::Transform, .DefaultValue = std::nullopt }
        }
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected
        = "TTuple<FVector, FRotator, FTransform> DecomposeTransform(FTransform Input)\n";
    EXPECT_EQ(Result, Expected);
}

TEST(CppBuilder_FunctionHeader, T10_TupleReturnWithMultipleTupleLikeParams)
{
    SCOPED_TRACE("T10: Tuple return and multiple structured params");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Int, EDataType::Bool},
        .FuncName = "AnalyzeData",
        .Params = {
            { .ParamName = "Data", .ParamType = EDataType::Vector, .DefaultValue = std::nullopt },
            { .ParamName = "bVerbose", .ParamType = EDataType::Bool, .DefaultValue = std::nullopt }
        }
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected
        = "TTuple<int, bool> AnalyzeData(FVector Data, bool bVerbose)\n";
    EXPECT_EQ(Result, Expected);
}

TEST(CppBuilder_FunctionHeader, T11_ParamWithIntDefaultValue)
{
    SCOPED_TRACE("T11: Single int parameter with default value");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Int},
        .FuncName = "Square",
        .Params = {
            { .ParamName = "X", .ParamType = EDataType::Int, .DefaultValue = std::make_optional<std::string>("1") }
        }
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected = "int Square(int X = 1)\n";
    EXPECT_EQ(Result, Expected);
}


TEST(CppBuilder_FunctionHeader, T12_ParamWithBoolDefaultValue)
{
    SCOPED_TRACE("T12: Bool parameter with default value");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Bool},
        .FuncName = "IsEnabled",
        .Params = {
            { .ParamName = "bFlag", .ParamType = EDataType::Bool, .DefaultValue = std::make_optional<std::string>("true") }
        }
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected = "bool IsEnabled(bool bFlag = true)\n";
    EXPECT_EQ(Result, Expected);
}


TEST(CppBuilder_FunctionHeader, T13_ParamWithStringDefaultValue)
{
    SCOPED_TRACE("T13: String parameter with default value");
    const FFuncHeader Header {
        .ReturnType = {EDataType::String},
        .FuncName = "Echo",
        .Params = {
            { .ParamName = "Msg", .ParamType = EDataType::String, .DefaultValue = std::make_optional<std::string>("\"Hello\"") }
        }
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected = "FString Echo(FString Msg = \"Hello\")\n";
    EXPECT_EQ(Result, Expected);
}


TEST(CppBuilder_FunctionHeader, T14_ParamWithCustomTypeDefaultValue)
{
    SCOPED_TRACE("T14: Custom type parameter with default value");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Vector},
        .FuncName = "NormalizeOrDefault",
        .Params = {
            { .ParamName = "Input", .ParamType = EDataType::Vector, .DefaultValue = std::make_optional<std::string>("FVector::ZeroVector") }
        }
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected = "FVector NormalizeOrDefault(FVector Input = FVector::ZeroVector)\n";
    EXPECT_EQ(Result, Expected);
}


TEST(CppBuilder_FunctionHeader, T15_MultipleParamsWithDefaults)
{
    SCOPED_TRACE("T15: Multiple parameters, some with defaults");
    const FFuncHeader Header {
        .ReturnType = {EDataType::Float},
        .FuncName = "Mix",
        .Params = {
            { .ParamName = "A", .ParamType = EDataType::Float, .DefaultValue = std::nullopt },
            { .ParamName = "B", .ParamType = EDataType::Float, .DefaultValue = std::make_optional<std::string>("0.5f") },
            { .ParamName = "Alpha", .ParamType = EDataType::Float, .DefaultValue = std::make_optional<std::string>("1.0f") }
        }
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected = "float Mix(float A, float B = 0.5f, float Alpha = 1.0f)\n";
    EXPECT_EQ(Result, Expected);
}

TEST(CppBuilder_FunctionHeader, T16_SingleInvalidType)
{
    SCOPED_TRACE("T16: Single invalid type returns auto");
    const FFuncHeader Header {
        .ReturnType = {EDataType::MAX}, // невалидный
        .FuncName = "Fallback",
        .Params = {}
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected = "auto Fallback()\n";
    EXPECT_EQ(Result, Expected);
}


TEST(CppBuilder_FunctionHeader, T17_TupleMixedValidInvalid)
{
    SCOPED_TRACE("T17: Tuple return skips invalid and void types but keeps valid");
    const FFuncHeader Header {
        .ReturnType = {EDataType::MAX, EDataType::Int, EDataType::Void, EDataType::Float},
        .FuncName = "Mixed",
        .Params = {}
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected = "TTuple<int, float> Mixed()\n";
    EXPECT_EQ(Result, Expected);
}


TEST(CppBuilder_FunctionHeader, T18_TupleAllInvalid)
{
    SCOPED_TRACE("T18: Tuple return all invalid/void types => auto");
    const FFuncHeader Header {
        .ReturnType = {EDataType::MAX, EDataType::None, EDataType::Exec, EDataType::Void},
        .FuncName = "AllInvalid",
        .Params = {}
    };
    const std::optional<std::string> Result = BuildHeaderWrapper(Header);
    const std::optional<std::string> Expected = "auto AllInvalid()\n";
    EXPECT_EQ(Result, Expected);
}
