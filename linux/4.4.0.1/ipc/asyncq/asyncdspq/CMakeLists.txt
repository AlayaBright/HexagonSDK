# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.14.3)

project(asyncdspq C CXX)

set(CMAKE_DEBUG_TARGET_PROPERTIES
    INCLUDE_DIRECTORIES
    COMPILE_DEFINITIONS
    POSITION_INDEPENDENT_CODE
    CONTAINER_SIZE_REQUIRED
    LIB_VERSION
)

include(${HEXAGON_CMAKE_ROOT}/hexagon_fun.cmake)

# Necessary Includes for Base Build
set(common_incs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${HEXAGON_SDK_ROOT}/incs/stddef
    ${HEXAGON_SDK_ROOT}/incs/
    ${CMAKE_CURRENT_BINARY_DIR}
    ${HEXAGON_SDK_ROOT}/ipc/asyncq/asyncdspq/inc/
    ${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem/inc/
    ${HEXAGON_SDK_ROOT}/utils/examples/
)

include_directories(${common_incs})

if(${OS_TYPE} MATCHES "HLOS")

    # Static asyncdspq libarary
    add_library(asyncdspq_static STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/asyncdspq.c)
    build_idl(inc/asyncdspq_rpc.idl asyncdspq_static)
    set_target_properties(asyncdspq_static PROPERTIES OUTPUT_NAME "asyncdspq")
    set_target_properties(asyncdspq_static PROPERTIES PREFIX "")

    # Dynamic asyncdspq library
    add_library(asyncdspq SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asyncdspq.c
        ${CMAKE_CURRENT_BINARY_DIR}/asyncdspq_stub.c
    )

    if(${CMAKE_SYSTEM_NAME} MATCHES "UbuntuARM")
        target_compile_definitions(asyncdspq_static PUBLIC __LE__)
        target_compile_definitions(asyncdspq PUBLIC __LE__)
        target_link_options(asyncdspq_static PUBLIC -lpthread)
        target_link_options(asyncdspq PUBLIC -lpthread)
    endif()

    choose_dsprpc(${DSP_TYPE} dsprpc)
    link_custom_library(asyncdspq ${dsprpc})
    link_custom_library(asyncdspq rpcmem)

    # The dynamic libraries are to be installed in ${CMAKE_BINARY_DIR}
    install(TARGETS asyncdspq DESTINATION ./)

    # The static libraries are to be installed in ${CMAKE_BINARY_DIR}/lib/
    install(TARGETS asyncdspq_static DESTINATION ./lib/)

else()

    include_directories(${HEXAGON_SDK_ROOT}/rtos/qurt/)

    set(source_files
        ${CMAKE_CURRENT_BINARY_DIR}/asyncdspq_skel.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asyncdspq.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/asyncdspq_rpc_imp.c
    )

    # skel libraries
    add_library(asyncdspq_skel_static STATIC ${source_files})
    add_library(asyncdspq_skel SHARED ${source_files})

    build_idl(inc/asyncdspq_rpc.idl asyncdspq_skel_static)

    set_target_properties(asyncdspq_skel_static PROPERTIES OUTPUT_NAME "asyncdspq_skel")

    # The dynamic libraries are to be installed in ${CMAKE_BINARY_DIR}
    install(TARGETS asyncdspq_skel DESTINATION ./)

endif()
