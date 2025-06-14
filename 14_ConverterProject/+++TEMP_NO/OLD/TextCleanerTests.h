#ifndef TEXTCLEANERTESTS_H
#define TEXTCLEANERTESTS_H

#include <cassert>
#include <cctype>
#include <string>
#include <functional>
////////
#include "TestClass.h"
#include "../TextCleaner.h"

class TextCleanerTests : public TestClass
{
public:
    void RunTests() override
    {
        const std::string Sample1 = "Hello world, this is a test.";
        const size_t Result1 = TextCleanerTEMP::FindStopPosition(Sample1, 6, [](char c)
        {
            return std::isspace(static_cast<unsigned char>(c)) || c == ',';
        });
        assert(Result1 == 11);

        const std::string Sample2 = "Test1234";
        const size_t Result2 = TextCleanerTEMP::FindStopPosition(Sample2, 4, [](char c)
        {
            return !std::isdigit(static_cast<unsigned char>(c));
        });
        assert(Result2 == 8);

        const std::string Sample3 = "Stop at space";
        const size_t Result3 = TextCleanerTEMP::FindStopPosition(Sample3, 0, [](char c)
        {
            return std::isspace(static_cast<unsigned char>(c));
        });
        assert(Result3 == 4);

        std::string Text1 = "Remove this, please";
        TextCleanerTEMP::RemoveSubstringFromMarker(Text1, 7, 4);
        assert(Text1 == "Remove , please");

        std::string Text2 = "abc def, ghi";
        TextCleanerTEMP::RemoveSubstringFromMarker(Text2, 4, 3);
        assert(Text2 == "abc ghi");

        std::string Text3 = "noStopHere";
        TextCleanerTEMP::RemoveSubstringFromMarker(Text3, 2, 2);
        assert(Text3 == "noHere");

        std::string Text4 = "   leading spaces";
        TextCleanerTEMP::RemoveSubstringFromMarker(Text4, 0, 3);
        assert(Text4 == "leading spaces");

        std::string Text5 = "trailing,";
        TextCleanerTEMP::RemoveSubstringFromMarker(Text5, 0, 4);
        assert(Text5 == ",");

        std::string Text6 = "comma, here";
        TextCleanerTEMP::RemoveSubstringFromMarker(Text6, 0, 6);
        assert(Text6 == ", here");
    }
};

#endif // TEXTCLEANERTESTS_H
