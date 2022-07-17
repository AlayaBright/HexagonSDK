#!/bin/bash
#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash

# invoke command to construct Build Tag and Build the required variant
doBuild "$@" TEST_NAME=qhl_hvx
if [ $CLEAN_ONLY -eq  1 ];
then
    echo "Cleaning the build Directories"
    doClean
else
    cd ${_src} && \
    ${CMAKE} \
    --build \
    ${_build} \
    --target qhmath_hvx qhblas_hvx qhdsp_hvx -- \
    && \
    cd -
fi
