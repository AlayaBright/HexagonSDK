cmake_minimum_required(VERSION 3.14.3)

project(qhl_hvx C CXX ASM)

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

add_library(qhl_hvx_example_skel SHARED qhl_hvx_example)
add_library(qhl_hvx_example_q SHARED qhl_hvx_example)

# This is a workaround for missing symbols in fastrpc shell. Will remove once fastrpc shell have those symbols
target_link_options(qhl_hvx_example_skel PUBLIC ${HEXAGON_TOOLCHAIN}/Tools/target/hexagon/lib/${V_ARCH}/G0/pic/libgcc.a)

if(${DSP_VERSION} MATCHES "v66" OR ${DSP_VERSION} MATCHES "v65")
    add_compile_definitions(__V_DYNAMIC__)
else()
    add_compile_definitions(RUN_ON_V68 __V_DYNAMIC__)
endif()

link_custom_library(qhl_hvx_example_skel qhl_hvx)
link_custom_library(qhl_hvx_example_q qhl_hvx)

set_target_properties(qhl_hvx_example_skel PROPERTIES OUTPUT_NAME "qhl_hvx_example")

add_dependencies(qhl_hvx_example_skel qhl_hvx_example_q)

# Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
copy_binaries(qhl_hvx_example_q qhl_hvx_example_skel)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_data/fft2D_float_real_test_data_HVX_in128x128.dat DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(HEXAGON_EXEC qhl_hvx_example_q)
set(HEXAGON_EXEC_CMD_OPTIONS ${HEXAGON_EXEC_CMD_OPTIONS} 128 ${CMAKE_CURRENT_BINARY_DIR}/fft2D_float_real_test_data_HVX_in128x128.dat ${CMAKE_CURRENT_BINARY_DIR}/fft2D_float_real_test_data_HVX_in128x128.out)

runMainOnHexagonSim(qhl_hvx_example_q)
