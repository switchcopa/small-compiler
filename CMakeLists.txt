cmake_minimum_required(VERSION 3.15)
project(small-compiler VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/lexer.c
    src/main.c
    src/types.c
)

add_executable(small-compiler ${SOURCES})

target_include_directories(small-compiler PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Strict warnings for clean code
target_compile_options(small-compiler PRIVATE
    -Wall -Wextra -Wpedantic -Werror
)

# Debug Config: Sanitizers enabled for bug hunting
target_compile_options(small-compiler PRIVATE 
    $<$<CONFIG:Debug>:-g;-O0;-fno-omit-frame-pointer;-fsanitize=address;-fsanitize=undefined>
)
target_link_options(small-compiler PRIVATE 
    $<$<CONFIG:Debug>:-fsanitize=address;-fsanitize=undefined>
)

# Release Config: Max optimizations and Link Time Optimization (LTO)
target_compile_options(small-compiler PRIVATE 
    $<$<CONFIG:Release>:-O3;-march=native;-flto>
)
target_link_options(small-compiler PRIVATE 
    $<$<CONFIG:Release>:-flto>
)
