#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash

doTest() {

    adb wait-for-device
    adb root && adb remount
    ANDROID_BUILD=${1}
    HEXAGON_BUILD=${2}
    app=calculator_plus

    # ANDROID LIBS
    adb shell mkdir -p //vendor/bin
    adb push ${ANDROID_BUILD}/ship/${app} //vendor/bin
    adb shell chmod 777 //vendor/bin/${app}
    adb push ${ANDROID_BUILD}/ship/libcalculator_plus.so //vendor/lib64/

    # HEXAGON LIBS
    adb shell mkdir -p //vendor/lib/rfsa/adsp/
    adb push ${HEXAGON_BUILD}/ship/libcalculator_plus_skel.so //vendor/lib/rfsa/adsp/
    adb push ${HEXAGON_BUILD}/libc++.so.1 //vendor/lib/rfsa/adsp/
    adb push ${HEXAGON_BUILD}/libc++abi.so.1 //vendor/lib/rfsa/adsp/

    arr=(${HEXAGON_BUILD//_/ })
    tools_version=${arr[2]}
    if [[ ${tools_version} != "toolv82" ]] && [[ ${tools_version} != "toolv83" ]]; then
        adb push ${HEXAGON_SDK_ROOT}/libs/weak_refs/ship/hexagon_toolv84/weak_refs.so //vendor/lib/rfsa/adsp/
    fi

    adb shell "echo 0x1f > //vendor/lib/rfsa/adsp/calculator_plus.farf"

    # Run the executable
    adb shell //vendor/bin/${app} -d 3 -m sum -i 10
}

if [ "to$1" = "totest" ];
then
    # Test on Device ( make sure that the test is already built to run on device)
    doTest $2 $3
else
    # invoke command to construct Build Tag and Build the required variant
    doBuild "$@" TEST_NAME=calculator_plus
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
