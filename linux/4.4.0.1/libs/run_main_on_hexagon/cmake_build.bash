#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash

doTest() {

    adb wait-for-device
    adb root && adb remount
    ANDROID_BUILD=${1}
    HEXAGON_BUILD=${2}
    app=run_main_on_hexagon_device

    # ANDROID LIBS
    adb shell mkdir -p //vendor/bin
    adb push ${ANDROID_BUILD}/ship/${app} //vendor/bin
    adb shell chmod 777 //vendor/bin/${app}


    # HEXAGON LIBS
    adb shell mkdir -p //vendor/lib/rfsa/adsp/
    adb push ${HEXAGON_BUILD}/ship/librun_main_on_hexagon_skel.so //vendor/lib/rfsa/adsp/
    adb push ${HEXAGON_BUILD}/ship/libtest_main.so //vendor/lib/rfsa/adsp/

    adb shell "echo 0x1f > //vendor/lib/rfsa/adsp/run_main_on_hexagon_device.farf"

    # Run the executable
    adb shell //vendor/bin/${app} 3 libtest_main.so 1 foo2 2 bar
}

if [ "to$1" = "totest" ];
then
    # Test on Device ( make sure that the test is already built to run on device)
    doTest $2 $3
else
    # invoke command to construct Build Tag and Build the required variant
    doBuild "$@" TEST_NAME=run_main_on_hexagon
    if [ "$?" -ne 0 ]; then
        exit 1
    elif [ $CLEAN_ONLY -eq  1 ];
    then
        echo "Cleaning the build Directories"
        doClean rpcmem atomic sim_util
    else
        cd ${_src} && \
          ${CMAKE} \
          --build \
          ${_build} \
          --clean-first \
          --target all \
          --config ${MY_BUILD_OPTION} \
          && \
          cd -
    fi
fi
