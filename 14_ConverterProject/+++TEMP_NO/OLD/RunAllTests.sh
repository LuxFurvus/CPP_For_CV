#!/usr/bin/env bash

# Set common compiler and linker flags
CXX="ccache g++"
CXXFLAGS="-std=c++26 -fsanitize=undefined -fsanitize=address -Wall -pedantic -Wextra"
LDFLAGS="-lgtest -lgtest_main -pthread"

# All test source files together
ALL_SOURCES="../TextCleaner/TextCleaner.cpp \
             ../TextCleaner/TextCleanerTests.cpp"

clear
echo -e "\n\033[1;34mBuilding and running ALL test suites together...\033[0m"
time ${CXX} ${CXXFLAGS} ${ALL_SOURCES} ${LDFLAGS} -o test && ./test; rm test
