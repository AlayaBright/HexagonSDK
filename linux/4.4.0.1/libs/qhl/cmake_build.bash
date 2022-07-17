#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash

# invoke command to construct Build Tag and Build the required variant
doBuild "$@" TEST_NAME=qhl
if [ $CLEAN_ONLY -eq  1 ];
then
    echo "Cleaning the build Directories"
    doClean
else
    cd ${_src} && \
    ${CMAKE} \
    --build \
    ${_build} \
    --target qhmath qhcomplex qhblas qhdsp -- \
    && \
    cd -
fi
