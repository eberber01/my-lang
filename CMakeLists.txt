cmake_minimum_required(VERSION 3.31)
project(my-lang)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall -Wextra -Werror")

include_directories(
    "${CMAKE_SOURCE_DIR}/src/"
    "${CMAKE_SOURCE_DIR}/build/"
)

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
    ../src/hashmap.c
    ../src/hashmap.h
    ../src/sema.c
    ../src/sema.h
    ../src/util.c
    ../src/util.h
)

add_library(my-lib  
    ../src/asm.c
    ../src/asm.h
    ../src/ast.c
    ../src/ast.h
    ../src/lex.c
    ../src/lex.h
    ../src/parse.c
    ../src/parse.h
    ../src/hashmap.c
    ../src/hashmap.h
    ../src/util.c
    ../src/util.h
)

find_package(GTest REQUIRED)
enable_testing()

add_executable(
    my-lang-tests
    ../src/test/hashmap.cpp
    ../src/test/vector.cpp
    ../src/test/string.cpp
    ../src/test/parse.cpp
    ../src/test/lex.cpp
)

target_link_libraries(
    my-lang-tests
    my-lib
    GTest::gtest_main
)

target_compile_definitions(my-lang PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

add_test(NAME my-lang-tests COMMAND my-lang-tests)

find_program(CLANG_FORMAT_BIN clang-format)

if(CLANG_FORMAT_BIN)
    file(GLOB_RECURSE ALL_SOURCE_FILES
        "${CMAKE_SOURCE_DIR}/src/*.c"
        "${CMAKE_SOURCE_DIR}/src/*.h"
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
    )

    add_custom_target(format
        COMMAND ${CLANG_FORMAT_BIN} -style=microsoft -i ${ALL_SOURCE_FILES}
        COMMENT "Formatting all source files with clang-format"
    )

    add_custom_target(check-format
        COMMAND clang-format -style=microsoft --dry-run --Werror ${ALL_SOURCE_FILES}
        COMMENT "Checking clang-format compliance"
    )


else()
    message(WARNING "clang-format not found. 'make format' target will be unavailable.")
endif()
