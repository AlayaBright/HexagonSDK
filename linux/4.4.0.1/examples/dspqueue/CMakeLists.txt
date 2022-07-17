cmake_minimum_required(VERSION 3.14.3)

project(dspqueue C CXX ASM)

set(CMAKE_DEBUG_TARGET_PROPERTIES
        INCLUDE_DIRECTORIES
        COMPILE_DEFINITIONS
        POSITION_INDEPENDENT_CODE
        CONTAINER_SIZE_REQUIRED
        LIB_VERSION
    )

include(${HEXAGON_CMAKE_ROOT}/hexagon_fun.cmake)

set(common_incs
    ${CMAKE_CURRENT_BINARY_DIR}
    ${HEXAGON_SDK_ROOT}/incs/
    ${HEXAGON_SDK_ROOT}/incs/stddef/
    ${HEXAGON_SDK_ROOT}/utils/examples/
    ${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem/inc/
    ${HEXAGON_SDK_ROOT}/examples/dspqueue/inc/
)
include_directories(${common_incs})

if(${OS_TYPE} MATCHES "HLOS")

    # HLOS executable
    add_executable(dspqueue_sample_device
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dspqueue_sample
        ${CMAKE_CURRENT_BINARY_DIR}/dspqueue_sample_stub
        ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
    )

    build_idl(inc/dspqueue_sample.idl dspqueue_sample_device)

    target_compile_definitions(dspqueue_sample_device PUBLIC VERIFY_PRINT_ERROR)
    target_link_options(dspqueue_sample_device PUBLIC -llog -ldl)

    # Find the DSP from provided domain value, default is CDSP
    choose_dsprpc(${DSP_TYPE} dsprpc)

    link_custom_library(dspqueue_sample_device ${dsprpc})
    link_custom_library(dspqueue_sample_device rpcmem)

    set_target_properties(dspqueue_sample_device PROPERTIES OUTPUT_NAME "dspqueue_sample")

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(dspqueue_sample_device)

else()

    # Shared skel library
    add_library(dspqueue_sample_skel SHARED
        ${CMAKE_CURRENT_BINARY_DIR}/dspqueue_sample_skel
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dspqueue_sample_imp
    )
    build_idl(inc/dspqueue_sample.idl dspqueue_sample_skel)

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(dspqueue_sample_skel)

endif()
