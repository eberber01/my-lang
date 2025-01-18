# my-lang

This is my attempt at creating a C compiler. The motivation for this is to learn about low level program creation and execution. I plan to apply this knowledge in the future by writing code that is effcient at the assembly level.


## Progress

The project is still in very early stages. The current version of this compiler only supports the int type with basic arthimetic expressions. Function definitions with parameters are able 
to be parsed but assembly generation only supports function calls with zero parameters.

I am still new to C development so I have been researching different tools. I am trying out CMake/Gtest for building and testing. 

There is no strict roadmap, I just work on new features when I feel like it. However, I plan to only support int types until conditionals, loops, argument passing, are fully implemented. 

In the future, I am planning to add support for key C features as well as additional CPU targets. Looking ahead, I also plan to write an assembler and linker.

## Todo

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
