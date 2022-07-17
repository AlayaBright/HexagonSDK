#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash

doTest() {
# Test on the Device
    adb wait-for-device
    adb root && adb remount
    ANDROID_BUILD=${1}
    HEXAGON_BUILD=${2}
    app=qprintf_example
    adb shell mkdir -p //vendor/bin
    adb push ${ANDROID_BUILD}/ship/${app} //vendor/bin
    adb shell mkdir -p //vendor/lib/rfsa/adsp/
    adb push ${HEXAGON_BUILD}/ship/libqprintf_example_skel.so //vendor/lib/rfsa/adsp/
    adb push ${HEXAGON_BUILD}/libqprintf.so //vendor/lib/rfsa/adsp/
    adb push ${HEXAGON_BUILD}/libworker_pool.so //vendor/lib/rfsa/adsp/
    adb shell "echo 0x1f > //vendor/lib/rfsa/adsp/qprintf_example.farf"
    adb shell chmod 777 //vendor/bin/${app}
    adb shell ADSP_LIBRARY_PATH="//vendor/lib/rfsa/adsp:/system/lib/rfsa/adsp:/dsp;" //vendor/bin/${app} -d 3 -U 0
    adb shell ADSP_LIBRARY_PATH="//vendor/lib/rfsa/adsp:/system/lib/rfsa/adsp:/dsp;" //vendor/bin/${app} -d 3 -U 1
}
if [ "to$1" = "totest" ];
then
    # Test on Device ( make sure that the test is already built to run on device)
    doTest $2 $3
else
    # invoke command to construct Build Tag and Build the required variant
    doBuild "$@" TEST_NAME=qprintf_example

    if [ "$?" -ne 0 ]
    then
        exit 1
    elif [ $CLEAN_ONLY -eq  1 ];
    then
        echo "Cleaning the build Directories"
        doClean worker_pool qprintf
    else
        cd ${_src} && \
          ${CMAKE} \
          --build \
          ${_build} \
          --target ${BUILD_TARGET_NAME} -- \
          && \
          cd -
    fi
fi
