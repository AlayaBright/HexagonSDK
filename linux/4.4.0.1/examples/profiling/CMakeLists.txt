cmake_minimum_required(VERSION 3.14.3)

project(profiling C CXX ASM)

enable_language(ASM)

set(CMAKE_DEBUG_TARGET_PROPERTIES
        INCLUDE_DIRECTORIES
        COMPILE_DEFINITIONS
        POSITION_INDEPENDENT_CODE
        CONTAINER_SIZE_REQUIRED
        LIB_VERSION
    )

include(${HEXAGON_CMAKE_ROOT}/hexagon_fun.cmake)

# Base Include dirs for the Project
set(common_incs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_BINARY_DIR}/
    ${HEXAGON_SDK_ROOT}/incs/
    ${HEXAGON_SDK_ROOT}/incs/stddef/
    ${HEXAGON_SDK_ROOT}/rtos/qurt/
    ${HEXAGON_SDK_ROOT}/libs/qprintf/inc/
    ${HEXAGON_SDK_ROOT}/utils/examples/
    ${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem/inc/
    )

include_directories(${common_incs})

if(${OS_TYPE} MATCHES "HLOS")

    ## HLOS Dynamic Stub Library
    add_library(profiling SHARED ${CMAKE_CURRENT_BINARY_DIR}/profiling_stub)

    # Build interface files using user IDL
    build_idl(inc/profiling.idl profiling)

    # Find the DSP from provided domain value, default is CDSP
    choose_dsprpc(${DSP_TYPE} dsprpc)

    ## HLOS Executable
    add_executable(profiling_device
        ${CMAKE_CURRENT_SOURCE_DIR}/src/profiling_main
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rpcperf
        ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
        ${HEXAGON_SDK_ROOT}/utils/examples/time_utils
    )

    if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set_target_properties(profiling_device PROPERTIES OUTPUT_NAME "profiling")
    endif()

    # Add common HLOS-specific linker options
    link_options(profiling_device)

    link_custom_library(profiling ${dsprpc})
    link_custom_library(profiling_device ${dsprpc})
    # Link target with custom libraries
    link_custom_library(profiling_device rpcmem)
    link_custom_library(profiling_device getopt_custom)

    add_dependencies(profiling_device profiling)
    target_link_libraries(profiling_device profiling)

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(profiling_device)

else()

    set(skel_srcs ${CMAKE_CURRENT_BINARY_DIR}/profiling_skel
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/profiling_imp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/rpcperf_imp
                    ${CMAKE_CURRENT_SOURCE_DIR}/asm_src/profiling_assembly.S
        )

    # creating shared skel library
    add_library(profiling_skel SHARED ${skel_srcs})

    build_idl(inc/profiling.idl profiling_skel)

    set(q_exec_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/profiling_main
                ${CMAKE_CURRENT_SOURCE_DIR}/src/profiling_imp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/rpcperf
                ${CMAKE_CURRENT_SOURCE_DIR}/src/rpcperf_imp
                ${CMAKE_CURRENT_SOURCE_DIR}/asm_src/profiling_assembly.S
                ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
                ${HEXAGON_SDK_ROOT}/utils/examples/time_utils
    )

    add_library(profiling_q SHARED ${q_exec_srcs})

    add_library(profiling_skel_static STATIC ${skel_srcs})

    #Workaround for link error with 8.2.08 and 8.1.05 tools
    string(FIND ${PREBUILT_LIB_DIR} "toolv82" toolv82)
    string(FIND ${PREBUILT_LIB_DIR} "toolv81" toolv81)

    if ((NOT ${toolv81} EQUAL -1) OR (NOT ${toolv82} EQUAL -1))
        target_compile_options(profiling_skel_static PUBLIC -mllvm -disable-nvjump)
        target_compile_options(profiling_skel PUBLIC -mllvm -disable-nvjump)
    endif()

    # Link target with custom libraries
    link_custom_library(profiling_q rpcmem)

    add_dependencies(profiling_q profiling_skel_static)
    target_link_libraries(profiling_q profiling_skel_static)

    prepare_libraries_hexagon(hexagonTargets hexagonIncs hexagonLibs libqprintf.a)
    include_directories(${hexagonIncs})
    add_dependencies(profiling_q ${hexagonTargets})
    target_link_libraries(profiling_q ${hexagonLibs})

    add_dependencies(profiling_skel profiling_q)

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(profiling_q profiling_skel)

    set(HEXAGON_EXEC_CMD_OPTIONS ${HEXAGON_EXEC_CMD_OPTIONS} -f timers)
    runMainOnHexagonSim(profiling_q)

endif()

# vim: set noet fenc=utf-8 ff=unix ft=cmake :
