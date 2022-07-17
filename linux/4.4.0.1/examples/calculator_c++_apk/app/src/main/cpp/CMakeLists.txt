# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.14.3)

project("calculator")

# Set HEXAGON_SDK_ROOT and HEXAGON_CMAKE_ROOT
set(HEXAGON_SDK_ROOT "/home/eieyanibright/Applications/Qualcomm/Hexagon_SDK/4.4.0.1/4.4.0.1")
set(ENV{HEXAGON_SDK_ROOT} ${HEXAGON_SDK_ROOT})
set(HEXAGON_CMAKE_ROOT "${HEXAGON_SDK_ROOT}/build/cmake")
set(PREBUILT_LIB_DIR "android_aarch64")

# Use the file `hexagon_helper.cmake` as helper to CMakeLists.txt.
# It contains the definition of functions `build_idl` to build the IDL.
include(${HEXAGON_CMAKE_ROOT}/hexagon_fun.cmake)


# Add necessary includes for the project
set(common_incs
        ${CMAKE_CURRENT_BINARY_DIR}/
        ${HEXAGON_SDK_ROOT}/incs/
        ${HEXAGON_SDK_ROOT}/incs/stddef/
        ${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem/inc/
        ${HEXAGON_SDK_ROOT}/utils/examples
        )

# Add include directories to the build
include_directories(${common_incs})

# Create calculator library as SHARED to be packaged with the APK.
add_library(calculator SHARED
            ${CMAKE_CURRENT_SOURCE_DIR}/calculator-jni.cpp
            ${CMAKE_CURRENT_BINARY_DIR}/calculator_stub.c
            )

# Build interface files using user IDL
build_idl(inc/calculator.idl calculator)

# Prepare and link with remote library
target_link_directories(calculator PUBLIC "${HEXAGON_SDK_ROOT}/ipc/fastrpc/remote/ship/${PREBUILT_LIB_DIR}")
target_link_libraries(calculator cdsprpc)

# Prepare and link with rpcmem library
link_custom_library(calculator rpcmem)

# Prepare and link with log library
find_library(log-lib log)
target_link_libraries(calculator ${log-lib})