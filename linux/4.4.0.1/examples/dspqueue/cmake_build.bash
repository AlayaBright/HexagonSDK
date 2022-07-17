#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash

doTest() {
    adb wait-for-device root && adb wait-for-device remount
    ANDROID_BUILD=${1}
    HEXAGON_BUILD=${2}
    adb wait-for-device shell mkdir -p //vendor/bin
    adb wait-for-device shell mkdir -p //vendor/lib/rfsa/dsp/sdk/

    app=dspqueue_sample
    adb wait-for-device push ${ANDROID_BUILD}/${app} //vendor/bin
    adb wait-for-device shell chmod 777 //vendor/bin/${app}
    adb wait-for-device push ${HEXAGON_BUILD}/libdspqueue_sample_skel.so //vendor/lib/rfsa/dsp/sdk/

    # Run Asynchronous Packet Queue Example on cDSP
    adb wait-for-device shell export LD_LIBRARY_PATH=//vendor/lib64/:$LD_LIBRARY_PATH ADSP_LIBRARY_PATH="//vendor/lib/rfsa/dsp/sdk\;//vendor/lib/rfsa/adsp;" //vendor/bin/${app} -U 1

}

if [ "to$1" = "totest" ];
then
    # Test on Device ( make sure that the test is already built to run on device)
    doTest $2 $3
else
    # invoke command to construct Build Tag and Build the required variant
    doBuild "$@" TEST_NAME=dspqueue_sample

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
          --target ${BUILD_TARGET_NAME} \
          --config ${MY_BUILD_OPTION} \
          && \
          cd -
    fi
fi
