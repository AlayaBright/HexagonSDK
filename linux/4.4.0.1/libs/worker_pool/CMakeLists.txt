cmake_minimum_required(VERSION 3.14.3)

project(worker_pool C CXX ASM)

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
    src/
)

set(srcs
    src/worker_pool
)

include_directories(${incs})
add_library(worker_pool SHARED ${srcs})

target_link_options(worker_pool PUBLIC -Wl,--gc-sections -Wl,--print-gc-sections)
set(WORKER_POOL_FLAGS "-ffunction-sections -fvisibility=hidden \"-D__QAIC_HEADER_EXPORT=__attribute__((__visibility__(\"default\")))\" \"-D__QAIC_SKEL_EXPORT=__attribute__((__visibility__(\"default\")))\"")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${WORKER_POOL_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${WORKER_POOL_FLAGS}")

# Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
copy_binaries(worker_pool)

install(TARGETS worker_pool DESTINATION ./ EXPORT worker_pool-targets)
install(EXPORT worker_pool-targets DESTINATION ./worker_pool)
