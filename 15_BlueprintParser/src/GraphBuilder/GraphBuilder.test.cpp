
#include "../GraphBuilder/GraphBuilder.h"
//
#include <string>
#include <vector>
//
#include <gtest/gtest.h>
//

namespace
{
    void CheckIfBpToCppCorrect(const std::string_view BpCode, const std::string_view ExpectedCppCode)
    {
        const std::string ResultCppCode = FGraphBuilder::Evaluate(BpCode);
        EXPECT_EQ(ResultCppCode, ExpectedCppCode);
    }
} // namespace


TEST(FGraphBuilder, T01_E2E_OneCallFunctionNodeNoReturnNoArgs)
{
    SCOPED_TRACE("T01: End-to-End test: K2Node_CallFunction, no return, not args");

    std::string BlueprintCode
    {
        R"(
        Begin Object Class=/Script/BlueprintGraph.K2Node_CallFunction
        FunctionReference=(MemberParent="/Script/CoreUObject.Class'/Script/TraceUtilities.TraceUtilLibrary'",MemberName="DoSomething")
        End Object
        )"
    };

    const std::string ExpectedCppCode = "UTraceUtilLibrary::DoSomething();";

    CheckIfBpToCppCorrect(BlueprintCode, ExpectedCppCode);
}


TEST(FGraphBuilder, T02_E2E_OneCallFunctionNodeNoReturn_OneStringArg)
{
    SCOPED_TRACE("T02: End-to-End: K2Node_CallFunction, no return, one string arg");

    const std::string BlueprintCode{
        R"(
        Begin Object Class=/Script/BlueprintGraph.K2Node_CallFunction
           FunctionReference=(MemberParent="/Script/CoreUObject.Class'/Script/FurnishMaster.BPLib'",MemberName="LaunchProcess")
           CustomProperties Pin (PinId=CMD,PinName="Command",PinType.PinCategory="string",DefaultValue="cmd")
        End Object
        )"
    };

    const std::string ExpectedCppCode = "UBPLib::LaunchProcess(\"cmd\");";

    CheckIfBpToCppCorrect(BlueprintCode, ExpectedCppCode);
}


TEST(FGraphBuilder, T03_EtoE_OneCallFunctionNodeNoReturn_TwoStringArgs)
{
    SCOPED_TRACE("T03: End-to-End: K2Node_CallFunction, no return, two string args");

    const std::string BlueprintCode{
        R"(
        Begin Object Class=/Script/BlueprintGraph.K2Node_CallFunction
           FunctionReference=(MemberParent="/Script/CoreUObject.Class'/Script/FurnishMaster.BPLib'",MemberName="LaunchProcess")
           CustomProperties Pin (PinId=CMD,PinName="Command",PinType.PinCategory="string",DefaultValue="tool")
           CustomProperties Pin (PinId=PAR,PinName="Params", PinType.PinCategory="string",DefaultValue="--help")
        End Object
        )"
    };

    const std::string ExpectedCppCode = "UBPLib::LaunchProcess(\"tool\", \"--help\");";

    CheckIfBpToCppCorrect(BlueprintCode, ExpectedCppCode);
}

#if 0

TEST(FGraphBuilder, T04_E2E_PureReturnFeedsVoidCall_TwoUses)
{
    SCOPED_TRACE("T04: End-to-End: Pure function return feeds a two-arg void call");

    const std::string BlueprintCode{
        R"(
        Begin Object Class=/Script/BlueprintGraph.K2Node_CallFunction Name="ConvIntToString"
           bIsPureFunc=True
           FunctionReference=(MemberParent="/Script/CoreUObject.Class'/Script/Engine.KismetStringLibrary'",MemberName="Conv_IntToString")
           CustomProperties Pin (PinId=IN,PinName="InInt",PinType.PinCategory="int",DefaultValue="7")
           CustomProperties Pin (PinId=RV, PinName="ReturnValue", PinType.PinCategory="string")
        End Object
        Begin Object Class=/Script/BlueprintGraph.K2Node_CallFunction Name="LaunchProcess"
           FunctionReference=(MemberParent="/Script/CoreUObject.Class'/Script/FurnishMaster.BPLib'",MemberName="LaunchProcess")
           CustomProperties Pin (PinId=C, PinName="Command", PinType.PinCategory="string", LinkedTo=(ConvIntToString RV))
           CustomProperties Pin (PinId=P, PinName="Params",  PinType.PinCategory="string", LinkedTo=(ConvIntToString RV))
        End Object
        )"
    };

    const std::string ExpectedCppCode =
        "UBPLib::LaunchProcess(UKismetStringLibrary::Conv_IntToString(7), UKismetStringLibrary::Conv_IntToString(7));";

    CheckIfBpToCppCorrect(BlueprintCode, ExpectedCppCode);
}


#endif