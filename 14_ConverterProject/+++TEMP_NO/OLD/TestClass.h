#ifndef TESTCLASS_H
#define TESTCLASS_H

class TestClass
{
public:
    virtual ~TestClass() = default;
    virtual void RunTests() = 0;
};

#endif // TESTCLASS_H
