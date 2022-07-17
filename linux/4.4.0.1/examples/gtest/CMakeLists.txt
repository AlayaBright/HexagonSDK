cmake_minimum_required(VERSION 3.14.3)

project(gtest C CXX ASM)

enable_language(ASM)

set(CMAKE_DEBUG_TARGET_PROPERTIES
    INCLUDE_DIRECTORIES
    COMPILE_DEFINITIONS
    POSITION_INDEPENDENT_CODE
    CONTAINER_SIZE_REQUIRED
    LIB_VERSION
)

include(${HEXAGON_CMAKE_ROOT}/hexagon_fun.cmake)

set(common_incs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_BINARY_DIR}/
    ${HEXAGON_SDK_ROOT}/incs/
    ${HEXAGON_SDK_ROOT}/incs/stddef/
    ${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem/inc/
    )

if (${OS_TYPE} MATCHES "HLOS")

    add_executable(gtest_device
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest
    )

    set_target_properties(gtest_device PROPERTIES OUTPUT_NAME "gtest")

    set(ignoreMe ${DSP_TYPE})

    add_library(gtest
        SHARED
        ${CMAKE_CURRENT_BINARY_DIR}/gtest_stub
    )
    build_idl(inc/gtest.idl gtest)
    SET(CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY} -lc++")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++")

    include_directories(
        ${common_incs}
    )
    link_custom_library(gtest_device googletest)

    choose_dsprpc(${DSP_TYPE} dsprpc)
    link_options(gtest_device)
    link_custom_library(gtest_device ${dsprpc})
    link_custom_library(gtest ${dsprpc})
    add_dependencies(gtest_device gtest)
    target_link_libraries(gtest_device gtest)

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(gtest_device)

else()
    #################################################
    #Hexagon Dynamic Library
    #################################################
    add_library(gtest_skel SHARED
        ${CMAKE_CURRENT_BINARY_DIR}/gtest_skel
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest_imp
    )
    set(incs
        ${CMAKE_CURRENT_SOURCE_DIR}/src/
    )
    include_directories(
        ${common_incs}
        ${incs}
    )

    link_custom_library(gtest_skel googletest)
    target_link_libraries(gtest_skel ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++abi.so.1)
    target_link_libraries(gtest_skel ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++.so.1)

    build_idl(inc/gtest.idl gtest_skel incs)

    #With Hexagon Tools 8.4.0x, new symbols are added for c++17 compliance,
    #whose definitions are not present on targets earlier to saipan.
    #Link this weak_refs to your library for weak definitions of these new symbols.
    string(FIND ${PREBUILT_LIB_DIR} "toolv81" is_toolv81)
    string(FIND ${PREBUILT_LIB_DIR} "toolv82" is_toolv82)
    string(FIND ${PREBUILT_LIB_DIR} "toolv83" is_toolv83)

    if (${is_toolv81} EQUAL -1 AND ${is_toolv82} EQUAL -1 AND ${is_toolv83} EQUAL -1)
        target_link_libraries(gtest_skel ${HEXAGON_SDK_ROOT}/libs/weak_refs/ship/hexagon_toolv84/weak_refs.a)
    endif()

    add_library(gtest_q SHARED
        ${CMAKE_CURRENT_BINARY_DIR}/gtest_skel
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gtest_imp
    )

    if(${DSP_VERSION} MATCHES "v66" OR ${DSP_VERSION} MATCHES "v65")
        target_compile_options(gtest_q  PRIVATE -D__V_DYNAMIC__)
    else()
        target_compile_options(gtest_q  PRIVATE -DRUN_ON_V68 -D__V_DYNAMIC__)
    endif()
    target_link_libraries( gtest_q  ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++abi.so.1)
    target_link_libraries( gtest_q  ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++.so.1 )


    file(COPY ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++abi.so.1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++abi.so.1.0 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++.so.1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++.so.1.0 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

    link_custom_library(gtest_q googletest)

    add_dependencies(gtest_skel gtest_q)

    # Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
    copy_binaries(gtest_q gtest_skel)

    set(HEXAGON_EXEC gtest_q )
    set(HEXAGON_EXEC_CMD_OPTIONS ${HEXAGON_EXEC_CMD_OPTIONS})

    runMainOnHexagonSim(gtest_q )

endif()
