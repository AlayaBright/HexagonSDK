# ===============================================================================
#     Copyright 2018 Qualcomm Technologies, Inc.  All rights reserved.
#     Confidential & Proprietary
# ===============================================================================

# Cross Compiling for Hexagon
set(HEXAGON TRUE)
set(CMAKE_SYSTEM_NAME QURT)
set(CMAKE_SYSTEM_PROCESSOR Hexagon)
set(CMAKE_SYSTEM_VERSION "1") #${HEXAGON_PLATFORM_LEVEL})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

#Get the Binary extension of the Hexagon Toolchain
if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
    set(HEXAGON_TOOLCHAIN_SUFFIX .exe)
endif()
message(STATUS "CMAKE_HOST_SYSTEM_NAME:${CMAKE_HOST_SYSTEM_NAME}")

# Root of 'Hexagon SDK'
get_filename_component(HEXAGON_SDK_ROOT "$ENV{HEXAGON_SDK_ROOT}" REALPATH)
# Root of 'Hexagon LLVM Toolchain Release'
# user need to set ENV HEXAGON_TOOLS_ROOT if customs tools are to be used, Defaults to the version SDK supports
get_filename_component(HEXAGON_TOOLCHAIN "$ENV{HEXAGON_TOOLS_ROOT}" REALPATH)
if(HEXAGON_TOOLCHAIN  MATCHES  "^.*HEXAGON_Tools.*$")
    message(STATUS "Hexagon TOOLS ROOT:${HEXAGON_TOOLCHAIN}")
else()
    get_filename_component(HEXAGON_TOOLCHAIN "$ENV{DEFAULT_HEXAGON_TOOLS_ROOT}" REALPATH)
endif()

#Currently supportig V65 and above targets up to v68
if(DSP_VERSION MATCHES "v66" )
    set(HEXAGON_ARCH v66)
    set(SIM_V_ARCH "${HEXAGON_ARCH}g_1024")
elseif(DSP_VERSION MATCHES "v68")
    set(HEXAGON_ARCH v68)
    set(SIM_V_ARCH "${HEXAGON_ARCH}n_1024")
else()
    # default V65
    set(HEXAGON_ARCH v65)
    set(SIM_V_ARCH "${HEXAGON_ARCH}a_1024")
endif()

set(HEXAGON_LIB_DIR "${HEXAGON_TOOLCHAIN}/Tools/target/hexagon/lib")
set(HEXAGON_ISS_DIR ${HEXAGON_TOOLCHAIN}/Tools/lib/iss)
set(RUN_MAIN_HEXAGON "${HEXAGON_SDK_ROOT}/libs/run_main_on_hexagon/ship/${PREBUILT_LIB_DIR}/run_main_on_hexagon_sim")
#QURT SPECIFIC LIBS and Includes
set(HEXAGON_CMAKE_ROOT ${HEXAGON_SDK_ROOT}/build/cmake)
include(${HEXAGON_CMAKE_ROOT}/common_helper.cmake)
# Linker Flags
# QURT Related includes and linker flags

set(V_ARCH ${HEXAGON_ARCH})
set(_QURT_INSTALL_DIR "${HEXAGON_SDK_ROOT}/rtos/qurt/ADSP${V_ARCH}MP${V_ARCH_EXTN}")
set(_QURT_INSTALL_DIR "${HEXAGON_SDK_ROOT}/rtos/qurt/compute${V_ARCH}${V_ARCH_EXTN}")

if( ${TREE} MATCHES PAKMAN )
    set(_QURT_INSTALL_DIR "${QURT_IMAGE_DIR}/compute${V_ARCH}${V_ARCH_EXTN}")
endif()
message(STATUS "_QURT_INSTALL_DIR:${_QURT_INSTALL_DIR}")
set(RTOS_DIR ${_QURT_INSTALL_DIR})
set(QCC_DIR "${HEXAGON_QCC_DIR}/${V_ARCH}/G0")
set(TARGET_DIR "${HEXAGON_LIB_DIR}/${V_ARCH}/G0")
include_directories(
    ${_QURT_INSTALL_DIR}/include
    ${_QURT_INSTALL_DIR}/include/qurt
    ${_QURT_INSTALL_DIR}/include/posix
    )

set(QURT_START_LINK_LIBS)
list2string(QURT_START_LINK_LIBS
    "${TARGET_DIR}/init.o"
    "${RTOS_DIR}/lib/crt1.o"
    "${RTOS_DIR}/lib/debugmon.o"
    "${RTOS_DIR}/lib/libqurt.a"
    "${TARGET_DIR}/libc.a"
    "${TARGET_DIR}/libqcc.a"
    "${TARGET_DIR}/libhexagon.a"
    "${RTOS_DIR}/lib/libqurtcfs.a"
    "${RTOS_DIR}/lib/libtimer_island.a"
    "${RTOS_DIR}/lib/libtimer_main.a"
    "${RTOS_DIR}/lib/libposix.a"
    )

set(QURT_END_LINK_LIBS
    ${TARGET_DIR}/fini.o
    )

# Non QURT related includes and linker flags

set(TARGET_DIR_NOOS "${HEXAGON_TOOLCHAIN}/Tools/target/hexagon/lib/${HEXAGON_ARCH}")

set(EXE_LD_FLAGS)
list2string(EXE_LD_FLAGS
    -m${V_ARCH}
    "-o <TARGET>"
    -G0
    -Wl,-L${TARGET_DIR_NOOS}/G0/
    -Wl,-L${HEXAGON_TOOLCHAIN}/Tools/target/hexagon/lib/
    -Wl,--no-threads -Wl,--dynamic-linker= -Wl,-E -Wl,--force-dynamic,-u,main
    -Wl,--start-group
    "<OBJECTS>"
    "<LINK_LIBRARIES>"
    "<LINK_FLAGS>"
    -Wl,--end-group
    -Wl,--start-group -lstandalone -lc -lgcc -Wl,--end-group
    )

set(EXE_QURT_LD_FLAGS)
list2string(EXE_QURT_LD_FLAGS
    -m${V_ARCH}
    -nodefaultlibs
    -nostdlib
    -L${TARGET_DIR_NOOS}/G0/
    -L${HEXAGON_TOOLCHAIN}/Tools/target/hexagon/lib/
    -Wl,--section-start -Wl,.interp=0x23000000
    -Wl,--dynamic-linker= -Wl,--force-dynamic -Wl,-E -Wl,-z -Wl,muldefs -Wl,--whole-archive
    "-o <TARGET>"
    -Wl,--start-group
    ${QURT_START_LINK_LIBS}
    "<OBJECTS>"
    "<LINK_LIBRARIES>"
    "<LINK_FLAGS>"
    ${QURT_END_LINK_LIBS}
    -Wl,--end-group
    )

set(PIC_SHARED_LD_FLAGS)
list2string(PIC_SHARED_LD_FLAGS
    -m${V_ARCH}
    -G0
    -fpic
    -Wl,-Bsymbolic
    -Wl,-L${TARGET_DIR_NOOS}/G0/pic
    -Wl,-L${HEXAGON_TOOLCHAIN}/Tools/target/hexagon/lib/
    -Wl,--no-threads -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=memalign
    -shared
    "-o <TARGET>"
    -Wl,--start-group
    "<OBJECTS>"
    "<LINK_LIBRARIES>"
    "<LINK_FLAGS>"
    -Wl,--end-group
    -lc
    )

if(QURT_OS)
    set(HEXAGON_C_LINK_EXECUTABLE_LINK_OPTIONS "${EXE_QURT_LD_FLAGS}" )
    message(STATUS "Hexagon C Executable Linker Line:${HEXAGON_C_LINK_EXECUTABLE_LINK_OPTIONS}")
    set(HEXAGON_CXX_LINK_EXECUTABLE_LINK_OPTIONS "${EXE_QURT_LD_FLAGS}")
    message(STATUS "Hexagon  CXX Executable Linker Line:${HEXAGON_CXX_LINK_EXECUTABLE_LINK_OPTIONS}")
else()
    set(HEXAGON_C_LINK_EXECUTABLE_LINK_OPTIONS "${EXE_LD_FLAGS}")
    message(STATUS "Hexagon C Executable Linker Line:${HEXAGON_C_LINK_EXECUTABLE_LINK_OPTIONS}")
    set(HEXAGON_CXX_LINK_EXECUTABLE_LINK_OPTIONS "${EXE_LD_FLAGS}")
    message(STATUS "Hexagon  CXX Executable Linker Line:${HEXAGON_CXX_LINK_EXECUTABLE_LINK_OPTIONS}")
endif()

set(HEXAGON_PIC_SHARED_LINK_OPTIONS "${PIC_SHARED_LD_FLAGS}")

# System include paths
include_directories(SYSTEM ${HEXAGON_SDK_ROOT}/incs)
include_directories(SYSTEM ${HEXAGON_SDK_ROOT}/incs/stddef)
include_directories(SYSTEM ${HEXAGON_SDK_ROOT}/ipc/fastrpc/incs)

# LLVM toolchain setup
# Compiler paths, options and architecture
set(CMAKE_C_COMPILER ${HEXAGON_TOOLCHAIN}/Tools/bin/hexagon-clang${HEXAGON_TOOLCHAIN_SUFFIX})
set(CMAKE_CXX_COMPILER ${HEXAGON_TOOLCHAIN}/Tools/bin/hexagon-clang++${HEXAGON_TOOLCHAIN_SUFFIX})
set(CMAKE_AR ${HEXAGON_TOOLCHAIN}/Tools/bin/hexagon-ar${HEXAGON_TOOLCHAIN_SUFFIX})
set(CMAKE_ASM_COMPILER ${HEXAGON_TOOLCHAIN}/Tools/bin/hexagon-clang++${HEXAGON_TOOLCHAIN_SUFFIX})
set(HEXAGON_LINKER ${CMAKE_C_COMPILER})
set(CMAKE_PREFIX_PATH ${HEXAGON_TOOLCHAIN}/Tools/target/hexagon)
#Hexagon Simulator
set(HEXAGON_SIM    "${HEXAGON_TOOLCHAIN}/Tools/bin/hexagon-sim${HEXAGON_TOOLCHAIN_SUFFIX}")

#Compiler Options
set(DEBUG_FLAGS "-O0 -g")
set(RELEASE_FLAGS "-O2")
set(COMMON_FLAGS "-m${HEXAGON_ARCH} -G0 -Wall -Werror -fno-exceptions -fno-zero-initialized-in-bss -fdata-sections -fpic")

string(FIND "${PREBUILT_LIB_DIR}" "toolv82" toolv82)
string(FIND "${PREBUILT_LIB_DIR}" "toolv81" toolv81)
if(NOT ${toolv82} EQUAL -1)
   set(COMMON_FLAGS "${COMMON_FLAGS} -Wno-missing-braces")
endif()

if(NOT ${toolv81} EQUAL -1)
    set(COMMON_FLAGS "${COMMON_FLAGS} -mhvx-double -DTOOLV81")
else()
    set(COMMON_FLAGS "${COMMON_FLAGS} -mhvx -mhvx-length=128B")
endif()

set(CMAKE_CXX_FLAGS_DEBUG   "${COMMON_FLAGS} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_FLAGS} ${RELEASE_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG     "${COMMON_FLAGS} ${DEBUG_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE   "${COMMON_FLAGS} ${RELEASE_FLAGS}")
if(ADD_SYMBOLS)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g ")
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} -g ")
endif()
set(CMAKE_ASM_FLAGS_DEBUG   "${CMAKE_ASM_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS_RELEASE}")

# Linker Options
set(CMAKE_C_LINK_EXECUTABLE "${HEXAGON_LINKER} ${HEXAGON_C_LINK_EXECUTABLE_LINK_OPTIONS}")
set(CMAKE_C_CREATE_SHARED_LIBRARY "${HEXAGON_LINKER} ${HEXAGON_PIC_SHARED_LINK_OPTIONS}")
set(CMAKE_CXX_LINK_EXECUTABLE "${HEXAGON_LINKER} ${HEXAGON_CXX_LINK_EXECUTABLE_LINK_OPTIONS}")
set(CMAKE_CXX_CREATE_SHARED_LIBRARY "${HEXAGON_LINKER} ${HEXAGON_PIC_SHARED_LINK_OPTIONS}")

set(HEXAGON_EXEC_CMD_OPTIONS "")
set(HEXAGON_EXEC_SIM_OPTIONS "")

#Helper Function to run on Hexagon SIM
##############################
#
# runHexagonSim (<targetToRunOnSimulator>)
#
# This fuction will help create a target to run simulator on the target
# specified from the argument
#
# You can update <HEXAGON_EXEC_SIM_OPTIONS> to customize the options for
# simulator
#
# You can also update <HEXAGON_EXEC_CMD_OPTIONS> to customize the options
# for executable
#
##############################

function(runHexagonSim currentTarget)
    message(STATUS "runHexagonSim:currentTarget:${currentTarget}")
    string(REGEX MATCH "_q$" MATCH_OUTPUT ${BUILD_NAME})

    if(QURT_OS)

        # Do not proceed with simulator test if tools version is 8.2.x
        string(FIND ${CMAKE_CURRENT_BINARY_DIR} "toolv82" status)
        if (NOT(${status} EQUAL -1))
            add_custom_command(TARGET ${currentTarget} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold
                    "QuRT based simulator tests are not supported with 8.2.x tools"
            )
            return()
        endif()

        set(q6ssOUT ${CMAKE_CURRENT_BINARY_DIR}/q6ss.cfg)
        if(CMAKE_HOST_WIN32)
            set(q6ssLine1
                ${HEXAGON_ISS_DIR}/qtimer.dll --csr_base=0xFC900000 --irq_p=1 --freq=19200000 --cnttid=1)
            set(q6ssLine2
                ${HEXAGON_ISS_DIR}/l2vic.dll 32 0xab010000)
        else()
            set(q6ssLine1
                ${HEXAGON_ISS_DIR}/qtimer.so --csr_base=0xFC900000 --irq_p=1 --freq=19200000 --cnttid=1)
            set(q6ssLine2
                "${HEXAGON_ISS_DIR}/l2vic.so 32 0xFC910000")
        endif()
        set(osamOUT ${CMAKE_CURRENT_BINARY_DIR}/osam.cfg)
        if(CMAKE_HOST_WIN32)
            set(dll qurt_model.dll)
            set(osamString ${RTOS_DIR}/debugger/Win/${dll})
        else()
            set(osamString ${RTOS_DIR}/debugger/lnx64/qurt_model.so)
        endif()
        set(HEXAGON_EXEC ${currentTarget})

        add_custom_command(
            TARGET ${currentTarget}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
            COMMAND ${CMAKE_COMMAND} -E echo ${q6ssLine1} > "q6ss.cfg"
            COMMAND ${CMAKE_COMMAND} -E echo ${q6ssLine2} >> "q6ss.cfg"
            DEPENDS ${HEXAGON_EXEC}
        )
        add_custom_command(
            TARGET ${currentTarget}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
            COMMAND ${CMAKE_COMMAND} -E echo ${osamString} > "osam.cfg"
            DEPENDS ${HEXAGON_EXEC}
        )

        # Checking whether to run simulator or not
        if(${MATCH_OUTPUT} MATCHES "_q")
            set(QEXE_EXEC
                ${HEXAGON_SIM} -m${SIM_V_ARCH} --simulated_returnval
                --usefs ${CMAKE_CURRENT_BINARY_DIR}
                --pmu_statsfile ${CMAKE_CURRENT_BINARY_DIR}/pmu_stats.txt
                ${HEXAGON_EXEC_SIM_OPTIONS}
                --cosim_file ${CMAKE_CURRENT_BINARY_DIR}/q6ss.cfg
                --l2tcm_base 0xd800
                --rtos ${CMAKE_CURRENT_BINARY_DIR}/osam.cfg
                ${RTOS_DIR}/sdksim_bin/runelf.pbn --
                ${HEXAGON_EXEC} --
                ${HEXAGON_EXEC_CMD_OPTIONS}
            )
            message(STATUS "QEXE_EXEC:${QEXE_EXEC}")

            add_custom_command(
                TARGET ${currentTarget}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
                COMMAND ${QEXE_EXEC}
                DEPENDS ${HEXAGON_EXEC} OSAM Q6SS
            )

            list2string(QEXE_SIM "${QEXE_EXEC}")
            file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/sim_cmd_line.txt "${QEXE_SIM}")

        endif()

    else()

        # Checking whether to run simulator or not
        if(${MATCH_OUTPUT} MATCHES "_q")
            set(HEXAGON_EXEC ${currentTarget})

            set(QEXE_EXEC
                ${HEXAGON_SIM} -m${SIM_V_ARCH} --simulated_returnval
                --usefs ${CMAKE_CURRENT_BINARY_DIR}
                --pmu_statsfile ${CMAKE_CURRENT_BINARY_DIR}/pmu_stats.txt
                ${HEXAGON_EXEC_SIM_OPTIONS}
                --l2tcm_base 0xd800 --
                ${HEXAGON_EXEC}
                ${HEXAGON_EXEC_CMD_OPTIONS}
            )
            message(STATUS "QEXE_EXEC:${QEXE_EXEC}")

            add_custom_command(
                TARGET ${currentTarget}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
                COMMAND ${QEXE_EXEC}
                DEPENDS ${HEXAGON_EXEC}
            )

            list2string(QEXE_SIM "${QEXE_EXEC}")
            file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/sim_cmd_line.txt "${QEXE_SIM}")

        endif()

    endif()

endfunction()


function(runMainOnHexagonSim currentTarget)
    message(STATUS "runMainOnHexagonSim:currentTarget:${currentTarget}")
    string(REGEX MATCH "_q$" MATCH_OUTPUT ${BUILD_NAME})

    if(QURT_OS)

        # Do not proceed with simulator test if tools version is 8.2.x
        string(FIND ${CMAKE_CURRENT_BINARY_DIR} "toolv82" status)
        if (NOT(${status} EQUAL -1))
            add_custom_command(TARGET ${currentTarget} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --red --bold
                    "QuRT based simulator tests are not supported with 8.2.x tools"
            )
            return()
        endif()

        set(q6ssOUT ${CMAKE_CURRENT_BINARY_DIR}/q6ss.cfg)
        if(CMAKE_HOST_WIN32)
            set(q6ssLine1
                ${HEXAGON_ISS_DIR}/qtimer.dll --csr_base=0xFC900000 --irq_p=1 --freq=19200000 --cnttid=1)
            set(q6ssLine2
                ${HEXAGON_ISS_DIR}/l2vic.dll 32 0xab010000)
        else()
            set(q6ssLine1
                ${HEXAGON_ISS_DIR}/qtimer.so --csr_base=0xFC900000 --irq_p=1 --freq=19200000 --cnttid=1)
            set(q6ssLine2
                "${HEXAGON_ISS_DIR}/l2vic.so 32 0xFC910000")
        endif()
        set(osamOUT ${CMAKE_CURRENT_BINARY_DIR}/osam.cfg)
        if(CMAKE_HOST_WIN32)
            set(dll qurt_model.dll)
            set(osamString ${RTOS_DIR}/debugger/Win/${dll})
        else()
            set(osamString ${RTOS_DIR}/debugger/lnx64/qurt_model.so)
        endif()
        set(HEXAGON_EXEC ${currentTarget})

        add_custom_command(
            TARGET ${currentTarget}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
            COMMAND ${CMAKE_COMMAND} -E echo ${q6ssLine1} > "q6ss.cfg"
            COMMAND ${CMAKE_COMMAND} -E echo ${q6ssLine2} >> "q6ss.cfg"
            DEPENDS ${HEXAGON_EXEC}
        )
        add_custom_command(
            TARGET ${currentTarget}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
            COMMAND ${CMAKE_COMMAND} -E echo ${osamString} > "osam.cfg"
            DEPENDS ${HEXAGON_EXEC}
        )

        # Checking whether to run simulator or not
        if(${MATCH_OUTPUT} MATCHES "_q")
            set(QEXE_EXEC
                ${HEXAGON_SIM} -m${SIM_V_ARCH} --simulated_returnval
                --usefs ${CMAKE_CURRENT_BINARY_DIR}
                --pmu_statsfile ${CMAKE_CURRENT_BINARY_DIR}/pmu_stats.txt
                ${HEXAGON_EXEC_SIM_OPTIONS}
                --cosim_file ${CMAKE_CURRENT_BINARY_DIR}/q6ss.cfg
                --l2tcm_base 0xd800
                --rtos ${CMAKE_CURRENT_BINARY_DIR}/osam.cfg
                ${RTOS_DIR}/sdksim_bin/runelf.pbn --
                ${RUN_MAIN_HEXAGON} --
                lib${HEXAGON_EXEC}.so
                ${HEXAGON_EXEC_CMD_OPTIONS}
            )
            message(STATUS "QEXE_EXEC:${QEXE_EXEC}")

            add_custom_command(
                TARGET ${currentTarget}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
                COMMAND ${QEXE_EXEC}
                DEPENDS ${HEXAGON_EXEC} OSAM Q6SS
            )

            list2string(QEXE_SIM "${QEXE_EXEC}")
            file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/sim_cmd_line.txt "${QEXE_SIM}")

        endif()

    else()

        # Checking whether to run simulator or not
        if(${MATCH_OUTPUT} MATCHES "_q")
            set(HEXAGON_EXEC ${currentTarget})

            set(QEXE_EXEC
                ${HEXAGON_SIM} -m${SIM_V_ARCH} --simulated_returnval
                --usefs ${CMAKE_CURRENT_BINARY_DIR}
                --pmu_statsfile ${CMAKE_CURRENT_BINARY_DIR}/pmu_stats.txt
                ${HEXAGON_EXEC_SIM_OPTIONS}
                --l2tcm_base 0xd800
                ${RUN_MAIN_HEXAGON} --
                lib${HEXAGON_EXEC}.so
                ${HEXAGON_EXEC_CMD_OPTIONS}
            )
            message(STATUS "QEXE_EXEC:${QEXE_EXEC}")

            add_custom_command(
                TARGET ${currentTarget}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
                COMMAND ${QEXE_EXEC}
                DEPENDS ${HEXAGON_EXEC}
            )

            list2string(QEXE_SIM "${QEXE_EXEC}")
            file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/sim_cmd_line.txt "${QEXE_SIM}")

        endif()

    endif()

endfunction()
