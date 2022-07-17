cmake_minimum_required(VERSION 3.14.3)

project(run_main_on_hexagon C CXX ASM)

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
set(incs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_BINARY_DIR}/
    ${HEXAGON_SDK_ROOT}/incs/
    ${HEXAGON_SDK_ROOT}/incs/stddef/
    ${HEXAGON_SDK_ROOT}/utils/examples/
    ${HEXAGON_SDK_ROOT}/rtos/qurt/
    ${HEXAGON_SDK_ROOT}/incs/HAP/
    )

include_directories(${incs})


if(${OS_TYPE} MATCHES "HLOS")

    #creating executable
    add_executable(run_main_on_hexagon_device
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/run_main_on_hexagon_ap
                    ${CMAKE_CURRENT_BINARY_DIR}/run_main_on_hexagon_stub
    )

    # Add common HLOS-specific linker options
    link_options(run_main_on_hexagon_device)

    build_idl(inc/run_main_on_hexagon.idl run_main_on_hexagon_device)

    # Find the DSP from provided domain value, default is CDSP
    choose_dsprpc(${DSP_TYPE} dsprpc)
    link_custom_library(run_main_on_hexagon_device ${dsprpc})

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(run_main_on_hexagon_device)

else()

    #build skel library
    add_library(run_main_on_hexagon_skel SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/src/run_main_on_hexagon_dsp
        ${CMAKE_CURRENT_BINARY_DIR}/run_main_on_hexagon_skel
    )

    build_idl(inc/run_main_on_hexagon.idl run_main_on_hexagon_skel)

    # creating test_main DLL
    add_dependencies(run_main_on_hexagon_skel test_main)
    add_library(test_main SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/test_main)
    set(TOOLS_HEXAGON_TARGET_DIR "${HEXAGON_TOOLCHAIN}/Tools/target/hexagon/lib/${DSP_VERSION}/G0")

    string(FIND ${PREBUILT_LIB_DIR} "toolv81" is_toolv81)
    string(FIND ${PREBUILT_LIB_DIR} "toolv82" is_toolv82)

    if (${is_toolv82} EQUAL -1 AND ${is_toolv81} EQUAL -1)
        add_executable(run_main_on_hexagon_sim
            ${CMAKE_CURRENT_SOURCE_DIR}/src/run_main_on_hexagon_sim
            ${CMAKE_CURRENT_SOURCE_DIR}/src/run_main_on_hexagon_dsp
        )

        build_idl(inc/run_main_on_hexagon.idl run_main_on_hexagon_sim)

        # Link target with custom libraries
        link_custom_library(run_main_on_hexagon_sim test_util)
        link_custom_library(run_main_on_hexagon_sim atomic)
        link_custom_library(run_main_on_hexagon_sim rtld)

        add_dependencies(run_main_on_hexagon_sim test_main)
        target_link_libraries(run_main_on_hexagon_sim ${TOOLS_HEXAGON_TARGET_DIR}/libc_eh.a)

        # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
        copy_binaries(run_main_on_hexagon_sim run_main_on_hexagon_skel)

        add_dependencies(run_main_on_hexagon_skel run_main_on_hexagon_sim)

        set(HEXAGON_EXEC_CMD_OPTIONS ${HEXAGON_EXEC_CMD_OPTIONS} libtest_main.so 1 foo 2 bar)
        runHexagonSim(run_main_on_hexagon_sim)

    endif()

endif()
