cmake_minimum_required(VERSION 3.28)

project(clox)

set(CMAKE_C_STANDARD 23)

set(SOURCES
    src/chunk.c
    src/compiler.c
    src/debug.c
    src/main.c
    src/memory.c
    src/object.c
    src/scanner.c
    src/table.c
    src/value.c
    src/vm.c
)

set(HEADERS
    src/chunk.h
    src/common.h
    src/compiler.h
    src/debug.h
    src/memory.h
    src/object.h
    src/scanner.h
    src/table.h
    src/value.h
    src/vm.h
)

add_executable(clox ${SOURCES} ${HEADERS})
target_compile_options(clox PUBLIC -Wall -Wextra -O2)
target_link_libraries(clox PUBLIC m)
