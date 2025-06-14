#!/usr/bin/env bash

# Set common compiler flags and linker flags
CXX="ccache g++"
CXXFLAGS="-std=c++26 -fsanitize=undefined -fsanitize=address -Wall -pedantic -Wextra"
LDFLAGS="-lgtest -lgtest_main -pthread"

# Function to compile and run a single test suite
run_test_suite() {
    SOURCES=$1
    echo -e "\n\033[1;34mRunning tests for: ${SOURCES}\033[0m"
    time ${CXX} ${CXXFLAGS} ${SOURCES} ${LDFLAGS} -o test && ./test && rm test
}

clear

# Test suite 1
run_test_suite "TextCleanerNS.cpp Tests/TextCleanerNSTests.cpp"

# Test suite 2
run_test_suite "TextCleanerByMarker.cpp TextCleanerNS.cpp Tests/TextCleanerByMarkerTests.cpp"

# Test suite 3
run_test_suite "TextCleanerByEndings.cpp TextCleanerNS.cpp Tests/TextCleanerByEndingsTests.cpp"
