#################################################
# Qualcomm Hexagon Math Library
#################################################

# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.
cmake_minimum_required(VERSION 3.16.5)

project(qhl_hvx VERSION 1.0 DESCRIPTION "Qualcomm Hexagon Libraries for HVX" LANGUAGES C)

enable_language(C)

include(${HEXAGON_CMAKE_ROOT}/hexagon_fun.cmake)

option(QHL_HVX_STATIC_LIB "Build static library" ON)

add_compile_options(-G0 -Wall -Werror)

if(QHL_HVX_STATIC_LIB)
	set(CMAKE_C_CREATE_STATIC_LIBRARY
	  "${CMAKE_AR} -rcs <TARGET> <LINK_FLAGS> <OBJECTS> "
	  "<CMAKE_RANLIB> <TARGET> ")
	  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
else()
	set(CMAKE_C_CREATE_SHARED_LIBRARY
  "${CMAKE_C_COMPILER} <LANGUAGE_COMPILE_FLAGS> <LINK_FLAGS> <SONAME_FLAG><TARGET_SONAME> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")

	# Set PIC Compile Flag
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(incs
    ${CMAKE_CURRENT_SOURCE_DIR}/../qhl/inc/qhmath
	${CMAKE_CURRENT_SOURCE_DIR}/../qhl/inc/qhcomplex
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/qhmath_hvx
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/qhblas_hvx
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/qhdsp_hvx
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/internal
    )

include_directories(
    ${incs}
)

set(QHL_LIBS
	qhmath_hvx
	qhblas_hvx
	qhdsp_hvx
   )

if(${HEXAGON_ARCH} MATCHES v68)
	file(GLOB qhmath_hvx_SRCS src/qhmath_hvx/*.c)
	file(GLOB qhblas_hvx_SRCS src/qhblas_hvx/*.c)
	file(GLOB qhdsp_hvx_SRCS src/qhdsp_hvx/image_processing/*.c  src/qhdsp_hvx/lms/*.c src/qhdsp_hvx/fft/*.c src/qhdsp_hvx/filters/*.c)
else()
	file(GLOB qhmath_hvx_SRCS src/qhmath_hvx/*.c)
	list(FILTER qhmath_hvx_SRCS EXCLUDE REGEX "_vf.c$")
	list(FILTER qhmath_hvx_SRCS EXCLUDE REGEX "_vhf.c$")
	list(FILTER qhmath_hvx_SRCS EXCLUDE REGEX "_vsf.c$")
	file(GLOB qhblas_hvx_SRCS src/qhblas_hvx/*.c)
	list(FILTER qhblas_hvx_SRCS EXCLUDE REGEX "_vf.c$")
	list(FILTER qhblas_hvx_SRCS EXCLUDE REGEX "_vhf.c$")
	file(GLOB qhdsp_hvx_SRCS src/qhdsp_hvx/image_processing/*.c  src/qhdsp_hvx/lms/*.c src/qhdsp_hvx/fft/*.c src/qhdsp_hvx/filters/*.c)
	list(FILTER qhdsp_hvx_SRCS EXCLUDE REGEX "_vf.c$")
	list(FILTER qhdsp_hvx_SRCS EXCLUDE REGEX "_vhf.c$")
endif()

# Build libraries
# For each lib in the list...
foreach( lib ${QHL_LIBS} )
	# declare the target...
	if(QHL_HVX_STATIC_LIB)
		add_library( ${lib} STATIC ${${lib}_SRCS} )
	else()
		add_library( ${lib} SHARED ${${lib}_SRCS} )
		target_link_libraries( ${lib} PUBLIC -shared -lhexagon)
	endif()
endforeach()

# Copy binaries from ship to CMAKE_CURRENT_BINARY_DIR folder
copy_binaries(${QHL_LIBS})

install(TARGETS ${QHL_LIBS} DESTINATION ./lib EXPORT qhl_hvx-targets)
install(EXPORT qhl_hvx-targets DESTINATION ./lib/libqhl_hvx)
# vim: set noet fenc=utf-8 ff=unix ft=cmake :
