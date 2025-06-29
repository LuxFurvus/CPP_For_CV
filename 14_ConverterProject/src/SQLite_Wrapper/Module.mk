MODULE_NAME := SQLite_Wrapper

SRC_FILES := \
	SQLite_DbConnection.cpp \
	SQLite_DbConnection_Tests.cpp \
	SQLite_Statement.cpp \
	SQLite_Statement_Tests.cpp \
	SQLite_ConnectionSentinel_Tests.cpp \
	SQLite_NamedParamBinder.cpp \
	SQLite_NamedParamBinder_Tests.cpp \
	SQLite_ParamVisitor_Tests.cpp \
	SQLite_FieldReader.cpp \
	SQLite_FieldReader_Tests.cpp \
	SQLite_ConnectionPool.cpp \
	SQLite_ConnectionPool_Tests.cpp \
	SQLite_Executor.cpp \
	SQLite_Executor_Tests.cpp \


DEP_MODULES := \
	ConditionChecker \
	SQL_Generals

EXTRA_LDFLAGS := -lsqlite3

IS_TEST_MODULE := true