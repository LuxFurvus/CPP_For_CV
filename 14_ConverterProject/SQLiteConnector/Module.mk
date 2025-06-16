MODULE_NAME := SQLiteConnector

SRC_FILES := \
	SQLiteConnector/SQLiteConnector.cpp \
	SQLiteConnector/SQLiteConnectorTests.cpp

DEP_MODULES :=

EXTRA_LDFLAGS := -lsqlite3

IS_TEST_MODULE := true