#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash

if [ "to$1" = "totest" ];
then
    # Test on Device ( make sure that the test is already built to run on device)
    echo "Test option is not supported"
else
    doBuild "$@" TEST_NAME=worker_pool
    if [ "$?" -ne 0 ]
    then
        exit 1
    elif [ $CLEAN_ONLY -eq  1 ];
    then
         echo "Cleaning the build Directories"
         doClean
    else
         cd ${_src} && \
           ${CMAKE} \
           --build \
           ${_build} \
           --config ${MY_BUILD_OPTION} \
         && \
         cd -
    fi
fi
