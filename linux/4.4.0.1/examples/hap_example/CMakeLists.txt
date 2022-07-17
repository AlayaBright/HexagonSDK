cmake_minimum_required(VERSION 3.14.3)

project(hap_example C CXX ASM)

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
    ${HEXAGON_SDK_ROOT}/incs/HAP/
    ${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem/inc/
    )

include_directories(${common_incs})


if(${OS_TYPE} MATCHES "HLOS")

    ## HLOS stub library
    add_library(hap_example SHARED ${CMAKE_CURRENT_BINARY_DIR}/hap_example_stub)

    # Build interface files using user IDL
    build_idl(inc/hap_example.idl hap_example)

    # Find the DSP from provided domain value, default is CDSP
    choose_dsprpc(${DSP_TYPE} dsprpc)

    ## HLOS executable
    add_executable(hap_example_device
                        ${CMAKE_CURRENT_SOURCE_DIR}/src_app/hap_example_main
                        ${CMAKE_CURRENT_SOURCE_DIR}/src_app/hap_example
                        ${CMAKE_CURRENT_SOURCE_DIR}/src_app/hap_unit_test
                        ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
                   )

    if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set_target_properties(hap_example_device PROPERTIES OUTPUT_NAME "hap_example")
    endif()

    if(${CMAKE_SYSTEM_NAME} MATCHES "UbuntuARM")
        target_link_options(hap_example_device PUBLIC -pie)
    endif()

    # Add common HLOS-specific linker options
    link_options(hap_example_device)

    # Add compile definitions to the target
    target_compile_definitions(hap_example_device PUBLIC VERIFY_PRINT_ERROR)

    # Link target with custom libraries
    link_custom_library(hap_example_device rpcmem)
    link_custom_library(hap_example_device getopt_custom)
    link_custom_library(hap_example ${dsprpc})
    link_custom_library(hap_example_device ${dsprpc})

    add_dependencies(hap_example_device hap_example)
    target_link_libraries(hap_example_device hap_example)

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(hap_example_device)

else()

    set(skel_srcs ${CMAKE_CURRENT_BINARY_DIR}/hap_example_skel
                ${CMAKE_CURRENT_SOURCE_DIR}/src_dsp/hap_example_imp
                ${CMAKE_CURRENT_SOURCE_DIR}/src_dsp/hap_example_compute_res_imp
                ${CMAKE_CURRENT_SOURCE_DIR}/src_dsp/hap_example_farf_runtime
                ${CMAKE_CURRENT_SOURCE_DIR}/src_dsp/hap_example_mem_dmahandle_imp
                ${CMAKE_CURRENT_SOURCE_DIR}/src_dsp/hap_example_mem_fastrpc_mmap_imp
                ${CMAKE_CURRENT_SOURCE_DIR}/src_dsp/hap_example_perf_imp
                ${CMAKE_CURRENT_SOURCE_DIR}/src_dsp/hap_example_power_imp
                ${CMAKE_CURRENT_SOURCE_DIR}/src_dsp/hap_example_sysmon_cachelock_imp
                ${CMAKE_CURRENT_SOURCE_DIR}/src_dsp/hap_example_vtcm_mgr_imp
                ${CMAKE_CURRENT_SOURCE_DIR}/src_dsp/hap_unit_test_imp
    )

    # creating shared skel library
    add_library(hap_example_skel SHARED ${skel_srcs})

    build_idl(inc/hap_example.idl hap_example_skel)

    set(q_exec_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src_app/hap_example_main
                ${CMAKE_CURRENT_SOURCE_DIR}/src_app/hap_example
                ${CMAKE_CURRENT_SOURCE_DIR}/src_app/hap_unit_test
                ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
    )

    add_library(hap_example_skel_static STATIC ${skel_srcs})
    add_library(hap_example_q SHARED ${q_exec_srcs})

    # Link target with custom libraries
    link_custom_library(hap_example_q rpcmem)

    add_dependencies(hap_example_q hap_example_skel_static)
    target_link_libraries(hap_example_q hap_example_skel_static)

    add_dependencies(hap_example_skel hap_example_q)

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(hap_example_q hap_example_skel)

    set(HEXAGON_EXEC_CMD_OPTIONS ${HEXAGON_EXEC_CMD_OPTIONS} 0)
    set(HEXAGON_EXEC_SIM_OPTIONS ${HEXAGON_EXEC_SIM_OPTIONS} --l2tcm_base 0xd800 --dsp_clock 1000 --ahb:lowaddr 0xc0000000 --ahb:highaddr 0xc0ffffff)

    runMainOnHexagonSim(hap_example_q)

endif()

# vim: set noet fenc=utf-8 ff=unix ft=cmake :
