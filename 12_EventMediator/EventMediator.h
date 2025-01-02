#pragma once

#include <functional>
#include <memory>
#include <ostream>
#include <ranges>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

// Base interface providing a way to call function packs uniformly, regardless of template parameters
class IFunctionPack {
public:
    virtual ~IFunctionPack() = default;

    // Invokes the stored function pointer using the parameter block
    virtual void Invoke(void* Params) = 0;

    // Returns the object instance on which the function pointer is called
    virtual void* GetObjectInstance() const = 0;
};

// Template class that stores a pointer-to-member-function (PMF) and an object pointer
// Allows invocation by passing function parameters in a tuple

template <typename RetValType, typename UserClass, bool IsConst, typename... ParamTypes>
class TFunctionPack : public IFunctionPack {
private:
    // Defines the type of the function pointer (const or non-const based on IsConst)
    using FunctionPtrType = std::conditional_t<
        IsConst,
        RetValType(UserClass::*)(ParamTypes...) const,
        RetValType(UserClass::*)(ParamTypes...)>;

    friend class EventMediator;

    FunctionPtrType FunctionPointer = nullptr; // The stored member function pointer
    UserClass* ObjectInstance = nullptr; // Pointer to the object instance on which the function is called

    // Helper method to unpack parameters from the tuple and call the function
    template <std::size_t... Indexes>
    void CallFunctionWithUnpackedParams(std::tuple<ParamTypes...>& tupleParams,
                                        std::index_sequence<Indexes...>)
    {
        if (ObjectInstance == nullptr) return;
        (ObjectInstance->*FunctionPointer)(std::get<Indexes>(tupleParams)...);
    }

    // Calls the function when there are no parameters
    void CallFunctionWithoutParams() {
        if (ObjectInstance == nullptr) {
            return;
        }
        (ObjectInstance->*FunctionPointer)();
    }

public:
    // Constructor to initialize with an object instance and its member function pointer
    TFunctionPack(UserClass* InObjectInstance, FunctionPtrType InFunctionPointer)
        : FunctionPointer(InFunctionPointer)
        , ObjectInstance(InObjectInstance)
    {
    }

    // Invokes the stored member function using the provided params pointer
    void Invoke(void* Params) override {
        if constexpr (sizeof...(ParamTypes) > 0) {
            auto* UnpackedParams = static_cast<std::tuple<ParamTypes...>*>(Params);
            CallFunctionWithUnpackedParams(*UnpackedParams, std::index_sequence_for<ParamTypes...>{});
        }
        else {
            CallFunctionWithoutParams();
        }
    }

    // Returns the pointer to the object on which the function pointer is called
    void* GetObjectInstance() const override {
        return static_cast<void*>(ObjectInstance);
    }
};

// Manages events identified by strings and their associated listeners (callbacks)
// Provides mechanisms to add, invoke, and remove events or associated objects
class EventMediator {
private:
    static inline std::shared_ptr<EventMediator> Core = nullptr; // Singleton instance of EventMediator

    using FunctionPackArray = std::vector<std::shared_ptr<IFunctionPack>>; // Array of function packs for each event
    std::unordered_map<std::string, FunctionPackArray> FunctionLedger; // Map associating event IDs to function packs

private:
    EventMediator() = default; // Private constructor for singleton implementation

    // Retrieves the function pack array associated with an event ID
    FunctionPackArray& GetFunctionArray(const std::string& EventId) {
        return FunctionLedger[EventId];
    }

public:
    // Returns a shared instance (singleton) of the EventMediator
    static std::shared_ptr<EventMediator> Get() {
        if (!Core) {
            Core = std::shared_ptr<EventMediator>(new EventMediator());
        }
        return Core;
    }

    ~EventMediator() = default;

    // Adds a non-const member function (void or otherwise) to the mediator
    template <typename RetValType, typename UserClass, typename... ParamTypes>
    bool AddEvent(
        const std::string& EventId,
        UserClass* ObjectInstance,
        RetValType(UserClass::* InFunctionPointer)(ParamTypes...))
    {
        auto& TargetPack = GetFunctionArray(EventId);
        for (const auto& Pack : TargetPack) {
            auto TypedPack = std::dynamic_pointer_cast<TFunctionPack<RetValType, UserClass, false, ParamTypes...>>(Pack);
            if (TypedPack && TypedPack->GetObjectInstance() == ObjectInstance && TypedPack->FunctionPointer == InFunctionPointer) {
                return false;
            }
        }
        TargetPack.push_back(std::make_shared< TFunctionPack<RetValType, UserClass, false, ParamTypes...> >(
            ObjectInstance,
            InFunctionPointer
        ));
        return true;
    }

    // Adds a const member function to the mediator
    template <typename RetValType, typename UserClass, typename... ParamTypes>
    bool AddEvent(
        const std::string& EventId,
        UserClass* ObjectInstance,
        RetValType(UserClass::* InFunctionPointer)(ParamTypes...) const)
    {
        auto& TargetPack = GetFunctionArray(EventId);
        for (const auto& Pack : TargetPack) {
            auto TypedPack = std::dynamic_pointer_cast<TFunctionPack<RetValType, UserClass, true, ParamTypes...>>(Pack);
            if (TypedPack && TypedPack->GetObjectInstance() == ObjectInstance && TypedPack->FunctionPointer == InFunctionPointer) {
                return false;
            }
        }
        TargetPack.push_back(std::make_shared< TFunctionPack<RetValType, UserClass, true, ParamTypes...> >(
            ObjectInstance,
            InFunctionPointer
        ));
        return true;
    }

    // Invokes all callbacks bound to the specified event with the provided parameters
    template <typename... ParamTypes>
    void CallEvent(const std::string& EventId, ParamTypes... Params) {
        const auto It = FunctionLedger.find(EventId);
        if (It != FunctionLedger.end()) {
            const auto& CallingFunctionArray = It->second;
            for (const auto& OneFunctionPack : CallingFunctionArray) {
                std::tuple<ParamTypes...> TupleParams(Params...);
                OneFunctionPack->Invoke(&TupleParams);
            }
        }
    }

    // Removes all callbacks for the specified event
    void RemoveEvent(const std::string& EventId) {
        FunctionLedger.erase(EventId);
    }

    // Removes all callbacks associated with a particular object
    template <typename UserClass>
    void RemoveObject(UserClass* ObjectInstance) {
        for (auto& FunctionArray : FunctionLedger | std::views::values) {
            FunctionArray.erase(
                std::remove_if(
                    FunctionArray.begin(),
                    FunctionArray.end(),
                    [ObjectInstance](const std::shared_ptr<IFunctionPack>& Pack) {
                return Pack->GetObjectInstance() == static_cast<void*>(ObjectInstance);
            }
                ),
                FunctionArray.end()
            );
        }
    }

    // Removes all callbacks of a particular event that are associated with a specific object
    template <typename UserClass>
    void RemoveObjectFromEvent(const std::string& EventIdParam, UserClass* ObjectInstance) {
        const auto It = FunctionLedger.find(EventIdParam);
        if (It == FunctionLedger.end()) return;
        auto& FunctionArray = It->second;
        FunctionArray.erase(
            std::remove_if(
                FunctionArray.begin(),
                FunctionArray.end(),
                [ObjectInstance](const std::shared_ptr<IFunctionPack>& Pack) {
            return Pack->GetObjectInstance() == static_cast<void*>(ObjectInstance);
        }
            ),
            FunctionArray.end()
        );
    }
};
