cmake_minimum_required(VERSION 3.22)
project(RandoGenTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(RandoGenTests
    ../SortedRollsMaker.cpp
    ../SuccessChanceCounter.cpp
    SortedRollsMaker.test.cpp
    SuccessChanceCounter.test.cpp
)

target_include_directories(RandoGenTests PRIVATE ..)

target_link_libraries(RandoGenTests
    GTest::gtest_main
)

enable_testing()