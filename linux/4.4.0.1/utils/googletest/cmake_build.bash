#!/bin/bash
#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash

# invoke command to construct Build Tag and Build the required variant
doBuild "$@" TEST_NAME=gtest
# Remove Device from Android Build name as there is no device test
BUILD_TARGET_NAME=${BUILD_TARGET_NAME/_device}
BUILD_TARGET_NAME=${BUILD_TARGET_NAME/_skel}
if [ $CLEAN_ONLY -eq  1 ];
then
    echo "Cleaning the build Directories"
    doClean
else
    cd ${_src} && \
    ${CMAKE} \
    --build \
    ${_build} \
    --target gtest gtest_main gmock gmock_main -- \
    && \
    cd -
fi
