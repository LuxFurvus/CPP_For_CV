
The **`EventMediator`** and **`PropertyMediator`** classes both provide centralized, highly flexible mechanisms for managing communication and data sharing across various parts of an application. They eliminate point-to-point dependencies by decoupling the code that fires an event or requires a property’s value from the specific handlers or property owners. The following sections describe each class in detail, including their general structure, how they operate, and recommended usage scenarios.

---

## EventMediator

### Purpose and Overview

`EventMediator` offers a **publish-subscribe** or **observer** pattern implementation without coupling the publisher and subscribers directly. It maintains an internal mapping of **event identifiers** to **callbacks** and provides methods to register callbacks, invoke them, and remove them. By registering multiple callback functions under a single event identifier, any part of the system can invoke those callbacks without knowledge of the classes that implement them.

### Key Components

1. **Singleton Instance (`Get()`):**

    - The class enforces a singleton pattern with a static shared pointer (`Core`).
    - The `Get()` function guarantees there is only one mediator object.
    - This design ensures that any part of the program referencing `EventMediator::Get()` interacts with the same event system.
2. **Function Packs (`TFunctionPack` and `IFunctionPack`):**

    - `IFunctionPack` is a base interface that unifies how the mediator invokes a function.
    - `TFunctionPack` stores a pointer-to-member-function (PMF) and an instance pointer. It supports both const and non-const functions and manages invocation using compile-time template expansion.
    - When `EventMediator` invokes these function packs, it passes parameters as a `std::tuple`, which is unpacked at runtime to call the stored member function.
3. **Function Ledger (`FunctionLedger`):**

    - Internally, `EventMediator` uses an `std::unordered_map<std::string, FunctionPackArray>` to associate an event identifier with an array of function packs.
    - Each entry in the ledger corresponds to a single event name/key, and its associated vector stores all callbacks (function packs) that should be notified when this event is triggered.

### Core Methods

1. **AddEvent(...):**

    - Registers a member function pointer for a given event identifier.
    - Supports both const and non-const member functions.
    - Prevents duplicates by checking if the same object-pointer and function-pointer tuple already exists.
2. **CallEvent(...):**

    - Invokes all registered callbacks for the event ID, passing in any parameters as a parameter pack.
    - Internally converts the parameter pack to a `std::tuple`, which each function pack unpacks for invocation.
3. **RemoveEvent(...):**

    - Removes all callbacks associated with a specific event ID.
4. **RemoveObject(...):**

    - Removes all callbacks associated with a particular object instance from every event in the mediator.
5. **RemoveObjectFromEvent(...):**

    - Removes all callbacks for a specific event that are associated with a particular object instance.

### Usage Recommendations

- **Global or Cross-Cutting Events:**
    Use `EventMediator` to handle events that may originate anywhere in the system but need to be processed by multiple disparate modules. For instance, UI updates, game engine events, or general messaging across major subsystems can be routed through an `EventMediator`.
- **Reduced Coupling:**
    When multiple consumers need to listen for the same triggers without introducing direct dependencies among these modules, `EventMediator` is beneficial.
- **Lifetime Management:**
    Be mindful of the object lifetimes. When objects subscribe to events, ensure that these objects are properly unsubscribed (e.g., `RemoveObject`) before destruction to avoid dangling pointers.

---

## PropertyMediator

### Purpose and Overview

`PropertyMediator` streamlines the registration and retrieval of **properties** (generic data values) while preserving independence among different parts of the application. It is particularly useful for sharing arbitrary data references between systems that otherwise do not need direct references to each other. Multiple entries can be registered under a single property identifier, thereby allowing different owners to store similarly categorized data in a central location.

### Key Components

1. **Singleton Instance (`Get()`):**

    - Similar to `EventMediator`, `PropertyMediator` uses a singleton pattern with a static shared pointer (`Core`).
    - Any code that retrieves `PropertyMediator::Get()` interacts with the same centralized property ledger.
2. **Property Packs (`PropertyPack`):**

    - Encapsulates basic information about a property, including:
        - **`PropertyOwner`:** Points to the object that owns the property.
        - **`PropertyItem`:** Points to the actual data.
        - **`bIsConstValue`:** Indicates whether this property is stored as a const pointer, restricting mutable access.
    - Allows safely retrieving typed values or mutable pointers, with checks against invalid usage.
3. **Property Ledger (`PropertyLedger`):**

    - The internal data structure uses an `std::unordered_map<std::string, std::vector<PropertyPack>>`.
    - Each key in the map corresponds to a property identifier, storing a list of `PropertyPack` entries that all share that identifier but might come from various owners.

### Core Methods

1. **AddProperty(...):**

    - Associates a property with an identifier (`PropertyId`).
    - Avoids duplicates by checking if the exact `(Owner, PropertyItem)` pair is already registered under the same `PropertyId`.
    - Stores whether the property is const or non-const, influencing how it can be retrieved later.
2. **RemoveObject(...):**

    - Removes all properties owned by a specific pointer (`PropertyOwner`).
    - Cleans up empty vectors from the property ledger to avoid accumulating unused entries.
3. **GetProperty(...):**

    - Retrieves all `PropertyPack` instances associated with a particular `PropertyId`.
    - Allows multiple owners for the same identifier, so this returns a vector of all matching properties.

### Usage Recommendations

- **Data Sharing Across Modules:**
    Use `PropertyMediator` where multiple systems need to register or look up values under a common category (e.g., to share configuration data, game states, or any structured data that should not be tightly coupled).
- **Loose Coupling in Large Systems:**
    If certain parts of an application must query properties that belong to various unrelated objects, `PropertyMediator` helps avoid direct references and keeps property data discoverable through a common identification scheme.
- **Read-Only vs. Mutable Access:**
    Always decide carefully whether the property should be registered as const (`bIsConstValue = true`) or non-const (`bIsConstValue = false`). This ensures the intended usage—read-only or modifiable—remains explicit and reduces the risk of unintended state changes.

---

## Summary

Together, `EventMediator` and `PropertyMediator` provide a flexible infrastructure for cross-module interactions. They both use singleton instances to ensure a single point of reference and rely on registering identifiers (event IDs or property IDs) to map to dynamically allocated callback functions or property data. This design significantly **lowers coupling** across the codebase:

- **`EventMediator`** uses a loosely coupled **events** system, allowing any part of the program to broadcast events to all interested listeners without direct dependencies.
- **`PropertyMediator`** uses an identifier-based **property** registration and retrieval mechanism, which helps separate data owners from data consumers.

Both systems should be **used sparingly**, primarily for cross-cutting concerns or where natural direct interactions would create cyclic dependencies or unmanageable coupling. By centralizing event dispatching and property lookups, they serve as powerful tools in large, modular applications where maintainability and clarity are paramount.

---

# Addendum: Magic numbers in CombineHash()

The specific constants in this hash-combining function come from established bit-manipulation and mathematical techniques designed to achieve good distribution and reduce collisions when merging two hash values:

1. **`0x9e3779b9`**
    This hexadecimal value (which is `2654435769` in decimal) is closely related to the fractional part of the golden ratio in the 32-bit integer space. Multiplying the golden ratio (~1.618…) by 2322^{32}232 produces a constant near this integer. Using this value in hashing helps distribute bits more uniformly and reduce collisions. It is often referred to as the **golden ratio prime** in hashing contexts.

2. **Shifts by 6 (left shift) and 2 (right shift)**
    These bitwise shifts further mix the bits of the already-existing hash (`Hash1`) before combining it with the new one (`Hash2`). Shifting by 6 (i.e., `(Hash1 << 6)`) and by 2 in the opposite direction (`(Hash1 >> 2)`) ensures that bits from different positions of the original hash get “scrambled.” This scrambling helps avoid patterns that could diminish hash quality.

3. **Final XOR operation**
    The function combines all these transformed pieces—`Hash1`, `Hash2`, and the golden-ratio-based constant—using XOR (`^`). XOR is a common choice in hash-combining functions because of its property of toggling bits and preserving variability, thereby contributing to a better distribution of final hash values.

Overall, these “magic numbers” and shifts come from well-known heuristics in hashing. They aim to produce a final hash value that maximizes randomness and minimizes collisions when combining two hash values.
