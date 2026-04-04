#pragma once

#include <cstdint>
#include <optional>
#include <charconv>
#include <string_view>

struct FBlueprintGuid
{
private:

    const uint32_t A;
    const uint32_t B;
    const uint32_t C;
    const uint32_t D;

public:

    FBlueprintGuid() : A(0), B(0), C(0), D(0) {}

    FBlueprintGuid(uint32_t InA, uint32_t InB, uint32_t InC, uint32_t InD)
        : A(InA), B(InB), C(InC), D(InD)
    {}

    auto operator<=>(const FBlueprintGuid& Other) const = default;

    static std::optional<FBlueprintGuid> MakeGuidFromString(const std::string_view GuidString)
    {
        if (GuidString.size() != 32) return std::nullopt;

        uint32_t Parts[4] = {};

        for (int i = 0; i < 4; ++i)
        {
            const std::string_view Segment = GuidString.substr(i * 8, 8);
            auto Result = std::from_chars(Segment.data(), Segment.data() + Segment.size(), Parts[i], 16);
            if (Result.ec != std::errc{}) return std::nullopt;
        }

        return FBlueprintGuid(Parts[0], Parts[1], Parts[2], Parts[3]);
    }

};
