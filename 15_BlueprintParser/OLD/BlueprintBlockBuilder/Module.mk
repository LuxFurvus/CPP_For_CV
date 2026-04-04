MODULE_NAME := BlueprintBlockBuilder

SRC_FILES :=\
    BlueprintBlockBuilder.cpp\

TEST_FILES :=\
    GetNodeBlocks.test\
    GetCustomPropertiesBlocks.test\
    GetNodeHeaderBlocks.test\
    BuildNodeBlockObjects.test\

DEP_MODULES :=\
    SubstringExtractor\

EXTRA_LDFLAGS :=\
