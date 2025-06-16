MODULE_NAME := SQLiteConnector

SRC_FILES := \
	SQLiteConnector/SQLiteConnectorTests.cpp

DEP_MODULES :=

EXTRA_LDFLAGS := -lsqlite3

IS_TEST_MODULE := true