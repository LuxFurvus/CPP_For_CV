MODULE_NAME := SQLiteConnector

SRC_FILES := \
	SQLiteConnector/SQLiteConnector.cpp \
	SQLiteConnector/SQLite_DbConnectionTests.cpp \
	SQLiteConnector/SQLite_Statement_Tests.cpp \
	SQLiteConnector/SQLite_ConnectionSentinel_Tests.cpp \
	SQLiteConnector/SQLite_ParamVisitor_Tests.cpp \
	SQLiteConnector/SQLite_NamedParamBinder_Tests.cpp \
	

DEP_MODULES := ConditionChecker

EXTRA_LDFLAGS := -lsqlite3

IS_TEST_MODULE := true