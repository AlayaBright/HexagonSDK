cmake_minimum_required(VERSION 3.14.3)

# Check if ASM language is required or not
project(lpi_example C CXX ASM)
enable_language(ASM)

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

# Build libraries only for Hexagon
if("${OS_TYPE}" STREQUAL "")

    # Whether LPI shared object or Non-LPI shared object is created, will depend
    # on command line argument given in the cmake command
    # LPI shared object
    add_library(LPI_skel SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/src/so_source
        ${CMAKE_CURRENT_SOURCE_DIR}/src/uimg_dl_v2.c
    )

    # Using -T option to specify the linker script file
    target_link_options(LPI_skel PUBLIC -T${CMAKE_CURRENT_SOURCE_DIR}/src/uimage_v2.lcs)
    set_target_properties(LPI_skel PROPERTIES OUTPUT_NAME "LPI")


    # Non-LPI shared object
    add_library(Non-LPI_skel SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/src/so_source
    )
    set_target_properties(Non-LPI_skel PROPERTIES OUTPUT_NAME "Non-LPI")

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(LPI_skel Non-LPI_skel)


endif()
