#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash

doTest() {

    adb wait-for-device
    adb root && adb remount
    ANDROID_BUILD=${1}
    HEXAGON_BUILD=${2}
    app=multithreading

    # ANDROID LIBS
    adb shell mkdir -p //vendor/bin
    adb push ${ANDROID_BUILD}/ship/${app} //vendor/bin
    adb shell chmod 777 //vendor/bin/${app}
    adb push ${ANDROID_BUILD}/ship/libmultithreading.so //vendor/lib64/

    # HEXAGON LIBS
    adb shell mkdir -p //vendor/lib/rfsa/adsp/
    adb push ${HEXAGON_BUILD}/ship/libmultithreading_skel.so //vendor/lib/rfsa/adsp/

    adb shell "echo 0x1f > //vendor/lib/rfsa/adsp/multithreading.farf"

    # Run the executable
    adb shell //vendor/bin/${app} -d 3
}

if [ "to$1" = "totest" ];
then
    # Test on Device ( make sure that the test is already built to run on device)
    doTest $2 $3
else
    # invoke command to construct Build Tag and Build the required variant
    doBuild "$@" TEST_NAME=multithreading
    if [ "$?" -ne 0 ]; then
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
          --target ${BUILD_TARGET_NAME} \
          --config ${MY_BUILD_OPTION} \
          && \
          cd -
    fi
fi
