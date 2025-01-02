
#include <iostream>
////////
#include "EventMediator.h"
#include "PropertyMediator.h"

class PropertyTester
{
private:
    int PrivateMutableInt = 100;
    const int PrivateConstInt = 5;
    static inline int Counter = 0;
public:
    std::string PublicString = "PublicString";

    PropertyTester()
    {
        ++Counter;
        PrivateMutableInt += Counter;
        PublicString.append(Counter, '*');

        bool IsPropertyAdded = false;
        IsPropertyAdded = PropertyMediator::Get()->AddProperty("PrivateMutableInt", this, &PrivateMutableInt, false);
        assert(IsPropertyAdded);
        IsPropertyAdded = PropertyMediator::Get()->AddProperty("PrivateConstInt", this, &PrivateConstInt, true);
        assert(IsPropertyAdded);
        IsPropertyAdded = PropertyMediator::Get()->AddProperty("PublicString", this, &PublicString, false);
        assert(IsPropertyAdded);

        //////////////

        IsPropertyAdded = PropertyMediator::Get()->AddProperty("PublicString", this, &PublicString, false);
        assert(IsPropertyAdded == false);
    }

    ~PropertyTester()
    {
        PropertyMediator::Get()->RemoveObject(this);
    }

    int GetPrivateMutableInt() const { return PrivateMutableInt; }
    int GetPrivateConstInt() const { return PrivateConstInt; }
    const char* GetPublicString() const { return PublicString.c_str(); }
};

/////////////

class FunctionTester {

private:

    void PrivateFunctionWithRef(const std::string& message) {
        printf("PrivateFunctionWithRef called with message: %s \n", message.c_str());
    }


    void PrivateFunctionNoRef(const std::string message) {
        printf("PrivateFunctionNoRef called with message: %s \n", message.c_str());
    }

public:
    // Constructor that binds the private function to the mediator
    FunctionTester()
    {
        EventMediator::Get()->AddEvent("PrivateFunctionWithRef", this, &FunctionTester::PrivateFunctionWithRef);
        EventMediator::Get()->AddEvent("PrivateFunctionNoRef", this, &FunctionTester::PrivateFunctionNoRef);
    }

    ~FunctionTester()
    {
        EventMediator::Get()->RemoveObject(this);
    }

    // Public method to demonstrate additional functionality
    void NonConstFunction(int value) {
        std::cout << "Non-const function called with value: " << value << "\n";
    }

    std::string ConstFunction() const {
        std::string Message("Hello from Const function!");
        std::cout << Message << "\n";
        return Message;
    }
};

void EventMediatorTestFunction()
{
    const std::shared_ptr<EventMediator> EventMed = EventMediator::Get();
    FunctionTester TestObject;
    FunctionTester TestObjectAnother;
    FunctionTester* HeapTestObject = new FunctionTester();

    /////////////

    bool bNewEventAdded = false;

    // Add a non-const member function to the event
    bNewEventAdded = EventMed->AddEvent("EventID1", &TestObject, &FunctionTester::NonConstFunction);
    assert(bNewEventAdded);

    bNewEventAdded = EventMed->AddEvent("EventID1", &TestObjectAnother, &FunctionTester::NonConstFunction);
    assert(bNewEventAdded);

    bNewEventAdded = EventMed->AddEvent("EventID1", HeapTestObject, &FunctionTester::NonConstFunction);
    assert(bNewEventAdded);

    bNewEventAdded = EventMed->AddEvent("EventID2", &TestObject, &FunctionTester::ConstFunction);
    assert(bNewEventAdded);

    bNewEventAdded = EventMed->AddEvent("EventID2", &TestObjectAnother, &FunctionTester::ConstFunction);
    assert(bNewEventAdded);

    bNewEventAdded = EventMed->AddEvent("EventID2", HeapTestObject, &FunctionTester::ConstFunction);
    assert(bNewEventAdded);

    delete HeapTestObject;
    /////////////

    // Call the non-const event
    EventMed->CallEvent("EventID1", 33);

    // Call the const event
    EventMed->CallEvent("EventID2");

    // Call the private event
    EventMed->CallEvent("PrivateFunctionNoRef", std::string("Hello from private function"));

    EventMed->CallEvent<const std::string&>("PrivateFunctionWithRef", std::string("Hello from private function"));

    printf("\n\n===============\n\n");
}

void PropertyMediatorTestFunction()
{
    const std::shared_ptr<PropertyMediator> PropMed = PropertyMediator::Get();

    /////////////////

    const PropertyTester Tester;
    PropertyTester* HeapTestObject = new PropertyTester();

    /////////////////

    const auto Pack = PropMed->GetProperty("PrivateMutableInt");

    for (const PropertyPack& PropertyPack : Pack)
    {
        auto* const PrivateMutableIntPtr = static_cast<int*>(PropertyPack.GetPointerToMutableValue());

        const auto PrivateMutableIntCopy = *PrivateMutableIntPtr;
        printf("PrivateMutableInt = %d\n", PrivateMutableIntCopy);

        *PrivateMutableIntPtr = *PrivateMutableIntPtr * 2;
        printf("PrivateMutableInt Updated = %d\n\n", Tester.GetPrivateMutableInt());
    }

    const auto StringPack = PropMed->GetProperty("PublicString");

    for (const PropertyPack& PropertyPack : StringPack)
    {
        std::string* PublicStringPtr = static_cast<std::string*>(PropertyPack.GetPointerToMutableValue());

        std::string PublicStringCopy = *PublicStringPtr;
        printf("PublicString = %s\n", PublicStringCopy.c_str());

        *PublicStringPtr = PublicStringPtr->append(*PublicStringPtr);
    }
    printf("PublicString in Tester Updated = %s\n", Tester.GetPublicString());
    printf("PublicString in HeapTestObject Updated = %s\n\n", HeapTestObject->GetPublicString());

    /////////////////

    delete HeapTestObject;

    /////////////////

    const auto PackConst = PropMed->GetProperty("PrivateConstInt");

    for (auto PropertyPack : PackConst)
    {
        auto PrivateConstIntCopy = PropertyPack.GetTypedValue<int>();
        printf("PrivateConstInt = %d\n", PrivateConstIntCopy);
        printf("PrivateConstInt = %d\n\n", Tester.GetPrivateConstInt());
    }

    /////////////////

    // Erroneous call
    try
    {
        (void)PackConst[0].GetPointerToMutableValue();
    }
    catch (...)
    {
        printf("\nError in PackConst.GetPointerToMutableValue();\n");
    }

    const auto InvalidPack = PropMed->GetProperty("InvalidValue");

    printf("Is InvalidPack valid = %s\n\n", !InvalidPack.empty() ? "Yes" : "No");

    printf("\n\n===============\n\n");
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

int main()
{
    PropertyMediatorTestFunction();

    EventMediatorTestFunction();
}