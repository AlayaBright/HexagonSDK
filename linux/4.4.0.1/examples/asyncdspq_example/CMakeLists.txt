cmake_minimum_required(VERSION 3.14.3)

project(asyncdspq_example C CXX)

set(CMAKE_DEBUG_TARGET_PROPERTIES
        INCLUDE_DIRECTORIES
        COMPILE_DEFINITIONS
        POSITION_INDEPENDENT_CODE
        CONTAINER_SIZE_REQUIRED
        LIB_VERSION
    )

include(${HEXAGON_CMAKE_ROOT}/hexagon_fun.cmake)

set(common_incs
    ${CMAKE_CURRENT_BINARY_DIR}
    ${HEXAGON_SDK_ROOT}/incs/
    ${HEXAGON_SDK_ROOT}/incs/stddef/
    ${HEXAGON_SDK_ROOT}/utils/examples/
    ${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem/inc/
    ${HEXAGON_SDK_ROOT}/ipc/asyncq/asyncdspq/inc/
    ${HEXAGON_SDK_ROOT}/examples/asyncdspq_example/inc/
)
include_directories(${common_incs})

if(${OS_TYPE} MATCHES "HLOS")

    # All binaries that are to be build
    set(ST_EXE_NAME queuetest)
    set(FCVQUEUE_EXE_NAME fcvqueuetest)
    set(QUEUEPERF_EXE_NAME queueperf)
    set(all_binaries ${ST_EXE_NAME} ${FCVQUEUE_EXE_NAME} ${QUEUEPERF_EXE_NAME})


    # queuetest executable
    add_executable(${ST_EXE_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/queuetest
        ${CMAKE_CURRENT_BINARY_DIR}/queuetest_stub
        ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
    )
    build_idl(inc/queuetest.idl ${ST_EXE_NAME})

    # fcvqueuetest executable
    add_executable(${FCVQUEUE_EXE_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fcvqueuetest
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fcvqueue
        ${CMAKE_CURRENT_BINARY_DIR}/fcvqueuetest_stub
        ${CMAKE_CURRENT_BINARY_DIR}/fcvqueue_dsp_stub
        ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
    )
    build_idl(inc/fcvqueuetest.idl ${FCVQUEUE_EXE_NAME})
    build_idl(inc/fcvqueue_dsp.idl ${FCVQUEUE_EXE_NAME})

    # queueperf executable
    add_executable(${QUEUEPERF_EXE_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/queueperf
        ${CMAKE_CURRENT_BINARY_DIR}/queuetest_stub
        ${HEXAGON_SDK_ROOT}/utils/examples/dsp_capabilities_utils
    )

    # Common linking for all the executables that are build
    choose_dsprpc(${DSP_TYPE} dsprpc)
    foreach(binary ${all_binaries})

        if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
            target_compile_definitions(${binary} PUBLIC VERIFY_PRINT_ERROR)
            target_link_options(${binary} PUBLIC -llog)
        endif()

        if(${CMAKE_SYSTEM_NAME} MATCHES "UbuntuARM")
            target_compile_definitions(${binary} PUBLIC __LE__)
            target_link_options(${binary} PUBLIC -lpthread)
        endif()

        link_custom_library(${binary} ${dsprpc})
        link_custom_library(${binary} rpcmem)
        link_custom_library(${binary} asyncdspq_rpc)

        # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
        copy_binaries(${binary})

    endforeach()



else()

    # All the skel libraries to be build
    set(QUEUE_TEST "queuetest_skel")
    set(FCVQUEUE_DSP "fcvqueue_dsp_skel")
    set(FCVQUEUE_TEST "fcvqueuetest_skel")

    # Queue test skeleton
    add_library(${QUEUE_TEST} SHARED
        ${CMAKE_CURRENT_BINARY_DIR}/queuetest_skel
        ${CMAKE_CURRENT_SOURCE_DIR}/src/queuetest_imp
    )
    build_idl(inc/queuetest.idl ${QUEUE_TEST})
    link_custom_library(${QUEUE_TEST} asyncdspq_skel)

    # fcvqueue test/example
    add_library(${FCVQUEUE_DSP} SHARED
        ${CMAKE_CURRENT_BINARY_DIR}/fcvqueue_dsp_skel
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fcvqueue_dsp_imp
    )
    build_idl(inc/fcvqueue_dsp.idl ${FCVQUEUE_DSP})
    link_custom_library(${FCVQUEUE_DSP} asyncdspq_skel)

    # fcvqueue test
    add_library(${FCVQUEUE_TEST} SHARED
        ${CMAKE_CURRENT_BINARY_DIR}/fcvqueuetest_skel
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fcvqueuetest_imp
    )
    build_idl(inc/fcvqueuetest.idl ${FCVQUEUE_TEST})

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(${QUEUE_TEST} ${FCVQUEUE_DSP} ${FCVQUEUE_TEST})

endif()
