# small-compiler

small-compiler is a small compiler made in the C language, to compile basic C code. I will be committing more changes and features as I progress in my computer architecture knowledge and in assembly. Its purpose is personal; so I could learn about writing beautiful C code, managing my projects, and long term progression of feature implementations.

## Features

* A lexer to look at the source code and produce tokens (testing)
* Nothing more yet...

## Purpose

small-compiler documents my journey in systems programming, this includes my computer architecture and assembly knowledge, writing good-looking C code, and operating systems concepts.

## Quick Start

### Prerequisites

To make this and run the code, you need:
* a Linux x86-64 environment
* gcc/clang compiler for C
* **CMake** 3.10+

### Building the compiler

```bash
# Create a build directory
mkdir build

# If you want to hunt bugs:
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Or if you want the regular one:
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run
./build/small-compiler test.c
```

### Running an Example
Create a test file named file.c containing nothing more than what the features present in this compiler.

Run the compiler:
```bash
./build/small-program test.c > out.s
```

## Syntax specification

Currently, the project only has a lexer, more features will be added soon.

## 'To-do's

* Finish the lexer (testing).
* Add the parser.
* Generate the assembly.

The language semantics and symbol table will be added after these work nicely.

## LICENSE

MIT License.
