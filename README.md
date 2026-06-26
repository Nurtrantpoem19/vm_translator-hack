# Virtual Machine Translator (Nand2Tetris Project 7)

This project is a C++ implementation of Nand2Tetris Project 7: a virtual machine translator that converts VM bytecode into Hack assembly language to be run on the Hack hardware architecture.

## Overview

This is a basic VM Translator for Hack bytecode. This repository covers **Part 1** of the translator, which exclusively handles push/pop commands along with logical and arithmetic operations.

The program reads a single `.vm` file and outputs an assembly (`.asm`) file, which can then be processed by the Hack Assembler into executable `.hack` binary.

The architecture is divided into two main modules:

* **The Parser:** Responsible for reading and parsing the input.
* **The Code Writer:** Responsible for translating VM commands into Hack assembly.

> [!NOTE]
> There is no syntax validation or error correction support. The translator assumes it is receiving entirely valid `.vm` files.

---

## Features

This basic translator supports:

* **Memory Access:** `push` and `pop` commands.
* **Full RAM Segment Routing:** `static`, `local`, `argument`, `this`, `that`, `pointer`, `temp`, and `constant`.
* **Arithmetic Operations:** `add`, `sub`, `neg`
* **Logical Operations:** `eq`, `lt`, `gt`, `not`, `and`, `or`

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

The main binary accepts a single `.vm` file as an argument:

```bash
./Main foo.vm

```

*This will generate a corresponding assembly output file (e.g., `foo.asm`).*

### 3. Running Tests

This project includes unit tests built with the **GoogleTest (GTest)** framework. You can run them directly from the build directory using `ctest` or by executing the test binaries generated in the `test/` directory.

---

## Translation Example

### Input (`foo.vm`)

```vm
push constant 111
push constant 333

```

### Output Generated Assembly

```hack
@111
D=A
@SP
A=M
M=D
@SP
M=M+1
@333
D=A
@SP
A=M
M=D
@SP
M=M+1

```

---

## Retrospective & Limitations

* **Code Writer Density:** In hindsight, my code writer class is remarkably dense. I probably should have implemented a helper class to handle repetitive assembly generation tasks. This would have kept the main writer class from being so packed and significantly improved readability.
