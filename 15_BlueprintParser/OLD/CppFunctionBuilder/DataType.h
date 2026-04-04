#pragma once

#include <array>
#include <string_view>

class EDataType
{
public:

    // Define all types in one place
    #define DATATYPE_LIST\
        X(None,  "auto")\
        X(Void,  "void")\
        X(Exec,  "EXEC_TYPE")\
        X(Float, "float")\
        X(Int,   "int")\
        X(Bool,  "bool")\
        X(Vector, "FVector")\
        X(Rotator, "FRotator")\
        X(Transform, "FTransform")\
        X(String, "FString")\
////////////////////////

    enum class Type
    {
    #define X(Name, Str) Name,
        DATATYPE_LIST
    #undef X
        MAX
    };

private:

    Type Value;

    static constexpr std::array<std::string_view, static_cast<std::size_t>(Type::MAX)> TypeNames {
    #define X(Name, Str) Str,
        DATATYPE_LIST
    #undef X
    };

public:

    constexpr explicit EDataType(const Type InType) : Value(InType) {}

    constexpr operator Type() const { return Value; }

    constexpr bool IsVoid() const
    {
        return Value == Type::Void;
    }

    constexpr bool IsValidType() const
    {
        const bool IsValid = true
            && Value != Type::None
            && Value != Type::Exec
            && Value != Type::MAX;
        return IsValid;
    }

    operator std::string_view() const
    {
        const std::size_t Index = static_cast<std::size_t>(Value);
        return (Index < TypeNames.size()) ? TypeNames[Index] : "auto";
    }

    // Named constants
    #define X(Name, Str) static const EDataType Name;
        DATATYPE_LIST
    #undef X

    static const EDataType MAX;
};
inline const EDataType EDataType::MAX { EDataType::Type::MAX };

// Definition of static constants
#define X(Name, Str) inline const EDataType EDataType::Name { EDataType::Type::Name };
    DATATYPE_LIST
#undef X
#undef DATATYPE_LIST
