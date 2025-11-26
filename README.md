# Self-Hosting C Compiler for RISC-V

A self-hosting C compiler targeting the **RISC-V instruction set architecture**. The compiler starts with a subset of C and gradually adds features, culminating in the ability to compile itself. 

## Features

* [x] Integer arithmetic, variables, functions, return statements
* [x] Control flow (`if`, `else`, `while`, `for`)
* [ ] Logical and relational operators (`==`, `<`, `>`, `&&`, `||`, `!`)
* [ ] Function calling conventions (RISC-V ABI, multiple parameters, recursion)
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
* [ ] Add logical and relational operators

### Function Calling Conventions

* [ ] Implement RISC-V calling convention (registers/stack)
* [ ] Support multiple parameters
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

## Build Instructions

```bash
# Clone the repository
git clone https://github.com/yourusername/my-lang.git
cd my-lang

# Build using CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release # or Debug

cmake --build build

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
### Run Compile Tests
```
cmake --build build --target build_compile_tests
```
###  Qemu Tests
```
cmake --build build --target run_all_tests
```


## References & Inspiration

* [8cc](https://github.com/rui314/8cc)
* [chibicc](https://github.com/rui314/chibicc)
* [TinyCC](https://bellard.org/tcc/)

These compilers demonstrate minimal, self-hosting C compilers and were inspirations for this project.
