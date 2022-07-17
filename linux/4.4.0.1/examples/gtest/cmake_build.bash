#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash
doTest() {
# Test on the Device
    HEXAGON_BUILD=${2}
    ANDROID_BUILD=${1}
    app=gtest
    adb wait-for-device
    adb root && adb remount
    adb shell mkdir -p //vendor/bin/
    if [ ${machine} = "Linux" ]
    then
        adb push ${HEXAGON_SDK_ROOT}/tools/android-ndk-r19c/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android/libc++_shared.so  //vendor/lib64/
    else
        adb push ${HEXAGON_SDK_ROOT}/tools/android-ndk-r19c/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/lib/aarch64-linux-android/libc++_shared.so  //vendor/lib64/
    fi
    adb push ${ANDROID_BUILD}/ship/${app} //vendor/bin
    adb push ${ANDROID_BUILD}/ship/libgtest.so //vendor/lib64/
    adb shell mkdir -p //vendor/lib/rfsa/adsp/
    adb push ${HEXAGON_BUILD}/ship/libgtest_skel.so //vendor/lib/rfsa/adsp/
    adb shell "echo 0x1f > //vendor/lib/rfsa/adsp/${app}.farf"
    adb shell chmod 777 //vendor/bin/${app}
    adb shell ADSP_LIBRARY_PATH="//vendor/lib/rfsa/adsp:/system/lib/rfsa/adsp:/dsp;" //vendor/bin/${app}
}
if [ "to$1" = "totest" ];
then
    # Test on Device ( make sure that the test is already built to run on device)
    doTest $2 $3
else
    doBuild "$@" TEST_NAME=gtest
    if [ "$?" -ne 0 ]
    then
        exit 1
    elif [ $CLEAN_ONLY -eq  1 ];
    then
         echo "Cleaning the build Directories"
         doClean googletest
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
