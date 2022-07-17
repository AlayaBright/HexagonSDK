#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash
doTest() {
# Test on the Device
    HEXAGON_BUILD=${1}
    ARCH=`echo ${HEXAGON_BUILD} | cut -d"_" -f 3-4`
    echo ${ARCH}
    adb wait-for-device
    adb root && adb remount
    adb shell mkdir -p //vendor/bin/
    adb push ${HEXAGON_SDK_ROOT}/libs/run_main_on_hexagon/ship/android_aarch64/run_main_on_hexagon //vendor/bin/
    adb shell chmod 777 //vendor/bin/run_main_on_hexagon
    adb shell mkdir -p //vendor/lib/rfsa/dsp/sdk/
    adb push ${HEXAGON_SDK_ROOT}/libs/run_main_on_hexagon/ship/hexagon_${ARCH}/librun_main_on_hexagon_skel.so //vendor/lib/rfsa/dsp/sdk/
    adb push ${HEXAGON_BUILD}/ship/libqhl_example.so //vendor/lib/rfsa/dsp/sdk
    adb shell "echo 0x1f > //vendor/lib/rfsa/dsp/sdk/run_main_on_hexagon.farf"
    adb shell "export LD_LIBRARY_PATH=//vendor/lib64/:$LD_LIBRARY_PATH ADSP_LIBRARY_PATH=\"//vendor/lib/rfsa/dsp/sdk;\" ; //vendor/bin/run_main_on_hexagon 3 //vendor/lib/rfsa/dsp/sdk/libqhl_example.so 1 foo 2.0 bar"
}
if [ "to$1" = "totest" ];
then
    # Test on Device ( make sure that the test is already built to run on device)
    doTest $2
else
    doBuild "$@" TEST_NAME=qhl_example
    if [ "$?" -ne 0 ]
    then
        exit 1
    elif [ $CLEAN_ONLY -eq  1 ];
    then
         echo "Cleaning the build Directories"
         doClean qhl
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
