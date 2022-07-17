#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash

# invoke command to construct Build Tag and Build the required variant
doBuild "$@" TEST_NAME=rpcmem
# Remove Device from Android Build name as there is no device test
BUILD_TARGET_NAME=${BUILD_TARGET_NAME/_device}
if [ $CLEAN_ONLY -eq  1 ];
then
    echo "Cleaning the build Directories"
    doClean sim_utils
else
    cd ${_src} && \
      ${CMAKE} \
      --build ${_build} \
      --target ${BUILD_TARGET_NAME} \
      --config ${MY_BUILD_OPTION} -- \
      && \
      cd -
fi
