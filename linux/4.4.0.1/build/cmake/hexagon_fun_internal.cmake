##############################
#
# idl_generated_files (<idlFile> <generatedFiles>)
#
# This function takes <idlFile> as input and returns list of filenames
# that will be created when this idl file is built with qaic compiler.
#
##############################

function(idl_generated_files idlFile generatedFiles)
    file(STRINGS ${idlFile} lines_with_interface REGEX "interface")

    foreach(interface ${lines_with_interface})
        STRING(REGEX MATCH "interface[ ]+[A-Za-z0-9_]+" tmp_var ${interface})
        STRING(REGEX MATCH "[ ]+[A-Za-z0-9_]+" interface_name_with_spaces ${tmp_var})
        STRING(STRIP ${interface_name_with_spaces} interface_name)

        set(${generatedFiles} ${${generatedFiles}}
            ${CMAKE_CURRENT_BINARY_DIR}/${interface_name}_skel.c
            ${CMAKE_CURRENT_BINARY_DIR}/${interface_name}_stub.c
        )
    endforeach()

    set(${generatedFiles} ${${generatedFiles}} PARENT_SCOPE)
endfunction()

##################################################
#  find_hexagon_prebuilt_folder(PARENT_PREBUILT_DIR PREBUILT_DIR)
#
#  This function will return the prebuilt folder that matches the selected
#  architecture if one exists. Otherwise, it will return the prebuilt folder
#  for the most recent architecture older than the target architecture. For
#  example, for v68, the function will return the v68 prebuilt if it is present
#  othersiwe it will fallback to the v66 prebuilt, and finally to the v65 prebuilt
#  folder if no v66 prebuilt folder exists either.
#
#  This function takes directory path that contains the prebuilt folders
#  (PARENT_PREBUILT_DIR) as input and returns the full prebuilt path that
#  contains the prebuilt libs(PREBUILT_DIR).
#################################################

function(find_hexagon_prebuilt_folder PARENT_PREBUILT_DIR PREBUILT_DIR)
    if (${DSP_VERSION} MATCHES "v68")
        set(hexagon_arch_list v68 v66 v65)
    elseif (${DSP_VERSION} MATCHES "v66")
        set(hexagon_arch_list v66 v65)
    elseif (${DSP_VERSION} MATCHES "v65")
        set(hexagon_arch_list v65)
    endif()

    string(REGEX MATCH "[a-z0-9]*_[a-z0-9]*" PREBUILT_LIB_DIR_PREFIX  ${PREBUILT_LIB_DIR})

    foreach(dsp_arch ${hexagon_arch_list})
        if (EXISTS ${PARENT_PREBUILT_DIR}/${PREBUILT_LIB_DIR_PREFIX}_${dsp_arch})
            set(${PREBUILT_DIR} ${PARENT_PREBUILT_DIR}/${PREBUILT_LIB_DIR_PREFIX}_${dsp_arch} PARENT_SCOPE)
            break()
        endif()
   endforeach()
endfunction()

#############################################################################
#  link_prebuilt_lib_from_list(target custom_library_struct return_status)
#
#  This function links the prebuilt custom_library which is obtained from
#  custom_library_struct to the target. Sets the <return_status> argument to TRUE for success .
#
#################################################################################

function(link_prebuilt_lib_from_list target custom_library_struct return_status)
    set(LIB_PREBUILT_DIR "")

    LIST(GET ${custom_library_struct} ${CUSTOM_NAME_INDEX} CUSTOM_NAME)
    LIST(GET ${custom_library_struct} ${BINARY_NAME_INDEX} BINARY_NAMES)
    LIST(GET ${custom_library_struct} ${INCS_DIR_INDEX} LIB_INCLUDE_DIR)
    LIST(GET ${custom_library_struct} ${PARENT_PREBUILT_INDEX} LIB_PREBUILT_PARENT_DIR)

    if (${CMAKE_SYSTEM_NAME} MATCHES "QURT")
        find_hexagon_prebuilt_folder(${LIB_PREBUILT_PARENT_DIR} LIB_PREBUILT_DIR)
    else()
        if (EXISTS ${LIB_PREBUILT_PARENT_DIR}/${PREBUILT_LIB_DIR})
            set(LIB_PREBUILT_DIR ${LIB_PREBUILT_PARENT_DIR}/${PREBUILT_LIB_DIR})
        endif()
    endif()

    if (LIB_PREBUILT_DIR)
        foreach(binary_name incdir IN ZIP_LISTS ${BINARY_NAMES} ${LIB_INCLUDE_DIR})
            if (DEFINED binary_name)
                STRING(RANDOM _random)
                set(LIB_TARGET ${CUSTOM_NAME}-${target}-${_random})
                if (${CMAKE_SYSTEM_NAME} MATCHES "Android" OR ${CMAKE_SYSTEM_NAME} MATCHES "QURT")
                    set(DESTINATION_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/ship)
                else()
                    set(DESTINATION_FOLDER ${CMAKE_CURRENT_BINARY_DIR})
                endif()

                STRING(FIND ${binary_name} ".${DLL_EXT}" IS_DLL)
                if (NOT ${IS_DLL} EQUAL -1)
                    add_library(${LIB_TARGET} SHARED IMPORTED)
                    if (NOT (${SYSTEM_TYPE} MATCHES "AS" OR ${CUSTOM_NAME} MATCHES "dsprpc"))
                        set(LIB_COPY_TARGET ${LIB_TARGET}-copy)
                        add_custom_command(
                            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${binary_name}
                            COMMAND cp ${LIB_PREBUILT_DIR}/${binary_name} ${DESTINATION_FOLDER}
                            DEPENDS ${LIB_PREBUILT_DIR}/${binary_name}
                        )
                        add_custom_target(${LIB_COPY_TARGET}
                            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${binary_name}
                        )
                        add_dependencies(${LIB_TARGET} ${LIB_COPY_TARGET})
                    endif()
                else()
                    add_library(${LIB_TARGET} STATIC IMPORTED)
                endif()

                if (NOT incdir)
                    set(incdir ${LIB_PREBUILT_DIR})
                endif()

                set_target_properties(${LIB_TARGET}
                    PROPERTIES
                    IMPORTED_LOCATION "${LIB_PREBUILT_DIR}/${binary_name}"
                    INTERFACE_INCLUDE_DIRECTORIES "${incdir}"
                )

                add_dependencies(${target} ${LIB_TARGET})

                if ((NOT ${IS_DLL} EQUAL -1) AND (${CMAKE_SYSTEM_NAME} MATCHES "UbuntuARM"))
                    STRING(REGEX REPLACE "^lib" "" trunc_binary_name ${binary_name})
                    STRING(REGEX REPLACE "\.so$" "" final_binary_name ${trunc_binary_name})

                    target_link_options(${target} PUBLIC -L${LIB_PREBUILT_DIR} -l${final_binary_name})
                else()
                    target_link_libraries(${target} ${LIB_TARGET})
                endif()
            else()
                target_include_directories(${target} PUBLIC ${incdir})
            endif()
        endforeach()
        set(${return_status} TRUE PARENT_SCOPE)
    endif()
endfunction()

#######################################################################
#  link_custom_library_prebuilt(target custom_library)
#
#  This function links the prebuilt custom_library to the target passed
#  as argument
######################################################################

function(link_custom_library_prebuilt target custom_library status)
    foreach(lib_struct ${CUSTOM_LIBS_STRUCTS})
        LIST(GET ${lib_struct} ${CUSTOM_NAME_INDEX} CUSTOM_NAME)
        LIST(GET ${lib_struct} ${PARENT_PREBUILT_INDEX} LIB_PARENT_PREBUILT_DIR)
        STRING(FIND ${custom_library} ${CUSTOM_NAME} IS_SUBSTRING)
        if (NOT ${IS_SUBSTRING} EQUAL -1)
            set(local_status FALSE)
            if (LIB_PARENT_PREBUILT_DIR)
                link_prebuilt_lib_from_list(${target} ${lib_struct} local_status)
            endif()
            if (${status} AND local_status)
               set(${status} TRUE PARENT_SCOPE)
            else()
               set(${status} FALSE PARENT_SCOPE)
               break()
            endif()
        endif()
    endforeach()
endfunction()

#######################################################################
#  build_lib_struct(custom_library_struct built_target built_libs_path built_libs_incs)
#
#  This function only build the custom_library present in the list returns target list,
#  libs path and libs inc in built_target,built_libs_path,built_libs_incs respectively
#
#######################################################################

function(build_lib_struct custom_lib_struct built_target built_libs_path built_libs_incs)
    LIST(GET ${custom_lib_struct} ${CUSTOM_NAME_INDEX} CUSTOM_NAME)
    LIST(GET ${custom_lib_struct} ${BINARY_NAME_INDEX} BINARY_NAMES)
    LIST(GET ${custom_lib_struct} ${INCS_DIR_INDEX} LIB_INCLUDE_DIR)
    LIST(GET ${custom_lib_struct} ${BUILD_DIR_INDEX} LIB_BUILD_DIR)

    set(COMMON_CMAKE_ARGS -DV=${V} -DOS_VER=${OS_VER} -DDSP_TYPE=${DSP_TYPE} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM} -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DPREBUILT_LIB_DIR=${PREBUILT_LIB_DIR} -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}
        -DHEXAGON_CMAKE_ROOT=${HEXAGON_CMAKE_ROOT} -DADD_SYMBOLS=${ADD_SYMBOLS} -DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}
        -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${LIB_BUILD_DIR}/${V}/ship -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${LIB_BUILD_DIR}/${V}/ship
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${LIB_BUILD_DIR}/${V}/ship -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
    )

    if (${CMAKE_SYSTEM_NAME} MATCHES "Android")
        set(ANDROID_CMAKE_ARGS -DOS_TYPE=${OS_TYPE} -DANDROID_NDK=${ANDROID_NDK} -DANDROID_ABI=${ANDROID_ABI} -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}
            -DANDROID_STL=${ANDROID_STL})
        set(CMAKE_ARGS ${COMMON_CMAKE_ARGS} ${ANDROID_CMAKE_ARGS})
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "UbuntuARM")
        set(UBUNTUARM_CMAKE_ARGS -DOS_TYPE=${OS_TYPE} -DOPEN_WRT=${OPENWRT} -DSOFT_FLOAT=${SOFT_FLOAT})
        set(CMAKE_ARGS ${COMMON_CMAKE_ARGS} ${UBUNTUARM_CMAKE_ARGS})
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "QURT")
        set(HEXAGON_CMAKE_ARGS -DDSP_VERSION=${DSP_VERSION} -DBUILD_NAME=${CUSTOM_NAME}_skel)
        set(CMAKE_ARGS ${COMMON_CMAKE_ARGS} ${HEXAGON_CMAKE_ARGS})
    endif()

    set(LIBS_PATH)
    set(LIBS_BUILD_PATH)
    foreach(binary_name ${${BINARY_NAMES}})
        STRING(FIND ${binary_name} ".${DLL_EXT}" IS_DLL)
        if (NOT ${IS_DLL} EQUAL -1)
            set(LIBS_PATH ${LIBS_PATH} ${CMAKE_CURRENT_BINARY_DIR}/${binary_name})
        else()
            set(LIBS_PATH ${LIBS_PATH} ${CMAKE_CURRENT_BINARY_DIR}/lib/${binary_name})
        endif()
        set(LIBS_BUILD_PATH ${LIBS_BUILD_PATH} ${LIB_BUILD_DIR}/${V}/${binary_name})
    endforeach()

    STRING(RANDOM _random)
    set(LIB_TARGET ${CUSTOM_NAME}-${_random})
    set(copy_targets)

    if (${SYSTEM_TYPE} MATCHES "AS")
        set(CMAKE_ARGS ${CMAKE_ARGS} -DSYSTEM_TYPE="AS")
        MESSAGE("Inside AS code value of BUILD_BYPRODUCTS:${LIBS_BUILD_PATH} BYPRODUCTS: ${LIBS_PATH}")
        ExternalProject_Add(${LIB_TARGET}
            SOURCE_DIR ${LIB_BUILD_DIR}
            STAMP_DIR ${LIB_BUILD_DIR}/${V}/
            BINARY_DIR ${LIB_BUILD_DIR}/${V}/
            CONFIGURE_COMMAND ${HEXAGON_CMAKE_ROOT}/setup_env_AS.cmd && ${CMAKE_COMMAND} ${CMAKE_ARGS} <SOURCE_DIR><SOURCE_SUBDIR>
            BUILD_BYPRODUCTS ${LIBS_BUILD_PATH}
            BYPRODUCTS ${LIBS_PATH}
            INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
        )
    else()
        ExternalProject_Add(${LIB_TARGET}
            SOURCE_DIR ${LIB_BUILD_DIR}
            STAMP_DIR ${LIB_BUILD_DIR}/${V}/
            BINARY_DIR ${LIB_BUILD_DIR}/${V}/
            INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
            CMAKE_ARGS ${CMAKE_ARGS}
        )

        if (${CMAKE_SYSTEM_NAME} MATCHES "Android" OR ${CMAKE_SYSTEM_NAME} MATCHES "QURT")
            foreach(binary_name ${${BINARY_NAMES}})
                STRING(FIND ${binary_name} ".${DLL_EXT}" IS_DLL)
                if (NOT ${IS_DLL} EQUAL -1)
                    STRING(RANDOM rand)
                    set(LIB_COPY_TARGET ${CUSTOM_NAME}-${rand}-copy)

                    add_custom_command(
                        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ship/${binary_name}
                        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/${binary_name} ${CMAKE_CURRENT_BINARY_DIR}/ship/${binary_name}
                        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${binary_name}
                    )

                    add_custom_target(${LIB_COPY_TARGET}
                        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ship/${binary_name}
                    )

                    add_dependencies(${LIB_COPY_TARGET} ${LIB_TARGET})
                    set(copy_targets ${copy_targets} ${LIB_COPY_TARGET})
                endif()
            endforeach()
        endif()
    endif()

    # Return ExternalProject_Add target
    set(${built_target} ${${built_target}} ${LIB_TARGET} ${copy_targets} PARENT_SCOPE)

    # Return full libs path to use in target_link_libraries
    set(${built_libs_path} ${${built_libs_path}} ${LIBS_PATH} PARENT_SCOPE)

    # Return Include folders needed for using these libs
    set(${built_libs_incs} ${${built_libs_incs}} ${${LIB_INCLUDE_DIR}} PARENT_SCOPE)
endfunction()

#######################################################################
#  build_link_lib_struct(target custom_library_struct)
#
#  This function build the custom_library present in the list and links
#  to the target passed as argument.
#######################################################################

function(build_link_lib_struct target custom_library_struct)
    set(LIBS_INCLUDE_DIR)
    set(LIBS_FULL_PATH)
    set(LIBS_BUILT_TARGET)

    # Build libs
    build_lib_struct(${custom_library_struct} LIBS_BUILT_TARGET LIBS_FULL_PATH LIBS_INCLUDE_DIR)

    # Link libs to target
    add_dependencies(${target} ${LIBS_BUILT_TARGET})
    target_include_directories(${target} PUBLIC ${LIBS_INCLUDE_DIR})
    target_link_libraries(${target} ${LIBS_FULL_PATH})

endfunction()

###################################################################
# link_custom_library_source(<target> <custom_library>)
#
# This function build the custom_library source and links to the
# target passed as argument.
###################################################################

function(link_custom_library_source target custom_library)
    set(found_custom_library FALSE)

    foreach(lib_struct ${CUSTOM_LIBS_STRUCTS})
        LIST(GET ${lib_struct} ${CUSTOM_NAME_INDEX} CUSTOM_NAME)
        LIST(GET ${lib_struct} ${BUILD_DIR_INDEX} LIB_BUILD_DIR)
        STRING(FIND ${custom_library} ${CUSTOM_NAME} IS_SUBSTRING)
        if (NOT ${IS_SUBSTRING} EQUAL -1)
            set(found_custom_library TRUE)

            if (${CUSTOM_NAME} IN_LIST ${CMAKE_SYSTEM_NAME}_supported_libs)
                if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
                    link_custom_library_wos(${target} ${CUSTOM_NAME})
                elseif (${CMAKE_SYSTEM_NAME} MATCHES "Qnx")
                    link_custom_library_qnx(${target} ${CUSTOM_NAME})
                elseif (LIB_BUILD_DIR)
                    build_link_lib_struct(${target} ${lib_struct})
                else()
                    MESSAGE(STATUS "Building source for library ${CUSTOM_NAME} on ${CMAKE_SYSTEM_NAME} is not supported")
                endif()
            endif()
        endif()
    endforeach()

    if (NOT found_custom_library)
        MESSAGE(FATAL_ERROR "${custom_libray} is not found in the library list. Please add ${custom_library} to the list.")
    endif()
endfunction()
