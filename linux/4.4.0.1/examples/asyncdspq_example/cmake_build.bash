#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash

doTest() {
    adb wait-for-device root && adb wait-for-device remount
    ANDROID_BUILD=${1}
    HEXAGON_BUILD=${2}
    adb wait-for-device shell mkdir -p //vendor/bin
    adb wait-for-device shell mkdir -p //vendor/lib/rfsa/dsp/sdk/

    echo "Run asyncdspq_fcvqueuetest Example"
    # FCVQUEUETEST
    app=fcvqueuetest
    adb wait-for-device push ${ANDROID_BUILD}/${app} //vendor/bin
    adb wait-for-device shell chmod 777 //vendor/bin/${app}
    adb wait-for-device push ${ANDROID_BUILD}/libasyncdspq.so //vendor/lib64/
    adb wait-for-device push ${HEXAGON_BUILD}/libfcvqueuetest_skel.so //vendor/lib/rfsa/dsp/sdk/
    adb wait-for-device push ${HEXAGON_BUILD}/libfcvqueue_dsp_skel.so //vendor/lib/rfsa/dsp/sdk/
    adb wait-for-device push ${HEXAGON_BUILD}/libasyncdspq_skel.so //vendor/lib/rfsa/dsp/sdk/
    # Direct dsp messages to logcat
    adb wait-for-device shell "echo 0x1f > //vendor/lib/rfsa/dsp/sdk/${app}.farf"
    # Run the test
    adb wait-for-device shell export LD_LIBRARY_PATH=//vendor/lib64/ ADSP_LIBRARY_PATH="//vendor/lib/rfsa/dsp/sdk\;//vendor/lib/rfsa/adsp;" //vendor/bin/${app} -U 1

    echo "Run asyncdspq_queueperf Example"
    # QUEUEPERF
    app=queueperf
    adb wait-for-device push ${ANDROID_BUILD}/${app} //vendor/bin
    adb wait-for-device shell chmod 777 //vendor/bin/${app}
    adb wait-for-device push ${ANDROID_BUILD}/libasyncdspq.so //vendor/lib64/
    adb wait-for-device push ${HEXAGON_BUILD}/libqueuetest_skel.so //vendor/lib/rfsa/dsp/sdk/
    adb wait-for-device push ${HEXAGON_BUILD}/libasyncdspq_skel.so //vendor/lib/rfsa/dsp/sdk/
    # Direct dsp messages to logcat
    adb wait-for-device shell "echo 0x1f > //vendor/lib/rfsa/dsp/sdk/${app}.farf"
    # Run the test
    adb wait-for-device shell export LD_LIBRARY_PATH=//vendor/lib64/ ADSP_LIBRARY_PATH="//vendor/lib/rfsa/dsp/sdk\;//vendor/lib/rfsa/adsp;" //vendor/bin/${app} -U 1

    echo "Run asyncdspq_queuetest Example"
    # QUEUETEST
    app=queuetest
    adb wait-for-device push ${ANDROID_BUILD}/${app} //vendor/bin
    adb wait-for-device shell chmod 777 //vendor/bin/${app}
    adb wait-for-device push ${ANDROID_BUILD}/libasyncdspq.so //vendor/lib64/
    adb wait-for-device push ${HEXAGON_BUILD}/libqueuetest_skel.so //vendor/lib/rfsa/dsp/sdk/
    adb wait-for-device push ${HEXAGON_BUILD}/libasyncdspq_skel.so //vendor/lib/rfsa/dsp/sdk/
    # Direct dsp messages to logcat
    adb wait-for-device shell "echo 0x1f > //vendor/lib/rfsa/dsp/sdk/${app}.farf"
    # Run the test
    adb wait-for-device shell export LD_LIBRARY_PATH=//vendor/lib64/ ADSP_LIBRARY_PATH="//vendor/lib/rfsa/dsp/sdk\;//vendor/lib/rfsa/adsp;" //vendor/bin/${app} -U 1

}


if [ "to$1" = "totest" ];
then
    # Test on Device ( make sure that the test is already built to run on device)
    doTest $2 $3
else
    # invoke command to construct Build Tag and Build the required variant
    doBuild "$@" TEST_NAME=asyncdspq_example

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
