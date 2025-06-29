#include <gtest/gtest.h>
////////
#include "BlueprintLogicExtractor.h"
//clear; g++ -std=c++26 -fsanitize=undefined -fsanitize=address -Wall -pedantic -Wextra *.cpp -lgtest -lgtest_main -pthread -o test && ./test; rm test

/////////////////////////////////////
TEST(BlueprintLogicExtractor, RemoveLinkedToHexStrings)
{
    ///////////////// Typical Cases //////////////////

    // Input contains a single LinkedTo with space and hex identifier – should simplify
    {
        std::string Text = R"(LinkedTo=(K2Node_FunctionEntry_0 ED3C542047CD705A64F152A3620835FD,))";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, R"(LinkedTo=(K2Node_FunctionEntry_0))");
    }

    // Multiple LinkedTo entries with valid content should all simplify
    {
        std::string Text = R"(a LinkedTo=(Node1 123ABC) b LinkedTo=(Node2 FFFF) c)";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, R"(a LinkedTo=(Node1) b LinkedTo=(Node2) c)");
    }

    // CustomProperties example – correctly simplifies a single LinkedTo section
    {
        std::string Text =
            R"(CustomProperties Pin (PinId=3E8AB7304D52A08911FB33B7E2AAAF01,LinkedTo=(K2Node_1 DEADBEEF,),bHidden=False,))";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text,
            R"(CustomProperties Pin (PinId=3E8AB7304D52A08911FB33B7E2AAAF01,LinkedTo=(K2Node_1),bHidden=False,))");
    }

    // Spaces and line breaks outside LinkedTo – they should be ignored
    {
        std::string Text = "Before\nLinkedTo=(Node 123)\nAfter";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, "Before\nLinkedTo=(Node)\nAfter");
    }

    // Valid node name with numbers should be preserved
    {
        std::string Text = "LinkedTo=(Node99 ABC123)";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, "LinkedTo=(Node99)");
    }

    ///////////////// Boundary Cases //////////////////

    // LinkedTo exists but with no space – should remain unchanged
    {
        std::string Text = "LinkedTo=(OnlyOneToken)";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, "LinkedTo=(OnlyOneToken)");
    }

    // Closing parenthesis missing – should abort safely
    {
        std::string Text = "LinkedTo=(BrokenEntry";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, "LinkedTo=(BrokenEntry");
    }

    // Opening marker present but malformed (wrong case or missing = sign) – should ignore
    {
        std::string Text = "linkedto=(Node 123)";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, "linkedto=(Node 123)");
    }

    // LinkedTo right at the end of string – still handled
    {
        std::string Text = "SomeData LinkedTo=(FinalNode ABCDEF)";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, "SomeData LinkedTo=(FinalNode)");
    }

    // Entry with multiple spaces before hex – only first word should be kept
    {
        std::string Text = "LinkedTo=(Node Name MoreHex)";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, "LinkedTo=(Node)");
    }

    ///////////////// Edge Cases //////////////////

    // Multiple adjacent LinkedTo entries, mixed with malformed ones
    {
        std::string Text = "LinkedTo=(ValidNode HEX1) LinkedTo=(Incomplete LinkedTo=(AnotherNode HEX2)";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, "LinkedTo=(ValidNode) LinkedTo=(Incomplete)");
    }

    // Empty input
    {
        std::string Text = "";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, "");
    }

    // Just the marker without parentheses
    {
        std::string Text = "LinkedTo=";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, "LinkedTo=");
    }

    // Nested parentheses should not interfere (but not supported explicitly)
    {
        std::string Text = "LinkedTo=(OuterNode (Inner) HEX123)";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, "LinkedTo=(OuterNode) HEX123)");
    }

    // Excessively long hex tail – only name should be retained
    {
        std::string Text = "LinkedTo=(SuperNode ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789)";
        BlueprintLogicExtractor::RemoveLinkedToHexStrings(Text);
        EXPECT_EQ(Text, "LinkedTo=(SuperNode)");
    }
}
