#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash

if [ "to$1" = "totest" ];
then
    echo "Test option is not supported"
else
    # invoke command to construct Build Tag and Build the required variant
    doBuild "$@" TEST_NAME=asyncdspq

    if [ "$?" -ne 0 ]
    then
        exit 1
    elif [ $CLEAN_ONLY -eq  1 ];
    then
        echo "Cleaning the build Directories"
        doClean rpcmem atomic sim_utils
    else
        cd ${_src} && \
          ${CMAKE} \
          --build \
          ${_build} \
          --target all \
          --config ${MY_BUILD_OPTION} \
          && \
          cd -
    fi
fi
