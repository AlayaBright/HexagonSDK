cmake_minimum_required(VERSION 3.14.3)

project(qprintf C CXX ASM)

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

set(incs
    inc/
)

set(srcs
    src/qprintf
    asm_src/qprintf_asm.S
)

include_directories(${incs})
add_library(qprintf SHARED ${srcs})
add_library(qprintf_static STATIC ${srcs})
set_target_properties(qprintf_static PROPERTIES OUTPUT_NAME "qprintf")

# Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
copy_binaries(qprintf qprintf_static)

install(TARGETS qprintf DESTINATION ./ EXPORT qprintf-targets)
install(EXPORT qprintf-targets DESTINATION ./qprintf)
