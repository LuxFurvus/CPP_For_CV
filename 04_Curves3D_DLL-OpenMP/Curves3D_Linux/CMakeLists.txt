cmake_minimum_required(VERSION 3.12)

project(Curves3D)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(CurvesLib SHARED CurvesClasses.cpp)
target_include_directories(CurvesLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(CurvesLib PROPERTIES OUTPUT_NAME "CurvesLib")
set_target_properties(CurvesLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")  # Use forward slash (/) for path separation

add_executable(Curves3D HandleFunctions.cpp Main.cpp)
target_link_libraries(Curves3D PRIVATE CurvesLib)
target_include_directories(Curves3D PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(Curves3D PROPERTIES OUTPUT_NAME "Curves3D")
set_target_properties(Curves3D PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")  # Use forward slash (/) for path separation
