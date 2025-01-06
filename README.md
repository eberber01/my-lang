# my-lang

This is my attempt at creating a C compiler. The motivation for this is to learn about low level program creation and execution. I plan to apply this knowledge in the future by writing code that is effcient at the assembly level.


## Progress
The current version of this compiler supports a single main function with integer declarations. Basic arithmitec needs to be worked on further, accounting for division operations. Current target code generation is for RISCV. 

In the future, I am planning to add support for key C features as well as additional CPU targets. Looking ahead, I also plan to write an assembler and linker.

## Todo

### Lex

- [x] int
- [x] identifier
- [x] unsigned int literal
- [ ] signed int literal
- [ ] string literal

### Parse

- [x] basic function 
- [ ] function args

### Symbol Table
- [ ] expand keyword init 

### ASM
- [ ] stack 


## Build

### Requirements
- cmake
```
    git clone https://github.com/eberber01/my-lang.git

    cd ./my-lang
    mkdir build

    cd build
    cmake .. & make

    ../my-lang path-to-file
```

Assembly file will be created at the root of the git directory. 
