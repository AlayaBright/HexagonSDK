cmake_minimum_required(VERSION 3.14.3)

project(calculator_plus C CXX ASM)

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
    ${HEXAGON_SDK_ROOT}/examples/calculator_c++/inc/
    ${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem/inc/
    )

include_directories(${common_incs})

if(${OS_TYPE} MATCHES "HLOS")

    ## HLOS dynamic library
    add_library(calculator_plus SHARED ${CMAKE_CURRENT_BINARY_DIR}/calculator_plus_stub)

    # Build interface files using user IDL
    build_idl(inc/calculator_plus.idl calculator_plus)

    # Find the DSP from provided domain value, default is CDSP
    choose_dsprpc(${DSP_TYPE} dsprpc)

    ## HLOS Executable
    add_executable(calculator_plus_device
        ${CMAKE_CURRENT_SOURCE_DIR}/src/calculator_apps
        ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
    )

    if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set_target_properties(calculator_plus_device PROPERTIES OUTPUT_NAME "calculator_plus")
    endif()

    # Add common HLOS-specific linker options
    link_options(calculator_plus_device)

    target_compile_definitions(calculator_plus_device PUBLIC VERIFY_PRINT_ERROR)
    # Link target with HLOS-specific custom libraries
    link_custom_library(calculator_plus_device rpcmem)
    link_custom_library(calculator_plus_device getopt_custom)
    link_custom_library(calculator_plus_device ${dsprpc})
    link_custom_library(calculator_plus ${dsprpc})

    add_dependencies(calculator_plus_device calculator_plus)
    target_link_libraries(calculator_plus_device calculator_plus)

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(calculator_plus_device)

else()

    set(skel_srcs ${CMAKE_CURRENT_BINARY_DIR}/calculator_plus_skel
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/calculator_dsp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/thread_local
        )

    add_library(calculator_plus_skel SHARED ${skel_srcs})

    target_compile_definitions(calculator_plus_skel PUBLIC CALCULATOR_STATICLIB_TEST)

    target_link_libraries(calculator_plus_skel ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++abi.so.1)
    target_link_libraries(calculator_plus_skel ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++.so.1 )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -std=c++14")
    file(COPY ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++abi.so.1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++.so.1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++abi.so.1.0 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++.so.1.0 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

    add_library(calculator_plus_q SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/calculator_test
                                            ${CMAKE_CURRENT_SOURCE_DIR}/src/calculator_test_main
                                            ${CMAKE_CURRENT_SOURCE_DIR}/src/calculator_dsp
                                            ${CMAKE_CURRENT_SOURCE_DIR}/src/thread_local
                                            ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
    )

    build_idl(inc/calculator_plus.idl calculator_plus_q)

    link_custom_library(calculator_plus_q rpcmem)

    target_link_libraries(calculator_plus_q ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++abi.so.1)
    target_link_libraries(calculator_plus_q ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++.so.1 )

    #With Hexagon Tools 8.4.0x, new symbols are added for c++17 compliance,
    #whose definitions are not present on targets earlier to saipan.
    #Link this weak_refs.so to your library for weak definitions of these new symbols.
    string(FIND ${PREBUILT_LIB_DIR} "toolv81" is_toolv81)
    string(FIND ${PREBUILT_LIB_DIR} "toolv82" is_toolv82)
    string(FIND ${PREBUILT_LIB_DIR} "toolv83" is_toolv83)

    if (${is_toolv81} EQUAL -1 AND ${is_toolv82} EQUAL -1 AND ${is_toolv83} EQUAL -1)
        target_link_libraries(calculator_plus_skel ${HEXAGON_SDK_ROOT}/libs/weak_refs/ship/hexagon_toolv84/weak_refs.so)
    endif()

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/calculator.input DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

    add_dependencies(calculator_plus_skel calculator_plus_q)

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(calculator_plus_skel calculator_plus_q)

    runMainOnHexagonSim(calculator_plus_q)

endif()

# vim: set noet fenc=utf-8 ff=unix ft=cmake :
