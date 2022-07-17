#set the paths of the toolchain
get_filename_component(HEXAGON_SDK_ROOT "$ENV{HEXAGON_SDK_ROOT}" REALPATH)
get_filename_component(UBUNTUARM_TOOLS_DIR "$ENV{UBUNTUARM_TOOLS_DIR}" REALPATH)

set(HEXAGON_CMAKE_ROOT ${HEXAGON_SDK_ROOT}/build/cmake)
include(${HEXAGON_CMAKE_ROOT}/common_helper.cmake)

#Get the Binary extension of the Hexagon Toolchain
if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
    set(WINDOWS_SUFFIX .exe)
endif()

# Set the compilers
if(OPENWRT)
    set(UBUNTUARM_PREFIX ${UBUNTUARM_TOOLS_DIR}/bin/arm-openwrt-linux-muslgnueabi-)
    set(UBUNTUARM_LIB_DIR ${UBUNTUARM_TOOLS_DIR}/lib/gcc/arm-openwrt-linux-muslgnueabi/5.2.0)
else()
    if(SOFT_FLOAT)
        set(UBUNTUARM_PREFIX ${UBUNTUARM_TOOLS_DIR}/bin/arm-linux-gnueabi-)
        set(UBUNTUARM_LIB_DIR ${UBUNTUARM_TOOLS_DIR}/lib/gcc/arm-linux-gnueabi/4.9.3)
    else()
        set(UBUNTUARM_PREFIX ${UBUNTUARM_TOOLS_DIR}/bin/arm-linux-gnueabihf-)
        set(UBUNTUARM_LIB_DIR ${UBUNTUARM_TOOLS_DIR}/lib/gcc/arm-linux-gnueabihf/4.9.3)
    endif()
endif()

set(CMAKE_PREFIX_PATH ${UBUNTUARM_LIB_DIR})
set(CMAKE_C_COMPILER ${UBUNTUARM_PREFIX}gcc${WINDOWS_SUFFIX})
set(CMAKE_CXX_COMPILER ${UBUNTUARM_PREFIX}g++${WINDOWS_SUFFIX})
set(CMAKE_AR ${UBUNTUARM_PREFIX}ar${WINDOWS_SUFFIX})
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

#Compiler and Linker options
set(EXE_LD_FLAGS)
list2string(EXE_LD_FLAGS
    -march=armv7-a
    -mthumb-interwork
    -mfpu=neon
    -mtune=cortex-a8
    -Wl,-unresolved-symbols=ignore-in-shared-libs
    -Wl,-ldl
    "-o <TARGET>"
    -Wl,--start-group
    -L${CMAKE_CURRENT_BINARY_DIR}
    "<OBJECTS>"
    "<LINK_LIBRARIES>"
    "<LINK_FLAGS>"
    -Wl,--end-group
    -L${UBUNTUARM_LIB_DIR}
    -lm
    -lc
    -lsupc++
    -lgcc_eh
    -lgcc
)

set(DLL_LD_FLAGS)
list2string(DLL_LD_FLAGS
    -march=armv7-a
    -mthumb-interwork
    -mfpu=neon
    -mtune=cortex-a8
    -Wl,-unresolved-symbols=ignore-in-shared-libs
    -fpic
    -nostartfiles
    -shared
    -Bsymbolc
    "-o <TARGET>"
    -Wl,--start-group
    "<OBJECTS>"
    "<LINK_LIBRARIES>"
    "<LINK_FLAGS>"
    -Wl,--end-group
    -L${UBUNTUARM_LIB_DIR}
    -lm
    -lc
    -lsupc++
    -lgcc_eh
    -lgcc
)

set(COMMON_FLAGS "-mword-relocations -mthumb-interwork -mfpu=neon -mtune=cortex-a8 -march=armv7-a -c -fPIC -Wall -Wno-missing-braces -DARM_ARCH_7A -DUSE_SYSLOG")
set(CXX_FLAGS "-std=c++11")
set(C_FLAGS "-std=gnu99")
set(RELEASE_FLAGS "-O2 -fno-strict-aliasing")
set(ADD_SYMBOLS_FLAGS "-g" )
set(ASM_FLAGS "-DRUN_ON_NEON -DHAVE_NEON=1 -mfpu=neon")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${COMMON_FLAGS} ${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COMMON_FLAGS} ${CXX_FLAGS} ${RELEASE_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${COMMON_FLAGS} ${C_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${COMMON_FLAGS} ${C_FLAGS} ${RELEASE_FLAGS}")

#Flags for releaseG variant
if(ADD_SYMBOLS)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${ADD_SYMBOLS_FLAGS} ")
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} ${ADD_SYMBOLS_FLAGS} ")
endif()

set(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} ${CMAKE_C_FLAGS_DEBUG} ${ASM_FLAGS}")
set(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE} ${ASM_FLAGS}")

set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_COMPILER} ${EXE_LD_FLAGS}")
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_C_COMPILER} ${EXE_LD_FLAGS}")
set(CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_C_COMPILER} ${DLL_LD_FLAGS}")
set(CMAKE_C_CREATE_SHARED_LIBRARY "${CMAKE_C_COMPILER} ${DLL_LD_FLAGS}")
set(CMAKE_C_ARCHIVE_CREATE "${CMAKE_AR} -rsc <TARGET> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_CREATE "${CMAKE_AR} -rsc <TARGET> <OBJECTS>")

