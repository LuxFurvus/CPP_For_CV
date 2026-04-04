cmake_minimum_required(VERSION 3.20)

# === Project configuration ===
project(FireWorksApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# === Compiler flags

# --- ADAPTOR_FLAGS ---
add_compile_options(
    -g
    -O0
    -fno-omit-frame-pointer
    -fPIE
)

add_link_options(
    -pie
)

# --- COMPILE_LIMITS ---
add_compile_options(
    -fmax-errors=5
    -fno-rtti
    -fno-exceptions
)

# --- WARNINGS_CLASSIC ---
add_compile_options(
    -Wall
    -Wextra
    -pedantic
)

# --- WARNINGS_SANITIZER ---
add_compile_options(
    -fsanitize=address
    -fsanitize=undefined
    -fsanitize=leak
    -fsanitize=bounds,alignment,vptr,object-size,pointer-compare,pointer-subtract
)

add_link_options(
    -fsanitize=address
    -fsanitize=undefined
    -fsanitize=leak
    -fsanitize=bounds,alignment,vptr,object-size,pointer-compare,pointer-subtract
)

# === Source collection

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

# === Unity Build

set(CMAKE_UNITY_BUILD ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 16)

# === Executable

add_executable(FireWorksApp ${APP_SOURCES})

# === SFML

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)

target_link_libraries(FireWorksApp
    sfml-graphics
    sfml-window
    sfml-system
)
