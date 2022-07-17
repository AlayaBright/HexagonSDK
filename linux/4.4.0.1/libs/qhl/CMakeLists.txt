#################################################
# Qualcomm Hexagon Math Library
#################################################

# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.
cmake_minimum_required(VERSION 3.16.5)

project(qhl VERSION 1.0 DESCRIPTION "Qualcomm Hexagon Libraries" LANGUAGES ASM C)

enable_language(ASM)

include(${HEXAGON_CMAKE_ROOT}/hexagon_fun.cmake)
option(QHL_STATIC_LIB "Build static library" ON)

add_compile_options(-G0 -Wall -Werror)

if(QHL_STATIC_LIB)
	set(CMAKE_C_CREATE_STATIC_LIBRARY
	  "${CMAKE_AR} -rcs <TARGET> <LINK_FLAGS> <OBJECTS> "
	  "<CMAKE_RANLIB> <TARGET> ")
	set(CMAKE_ASM_CREATE_STATIC_LIBRARY ${CMAKE_C_CREATE_STATIC_LIBRARY})
else()
	set(CMAKE_C_CREATE_SHARED_LIBRARY
  "${CMAKE_C_COMPILER} <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <SONAME_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")
	set(CMAKE_ASM_CREATE_SHARED_LIBRARY ${CMAKE_C_CREATE_SHARED_LIBRARY})

	# Set PIC Compile Flag
	add_compile_options(-fPIC)
endif()

set(incs
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/qhmath
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/qhcomplex
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/qhblas
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/qhdsp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/internal
    )

include_directories(
    ${incs}
)

set(QHL_LIBS
	qhmath
	qhcomplex
	qhblas
	qhdsp
   )

file(GLOB qhmath_SRCS src/qhmath/*.S src/qhmath/*.c)
file(GLOB qhcomplex_SRCS src/qhcomplex/*.S src/qhcomplex/*.c)
file(GLOB qhblas_SRCS src/qhblas/*.S src/qhblas/*.c)
file(GLOB qhdsp_SRCS src/qhdsp/decimation/*.c src/qhdsp/fft/*.S src/qhdsp/fft/*.c src/qhdsp/filters/*.S src/qhdsp/interpolation/*.c)

# Build libraries
# For each lib in the list...
foreach( lib ${QHL_LIBS} )
	# declare the target...
	if(QHL_STATIC_LIB)
		add_library( ${lib} STATIC ${${lib}_SRCS} )
	else()
		add_library( ${lib} SHARED ${${lib}_SRCS} )
		target_link_libraries( ${lib} PUBLIC -shared -lhexagon)
	endif()
endforeach()

# Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
copy_binaries(${QHL_LIBS})

install(TARGETS ${QHL_LIBS} DESTINATION ./lib EXPORT qhl-targets)
install(EXPORT qhl-targets DESTINATION ./lib/libqhl)
# vim: set noet fenc=utf-8 ff=unix ft=cmake :
