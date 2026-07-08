# Virtual Machine Translator (Nand2Tetris Project 8)

This project is a C++ implementation of Nand2Tetris Project 8, based on my previous implementation of Project 7. This version of the virtual machine translator converts VM bytecode into Hack assembly language to be run on the Hack hardware architecture, with added support for complete VM instruction set.

## Overview

This is the full VM Translator for Hack bytecode. This repository covers **Part 2** of the translator, which handles labels, functions, conditional gotos, functions calls and returns. Support for the previous iteration of the project is also included (arithmetic, push/pop, etc).

The program reads a directory, and translates all `.vm` files into a `.asm` Hack assembly file.

The architecture is divided into two main modules:

* **The Parser:** Responsible for reading and parsing the input.
* **The Code Writer:** Responsible for translating VM commands into Hack assembly.

> [!NOTE]
> There is still no syntax validation or error correction support because I'm too lazy to add that. The translator assumes it is receiving entirely valid `.vm` files.

---

## Features

This basic translator supports:

* **Memory Access:** `push` and `pop` commands.
* **Full RAM Segment Routing:** `static`, `local`, `argument`, `this`, `that`, `pointer`, `temp`, and `constant`.
* **Arithmetic Operations:** `add`, `sub`, `neg`
* **Logical Operations:** `eq`, `lt`, `gt`, `not`, `and`, `or`
* **Branching and Program Flow:**
* `label [LABEL]`: Defines jump markers within functions.
* `goto [LABEL]`: Unconditional branching to a specific label.
* `if-goto [LABEL]`: Conditional branching that pops the value at the top of the stack and jumps if is not equal to 0.
* **Functions and Subroutines:**
* `function [name] [arguments]`: Declares a function and initializes [arguments] (number of arguemnts) to 0.
* `call [name] [nArgs]`: Saves the current stack frame (return address, LCL, THIS, THAT, ARG) and jumps to called function.
* `return`: This just erases the entire callee's stack and restores the parent's stack, saving the returned value at the top of the newly restored stack.
---

## Project Structure

```text
.
├── CMakeLists.txt
├── include
│   └── translator
│       ├── Code.hpp
│       ├── Parser.hpp
│       └── Translator.hpp
├── src
│   ├── Code.cpp
│   ├── Parser.cpp
│   └── Translator.cpp
└── test
    ├── CMakeLists.txt
    ├── CodeTest.cpp
    └── ParserTest.cpp

5 directories, 10 files

```

---

## Build & Usage

### 1. Build the Project

If no `build` directory exists, create one and compile using CMake:

```bash
mkdir build
cd build
cmake ..
cmake --build .

```

### 2. Run the Translator

The `Main` binary accepts a directory as an input, as well as a `--no-init` flag. The `--no-init` flag is used if the directory has no `Sys.vm` file.

```bash
./Main directory/
OR
./Main ../directory --no-init

```

*This will generate a `directory.asm` file in `directory/`*

### 3. Running Tests

This project includes unit tests built with the **GoogleTest (GTest)** framework. They can be run from `build/`. If tests are generated from `build`, and it will create a testing directory called `tests/` AKA `build/tests`.

---

## Retrospective

* I had to refactor quite a few things in my Parser class and my CodeWriter class. When I needed to implement the writeFunction() method, the class also had to save that function name for later use when there were to be labels. Moreover, for the writeCall() method, I would also need to save the callee's name for future use. The Parser class didn't generate the correct name when there was too much white space, and couldn't properly parse indentation. Then it turns out std::istringstream can be used for tokenization, which I did. And then all the subsequent methods that called for a specific token needed to redone. The point is, my code was not modular enough, my Parser class was doing too much and needed simplification. These final methods were finally iplemented into the CodeWriter class.
