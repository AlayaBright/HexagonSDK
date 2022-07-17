#!/bin/bash

source ${HEXAGON_SDK_ROOT}/build/cmake/cmake_configure.bash


app=profiling
HEAP_ID="25"

android_build_dir=
hexagon_build_dir=
domain="3"
power_boost=""
iters="1000"
ion_disable=""
check_mem=""
uncached=""
fastrpc_qos="300"
sleep_latency="0"

# Function to parse args
parse_args() {

    while getopts a:h:d:p:n:i:m:u:q:y: flag; do

        case "${flag}" in
            a) android_build_dir=${OPTARG};;
            h) hexagon_build_dir=${OPTARG};;

            d) domain=${OPTARG}
               if [ "$domain" != "0" ] && [ "$domain" != "3" ]; then
                   echo "Invalid domain value" && exit
               fi;;

            p) if [[ ${OPTARG} = "True" ]]; then
                power_boost="-p"
               fi;;

            n) iters=${OPTARG};;

            i) if [[ ${OPTARG} = "True" ]]; then
                ion_disable="-i"
               fi;;

            m) if [[ ${OPTARG} = "True" ]]; then
                check_mem="-m"
               fi;;

            u) if [[ ${OPTARG} = "True" ]]; then
                uncached="-u"
               fi;;

            q) fastrpc_qos=${OPTARG};;
            y) sleep_latency=${OPTARG};;
            *) echo "Usage: cmake_build.cmd test -a <android_build_dir> -h <hexagon_build_dir>" && exit

         esac
    done
}

run_performance_tests() {

   adb shell export RPCPERF_HEAP_ID=$HEAP_ID RPCPERF_HEAP_FLAGS=1
   while IFS= read -r line; do
        val=$(echo "$line")
        arr=(${val//,/ })
        adb shell -n //vendor/bin/${app} -f ${arr[0]} -n ${iters} ${power_boost} -s ${arr[1]} ${uncached} \
        ${ion_disable} ${check_mem} -q ${fastrpc_qos} -d ${domain} -y ${sleep_latency}
   done < tests.txt
}

doTest() {

    adb wait-for-device
    adb root && adb remount

    # ANDROID LIBS
    adb shell mkdir -p //vendor/bin
    adb push ${android_build_dir}/ship/${app} //vendor/bin
    adb shell chmod 777 //vendor/bin/${app}
    adb push ${android_build_dir}/ship/libprofiling.so //vendor/lib64/

    # HEXAGON LIBS
    adb shell mkdir -p //vendor/lib/rfsa/adsp/
    adb push ${hexagon_build_dir}/ship/libprofiling_skel.so //vendor/lib/rfsa/adsp/

    adb shell "echo 0x1f > //vendor/lib/rfsa/adsp/profiling.farf"

    # Run the timer demonstration
    adb shell //vendor/bin/${app} -f timers -d $domain $power_boost

    # Run fastrpc performance tests
    run_performance_tests
}

if [ "to$1" = "totest" ];
then
    # Test on Device ( make sure that the test is already built to run on device)
    # Usage: cmake_build.cmd test -a <android_build_dir> -h <hexagon_build_dir>
    shift 1
    parse_args "$@"
    doTest
else
    # invoke command to construct Build Tag and Build the required variant
    doBuild "$@" TEST_NAME=profiling
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

