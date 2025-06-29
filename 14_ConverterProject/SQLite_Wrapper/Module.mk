MODULE_NAME := SQLite_Wrapper

SRC_FILES := \
	SQLite_Wrapper/SQLite_DbConnection.cpp \
	SQLite_Wrapper/SQLite_DbConnection_Tests.cpp \
	SQLite_Wrapper/SQLite_Statement.cpp \
	SQLite_Wrapper/SQLite_Statement_Tests.cpp \
	SQLite_Wrapper/SQLite_ConnectionSentinel_Tests.cpp \
	SQLite_Wrapper/SQLite_NamedParamBinder.cpp \
	SQLite_Wrapper/SQLite_NamedParamBinder_Tests.cpp \
	SQLite_Wrapper/SQLite_ParamVisitor_Tests.cpp \
	SQLite_Wrapper/SQLite_FieldReader.cpp \
	SQLite_Wrapper/SQLite_FieldReader_Tests.cpp \
	SQLite_Wrapper/SQLite_ConnectionPool.cpp \
	SQLite_Wrapper/SQLite_ConnectionPool_Tests.cpp \
	SQLite_Wrapper/SQLite_Executor.cpp \
	SQLite_Wrapper/SQLite_Executor_Tests.cpp \


DEP_MODULES := \
	ConditionChecker \
	SQL_Generals

EXTRA_LDFLAGS := -lsqlite3

IS_TEST_MODULE := true