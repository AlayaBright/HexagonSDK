# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.14.3)

project(rpcmem C CXX ASM)

enable_language(ASM)

set(CMAKE_DEBUG_TARGET_PROPERTIES
    INCLUDE_DIRECTORIES
    COMPILE_DEFINITIONS
    POSITION_INDEPENDENT_CODE
    CONTAINER_SIZE_REQUIRED
    LIB_VERSION
)

include(${HEXAGON_CMAKE_ROOT}/hexagon_fun.cmake)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# Necessary Includes for Base Build
set(common_incs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${HEXAGON_SDK_ROOT}/incs/stddef
    ${HEXAGON_SDK_ROOT}/incs/
)

if(${OS_TYPE} MATCHES "HLOS")

    # build static library rpcmem
    set(ignoreMe ${DSP_TYPE})

    # Add necessary includes for the base build
    include_directories(${common_incs})

    # Build static library
    add_library(rpcmem STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/rpcmem_android.c)

    # This changes the libname to rpcmem from default librpcmem
    set_target_properties(rpcmem PROPERTIES PREFIX "" )

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(rpcmem)

    # install the built Libs
    install(TARGETS rpcmem DESTINATION ./lib EXPORT rpcmem-targets)
    install(EXPORT rpcmem-targets DESTINATION ./lib/rpcmem)

else()
    #################################################
    #Hexagon Dynamic Library
    #################################################
    add_library(rpcmem_skel SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/rpcmem_stub)
    add_library(rpcmem STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/rpcmem_stub)

    set_target_properties(rpcmem PROPERTIES PREFIX "" )

    # Set some Flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

    # Include necessary dependent lib directory
    include_directories(${common_incs})

    target_link_libraries(rpcmem_skel rpcmem)

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(rpcmem_skel)

    # install the built Libs
    install(TARGETS rpcmem DESTINATION ./lib EXPORT rpcmem-targets)
    install(EXPORT rpcmem-targets DESTINATION ./lib/rpcmem)
endif()

# vim: set noet fenc=utf-8 ff=unix ft=cmake :

