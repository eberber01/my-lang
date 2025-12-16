cmake_minimum_required(VERSION 3.28)
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
add_compile_definitions($<$<BOOL:${RV32}>:RV32>)

add_test(NAME my-lang-tests COMMAND my-lang-tests)

find_program(CLANG_FORMAT_BIN clang-format)

find_program(RISCV_AS riscv32-unknown-linux-gnu-as)
find_program(RISCV_LD riscv32-unknown-linux-gnu-ld)

if (NOT RISCV_AS OR NOT RISCV_LD)
    message(FATAL_ERROR "Failed to find RISC-V ELF binutils")
endif()

message(STATUS "Using RISCV AS: ${RISCV_AS}")
message(STATUS "Using RISCV LD: ${RISCV_LD}")

add_custom_target(run
    COMMAND ${RISCV_AS} -o ${CMAKE_BINARY_DIR}/prog.o ${CMAKE_SOURCE_DIR}/asm.s
    COMMAND ${RISCV_LD} -Ttext=0x10000 -o ${CMAKE_BINARY_DIR}/prog ${CMAKE_BINARY_DIR}/prog.o
    COMMAND qemu-riscv32 ${CMAKE_BINARY_DIR}/prog
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)


file(GLOB COMPILE_TEST_SOURCES tests/compile/*.c)
set(OUT_DIR ${CMAKE_BINARY_DIR}/out)
file(MAKE_DIRECTORY ${OUT_DIR})

set(COMPILE_ELFS "")

# ----------------------
# Compile tests
# ----------------------

foreach(TEST_FILE ${COMPILE_TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    set(ASM_FILE ${OUT_DIR}/${TEST_NAME}.s)
    set(OBJ_FILE ${OUT_DIR}/${TEST_NAME}.o)
    set(ELF_FILE ${OUT_DIR}/${TEST_NAME}.elf)

    # 1. Compile → asm.s
    add_custom_command(
        OUTPUT ${ASM_FILE}
        COMMAND ${CMAKE_COMMAND} -E echo "Compiling ${TEST_FILE} → ${ASM_FILE}"
        COMMAND my-lang ${TEST_FILE}  # generates asm.s
        COMMAND ${CMAKE_COMMAND} -E copy asm.s ${ASM_FILE} # copy to OUT_DIR
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        DEPENDS my-lang ${TEST_FILE}
        COMMENT "Compiling ${TEST_FILE} → ${ASM_FILE}"
    )

    # 2. Assemble → .o
    add_custom_command(
        OUTPUT ${OBJ_FILE}
        COMMAND ${RISCV_AS} ${ASM_FILE} -o ${OBJ_FILE}
        DEPENDS ${ASM_FILE}
        COMMENT "Assembling ${OBJ_FILE}"
    )

    # 3. Link → .elf
    add_custom_command(
        OUTPUT ${ELF_FILE}
        COMMAND ${RISCV_LD} ${OBJ_FILE} -o ${ELF_FILE} -Ttext=0x10000
        DEPENDS ${OBJ_FILE}
        COMMENT "Linking ${ELF_FILE}"
    )

    # Add to list for the aggregate target
    list(APPEND COMPILE_ELFS ${ELF_FILE})
endforeach()

# Aggregate target
add_custom_target(build_compile_tests
    DEPENDS ${COMPILE_ELFS}
    COMMENT "Building all compile-only tests"
)

# ----------------------
# Run tests
# ----------------------
file(GLOB RUN_TESTS tests/runtime/*.c)

set(OUT_DIR ${CMAKE_BINARY_DIR}/out)
file(MAKE_DIRECTORY ${OUT_DIR})

set(RUN_TARGETS)

foreach(TEST_FILE ${RUN_TESTS})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    set(ASM_FILE ${OUT_DIR}/${TEST_NAME}.s)
    set(OBJ_FILE ${OUT_DIR}/${TEST_NAME}.o)
    set(ELF_FILE ${OUT_DIR}/${TEST_NAME}.elf)
    set(RUN_LOG ${OUT_DIR}/${TEST_NAME}.run.log)
    set(EXPECTED_FILE ${TEST_FILE}.expected)

    file(READ ${EXPECTED_FILE} EXPECTED)
    string(STRIP ${EXPECTED} EXPECTED)

    # 1. Compile → .s
    add_custom_command(
        OUTPUT ${ASM_FILE}
        COMMAND my-lang ${TEST_FILE}
        COMMAND ${CMAKE_COMMAND} -E copy asm.s ${ASM_FILE}
        DEPENDS my-lang ${TEST_FILE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Compiling ${TEST_FILE} → ${ASM_FILE}"
    )

    # 2. Assemble → .o
    add_custom_command(
        OUTPUT ${OBJ_FILE}
        COMMAND ${RISCV_AS} ${ASM_FILE} -o ${OBJ_FILE}
        DEPENDS ${ASM_FILE}
        COMMENT "Assembling ${TEST_NAME}.o"
    )

    # 3. Link → .elf
    add_custom_command(
        OUTPUT ${ELF_FILE}
        COMMAND ${RISCV_LD} ${OBJ_FILE} -o ${ELF_FILE} -Ttext=0x10000
        DEPENDS ${OBJ_FILE}
        COMMENT "Linking ${TEST_NAME}.elf"
    )

    add_custom_command(
        OUTPUT ${RUN_LOG}
        COMMAND ${CMAKE_SOURCE_DIR}/run_test.sh ${ELF_FILE} ${EXPECTED} ${RUN_LOG}
        DEPENDS ${ELF_FILE}
        COMMENT "Running ${TEST_NAME} → checking against ${EXPECTED}"
    )

    # Create a target to build + run this test
    add_custom_target(run_${TEST_NAME} DEPENDS ${RUN_LOG})

    # Prevent GNU Make from deleting run log files on failure
    set_property(
        SOURCE ${RUN_LOG}
        PROPERTY
            SYMBOLIC TRUE
    )

    list(APPEND RUN_TARGETS run_${TEST_NAME})
endforeach()

# Master target to run all tests
add_custom_target(runtime_tests DEPENDS ${RUN_TARGETS})





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
