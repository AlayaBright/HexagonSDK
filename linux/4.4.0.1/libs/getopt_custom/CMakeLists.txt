# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.14.3)

project(getopt_custom C CXX ASM)

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
    ${HEXAGON_SDK_ROOT}/incs
    ${HEXAGON_SDK_ROOT}/incs/stddef
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    )

# Add include directories to the build
include_directories(${common_incs})

if(${OS_TYPE} MATCHES "HLOS")
    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        add_library(getopt_device STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/getopt_long_custom)
        set_target_properties(getopt_device PROPERTIES OUTPUT_NAME getopt_custom)

        # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
        copy_binaries(getopt_device)

    endif()
else()
    add_library(getopt_skel STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/getopt_long_custom)
    set_target_properties(getopt_skel PROPERTIES OUTPUT_NAME getopt_custom PREFIX "")

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(getopt_skel)

    # install the built Libs
    install(TARGETS getopt_skel DESTINATION ./lib EXPORT getopt_skel-targets)
    install(EXPORT getopt_skel-targets DESTINATION ./lib/getopt_custom)
endif()

# vim: set noet fenc=utf-8 ff=unix ft=cmake :
