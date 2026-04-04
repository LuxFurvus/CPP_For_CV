
#include "../BlueprintToCppMapper/BlueprintToCppMapper_FunctionHeader.h"
//
#include <gtest/gtest.h>

namespace
{
    auto CallGetFunctionHeader = [](const FSeparatedNodes& SeparatedNodes)
    {
        return BlueprintToCppMapper_FunctionHeader::GetFunctionHeader(SeparatedNodes);
    };
}


TEST(BlueprintToCppMapper_FunctionHeader, T01_HappyPath)
{
    SCOPED_TRACE("T01: Aggregates valid entry and result nodes into header");

    // entry node
    const FBlueprintNodeObject Entry {
        .NodeType = ENodeType::FunctionEntry,
        .FunctionReference = {.MemberName = std::make_optional<std::string>("DoWork")},
        .Pins = {
            {
                .PinType = {.PinCategory = EDataType::Int},
                .PinName = "Count",
                .DefaultValue = std::nullopt,
                .PinId = FBlueprintGuid{},
                .LinkedTo = FBlueprintGuid{}
            }
        }
    };

    // result node
    const FBlueprintNodeObject Result {
        .NodeType = ENodeType::FunctionResult,
        .FunctionReference = {},
        .Pins = {
            {
                .PinType = {.PinCategory = EDataType::Bool},
                .PinName = "bSuccess",
                .DefaultValue = std::nullopt,
                .PinId = FBlueprintGuid{},
                .LinkedTo = FBlueprintGuid{}
            }
        }
    };

    const FSeparatedNodes Nodes {
        .FunctionEntryNode = Entry,
        .FunctionResultNode = Result,
        .CallFunctionNodes = {}
    };

    const FFuncHeader Header = CallGetFunctionHeader(Nodes);

    const bool bFuncNameCorrect = Header.FuncName == "DoWork";
    EXPECT_TRUE(bFuncNameCorrect);

    const bool bHasParams = Header.Params.size() == 1 && Header.Params[0].ParamName == "Count";
    EXPECT_TRUE(bHasParams);

    const bool bHasReturn = Header.ReturnType.size() == 1 && Header.ReturnType[0] == EDataType::Bool;
    EXPECT_TRUE(bHasReturn);
}


TEST(BlueprintToCppMapper_FunctionHeader, T02_NoEntryNode)
{
    SCOPED_TRACE("T02: Produces only return types when entry is missing");

    const FBlueprintNodeObject Result {
        .NodeType = ENodeType::FunctionResult,
        .FunctionReference = {},
        .Pins = {
            {
                .PinType = {.PinCategory = EDataType::Float},
                .PinName = "OutValue",
                .DefaultValue = std::nullopt,
                .PinId = FBlueprintGuid{},
                .LinkedTo = FBlueprintGuid{}
            }
        }
    };

    const FSeparatedNodes Nodes {
        .FunctionEntryNode = std::nullopt,
        .FunctionResultNode = Result,
        .CallFunctionNodes = {}
    };

    const FFuncHeader Header = CallGetFunctionHeader(Nodes);

    const bool bFuncNameEmpty = !Header.FuncName.has_value() || Header.FuncName->empty();
    EXPECT_TRUE(bFuncNameEmpty);

    const bool bParamsEmpty = Header.Params.empty();
    EXPECT_TRUE(bParamsEmpty);

    const bool bReturnFloat = Header.ReturnType.size() == 1 && Header.ReturnType[0] == EDataType::Float;
    EXPECT_TRUE(bReturnFloat);
}

//.T03: Produces only parameters when result node is missing
TEST(BlueprintToCppMapper_FunctionHeader, T03_NoResultNode)
{
    SCOPED_TRACE("T03: Produces only parameters when result node is missing");

    const FBlueprintNodeObject Entry {
        .NodeType = ENodeType::FunctionEntry,
        .FunctionReference = {.MemberName = std::make_optional<std::string>("OnlyParams")},
        .Pins = {
            {
                .PinType = {.PinCategory = EDataType::String},
                .PinName = "Name",
                .DefaultValue = std::make_optional<std::string>("Default"),
                .PinId = FBlueprintGuid{},
                .LinkedTo = FBlueprintGuid{}
            }
        }
    };

    const FSeparatedNodes Nodes {
        .FunctionEntryNode = Entry,
        .FunctionResultNode = std::nullopt,
        .CallFunctionNodes = {}
    };

    const FFuncHeader Header = CallGetFunctionHeader(Nodes);

    const bool bFuncNameCorrect = Header.FuncName == "OnlyParams";
    EXPECT_TRUE(bFuncNameCorrect);

    const bool bHasOneParam = Header.Params.size() == 1 && Header.Params[0].ParamName == "Name";
    EXPECT_TRUE(bHasOneParam);

    const bool bReturnEmpty = Header.ReturnType.empty();
    EXPECT_TRUE(bReturnEmpty);
}

//.T04: Returns empty header when both nodes are missing
TEST(BlueprintToCppMapper_FunctionHeader, T04_NoEntryNoResult)
{
    SCOPED_TRACE("T04: Returns empty header when both nodes are missing");

    const FSeparatedNodes Nodes {
        .FunctionEntryNode = std::nullopt,
        .FunctionResultNode = std::nullopt,
        .CallFunctionNodes = {}
    };

    const FFuncHeader Header = CallGetFunctionHeader(Nodes);

    const bool bNoFuncName = !Header.FuncName.has_value() || Header.FuncName->empty();
    EXPECT_TRUE(bNoFuncName);

    const bool bParamsEmpty = Header.Params.empty();
    EXPECT_TRUE(bParamsEmpty);

    const bool bReturnEmpty = Header.ReturnType.empty();
    EXPECT_TRUE(bReturnEmpty);
}

//.T05: Filters out invalid parameter categories
TEST(BlueprintToCppMapper_FunctionHeader, T05_FiltersInvalidParams)
{
    SCOPED_TRACE("T05: Filters out invalid parameter categories");

    const FBlueprintNodeObject Entry {
        .NodeType = ENodeType::FunctionEntry,
        .FunctionReference = {.MemberName = std::make_optional<std::string>("FilterParams")},
        .Pins = {
            { {.PinCategory = EDataType::None}, "IgnoredNone", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} },
            { {.PinCategory = EDataType::Exec}, "IgnoredExec", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} },
            { {.PinCategory = EDataType::Int}, "Count", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} },
            { {.PinCategory = EDataType::Bool}, "bFlag", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} }
        }
    };

    const FSeparatedNodes Nodes {
        .FunctionEntryNode = Entry,
        .FunctionResultNode = std::nullopt,
        .CallFunctionNodes = {}
    };

    const FFuncHeader Header = CallGetFunctionHeader(Nodes);

    const bool bHasTwoParams = Header.Params.size() == 2;
    EXPECT_TRUE(bHasTwoParams);

    const bool bFirstIsCount = Header.Params[0].ParamName == "Count";
    EXPECT_TRUE(bFirstIsCount);

    const bool bSecondIsFlag = Header.Params[1].ParamName == "bFlag";
    EXPECT_TRUE(bSecondIsFlag);
}

//.T06: Filters out invalid return categories
TEST(BlueprintToCppMapper_FunctionHeader, T06_FiltersInvalidReturns)
{
    SCOPED_TRACE("T06: Filters out invalid return categories");

    const FBlueprintNodeObject Result {
        .NodeType = ENodeType::FunctionResult,
        .FunctionReference = {},
        .Pins = {
            { {.PinCategory = EDataType::Exec}, "IgnoredExec", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} },
            { {.PinCategory = EDataType::Float}, "ValidFloat", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} },
            { {.PinCategory = EDataType::MAX}, "IgnoredMax", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} }
        }
    };

    const FSeparatedNodes Nodes {
        .FunctionEntryNode = std::nullopt,
        .FunctionResultNode = Result,
        .CallFunctionNodes = {}
    };

    const FFuncHeader Header = CallGetFunctionHeader(Nodes);

    const bool bSingleReturnFloat = Header.ReturnType.size() == 1 && Header.ReturnType[0] == EDataType::Float;
    EXPECT_TRUE(bSingleReturnFloat);
}

//.T07: Handles present vs absent function names
TEST(BlueprintToCppMapper_FunctionHeader, T07_NameResolution)
{
    SCOPED_TRACE("T07: Handles present vs absent function names");

    { // with name
        const FBlueprintNodeObject Entry {
            .NodeType = ENodeType::FunctionEntry,
            .FunctionReference = {.MemberName = std::make_optional<std::string>("HasName")},
            .Pins = {}
        };
        const FSeparatedNodes Nodes {.FunctionEntryNode = Entry, .FunctionResultNode = std::nullopt, .CallFunctionNodes = {}};
        const FFuncHeader Header = CallGetFunctionHeader(Nodes);
        const bool bHasName = Header.FuncName == "HasName";
        EXPECT_TRUE(bHasName);
    }

    { // without name
        const FBlueprintNodeObject Entry {
            .NodeType = ENodeType::FunctionEntry,
            .FunctionReference = {.MemberName = std::nullopt},
            .Pins = {}
        };
        const FSeparatedNodes Nodes {.FunctionEntryNode = Entry, .FunctionResultNode = std::nullopt, .CallFunctionNodes = {}};
        const FFuncHeader Header = CallGetFunctionHeader(Nodes);
        const bool bEmptyName = !Header.FuncName.has_value();
        EXPECT_TRUE(bEmptyName);
    }
}

//.T08: Preserves parameter default values integrity
TEST(BlueprintToCppMapper_FunctionHeader, T08_ParamDefaultsIntegrity)
{
    SCOPED_TRACE("T08: Preserves parameter default values integrity");

    const FBlueprintNodeObject Entry {
        .NodeType = ENodeType::FunctionEntry,
        .FunctionReference = {.MemberName = std::make_optional<std::string>("Defaults")},
        .Pins = {
            { {.PinCategory = EDataType::Int}, "NoDefault", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} },
            { {.PinCategory = EDataType::String}, "WithDefault", std::make_optional<std::string>("Hello"), FBlueprintGuid{}, FBlueprintGuid{} },
            { {.PinCategory = EDataType::Bool}, "EmptyDefault", std::make_optional<std::string>(""), FBlueprintGuid{}, FBlueprintGuid{} }
        }
    };

    const FSeparatedNodes Nodes {.FunctionEntryNode = Entry, .FunctionResultNode = std::nullopt, .CallFunctionNodes = {}};

    const FFuncHeader Header = CallGetFunctionHeader(Nodes);

    const bool bCorrectParamCount = Header.Params.size() == 3;
    EXPECT_TRUE(bCorrectParamCount);

    const bool bSecondHasDefault = Header.Params[1].DefaultValue.has_value() && Header.Params[1].DefaultValue.value() == "Hello";
    EXPECT_TRUE(bSecondHasDefault);

    const bool bThirdHasEmptyDefault = Header.Params[2].DefaultValue.has_value();
    EXPECT_TRUE(bThirdHasEmptyDefault);
}

//.T09: Preserves order of params and returns
TEST(BlueprintToCppMapper_FunctionHeader, T09_OrderPreservation)
{
    SCOPED_TRACE("T09: Preserves order of params and returns");

    const FBlueprintNodeObject Entry {
        .NodeType = ENodeType::FunctionEntry,
        .FunctionReference = {.MemberName = std::make_optional<std::string>("Ordering")},
        .Pins = {
            { {.PinCategory = EDataType::Float}, "First", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} },
            { {.PinCategory = EDataType::Int}, "Second", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} }
        }
    };

    const FBlueprintNodeObject Result {
        .NodeType = ENodeType::FunctionResult,
        .FunctionReference = {},
        .Pins = {
            { {.PinCategory = EDataType::String}, "Ret1", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} },
            { {.PinCategory = EDataType::Bool}, "Ret2", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} }
        }
    };

    const FSeparatedNodes Nodes {.FunctionEntryNode = Entry, .FunctionResultNode = Result, .CallFunctionNodes = {}};

    const FFuncHeader Header = CallGetFunctionHeader(Nodes);

    const bool bParamOrder = Header.Params[0].ParamName == "First" && Header.Params[1].ParamName == "Second";
    EXPECT_TRUE(bParamOrder);

    const bool bReturnOrder = Header.ReturnType[0] == EDataType::String && Header.ReturnType[1] == EDataType::Bool;
    EXPECT_TRUE(bReturnOrder);
}

//.T10: Handles large heterogeneous signatures
TEST(BlueprintToCppMapper_FunctionHeader, T10_LargeHeterogeneous)
{
    SCOPED_TRACE("T10: Handles large heterogeneous signatures");

    std::vector<FPin> ManyPins;
    for (int i = 0; i < 10; ++i)
    {
        ManyPins.push_back({
            .PinType = {.PinCategory = EDataType::Int},
            .PinName = "Param" + std::to_string(i),
            .DefaultValue = std::nullopt,
            .PinId = FBlueprintGuid{},
            .LinkedTo = FBlueprintGuid{}
        });
    }
    ManyPins.push_back({ {.PinCategory = EDataType::None}, "Ignored", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} });

    const FBlueprintNodeObject Entry {
        .NodeType = ENodeType::FunctionEntry,
        .FunctionReference = {.MemberName = std::make_optional<std::string>("BigFunction")},
        .Pins = ManyPins
    };

    std::vector<FPin> ManyReturnPins;
    ManyReturnPins.push_back({ {.PinCategory = EDataType::Bool}, "R1", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} });
    ManyReturnPins.push_back({ {.PinCategory = EDataType::Exec}, "IgnoredExec", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} });
    ManyReturnPins.push_back({ {.PinCategory = EDataType::Float}, "R2", std::nullopt, FBlueprintGuid{}, FBlueprintGuid{} });

    const FBlueprintNodeObject Result {
        .NodeType = ENodeType::FunctionResult,
        .FunctionReference = {},
        .Pins = ManyReturnPins
    };

    const FSeparatedNodes Nodes {.FunctionEntryNode = Entry, .FunctionResultNode = Result, .CallFunctionNodes = {}};

    const FFuncHeader Header = CallGetFunctionHeader(Nodes);

    const bool bParamCountCorrect = Header.Params.size() == 10;
    EXPECT_TRUE(bParamCountCorrect);

    const bool bReturnCountCorrect = Header.ReturnType.size() == 2;
    EXPECT_TRUE(bReturnCountCorrect);
}
