# Self-Hosting C Compiler for RISC-V

A self-hosting C compiler targeting the **RISC-V instruction set architecture**. The compiler starts with a subset of C and gradually adds features, culminating in the ability to compile itself. 

## Features

* [x] Integer arithmetic, variables, functions, return statements
* [x] Control flow (`if`, `else`, `while`, `for`)
* [x] Logical and relational operators (`==`, `<`, `>`, `&&`, `||`, `!`)
* [x] Function calling conventions (RISC-V ABI, multiple parameters, recursion)
* [ ] Expanded type system (`char`, `void`, pointers syntax)
* [ ] Pointer arithmetic (`&`, `*`)
* [ ] Arrays and string literals
* [ ] Global variables (initialized/uninitialized, cross-function access)
* [ ] Minimal preprocessor (`#define`, `#include`)
* [ ] Self-hosting compiler (compile compiler with itself)
* [ ] Optional optimizations (constant folding, dead code elimination, register allocation)

## Project Timeline

### Validation & Variables

* [x] Verify correctness of current implementation (expressions, variables, scope, functions, int, return)
* [x] Establish regression test suite
* [x] Fix parsing/codegen bugs
* [x] Implement variable assignment

### Control Flow

* [x] Implement `if`, `else`, `while`, `for`
* [x] Add logical and relational operators

### Function Calling Conventions

* [x] Implement RISC-V calling convention (registers/stack)
* [x] Support multiple parameters
* [x] Handle simple recursion

### Type System Expansion

* [ ] Add `char` and `void`
* [ ] Allow implicit conversions (`int` ↔ `char`)
* [ ] Parse pointer types

### Pointers & Memory

* [ ] Implement `&` (address-of) and `*` (dereference)
* [ ] Pointer arithmetic

### Arrays & Strings

* [ ] Implement arrays with indexing
* [ ] Support string literals (`char[]`)

### Global Variables

* [ ] Add support for global variables
* [ ] Handle initialized/uninitialized globals
* [ ] Enable cross-function access

###  Minimal Preprocessor

* [ ] Implement `#define` macros
* [ ] Support `#include`

### Self-Hosting & Bootstrapping

* [ ] Compile the compiler with itself
* [ ] Patch missing features (`switch`, bitwise ops)
* [ ] Add simple optimizations

### (Optional Enhancements)

* [ ] TBD
## Requirements

- CMake **3.31** or newer
- C compiler with C99 support (e.g. `gcc`, `clang`)
- C++ compiler (for unit tests)
- [GoogleTest](https://github.com/google/googletest) (found via `find_package(GTest REQUIRED)`)
- RISC-V 32-bit ELF toolchain:
  - `riscv32-unknown-linux-gnu-as`
  - `riscv32-unknown-linux-gnu-ld`
- QEMU user-mode RISC-V emulator:
  - `qemu-riscv32`
- (Optional) `clang-format` for `format` / `check-format` targets
- A POSIX-like shell environment (for `run_test.sh`)

## Docker
  Run project inside Docker image with all necessary requirements.
  ```bash
  make dev
  ```

## Build Instructions

```bash
# Clone the repository
git clone https://github.com/yourusername/my-lang.git
cd my-lang

# Build using Make
make release # or debug

make build

```
### Usage

```
./build/my-lang [file.c]
```

### Run Unit Tests
```bash
ctest

# or 

./build/my-lang-tests
```
### Compile Tests
```
cmake --build build --target build_compile_tests
```
###  Runtime Tests
```
cmake --build build --target runtime_tests
```


## References & Inspiration

* [8cc](https://github.com/rui314/8cc)
* [chibicc](https://github.com/rui314/chibicc)
* [TinyCC](https://bellard.org/tcc/)

These compilers demonstrate minimal, self-hosting C compilers and were inspirations for this project.
