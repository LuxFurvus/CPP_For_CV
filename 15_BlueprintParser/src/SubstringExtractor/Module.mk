MODULE_NAME := SubstringExtractor

SRC_FILES :=\
    SubstringExtractor.cpp\

TEST_FILES :=\
    SubstringExtractorByRange.test\
    SubstringExtractorOneByRules.test\
    SubstringExtractorAllByRules.test\

DEP_MODULES :=\
    StringNormalizer\
    SubstringSearcher\

EXTRA_LDFLAGS :=\
