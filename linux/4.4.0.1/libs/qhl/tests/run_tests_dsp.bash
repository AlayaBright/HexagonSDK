#!/bin/bash
Q6VERSION=${1}
DSP_DOMAIN=3

LIBS="qhmath qhblas qhcomplex"

make V=${Q6VERSION}

# different name: qhdsp/fft : accuracy test and bencmark
# with arguments: qhdsp/filters qhdsp/interpolation qhdsp/decimation

DSP_OUT=/vendor/qhl

TEST_TYPE="test bench"

REPORTS_DIR=test_results
mkdir -p $REPORTS_DIR

REPORT_NAME=report-tests-dsp.txt
REPORT=$REPORTS_DIR/$REPORT_NAME

# Start with the new report every time
rm -f $REPORT

echo "Running accuracy tests and benchmarks on DSP ..."

# echo Start mini-dm to collect output prints from DSP | tee -a $REPORT

if [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]
then
    mini-dm | tee -a $REPORT.mini-dm.full.log | grep stdout | tee -a $REPORT.mini-dm.log &
    echo mini-dm is started in the background. You have to manually kill the process. | tee -a $REPORT
    sleep 5
else
    # Windows
    echo "For Windows with Cygwin, msys or gow see folowing example, determine valid serial port (change COM5) and execute:"
    echo "mini-dm --comport COM5 | tee -a $REPORT.mini-dm.full.log | grep stdout | tee -a $REPORT.mini-dm.log"
    read -p "Execute above command in another terminal and press [Enter] key to continue..."
fi


function clean_tests()
{
    echo Clean destination folder on the target for: $lib | tee -a $REPORT
    adb shell rm -rf /$DSP_OUT/$lib/
}

function push_tests()
{
    echo Push tests to the DSP target for: $lib

    echo Prepare loader for fast-RPC calls | tee -a $REPORT
    adb shell mkdir -p /$DSP_OUT/$lib/
    adb shell "cp -v //vendor/bin/*run_main_on_hexagon* //vendor/bin/testsig-*.so /$DSP_OUT/$lib"

    echo Push output/dsp/$lib test executables to the target | tee -a $REPORT
    adb push "output/dsp/$lib" /$DSP_OUT

    # if subfolders, get most right and move
    if [[ ! -z $2 ]]
    then
        adb shell "mv /$DSP_OUT/$2/* /$DSP_OUT/$lib"
        adb shell "rmdir /$DSP_OUT/$2"
    fi
}

function exec_tests()
{
    # For every test in output folder run
    echo Execute ${tsttype} for library: $lib ... | tee -a $REPORT

    adb shell "cd $DSP_OUT/$lib && for ff in ${tsttype}_*_qhl.so; do ./run_main_on_hexagon ${DSP_DOMAIN} \$ff ; done" | tee -a $REPORT
}

echo Wait for device on ADB | tee -a $REPORT
adb wait-for-device

for lib in $LIBS; do
    echo -----------------------------------------------------------

    clean_tests $lib

    push_tests $lib

    for tsttype in $TEST_TYPE; do
        exec_tests $lib $tsttype
    done

    clean_tests $lib
done


# Execute with different naming conventions:

adb push $HEXAGON_SDK_ROOT/libs/run_main_on_hexagon/ship/hexagon_toolv84_${Q6VERSION}/librun_main_on_hexagon_skel.so /vendor/bin
adb push $HEXAGON_SDK_ROOT/libs/run_main_on_hexagon/ship/hexagon_toolv84_${Q6VERSION}/test_main.so /vendor/bin
adb push $HEXAGON_SDK_ROOT/libs/run_main_on_hexagon/ship/android_aarch64/run_main_on_hexagon /vendor/bin

## QHDSP

# FFT tests, special name: test_bench_
lib=qhdsp/fft
clean_tests $lib
push_tests $lib fft
adb shell "cd $DSP_OUT/$lib && for fft_tst in test_bench_*fft*_qhl.so; do ./run_main_on_hexagon ${DSP_DOMAIN} \$fft_tst ; done" | tee -a $REPORT
clean_tests $lib

# Execution with arguments
# with arguments: qhdsp/filters qhdsp/interpolation qhdsp/decimation

# FIR/IIR
# copy FIR/IIR test stuffs to the board
DSP_TESTS="filters decimation interpolation"
for dsptst in $DSP_TESTS; do
    echo TEST: $dsptst

    lib=qhdsp/$dsptst
    clean_tests $lib
    push_tests $lib $dsptst

    adb shell "mkdir -p $DSP_OUT/$lib/test_data/outputs" | tee -a $REPORT

    for data_folders in inputs refs; do
        # adb push "qhdsp/$dsptst/test_data/$data_folders" /$DSP_OUT/$lib/test_data/$dsptst
        # workaround for above ADB push command to destination subfoler on Windows OS
        adb push "qhdsp/$dsptst/test_data/$data_folders" /$DSP_OUT
        adb shell mv /$DSP_OUT/$data_folders /$DSP_OUT/$lib/test_data/
    done
done

# decimation
adb shell "cd $DSP_OUT/qhdsp/decimation && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_fir_decim_qhl.so -itest_data/inputs/sine_multitone_1s_1ch_48000_16bPCM.wav -rtest_data/refs/sine_multitone_1s_1ch_24000_16bPCM_FIR_deci_REF.wav -dtest_data/outputs/sine_multitone_1s_1ch_24000_16bPCM_FIR_deci_DUT.wav -f2" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/decimation && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_iir_decim_qhl.so -itest_data/inputs/sine_multitone_1s_1ch_48000_16bPCM.wav -rtest_data/refs/sine_multitone_1s_1ch_24000_16bPCM_IIR_deci_REF.wav -dtest_data/outputs/sine_multitone_1s_1ch_24000_16bPCM_IIR_deci_DUT.wav -f2" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/decimation && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_fir_decim_qhl.so -itest_data/inputs/sine_multitone_1s_1ch_48000_16bPCM.wav -rtest_data/refs/sine_multitone_1s_1ch_12000_16bPCM_FIR_deci_REF.wav -dtest_data/outputs/sine_multitone_1s_1ch_12000_16bPCM_FIR_deci_DUT.wav -f4" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/decimation && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_iir_decim_qhl.so -itest_data/inputs/sine_multitone_1s_1ch_48000_16bPCM.wav -rtest_data/refs/sine_multitone_1s_1ch_12000_16bPCM_IIR_deci_REF.wav -dtest_data/outputs/sine_multitone_1s_1ch_12000_16bPCM_IIR_deci_DUT.wav -f4" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/decimation && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_fir_decim_qhl.so -itest_data/inputs/sine_multitone_1s_1ch_48000_float.wav -rtest_data/refs/sine_multitone_1s_1ch_24000_float_FIR_deci_REF.wav -dtest_data/outputs/sine_multitone_1s_1ch_24000_float_FIR_deci_DUT.wav -f2" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/decimation && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_iir_decim_qhl.so -itest_data/inputs/sine_multitone_1s_1ch_48000_float.wav -rtest_data/refs/sine_multitone_1s_1ch_24000_float_IIR_deci_REF.wav -dtest_data/outputs/sine_multitone_1s_1ch_24000_float_IIR_deci_DUT.wav -f2" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/decimation && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_fir_decim_qhl.so -itest_data/inputs/sine_multitone_1s_1ch_48000_float.wav -rtest_data/refs/sine_multitone_1s_1ch_12000_float_FIR_deci_REF.wav -dtest_data/outputs/sine_multitone_1s_1ch_12000_float_FIR_deci_DUT.wav -f4" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/decimation && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_iir_decim_qhl.so -itest_data/inputs/sine_multitone_1s_1ch_48000_float.wav -rtest_data/refs/sine_multitone_1s_1ch_12000_float_IIR_deci_REF.wav -dtest_data/outputs/sine_multitone_1s_1ch_12000_float_IIR_deci_DUT.wav -f4" | tee -a $REPORT
# iir
adb shell "cd $DSP_OUT/qhdsp/filters && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_iir_qhl.so -itest_data/inputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM.wav -rtest_data/refs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_IIR_LPF_2kHz_REF.wav -dctest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_IIR_LPF_2kHz_DUT_C.wav -datest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_IIR_LPF_2kHz_DUT_ASM.wav -fLP" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/filters && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_iir_qhl.so -itest_data/inputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM.wav -rtest_data/refs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_IIR_HPF_10kHz_REF.wav -dctest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_IIR_HPF_10kHz_DUT_C.wav -datest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_IIR_HPF_10kHz_DUT_ASM.wav -fHP" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/filters && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_iir_qhl.so -itest_data/inputs/White_noise_m6dBFs_2s_1ch_48000_float.wav -rtest_data/refs/White_noise_m6dBFs_2s_1ch_48000_float_IIR_LPF_2kHz_REF.wav -dctest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_IIR_LPF_2kHz_DUT_C.wav -datest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_IIR_LPF_2kHz_DUT_ASM.wav -fLP" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/filters && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_iir_qhl.so -itest_data/inputs/White_noise_m6dBFs_2s_1ch_48000_float.wav -rtest_data/refs/White_noise_m6dBFs_2s_1ch_48000_float_IIR_HPF_10kHz_REF.wav -dctest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_IIR_HPF_10kHz_DUT_C.wav -datest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_IIR_HPF_10kHz_DUT_ASM.wav -fHP" | tee -a $REPORT
# fir
adb shell "cd $DSP_OUT/qhdsp/filters && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_fir_qhl.so -itest_data/inputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM.wav -rtest_data/refs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_FIR_LPF_2kHz_REF.wav -dctest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_FIR_LPF_2kHz_DUT_C.wav -datest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_FIR_LPF_2kHz_DUT_ASM.wav -fLP" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/filters && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_fir_qhl.so -itest_data/inputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM.wav -rtest_data/refs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_FIR_HPF_10kHz_REF.wav -dctest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_FIR_HPF_10kHz_DUT_C.wav -datest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_FIR_HPF_10kHz_DUT_ASM.wav -fHP" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/filters && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_fir_qhl.so -itest_data/inputs/White_noise_m6dBFs_2s_1ch_48000_float.wav -rtest_data/refs/White_noise_m6dBFs_2s_1ch_48000_float_FIR_LPF_2kHz_REF.wav -dctest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_FIR_LPF_2kHz_DUT_C.wav -datest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_FIR_LPF_2kHz_DUT_ASM.wav -fLP" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/filters && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_fir_qhl.so -itest_data/inputs/White_noise_m6dBFs_2s_1ch_48000_float.wav -rtest_data/refs/White_noise_m6dBFs_2s_1ch_48000_float_FIR_HPF_10kHz_REF.wav -dctest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_FIR_HPF_10kHz_DUT_C.wav -datest_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_FIR_HPF_10kHz_DUT_ASM.wav -fHP" | tee -a $REPORT
# interpolation
adb shell "cd $DSP_OUT/qhdsp/interpolation && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_iir_interp_qhl.so -itest_data/inputs/sine_multitone_1s_1ch_48000_16bPCM.wav -rtest_data/refs/sine_multitone_1s_1ch_96000_16bPCM_IIR_interp_REF.wav -dtest_data/outputs/sine_multitone_1s_1ch_96000_16bPCM_IIR_interp_DUT.wav" | tee -a $REPORT
adb shell "cd $DSP_OUT/qhdsp/interpolation && ./run_main_on_hexagon ${DSP_DOMAIN} test_bench_iir_interp_qhl.so -itest_data/inputs/sine_multitone_1s_1ch_48000_float.wav -rtest_data/refs/sine_multitone_1s_1ch_96000_float_IIR_interp_REF.wav -dtest_data/outputs/sine_multitone_1s_1ch_96000_float_IIR_interp_DUT.wav" | tee -a $REPORT

for dsptst in $DSP_TESTS; do
    lib=qhdsp/$dsptst
    clean_tests $lib
done

echo "Done." | tee -a $REPORT

echo
echo "Report is generated in" $REPORT

