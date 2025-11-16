# Self-Hosting C Compiler for RISC-V

A self-hosting C compiler targeting the **RISC-V instruction set architecture**. The compiler starts with a subset of C and gradually adds features, culminating in the ability to compile itself. 

## Features

* [x] Integer arithmetic, variables, functions, return statements
* [ ] Control flow (`if`, `else`, `while`, `for`)
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

* [ ] Verify correctness of current implementation (expressions, variables, scope, functions, int, return)
* [ ] Establish regression test suite
* [ ] Fix parsing/codegen bugs
* [ ] Implement variable assignment

### Control Flow

* [ ] Implement `if`, `else`, `while`, `for`
* [ ] Add logical and relational operators

### Function Calling Conventions

* [ ] Implement RISC-V calling convention (registers/stack)
* [ ] Support multiple parameters
* [ ] Handle simple recursion

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
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Run tests
ctest
```

## References & Inspiration

* [8cc](https://github.com/rui314/8cc)
* [chibicc](https://github.com/rui314/chibicc)
* [TinyCC](https://bellard.org/tcc/)

These compilers demonstrate minimal, self-hosting C compilers and were inspirations for this project.
