# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.14.3)

project(qprintf_example C CXX ASM)

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
    ${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem/inc/
    ${HEXAGON_SDK_ROOT}/utils/examples
    )

if(${OS_TYPE} MATCHES "HLOS")

    #################################################
    # HLOS Executable
    #################################################
    add_executable(qprintf_example_device
        ${CMAKE_CURRENT_SOURCE_DIR}/src/qprintf_example
        ${CMAKE_CURRENT_BINARY_DIR}/qprintf_example_stub
        ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
    )

    set_target_properties(qprintf_example_device PROPERTIES OUTPUT_NAME "qprintf_example")

    # Add compile definitions to the target
    target_compile_definitions(qprintf_example_device PUBLIC VERIFY_PRINT_ERROR)

    # Add common HLOS-specific linker options
    link_options(qprintf_example_device)

    # Add include directories to the build
    include_directories(${common_incs})

    # Build interface files using user IDL
    build_idl(inc/qprintf_example.idl qprintf_example_device)

    # Find the DSP from provided domain value, default is CDSP
    choose_dsprpc(${DSP_TYPE} dsprpc)

    # Link target with HLOS-specific custom libraries
    link_custom_library(qprintf_example_device rpcmem)
    link_custom_library(qprintf_example_device getopt_custom)
    link_custom_library(qprintf_example_device ${dsprpc})

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(qprintf_example_device)

else()

    #################################################
    # Hexagon Dynamic Skel Library
    #################################################
    set(srcs
        ${CMAKE_CURRENT_BINARY_DIR}/qprintf_example_skel
        ${CMAKE_CURRENT_SOURCE_DIR}/src/qprintf_example_imp
        ${CMAKE_CURRENT_SOURCE_DIR}/asm_src/qprintf_example_asm.S
    )


    set(incs
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
    )

    add_library(qprintf_example_skel SHARED ${srcs})

    # Set CXX Flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

    # Add include directories to the build
    include_directories(${common_incs} ${incs})

    # Build interface files using user IDL
    build_idl("inc/qprintf_example.idl" qprintf_example_skel incs)
    link_custom_library(qprintf_example_skel qprintf)
    link_custom_library(qprintf_example_skel worker_pool)

    #################################################
    # Hexagon Simulator Test Executable
    #################################################
    add_library(qprintf_example_q
        SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/src/qprintf_example
        ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
    )

    include_directories(${common_incs} ${incs})

    # Build static skel library for building simulator test
    add_library(qprintf_example_skel_static STATIC ${srcs})
    add_dependencies(qprintf_example_q qprintf_example_skel_static)
    add_dependencies(qprintf_example_skel qprintf_example_q)
    link_custom_library(qprintf_example_skel_static qprintf)
    link_custom_library(qprintf_example_skel_static worker_pool)
    target_link_libraries(qprintf_example_q qprintf_example_skel_static)

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(qprintf_example_q qprintf_example_skel)

    set(HEXAGON_EXEC qprintf_example_q)
    set(HEXAGON_EXEC_SIM_OPTIONS ${HEXAGON_EXEC_SIM_OPTIONS} --dsp_clock 800)

    runMainOnHexagonSim(qprintf_example_q)
endif()

# vim: set noet fenc=utf-8 ff=unix ft=cmake :
