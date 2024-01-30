cmake_minimum_required(VERSION 3.0.0)
project(site_parser VERSION 0.1.0 LANGUAGES C CXX)

# Set the minimal C++ standard required (C++11 in this example)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
add_executable(site_parser main.cpp site_parser.cpp)

# Compiler options
target_compile_options(site_parser PRIVATE
    -fdiagnostics-color=always
    -g
    -Wall
)

# Linker options
# target_link_libraries(site_parser PRIVATE curl)

# Specify the output directory for the binary
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

# Enable testing
# include(CTest)
# enable_testing()

# Set project properties
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
