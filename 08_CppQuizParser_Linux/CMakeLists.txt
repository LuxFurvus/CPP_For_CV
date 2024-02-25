cmake_minimum_required(VERSION 3.0.0)
project(site_parser VERSION 0.1.0 LANGUAGES C CXX)

# Set the minimal C++ standard required (upgraded to C++17 based on your JSON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
add_executable(site_parser main.cpp site_parser.cpp)

# Compiler options - Updated to match your JSON settings
target_compile_options(site_parser PRIVATE
    -fdiagnostics-color=always
    -g
    -O3
    -Wall
    -Wextra
    -Wpedantic
)

# Specify the output directory for the binary - This ensures the output path matches your JSON settings
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# Set project properties
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
