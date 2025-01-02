#pragma once

#include <cassert>
#include <stacktrace>
#include <string>
#include <unordered_map>

// Class for storing property data related to specific objects
class PropertyPack {
private:
    void* PropertyOwner = nullptr;
    const void* PropertyItem = nullptr;
    bool bIsConstValue = true;

private:

    PropertyPack() = default;

    PropertyPack(void* Owner, const void* Property, bool IsConstProperty)
        : PropertyOwner(Owner),
        PropertyItem(Property),
        bIsConstValue(IsConstProperty) {
    }

    // Utility functions for hashing
    static size_t CombineHash(size_t Hash1, size_t Hash2) {
        return Hash1 ^ (Hash2 + 0x9e3779b9 + (Hash1 << 6) + (Hash1 >> 2));
    }

    // Hash function for PropertyPack
    friend size_t Hash(const PropertyPack& Pack) {
        const size_t OwnerHash = std::hash<void*>()(Pack.PropertyOwner);
        const size_t ItemHash = std::hash<const void*>()(Pack.PropertyItem);
        return CombineHash(OwnerHash, ItemHash);
    }

public:

    // Comparison operator for sorting
    bool operator==(const PropertyPack& Other) const {
        return PropertyOwner == Other.PropertyOwner
        && PropertyItem == Other.PropertyItem;
    }

    bool IsValid() const
    {
        return PropertyOwner != nullptr && PropertyItem != nullptr;
    }

    // Retrieves the constant property value
    template <typename Type>
    Type GetTypedValue() const {
        if (PropertyOwner == nullptr || PropertyItem == nullptr) {
            std::cerr << "\nError: Invalid property state in GetTypedValue.\n";
            std::cerr << "Callstack:\n" << std::stacktrace::current() << "\n\n";
            throw std::runtime_error("Invalid property state");
        }

        try {
            // Safely cast PropertyItem to the desired type pointer and dereference it
            return *static_cast<const Type*>(PropertyItem);
        }
        catch (const std::exception& e) {
            std::cerr << "\nError in GetTypedValue: " << e.what() << "\n";
            std::cerr << "Callstack:\n" << std::stacktrace::current() << "\n\n";
            throw std::runtime_error("Invalid property state");
        }
    }

    // Retrieves a non-constant property value
    void* GetPointerToMutableValue() const {
        if (PropertyOwner == nullptr || PropertyItem == nullptr || bIsConstValue) {
            std::cerr << "\nError: Invalid property state in GetPointerToMutableValue.\n";
            std::cerr << "Callstack:\n" << std::stacktrace::current() << "\n\n";
            throw std::runtime_error("Invalid property state");
        }

        try {
            return const_cast<void*>(PropertyItem);
        }
        catch (const std::exception& e) {
            std::cerr << "\nError in GetPointerToMutableValue: " << e.what() << "\n";
            std::cerr << "Callstack:\n" << std::stacktrace::current() << "\n\n";
            throw std::runtime_error("Invalid property state");
        }
    }

    // Grant access to private members to PropertyMediatorSubsystem
    friend class PropertyMediator;
};

// Class responsible for managing properties
class PropertyMediator {

private:

    static inline std::shared_ptr<PropertyMediator> Core = nullptr;

    // Now stores a vector of PropertyPack entries for each PropertyId
    std::unordered_map<std::string, std::vector<PropertyPack>> PropertyLedger;

private:

    PropertyMediator() = default;

public:

    // Returns a shared instance (singleton) of the PropertyMediator
    static std::shared_ptr<PropertyMediator> Get() {
        if (!Core) {
            Core = std::shared_ptr<PropertyMediator>(new PropertyMediator());
        }
        return Core;
    }

    ~PropertyMediator() = default;

public:

    // Adds a property to the ledger for a specific property ID.
    // Multiple entries can be stored under the same PropertyId.
    // Returns false if the same Owner and Property are already registered under the given PropertyId.
    bool AddProperty(const std::string& PropertyId, void* Owner, const void* Property, bool bIsConstValue) {
        if (Owner == nullptr || Property == nullptr) return false;

        // Retrieve the vector associated with the PropertyId
        std::vector<PropertyPack>& Packs = PropertyLedger[PropertyId];

        // Check if a PropertyPack with the same Owner and Property already exists
        for (const PropertyPack& Pack : Packs) {
            if (Pack.PropertyOwner != Owner || Pack.PropertyItem != Property) continue;
            return false; // Duplicate found, return false
        }

        // Add a new PropertyPack if no duplicate was found
        Packs.emplace_back(PropertyPack(Owner, Property, bIsConstValue));
        return true;
    }

    // Removes all registered properties of a specific object from the ledger.
    // Any PropertyPack associated with the provided PropertyOwner is erased.
    // If a vector becomes empty, its entry in the map is removed.
    void RemoveObject(const void* PropertyOwner) {
        if (PropertyOwner == nullptr) {
            return;
        }

        for (auto It = PropertyLedger.begin(); It != PropertyLedger.end(); ) {
            auto& Packs = It->second;

            // Erase any PropertyPack whose PropertyOwner matches the given pointer
            std::erase_if(
                Packs,
                [PropertyOwner](const PropertyPack& Pack) {
                    return Pack.PropertyOwner == PropertyOwner;
                }
            );

            // If the vector is now empty, remove the entry from the map
            if (Packs.empty()) {
                It = PropertyLedger.erase(It);
            }
            else {
                ++It;
            }
        }
    }

    // Retrieves all property packs associated with a specific property ID.
    // Returns an empty vector if the specified PropertyId is not found.
    std::vector<PropertyPack> GetProperty(const std::string& PropertyId) {
        assert(!PropertyId.empty());

        const auto It = PropertyLedger.find(PropertyId);
        if (It == PropertyLedger.end()) {
            return {};
        }

        // Return the entire vector of PropertyPack entries
        return It->second;
    }

};
