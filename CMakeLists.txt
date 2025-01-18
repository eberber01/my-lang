cmake_minimum_required(VERSION 3.31)
project(my-lang)

set(CMAKE_C_STANDARD 99)

include_directories(
        "${CMAKE_SOURCE_DIR}/src/"
        "${CMAKE_SOURCE_DIR}/build/")

add_executable(my-lang 
    ../src/asm.c
    ../src/asm.h
    ../src/ast.c
    ../src/ast.h
    ../src/lex.c
    ../src/lex.h
    ../src/main.c
    ../src/parse.c
    ../src/parse.h
    ../src/symtab.c
    ../src/symtab.h
    ../src/util.c
    ../src/util.h
)

find_package(GTest REQUIRED)

enable_testing()

add_executable(
    my-lang-tests
  ../src/test/symtab.cpp
)

target_link_libraries(
  my-lang-tests
  GTest::gtest_main
)
