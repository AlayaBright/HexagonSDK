include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${HEXAGON_CMAKE_ROOT}/common_helper.cmake)

enable_language(ASM)

get_filename_component(HEXAGON_SDK_ROOT "$ENV{HEXAGON_SDK_ROOT}" REALPATH)

# Include Hexagon internal functions
include(${HEXAGON_CMAKE_ROOT}/hexagon_fun_internal.cmake)

# Conditionally include add-on specific function file
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include(${HEXAGON_SDK_ROOT}/addons/wos/build/cmake/windows_fun.cmake)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Qnx")
    include(${HEXAGON_SDK_ROOT}/addons/qnx/build/cmake/qnx_fun.cmake)
endif()

# Binary Extensions
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(DLL_EXT "dll")
    set(LIB_EXT "lib")
    set(EXE_EXT "exe")
else()
    set(DLL_EXT "so")
    set(LIB_EXT "a")
    set(EXE_EXT "")
endif()

####################################
# Core part of the Hexagon SDK Tree data base for library name
# to library source and include file mapping
####################################

if(CMAKE_HOST_WIN32)
    set(hostOS WinNT)
else()
    if(OS_VER GREATER 18)
        set(hostOS Ubuntu18)
    elseif(OS_VER GREATER 16)
        set(hostOS Ubuntu16)
    endif()
endif()

message(STATUS "HEXAGON_SDK_ROOT:${HEXAGON_SDK_ROOT}")
set(rpcmem_src ${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem)
set(rpcmem_inc ${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem/inc)
set(libworker_pool_src ${HEXAGON_SDK_ROOT}/libs/worker_pool)
set(libworker_pool_inc ${HEXAGON_SDK_ROOT}/libs/worker_pool/inc)
set(atomic_src ${HEXAGON_SDK_ROOT}/libs/atomic)
set(atomic_inc)
set(remote_src ${HEXAGON_SDK_ROOT}/ipc/fastrpc/remote)
set(remote_inc ${HEXAGON_SDK_ROOT}/ipc/fastrpc/remote/ship/${PREBUILT_LIB_DIR}/)
set(adsprpc_prebuilt ${remote_src}/ship/${PREBUILT_LIB_DIR}/)
set(libqprintf_src ${HEXAGON_SDK_ROOT}/libs/qprintf)
set(libqprintf_inc
    ${HEXAGON_SDK_ROOT}/libs/qprintf/inc
    )
set(test_util_src ${HEXAGON_SDK_ROOT}/utils/sim_utils/)
set(test_util_inc)
set(proj-qaic_src ${HEXAGON_SDK_ROOT}/ipc/fastrpc/qaic/)

set(HEXAGON_ALL_INCS
    ${HEXAGON_SDK_ROOT}/incs/
    ${HEXAGON_SDK_ROOT}/incs/stddef/
    ${HEXAGON_SDK_ROOT}/libs/rpcmem/inc
    ${HEXAGON_SDK_ROOT}/libs/worker_pool/inc
    ${HEXAGON_SDK_ROOT}/ipc/fastrpc/remote/ship/${PREBUILT_LIB_DIR}/
    )

file(GLOB cpp_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src*/*.cpp)
list(LENGTH cpp_srcs cpp_srcs_size)

# This is to work around the NDK r19c C++ issues
if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    if(cpp_srcs_size)
        if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/c++/v1/")
            file(COPY ${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android/libc++_shared.so DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ship)
        endif()
        if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${ANDROID_NDK}/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1/")
            file(COPY ${ANDROID_NDK}/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/lib/aarch64-linux-android/libc++_shared.so DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ship)
        endif()
    endif()
endif()

#This is a workaround for c++ libs for hexagon

if(cpp_srcs_size AND ${CMAKE_SYSTEM_NAME} MATCHES "QURT")
    file(COPY ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++.so.1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ship)
    file(COPY ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++.so.1.0 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ship)
    file(COPY ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++abi.so.1 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ship)
    file(COPY ${HEXAGON_LIB_DIR}/${HEXAGON_ARCH}/G0/pic/libc++abi.so.1.0 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ship)

    string(FIND ${PREBUILT_LIB_DIR} "toolv81" is_toolv81)
    string(FIND ${PREBUILT_LIB_DIR} "toolv82" is_toolv82)
    string(FIND ${PREBUILT_LIB_DIR} "toolv83" is_toolv83)

    if (${is_toolv81} EQUAL -1 AND ${is_toolv82} EQUAL -1 AND ${is_toolv83} EQUAL -1)
        file(COPY ${HEXAGON_SDK_ROOT}/libs/weak_refs/ship/hexagon_toolv84/weak_refs.so DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ship)
    endif()

endif()

###########################################################
# Libs Structure
# Index   Index Variable           Value
#   0     CUSTOM_NAME_INDEX        Custom Name of your library
#   1     BINARY_NAME_INDEX        Binary Name of your library
#   2     INCS_DIR_INDEX           Header file directories of your library
#   3     PARENT_PREBUILT_INDEX    Parent directory of the folder where prebuilts are present
#   4     BUILD_DIR_INDEX          Directory where your library must be built
############################################################

set(CUSTOM_NAME_INDEX     0)
set(BINARY_NAME_INDEX     1)
set(INCS_DIR_INDEX        2)
set(PARENT_PREBUILT_INDEX 3)
set(BUILD_DIR_INDEX       4)

#Libs List
set(atomic_libs "atomic.${LIB_EXT}")
set(test_util_libs "test_util.${LIB_EXT}")
set(qprintf_libs "libqprintf.${DLL_EXT}")
set(worker_pool_libs "libworker_pool.${DLL_EXT}")
set(rtld_libs "rtld.${LIB_EXT}")
set(rpcmem_libs "rpcmem.${LIB_EXT}")
set(qhl_libs "libqhmath.${LIB_EXT}" "libqhcomplex.${LIB_EXT}" "libqhdsp.${LIB_EXT}" "libqhblas.${LIB_EXT}")
set(qhl_hvx_libs "libqhmath_hvx.${LIB_EXT}" "libqhdsp_hvx.${LIB_EXT}" "libqhblas_hvx.${LIB_EXT}")
set(googletest_libs "libgtest.${LIB_EXT}" "libgtest_main.${LIB_EXT}" "libgmock.${LIB_EXT}" "libgmock_main.${LIB_EXT}")
set(getopt_custom_libs "getopt_custom.${LIB_EXT}")
set(asyncdspq_libs "libasyncdspq.${DLL_EXT}")
set(asyncdspq_static_libs "asyncdspq.${LIB_EXT}")
set(asyncdspq_skel_libs "libasyncdspq_skel.${DLL_EXT}")
set(image_dspq_libs "image_dspq.${LIB_EXT}")
set(image_dspq_skel_libs "libimage_dspq_skel.${DLL_EXT}")

#Incs List
set(atomic_incs "${HEXAGON_SDK_ROOT}/libs/atomic/inc")
set(test_util_incs "${HEXAGON_SDK_ROOT}/utils/sim_utils/src")
set(qprintf_incs "${HEXAGON_SDK_ROOT}/libs/qprintf/inc")
set(worker_pool_incs "${HEXAGON_SDK_ROOT}/libs/worker_pool/inc")
set(rtld_incs "")
set(rpcmem_incs "${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem/inc")
set(qhl_incs
    "${HEXAGON_SDK_ROOT}/libs/qhl/inc/qhmath"
    "${HEXAGON_SDK_ROOT}/libs/qhl/inc/qhcomplex"
    "${HEXAGON_SDK_ROOT}/libs/qhl/inc/qhdsp"
    "${HEXAGON_SDK_ROOT}/libs/qhl/inc/qhblas"
)
set(qhl_hvx_incs
    "${HEXAGON_SDK_ROOT}/libs/qhl_hvx/inc/qhmath_hvx"
    "${HEXAGON_SDK_ROOT}/libs/qhl_hvx/inc/qhdsp_hvx"
    "${HEXAGON_SDK_ROOT}/libs/qhl_hvx/inc/qhblas_hvx"
    "${HEXAGON_SDK_ROOT}/libs/qhl_hvx/inc/internal"
)
set(googletest_incs
    "${HEXAGON_SDK_ROOT}/utils/googletest/gtest/include"
    "${HEXAGON_SDK_ROOT}/utils/googletest/include"
)
set(getopt_custom_incs "${HEXAGON_SDK_ROOT}/libs/getopt_custom/inc")

set(dsprpc_names "cdsprpc" "adsprpc" "mdsprpc" "sdsprpc")
set(asyncdspq_incs "${HEXAGON_SDK_ROOT}/ipc/asyncq/asyncdspq/inc")
set(image_dspq_incs "${HEXAGON_SDK_ROOT}/addons/compute/libs/image_dspq/inc")

# Supported OS
set(QURT_supported_libs "atomic" "test_util" "qprintf" "worker_pool" "rtld" "rpcmem" "qhl" "qhl_hvx" "googletest" "getopt_custom" "asyncdspq_skel" "image_dspq_skel")
set(UbuntuARM_supported_libs "rpcmem" "asyncdspq_rpc" "asyncdspq_static" "image_dspq_static" ${dsprpc_names})
set(Android_supported_libs "googletest" "rpcmem" "asyncdspq_rpc" "asyncdspq_static" "image_dspq_static" ${dsprpc_names})
set(Qnx_supported_libs "rpcmem" ${dsprpc_names})
set(Windows_supported_libs "rpcmem" "getopt_custom" ${dsprpc_names})

# Lib Structs
set(prebuilt_dsprpc_lib_structs)
foreach(lib ${dsprpc_names})
    set(${lib}_libs "lib${lib}.${DLL_EXT}")
    set(${lib}_incs "")
    set(${lib}_struct ${lib} ${lib}_libs ${lib}_incs "${HEXAGON_SDK_ROOT}/ipc/fastrpc/remote/ship" "")
    set(prebuilt_dsprpc_lib_structs ${prebuilt_dsprpc_lib_structs} ${lib}_struct)
endforeach()

set(atomic_struct           "atomic"            atomic_libs            atomic_incs        "${HEXAGON_SDK_ROOT}/libs/atomic/prebuilt"        "${HEXAGON_SDK_ROOT}/libs/atomic")
set(test_util_struct        "test_util"         test_util_libs         test_util_incs     "${HEXAGON_SDK_ROOT}/utils/sim_utils/prebuilt"    "${HEXAGON_SDK_ROOT}/utils/sim_utils")
set(qprintf_struct          "qprintf"           qprintf_libs           qprintf_incs       "${HEXAGON_SDK_ROOT}/libs/qprintf/prebuilt"       "${HEXAGON_SDK_ROOT}/libs/qprintf")
set(worker_pool_struct      "worker_pool"       worker_pool_libs       worker_pool_incs   "${HEXAGON_SDK_ROOT}/libs/worker_pool/prebuilt"   "${HEXAGON_SDK_ROOT}/libs/worker_pool")
set(rtld_struct             "rtld"              rtld_libs              rtld_incs          "${HEXAGON_SDK_ROOT}/ipc/fastrpc/rtld/ship"       "")
set(rpcmem_struct           "rpcmem"            rpcmem_libs            rpcmem_incs        "${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem/prebuilt" "${HEXAGON_SDK_ROOT}/ipc/fastrpc/rpcmem")
set(qhl_struct              "qhl"               qhl_libs               qhl_incs           "${HEXAGON_SDK_ROOT}/libs/qhl/prebuilt"           "${HEXAGON_SDK_ROOT}/libs/qhl")
set(qhl_hvx_struct          "qhl_hvx"           qhl_hvx_libs           qhl_hvx_incs       "${HEXAGON_SDK_ROOT}/libs/qhl_hvx/prebuilt"       "${HEXAGON_SDK_ROOT}/libs/qhl_hvx")
set(google_test_struct      "googletest"        googletest_libs        googletest_incs    ""                                                "${HEXAGON_SDK_ROOT}/utils/googletest")
set(getopt_custom_struct    "getopt_custom"     getopt_custom_libs     getopt_custom_incs ""                                                "${HEXAGON_SDK_ROOT}/libs/getopt_custom")
set(asyncdspq_struct        "asyncdspq_rpc"     asyncdspq_libs         asyncdspq_incs     ""                                                "${HEXAGON_SDK_ROOT}/ipc/asyncq/asyncdspq")
set(asyncdspq_static_struct "asyncdspq_static"  asyncdspq_static_libs  asyncdspq_incs     ""                                                "${HEXAGON_SDK_ROOT}/ipc/asyncq/asyncdspq")
set(asyncdspq_skel_struct   "asyncdspq_skel"    asyncdspq_skel_libs    asyncdspq_incs     ""                                                "${HEXAGON_SDK_ROOT}/ipc/asyncq/asyncdspq")
set(image_dspq_struct       "image_dspq_static" image_dspq_libs        image_dspq_incs    ""                                                "${HEXAGON_SDK_ROOT}/addons/compute/libs/image_dspq")
set(image_dspq_skel_struct  "image_dspq_skel"   image_dspq_skel_libs   image_dspq_incs    ""                                                "${HEXAGON_SDK_ROOT}/addons/compute/libs/image_dspq")

set(CUSTOM_LIBS_STRUCTS
    ${prebuilt_dsprpc_lib_structs}
    rpcmem_struct
    atomic_struct
    test_util_struct
    rtld_struct
    qprintf_struct
    worker_pool_struct
    qhl_hvx_struct
    qhl_struct
    google_test_struct
    getopt_custom_struct
    asyncdspq_struct
    asyncdspq_static_struct
    asyncdspq_skel_struct
    image_dspq_struct
    image_dspq_skel_struct
)

##############################
#
# build_idl (<idlFile> <currentTarget>)
#
# This function will set up a custom_target to build <idlFile> using qaic
# IDL compiler. For foo.idl, it wll generate foo.h, foo_stub.c and
# foo_skel.c into ${CMAKE_CURRENT_BINARY_DIR} diretory.
#
# This function will also add the custom_target created as the dependency
# of <currentTarget>
#
##############################

function(build_idl idlFile currentTarget)
    message(STATUS "BUILD IDL FILE ${idlFile} for ${currentTarget}")
    string(RANDOM _random)

    set(QIDLINC "")
    set(defaultIncs
        ${HEXAGON_SDK_ROOT}/incs/
        ${HEXAGON_SDK_ROOT}/incs/stddef/
        )
    foreach(path ${defaultIncs})
        set(QIDLINC ${QIDLINC} "-I${path}")
    endforeach(path)

    get_target_property(dirs ${currentTarget} INCLUDE_DIRECTORIES)
    foreach(path ${dirs})
        set(QIDLINC ${QIDLINC} "-I${path}")
    endforeach(path)

    get_filename_component(fileName ${idlFile} NAME_WE)

    set(cmdLineOptions -mdll -o ${CMAKE_CURRENT_BINARY_DIR} ${QIDLINC} ${idlFile})

    idl_generated_files(${idlFile} byproduct_files)

    set(qidlTarget qidlTarget${fileName}${_random})

    add_custom_command(
        OUTPUT ${byproduct_files} ${CMAKE_CURRENT_BINARY_DIR}/${fileName}.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        COMMAND "${proj-qaic_src}/${hostOS}/qaic" ${cmdLineOptions}
        DEPENDS ${idlFile}
    )

    add_custom_target( ${qidlTarget}
        DEPENDS ${byproduct_files} ${CMAKE_CURRENT_BINARY_DIR}/${fileName}.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    )

    message(STATUS
        "BYPRODUCTS:${byproduct_files} ${CMAKE_CURRENT_BINARY_DIR}/${fileName}.h")

    add_dependencies(${currentTarget} ${qidlTarget})

    set_source_files_properties(
        ${byproduct_files}
        ${CMAKE_CURRENT_BINARY_DIR}/${fileName}.h
        PROPERTIES
        GENERATED TRUE
        )
endfunction()

############################################################
#### buildIDL alias to build_idl for backward compatibility
############################################################

function(buildIDL idlFile currentTarget)
    build_idl(${idlFile} ${currentTarget})
endfunction()

##############################
#
# link_options (<TARGET>)
#
# This function is used to set up the Arch-specific
#  linker flags
#
##############################

function(link_options target)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
        target_compile_definitions(${target} PUBLIC __LA_FLAG)
        target_link_options(${target} PUBLIC -llog)
    endif()
endfunction()

##############################
#
# choose_dsprpc (<DOMAIN> <DSPRPC>)
#
# This function takes DOMAIN as argument and return the
# corresponding dsprpc library name in DSPRPC argument.
#
# Valid values of DOMAIN are 0-3.
#
##############################

function(choose_dsprpc DOMAIN dsprpc)
    if(DOMAIN MATCHES "0")
        set(${dsprpc} "adsprpc" PARENT_SCOPE)
    elseif(DOMAIN MATCHES "1")
        set(${dsprpc} "mdsprpc" PARENT_SCOPE)
    elseif(DOMAIN MATCHES "2")
        set(${dsprpc} "sdsprpc" PARENT_SCOPE)
    else() # Domain is 3
        set(${dsprpc} "cdsprpc" PARENT_SCOPE)
    endif()
endfunction()

#############################################################################################################
#
# build_custom_library (<custom_library> <built_libs_target> <built_libs_path> <built_libs_incs>)
#
# This function builds one of the SDK custom libraries and return as variables
# all the information needed to configure this library as a dependency
# * the library CMake dependencies
# * the library binary locations
# * the library include directories
#
# The code snippet below shows how to build the rpcmem library and link it to my_exec.
#
# build_custom_library(rpcmem rpcmem_targets rpcmem_libs rpcmem_incs)
# link_built_library(my_exec ${rpcmem_targets} ${rpcmem_libs} ${rpcmem_incs})
#
# When only one binary depends on a library, it is preferable to build and link
# in one command as follows:
#
# link_custom_library(<binary> <custom_library>)
#
############################################################################

function(build_custom_library custom_library built_libs_target built_libs_path built_libs_incs)
    set(found_custom_library FALSE)

    foreach(lib_struct ${CUSTOM_LIBS_STRUCTS})
        LIST(GET ${lib_struct} ${CUSTOM_NAME_INDEX} CUSTOM_NAME)
        LIST(GET ${lib_struct} ${BUILD_DIR_INDEX} LIB_BUILD_DIR)
        STRING(FIND ${custom_library} ${CUSTOM_NAME} IS_SUBSTRING)
        if (NOT ${IS_SUBSTRING} EQUAL -1)
            set(found_custom_library TRUE)

            if (${CUSTOM_NAME} IN_LIST ${CMAKE_SYSTEM_NAME}_supported_libs)
                if ((${CMAKE_SYSTEM_NAME} MATCHES "Windows") OR (${CMAKE_SYSTEM_NAME} MATCHES "Qnx"))
                    MESSAGE(STATUS "build_custom_library is not supported on ${CMAKE_SYSTEM_NAME}")
                elseif (LIB_BUILD_DIR)
                    build_lib_struct(${lib_struct} ${built_libs_target} ${built_libs_path} ${built_libs_incs})
                    set(${built_libs_target} ${${built_libs_target}} PARENT_SCOPE)
                    set(${built_libs_path} ${${built_libs_path}} PARENT_SCOPE)
                    set(${built_libs_incs} ${${built_libs_incs}} PARENT_SCOPE)
                else()
                    MESSAGE(STATUS "Building source for library ${CUSTOM_NAME} on ${CMAKE_SYSTEM_NAME} is not supported")
                endif()
            endif()

        endif()
    endforeach()

    if (NOT found_custom_library)
        MESSAGE(FATAL_ERROR "${custom_libray} is not found a known SDK library.")
    endif()

endfunction()

###########################################################################
#
# link_built_library(target built_libs_target built_libs_path built_libs_incs)
#
# This function links the library built using build_custom_library() to the target
# passed as argument. It also takes the variables returned from build_custom_libaray()
# as arguments i.e, library dependencies, library binary locations and library include
# directories.
#
##########################################################################

function(link_built_library target built_libs_target built_libs_path built_libs_incs)
    add_dependencies(${target} ${built_libs_target})
    target_link_libraries(${target} ${built_libs_path})
    target_include_directories(${target} PUBLIC ${built_libs_incs})
endfunction()

###########################################################################
#
# link_custom_library (<TARGET> <CUSTOM_LIBRARY> <OPTIONAL: BUILD_SOURCE>)
#
# This function links the custom_library to the target. If the prebuilt folder
# exists for the library it will link the prebuilts or else builds the sources
# and links the libraries generated. There is an optional BUILD_SOURCE argument
# to link the library built from source even though prebuilts folder is present.
#
############################################################################

function(link_custom_library target custom_library)
    set(prebuilt_status TRUE)
    set(BUILD_SOURCE_ONLY FALSE)

    foreach(optional_arg ${ARGN})
        if(${optional_arg} MATCHES "BUILD_SOURCE")
             set(BUILD_SOURCE_ONLY TRUE)
        endif()
    endforeach()

    if (NOT BUILD_SOURCE_ONLY)
        link_custom_library_prebuilt(${target} ${custom_library} prebuilt_status)
    endif()

    if(BUILD_SOURCE_ONLY OR (NOT prebuilt_status))
        MESSAGE(STATUS "Building ${custom_library} library from source")
        link_custom_library_source(${target} ${custom_library})
    endif()
endfunction()

##############################
#
# build_maked_project (<TARGET> <PLATFORM> <SRC> <BUILD> <ARCH>)
#
# This function builds a maked project
#
##############################

function(build_maked_project target platform src_path build_type dsp_arch)
    add_custom_target( ${target}_skel ALL
        WORKING_DIRECTORY ${src_path}
        COMMAND make ${platform} BUILD=${build_type} DSP_ARCH=${dsp_arch}
        BYPRODUCTS ${src_path}/${BUILD_NAME}/ship/lib${target}_skel.so
    )
    add_library( ${target}-lib
        SHARED
        IMPORTED
    )
    set_target_properties( # Specifies the target library.
        ${target}-lib
        PROPERTIES IMPORTED_LOCATION
        ${src_path}/${BUILD_NAME}/ship/lib${target}_skel.so
    )
endfunction()

##############################
#
# prepare_libraries_hexagon (<hexagon_targets> <hexagon_incs>  <hexagon_libs>
#   libName [libNames...])
#
# The first 3 arguments will be the output arguments.  And the
# following arguments will be the library names.  Without surfix, it is
# treated as a dynamic library. surfixed with ".a" will be processed as
# static library. And surffixed with ".so" will be processed as dynamic
# library.
#
# This function will do the following:
#
# (1) For all libraries that specified, it will search Hexagon SDK tree
# to find the corresponding library, and add a target into the
# <hexagon_targets> list.  The custom_target will specify what to do
# for that target.  It can be going into that corresponding directory to
# build the directory or do nothing if it's prebuilt library. Caller of
# this function can add this target as the dependency of their own
# target or multiple targets
#
# (2) This call will add the "library specific" include directories
# into <hexagon_incs> list. Caller of the function can add this include
# path list into their corresponding include list
#
# (3) This library call will also return a "library-target" list
# <hexagon_libs> so that it can be added into the linker dependency
# list from target_link_libraries call
#
##############################

function(prepare_libraries_hexagon hexagonTarget hexagonIncs hexagonLibs hexagonSharedLibs)
    add_custom_target(${hexagonTarget}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
        COMMAND echo "dump allHexTargets"
        BYPRODUCTS
        )
    set(allHexLibs)
    set(libsSharedList "")

    # When setting up the dependent tree, we don't want to set up current
    # target depending on all others in the first level tree. Rather we want
    # to set up the them depend on each other sequencially. The reason we
    # want to this is when compiling in mutli-process way, we want to
    # dependent modules to be built one after the other, because otherwise
    # it might be conflicting with complicated pakman dependent tree
    set(lastTarget ${hexagonTarget})

    foreach(libNameUnstripped ${ARGN})
        set(FIND_LIBRARY_HEXAGON_STATIC FALSE)

        # For prebuild library, the name and include file are with
        # different convention

        if(libNameUnstripped MATCHES "adsprpc")
            set(allHexLibs ${allHexLibs} -L${adsprpc_prebuilt} libadsprpc.so)
            set(HEXAGON_ALL_INCS ${HEXAGON_ALL_INCS} ${remote_inc})
            continue()
        endif()

        if(${libNameUnstripped} MATCHES "\.a$") #ends with .a
            STRING(REGEX REPLACE "(.*)\.a" "\\1"
                libName
                ${libNameUnstripped})
            set(FIND_LIBRARY_HEXAGON_STATIC TRUE)
        elseif() #ends with .so
            STRING( REGEX MATCH "(.*)\.so"
                libName
                ${libNameUnstripped})
        else() #dynamic by default
            set(libName
                ${libNameUnstripped})
        endif()
        message(STATUS "INC for libName:${libName}")

        set(libSrc ${${libName}_src})
        message(STATUS "INC for libSrc:${libSrc}")
        if(FIND_LIBRARY_HEXAGON_STATIC)
            if(DEEP_CLEAN)
                add_custom_target( qLibTarget${libName}
                    WORKING_DIRECTORY ${libSrc}/
                    COMMAND make tree_clean V=${V} VERBOSE=1 && make tree V=${V} VERBOSE=1
                    BYPRODUCTS
                    ${libSrc}/${V}/ship/${libName}.a
                    )
            else()
                add_custom_target( qLibTarget${libName}
                    WORKING_DIRECTORY ${libSrc}/
                    COMMAND make tree V=${V} VERBOSE=1
                    BYPRODUCTS
                    ${libSrc}/${V}/ship/${libName}.a
                    )
            endif()
            message(STATUS
                "BYPRODUCTS:${libSrc}/${V}/ship/${libName}.a")

            add_library( ${libName}-lib
                STATIC
                IMPORTED )
            set_target_properties( # Specifies the target library.
                ${libName}-lib
                PROPERTIES IMPORTED_LOCATION
                ${libSrc}/${V}/ship/${libName}.a
                )
        else()
            if(DEEP_CLEAN)
                add_custom_target( qLibTarget${libName}
                    WORKING_DIRECTORY ${libSrc}/
                    COMMAND make tree_clean V=${V} VERBOSE=1 && make tree V=${V} VERBOSE=1 && cp ${libSrc}/${V}/ship/${libName}.so ${CMAKE_CURRENT_BINARY_DIR}
                    BYPRODUCTS  ${libSrc}/${V}/ship/${libName}.so
                    )
            else()
                add_custom_target( qLibTarget${libName}
                    WORKING_DIRECTORY ${libSrc}/
                    COMMAND make tree V=${V} VERBOSE=1 && cp ${libSrc}/${V}/ship/${libName}.so ${CMAKE_CURRENT_BINARY_DIR}
                    BYPRODUCTS  ${libSrc}/${V}/ship/${libName}.so
                    )
            endif()
            message(STATUS
                "BYPRODUCTS:${libSrc}/${V}/ship/${libName}.so")
            list(APPEND libsSharedList "${libSrc}/${V}/ship/${libName}.so")

            add_library( ${libName}-lib
                SHARED
                IMPORTED )
            set_target_properties( # Specifies the target library.
                ${libName}-lib
                PROPERTIES IMPORTED_LOCATION
                ${libSrc}/${V}/ship/${libName}.so
                )
        endif()
        set(HEXAGON_ALL_INCS ${HEXAGON_ALL_INCS} ${${libName}_inc})
        add_dependencies(${lastTarget} qLibTarget${libName} )
        message(STATUS "${lastTarget} depending on qLibTarget${libName}")
        set(lastTarget qLibTarget${libName} )
        message(STATUS "lastTarget:${lastTarget}" )
        set(allHexLibs ${allHexLibs} ${libName}-lib)

    endforeach(libNameUnstripped)

    set(${hexagonTarget} ${hexagonTarget} PARENT_SCOPE)
    set(${hexagonLibs} ${allHexLibs} PARENT_SCOPE)
    set(${hexagonIncs} ${HEXAGON_ALL_INCS} PARENT_SCOPE)
    set(${hexagonSharedLibs} ${libsSharedList} PARENT_SCOPE)
    message(STATUS "hexagonTarget:${hexagonTarget}")
    message(STATUS "hexagonLibs:${${hexagonLibs}}")
    message(STATUS "hexagonIncs:${${hexagonIncs}}")
endfunction()

###############################################
# copy_binaries(currentTarget <optional:targets>)
#
# For Cmake build system, the output directory is ${CMAKE_CURRENT_BINARY_DIR}/ship.
# This function helps to copy the binaries from ${CMAKE_CURRENT_BINARY_DIR}/ship to
# ${CMAKE_CURRENT_BINARY_DIR} once <target> is built. You can pass multiple targets
# and copy will happen after each target built is done.
########################################

function(copy_binaries currentTarget)
    set(target_list ${currentTarget} ${ARGN})
    MESSAGE(STATUS "value of target_list: ${target_list}")
    foreach(target ${target_list})
        MESSAGE(STATUS "Copying for target: ${target}")
        if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
            add_custom_command(
                TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/ship
                COMMAND cp -rf ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/* ${CMAKE_CURRENT_BINARY_DIR}/ship
            )
        else()
            add_custom_command(
                TARGET ${target} POST_BUILD
                COMMAND cp -rf ${CMAKE_CURRENT_BINARY_DIR}/ship/* ${CMAKE_CURRENT_BINARY_DIR}
            )

            if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
                set(redirect_command 2>/dev/null)
            else()
                set(redirect_command 2>nul)
            endif()
            if(${CMAKE_SYSTEM_NAME} MATCHES "UbuntuARM")
                add_custom_command(
                    TARGET ${target} POST_BUILD
                    COMMAND cp -f ${CMAKE_CURRENT_BINARY_DIR}/*.so ${CMAKE_CURRENT_BINARY_DIR}/ship ${redirect_command} || :
                )
            endif()
        endif()
    endforeach()
endfunction()
