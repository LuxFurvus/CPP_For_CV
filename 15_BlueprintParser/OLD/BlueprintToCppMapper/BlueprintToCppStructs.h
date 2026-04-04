
#pragma once

#include <string>
#include <vector>
#include <optional>
//
#include "../FieldParser/BlueprintGuid.h"
#include "../CppFunctionBuilder/DataType.h"
#include "../CppFunctionBuilder/FunctionStructs.h"

enum struct ENodeType : uint8_t
{
    None,
    FunctionEntry,
    FunctionResult,
    CallFunction,
};

struct FFunctionReference
{
    const std::optional<std::string> MemberName{};
    const std::optional<std::string> MemberParent{};
};

struct FPinType
{
    const EDataType PinCategory;
};

struct FPin
{
    const FPinType PinType;
    const std::string PinName;
    const std::optional<std::string> DefaultValue;
    const FBlueprintGuid PinId; // Receiver
    const FBlueprintGuid LinkedTo; // Sender

    auto GetAsFunctionParam() const
    {
        return std::make_tuple(PinName, PinType.PinCategory, DefaultValue);
    }
};

struct FBlueprintNodeObject
{
    const ENodeType NodeType;
    const FFunctionReference FunctionReference;
    const std::vector<FPin> Pins;

public:

    std::vector<std::string> GetTypePinNames() const
    {
        std::vector<std::string> PinNames;
        for (const auto& Pin : Pins)
        {
            if (!Pin.PinType.PinCategory.IsValidType() || Pin.PinName.empty()) continue;
            PinNames.emplace_back(Pin.PinName);
        }
        return PinNames;
    }

    std::vector<EDataType> GetTypePinCategories() const
    {
        std::vector<EDataType> PinCategories;
        for (const auto& Pin : Pins)
        {
            if (!Pin.PinType.PinCategory.IsValidType()) continue;
            PinCategories.emplace_back(Pin.PinType.PinCategory);
        }
        return PinCategories;
    }

    std::optional<std::string> GetNodeName() const
    {
        return FunctionReference.MemberName;
    }

    std::vector<FFuncHeader::FFunctionParam> GetFunctionParameters() const
    {
        std::vector<FFuncHeader::FFunctionParam> Params;
        for (const auto& Pin : Pins)
        {
            const auto [PinName, PinType, DefaultValue] = Pin.GetAsFunctionParam();
            if (!PinType.IsValidType()) continue;

            Params.emplace_back(FFuncHeader::FFunctionParam{PinName, PinType, DefaultValue});
        }
        return Params;
    }
};
