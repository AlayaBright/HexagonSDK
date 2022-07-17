cmake_minimum_required(VERSION 3.14.3)

project(qhl C CXX ASM)

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

add_library(qhl_example_skel SHARED qhl_example)
add_library(qhl_example_q SHARED qhl_example)

set_target_properties(qhl_example_skel PROPERTIES OUTPUT_NAME "qhl_example")

if(${DSP_VERSION} MATCHES "v66" OR ${DSP_VERSION} MATCHES "v65")
    add_compile_definitions(__V_DYNAMIC__)
else()
    add_compile_definitions(RUN_ON_V68 __V_DYNAMIC__)
endif()

link_custom_library(qhl_example_skel qhl BUILD_SOURCE)
link_custom_library(qhl_example_q qhl BUILD_SOURCE)

add_dependencies(qhl_example_skel qhl_example_q)

# Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
copy_binaries(qhl_example_q qhl_example_skel)

set(HEXAGON_EXEC qhl_example_q)
set(HEXAGON_EXEC_CMD_OPTIONS ${HEXAGON_EXEC_CMD_OPTIONS} 1 foo 2.0 bar)

runMainOnHexagonSim(qhl_example_q)
