cmake_minimum_required(VERSION 3.15)
project(small-compiler VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SOURCES
    codegen.c
    lexer.c
    lexer_table.c
    main.c
    parser.c
    semantics.c
    symbol.c
    types.c
)

add_executable(small-compiler ${SOURCES})

target_compile_options(small-compiler PRIVATE
    -g
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -O3
    -march=native
    -fno-omit-frame-pointer
    -flto
)

target_link_options(small-compiler PRIVATE
    -fsanitize=address
    -fsanitize=undefined
    -flto
)
