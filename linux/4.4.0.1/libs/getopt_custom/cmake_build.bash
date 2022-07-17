#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash


# invoke command to construct Build Tag and Build the required variant
doBuild "$@" TEST_NAME=getopt
if [ $CLEAN_ONLY -eq  1 ];
then
    echo "Cleaning the build Directories"
    doClean
else
    cd ${_src} && \
    ${CMAKE} \
    --build ${_build} \
    --clean-first \
    --target ${BUILD_TARGET_NAME} \
    --config ${MY_BUILD_OPTION} \
    && \
    cd -
fi
