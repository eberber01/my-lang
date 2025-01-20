# my-lang

This is my attempt at creating a C compiler. The motivation for this is to learn about low level program creation and execution. I plan to apply this knowledge in the future by writing code that is effcient at the assembly level.


## Progress

The project is still in very early stages. The current version of this compiler only supports the int type with basic arthimetic expressions. Function definitions with parameters are able 
to be parsed but assembly generation only supports function calls with zero parameters.

I am still new to C development so I have been researching different tools. I am trying out CMake/Gtest for building and testing. 

There is no strict roadmap, I just work on new features when I feel like it. However, I plan to only support int types until conditionals, loops, argument passing, are fully implemented. 

In the future, I am planning to add support for key C features as well as additional CPU targets. Looking ahead, I also plan to write an assembler and linker.

## Todo

 - [ ] Structs
 - [ ] Pointers
 - [ ] typedef
 - [ ] union
 - [ ] Function args
 - [x] Function call
 - [x] Function definition
 - [ ] if-else
 - [ ] while
 - [ ] for
 - [ ] Symbol Table
 - [x] Variable definition
 - [x] int
 - [ ] float
 - [ ] string literal
 - [ ] char
 - [ ] Logical operations

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

### Tests
```
    cd ./my-lang/build 
    ./my-lang-tests
```

## Example
### Test File
```
int one(){
    return 1;
}

int two(int o){
    return one() + one();
}

int main(){
    int num1 = 10;
    return two(1) + two(1) * 2;
}

```
### RISCV Assembly
```
.globl main

	j main
one:
	addi t0, zero, 1
	add a0, t0, zero
	jalr ra
two:
	addi sp, sp, -8
	sw ra, 8(sp)
	jal one
	addi sp, sp, 8
	lw ra, 0(sp)
	add t0, zero, a0
	addi sp, sp, -8
	sw ra, 8(sp)
	addi sp, sp, -8
	sw t0, 8(sp)
	jal one
	addi sp, sp, 8
	lw t0, 0(sp)
	addi sp, sp, 8
	lw ra, 0(sp)
	add t1, zero, a0
	add t0, t0, t1
	add a0, t0, zero
	jalr ra
main:
	addi t0, zero, 10
	sw t0, 0(sp)
	addi t0, zero, 1
	addi t1, zero, 1
	add t0, t0, t1
	addi t1, zero, 10
	sub t0, t0, t1
	addi sp, sp, -8
	sw ra, 8(sp)
	addi sp, sp, -8
	sw t0, 8(sp)
	jal two
	addi sp, sp, 8
	lw t0, 0(sp)
	addi sp, sp, 8
	lw ra, 0(sp)
	add t1, zero, a0
	addi sp, sp, -8
	sw ra, 8(sp)
	addi sp, sp, -8
	sw t0, 8(sp)
	addi sp, sp, -8
	sw t1, 16(sp)
	jal two
	addi sp, sp, 8
	lw t0, 8(sp)
	addi sp, sp, 8
	lw t1, 0(sp)
	addi sp, sp, 8
	lw ra, 0(sp)
	add t2, zero, a0
	addi t3, zero, 2
	mul t2, t2, t3
	add t1, t1, t2
	li  a7, 10
	ecall 
```
