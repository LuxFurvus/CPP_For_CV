MODULE_NAME := FieldParser

SRC_FILES :=\
    FieldParser.cpp\

TEST_FILES :=\
    BlueprintGuidTests.cpp\
    FieldParserTestsBool.cpp\
    FieldParserTestsGuid.cpp\
    FieldParserTestsNumeric.cpp\
    FieldParserTestsString.cpp\

DEP_MODULES :=\
    SubstringExtractor\

EXTRA_LDFLAGS :=\
