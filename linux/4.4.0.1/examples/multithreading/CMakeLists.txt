cmake_minimum_required(VERSION 3.14.3)

project(multithreading C CXX ASM)

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
    )

include_directories(${common_incs})

if(${OS_TYPE} MATCHES "HLOS")

    ## HLOS dynamic library
    add_library(multithreading SHARED ${CMAKE_CURRENT_BINARY_DIR}/multithreading_stub)

    # Build interface files using user IDL
    build_idl(inc/multithreading.idl multithreading)

    # Find the DSP from provided domain value, default is CDSP
    choose_dsprpc(${DSP_TYPE} dsprpc)

    ## HLOS Executable
    add_executable(multithreading_device
        ${CMAKE_CURRENT_SOURCE_DIR}/src/multithreading
        ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
    )

    if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set_target_properties(multithreading_device PROPERTIES OUTPUT_NAME "multithreading")
    endif()

    # Add common HLOS-specific linker options
    link_options(multithreading_device)

    # Add target with HLOS-specific custom libraries
    link_custom_library(multithreading_device getopt_custom)

    # Add compile definitions to the target
    target_compile_definitions(multithreading_device PUBLIC VERIFY_PRINT_ERROR)

    # Add include directories to the build
    include_directories(
        ${common_incs}
        ${HEXAGON_SDK_ROOT}/utils/examples/
    )

    link_custom_library(multithreading ${dsprpc})
    link_custom_library(multithreading_device ${dsprpc})
    add_dependencies(multithreading_device multithreading)
    target_link_libraries(multithreading_device multithreading)

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(multithreading_device)

else()

    set(skel_srcs ${CMAKE_CURRENT_BINARY_DIR}/multithreading_skel
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/multithreading_imp
        )

    # creating shared skel library
    add_library(multithreading_skel SHARED ${skel_srcs})

    build_idl(inc/multithreading.idl multithreading_skel)

    set(skel_srcs ${CMAKE_CURRENT_BINARY_DIR}/multithreading_skel
                ${CMAKE_CURRENT_SOURCE_DIR}/src/multithreading_imp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/multithreading
                ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
    )
    include_directories(${common_incs})

    add_library(multithreading_q SHARED ${skel_srcs})
    add_dependencies(multithreading_skel multithreading_q)

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(multithreading_q multithreading_skel)

    set(HEXAGON_EXEC_SIM_OPTIONS ${HEXAGON_EXEC_SIM_OPTIONS} --dsp_clock 1000 --ahb:lowaddr 0xc0000000 --ahb:highaddr 0xc0ffffff)

    runMainOnHexagonSim(multithreading_q)

endif()

# vim: set noet fenc=utf-8 ff=unix ft=cmake :
