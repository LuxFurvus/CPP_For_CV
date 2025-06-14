#include <vector>
#include <memory>
#include <iostream>
////////
#include "TextCleanerTests.h"
//time ccache g++ -std=c++26 TestRunner.cpp "../TextCleaner.cpp" -o test && ./test; rm test

int main()
{
    std::vector<std::unique_ptr<TestClass>> tests;
    tests.emplace_back(std::make_unique<TextCleanerTests>());

    for (std::unique_ptr<TestClass>& test : tests)
    {
        try
        {
            test->RunTests();
            std::cout << "TestClass " << typeid(test).name() << " passed.\n";
        }
        catch (const std::exception& e)
        {
            std::cerr << "TestClass " << typeid(test).name() << " failed: " << e.what() << '\n';
            return 1;
        }
        catch (...)
        {
            std::cerr << "TestClass " << typeid(test).name() << " failed with unknown error.\n";
            return 1;
        }
    }

    std::cout << "All tests passed successfully.\n";
    return 0;
}
