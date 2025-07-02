MODULE_NAME := SQL_Generals

SRC_FILES := \
	SQL_TableValues.cpp\
	SQL_StatementSeparator.cpp\
	SQL_ScriptExecutor.cpp\

TEST_FILES := \
	SQL_TableValues_Tests.cpp \
	SQL_StatementSeparator_Tests.cpp \

DEP_MODULES := ConditionChecker

EXTRA_LDFLAGS := 
