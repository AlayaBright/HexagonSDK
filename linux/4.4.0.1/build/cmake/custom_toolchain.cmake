#Custom Toolchain
# Set the compilers
get_filename_component(HEXAGON_SDK_ROOT "$ENV{HEXAGON_SDK_ROOT}" REALPATH)
set(HEXAGON_CMAKE_ROOT ${HEXAGON_SDK_ROOT}/build/cmake)
include(${HEXAGON_CMAKE_ROOT}/common_helper.cmake)
set(CUSTOM_PREFIX "") #update this variable with the prefix of your toolchain
set(CUSTOM_LIB_DIR "") #Update the library path

set(CMAKE_PREFIX_PATH ${CUSTOM_LIB_DIR})
#set your compilers
set(CMAKE_C_COMPILER ${CUSTOM_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${CUSTOM_PREFIX}g++)
set(CMAKE_AR ${CUSTOM_PREFIX}ar)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

#set C and CXX FLAGS
set(RELEASE_FLAGS "-O2 -fno-strict-aliasing")
set(DEBUG_FLAGS "-O0")
set(ADD_SYMBOLS_FLAGS "-g")
set(CXX_FLAGS "-std=c++11")
set(C_FLAGS "-std=gnu99")

#set your default compiler flags
set(EXE_LD_FLAGS)
list2string(EXE_LD_FLAGS
    "-o <TARGET>"
    -Wl,--start-group
    "<OBJECTS>"
    "<LINK_LIBRARIES>"
    "<LINK_FLAGS>"
    -Wl,--end-group
)

set(DLL_LD_FLAGS)
list2string(DLL_LD_FLAGS
    -shared
    "-o <TARGET>"
    -Wl,--start-group
    "<OBJECTS>"
    "<LINK_LIBRARIES>"
    "<LINK_FLAGS>"
    -Wl,--end-group
)

##############################################################

# Below Code is not advisable to be changed. To update the configuration
# modify the above code section

#############################################################

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS} ${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS} ${CXX_FLAGS} ")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_FLAGS} ${C_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${RELEASE_FLAGS} ${C_FLAGS}")
set(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} ${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}")

#These Flags are for ReleaseG variant
if(ADD_SYMBOLS)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${ADD_SYMBOLS_FLAGS}")
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} ${ADD_SYMBOLS_FLAGS}")
endif()

#Set the linker flags for executable and shared library
set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_CXX_COMPILER} ${EXE_LD_FLAGS}")
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_COMPILER} ${EXE_LD_FLAGS}")
set(CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_COMPILER} ${DLL_LD_FLAGS}")
set(CMAKE_C_CREATE_SHARED_LIBRARY "${CMAKE_CXX_COMPILER} ${DLL_LD_FLAGS}")
set(CMAKE_C_ARCHIVE_CREATE "${CMAKE_AR} -rsc <TARGET> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_CREATE "${CMAKE_AR} -rsc <TARGET> <OBJECTS>")
