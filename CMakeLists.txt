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

# Release
add_executable(clox ${SOURCES} ${HEADERS})
target_compile_options(clox PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox PRIVATE NAN_BOXING)
target_link_libraries(clox PRIVATE m)

# Union Value
add_executable(clox__uv ${SOURCES} ${HEADERS})
target_compile_options(clox__uv PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox__uv PRIVATE)
target_link_libraries(clox__uv PRIVATE m)

# Debug Print Code
add_executable(clox__dpc ${SOURCES} ${HEADERS})
target_compile_options(clox__dpc PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox__dpc PRIVATE NAN_BOXING DEBUG_PRINT_CODE)
target_link_libraries(clox__dpc PRIVATE m)

# Debug Trace Execution
add_executable(clox__dte ${SOURCES} ${HEADERS})
target_compile_options(clox__dte PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox__dte PRIVATE NAN_BOXING DEBUG_TRACE_EXECUTION)
target_link_libraries(clox__dte PRIVATE m)

# Debug Stress GC
add_executable(clox__dsg ${SOURCES} ${HEADERS})
target_compile_options(clox__dsg PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox__dsg PRIVATE NAN_BOXING DEBUG_STRESS_GC)
target_link_libraries(clox__dsg PRIVATE m)

# Debug Log GC
add_executable(clox__dlg ${SOURCES} ${HEADERS})
target_compile_options(clox__dlg PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox__dlg PRIVATE NAN_BOXING DEBUG_LOG_GC)
target_link_libraries(clox__dlg PRIVATE m)

# Union Value & Debug Print Code
add_executable(clox__uv_dpc ${SOURCES} ${HEADERS})
target_compile_options(clox__uv_dpc PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox__uv_dpc PRIVATE DEBUG_PRINT_CODE)
target_link_libraries(clox__uv_dpc PRIVATE m)

# Union Value & Debug Trace Execution
add_executable(clox__uv_dte ${SOURCES} ${HEADERS})
target_compile_options(clox__uv_dte PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox__uv_dte PRIVATE DEBUG_TRACE_EXECUTION)
target_link_libraries(clox__uv_dte PRIVATE m)

# Union Value & Debug Stress GC
add_executable(clox__uv_dsg ${SOURCES} ${HEADERS})
target_compile_options(clox__uv_dsg PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox__uv_dsg PRIVATE DEBUG_STRESS_GC)
target_link_libraries(clox__uv_dsg PRIVATE m)

# Union Value & Debug Log GC
add_executable(clox__uv_dlg ${SOURCES} ${HEADERS})
target_compile_options(clox__uv_dlg PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox__uv_dlg PRIVATE DEBUG_LOG_GC)
target_link_libraries(clox__uv_dlg PRIVATE m)

# Debug
add_executable(clox__dbg ${SOURCES} ${HEADERS})
target_compile_options(clox__dbg PUBLIC -Wall -Wextra -O2)
target_compile_definitions(clox__dbg PRIVATE NAN_BOXING DEBUG_PRINT_CODE DEBUG_TRACE_EXECUTION DEBUG_STRESS_GC DEBUG_LOG_GC)
target_link_libraries(clox__dbg PUBLIC m)

# Debug GC
add_executable(clox__dbggc ${SOURCES} ${HEADERS})
target_compile_options(clox__dbggc PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox__dbggc PRIVATE NAN_BOXING DEBUG_STRESS_GC DEBUG_LOG_GC)
target_link_libraries(clox__dbggc PRIVATE m)

# Debug Code
add_executable(clox__dbgcode ${SOURCES} ${HEADERS})
target_compile_options(clox__dbgcode PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox__dbgcode PRIVATE NAN_BOXING DEBUG_PRINT_CODE DEBUG_TRACE_EXECUTION)
target_link_libraries(clox__dbgcode PRIVATE m)

# Union Value & Debug
add_executable(clox__uv_dbg ${SOURCES} ${HEADERS})
target_compile_options(clox__uv_dbg PUBLIC -Wall -Wextra -O2)
target_compile_definitions(clox__uv_dbg PRIVATE DEBUG_PRINT_CODE DEBUG_TRACE_EXECUTION DEBUG_STRESS_GC DEBUG_LOG_GC)
target_link_libraries(clox__uv_dbg PUBLIC m)

# Union Value & Debug GC
add_executable(clox__uv_dbggc ${SOURCES} ${HEADERS})
target_compile_options(clox__uv_dbggc PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox__uv_dbggc PRIVATE DEBUG_STRESS_GC DEBUG_LOG_GC)
target_link_libraries(clox__uv_dbggc PRIVATE m)

# Union Value & Debug Code
add_executable(clox__uv_dbgcode ${SOURCES} ${HEADERS})
target_compile_options(clox__uv_dbgcode PRIVATE -Wall -Wextra -O2)
target_compile_definitions(clox__uv_dbgcode PRIVATE DEBUG_PRINT_CODE DEBUG_TRACE_EXECUTION)
target_link_libraries(clox__uv_dbgcode PRIVATE m)
