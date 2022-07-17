#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash

doTest() {
    adb wait-for-device root && adb wait-for-device remount
    HEXAGON_BUILD=${1}

    if [[ ! -f "${HEXAGON_BUILD}/libLPI.so" ]]; then
        echo "Testing supported for LPI shared object only"
        return
    fi

    # run_main_on_hexagon libs
    hexagon_lib=${HEXAGON_BUILD//_Debug}
    hexagon_lib=${hexagon_lib//_ReleaseG}
    hexagon_lib=${hexagon_lib//_Release}

    rmoh_exec="${HEXAGON_SDK_ROOT}/libs/run_main_on_hexagon/ship/android_aarch64/"
    rmoh_lib="${HEXAGON_SDK_ROOT}/libs/run_main_on_hexagon/ship/${hexagon_lib}/"
    if [[ ! -d "${rmoh_exec}" || ! -d "${rmoh_lib}" ]];then
        echo "Cannot find run_main_on_hexagon libs directory"
        return
    fi

    adb wait-for-device shell mkdir -p //vendor/bin
    adb wait-for-device shell mkdir -p //vendor/lib/rfsa/dsp/sdk/
    adb wait-for-device push ${rmoh_exec}/run_main_on_hexagon //vendor/bin
    adb wait-for-device shell chmod 774 //vendor/bin/run_main_on_hexagon
    adb wait-for-device push ${rmoh_lib}/librun_main_on_hexagon_skel.so //vendor/lib/rfsa/dsp/sdk/
    adb wait-for-device push ${HEXAGON_BUILD}/libLPI.so //vendor/lib/rfsa/dsp/sdk/

    # Direct dsp messages to logcat
    adb wait-for-device shell "echo 0x1f > //vendor/lib/rfsa/dsp/sdk/run_main_on_hexagon.farf"

    adb wait-for-device shell export LD_LIBRARY_PATH=//vendor/lib64/:$LD_LIBRARY_PATH ADSP_LIBRARY_PATH="//vendor/lib/rfsa/dsp/sdk\;//vendor/lib/rfsa/adsp;" //vendor/bin//run_main_on_hexagon 0 libLPI.so

}

if [ "to$1" = "totest" ];
then
    # Test on Device ( make sure that the test is already built to run on device)
    doTest $2 $3
else
    array=()
    for arg in "$@"
    do
        if [[ "$arg" =~ LPI* ]]; then
            lpi=${arg#*=}
        else
            array+=(${arg})
        fi
    done

    # If LPI argument is not passed, then setting its values to 0.
    [[ -z ${lpi} ]] && lpi=0

    # Valid value of LPI is either 0 or 1
    [[ ${lpi} -gt 1 ]] && echo "Invalid LPI flag value. Valid values are 0/1" && exit 1

    [[ ${lpi} -eq 1 ]] && lib_name="LPI" || lib_name="Non-LPI"

    # invoke command to construct Build Tag and Build the required variant
    doBuild "${array[@]}" TEST_NAME=${lib_name}

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
