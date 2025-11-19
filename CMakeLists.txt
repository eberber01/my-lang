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

add_custom_target(run
    COMMAND riscv64-elf-as -o ${CMAKE_BINARY_DIR}/prog.o ${CMAKE_SOURCE_DIR}/asm.s
    COMMAND riscv64-elf-ld -Ttext=0x10000 -o ${CMAKE_BINARY_DIR}/prog ${CMAKE_BINARY_DIR}/prog.o
    COMMAND qemu-riscv64 ${CMAKE_BINARY_DIR}/prog
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

file(GLOB TEST_SOURCES tests/*.c)

set(OUT_DIR ${CMAKE_BINARY_DIR}/out)
file(MAKE_DIRECTORY ${OUT_DIR})

# Collect all build targets
set(ALL_COMPILE_TARGETS "")

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    set(ASM_FILE ${OUT_DIR}/${TEST_NAME}.s)
    set(OBJ_FILE ${OUT_DIR}/${TEST_NAME}.o)
    set(ELF_FILE ${OUT_DIR}/${TEST_NAME}.elf)

    #
    # 1. Compile → .s
    #
    add_custom_command(
        OUTPUT ${ASM_FILE}
        COMMAND my-lang ${TEST_FILE}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/asm.s ${ASM_FILE}
        DEPENDS my-lang ${TEST_FILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling ${TEST_FILE} → ${ASM_FILE}"
    )

    #
    # 2. Assemble → .o
    #
    add_custom_command(
        OUTPUT ${OBJ_FILE}
        COMMAND riscv64-elf-as ${ASM_FILE} -o ${OBJ_FILE}
        DEPENDS ${ASM_FILE}
        COMMENT "Assembling ${OBJ_FILE}"
    )

    #
    # 3. Link → .elf
    #
    add_custom_command(
        OUTPUT ${ELF_FILE}
        COMMAND riscv64-elf-ld ${OBJ_FILE} -o ${ELF_FILE} -Ttext=0x10000
        DEPENDS ${OBJ_FILE}
        COMMENT "Linking ${ELF_FILE}"
    )

    #
    # Build Only (no run)
    #
    add_custom_target(build_${TEST_NAME}
        DEPENDS ${ELF_FILE}
        COMMENT "Building test program ${TEST_NAME}"
    )

    # Collect for build_all_tests
    list(APPEND ALL_COMPILE_TARGETS build_${TEST_NAME})
endforeach()

# Target: compile all test programs (no run)
add_custom_target(build_all_tests
    DEPENDS ${ALL_COMPILE_TARGETS}
    COMMENT "Building ALL test programs (no QEMU run)"
)



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
