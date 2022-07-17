#!/bin/bash
Q6VERSION=${1}
DSP_DOMAIN=3

LIBS="qhmath_hvx qhblas_hvx"
LIBS_QHDSP_HVX_SUBDIR="lms"

make V=${Q6VERSION}

# with arguments: qhdsp_hvx/image_processing qhdsp_hvx/fft

DSP_OUT=/vendor/qhl_hvx

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
    adb shell rm -rf /$DSP_OUT/$lib
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

    adb shell "cd $DSP_OUT/$lib && for ff in ${tsttype}_*_qhl.so; do ./run_main_on_hexagon ${DSP_DOMAIN}  \$ff ; done" | tee -a $REPORT
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

for subdir in $LIBS_QHDSP_HVX_SUBDIR; do
    echo -----------------------------------------------------------

    lib=qhdsp_hvx/$subdir

    clean_tests $lib

    push_tests $lib $subdir

    for tsttype in $TEST_TYPE; do
        exec_tests $lib $tsttype
    done

    clean_tests $lib
done

# Execution with arguments

echo -----------------------------------------------------------

adb push $HEXAGON_SDK_ROOT/libs/run_main_on_hexagon/ship/hexagon_toolv84_${Q6VERSION}/librun_main_on_hexagon_skel.so /vendor/bin
adb push $HEXAGON_SDK_ROOT/libs/run_main_on_hexagon/ship/hexagon_toolv84_${Q6VERSION}/test_main.so /vendor/bin
adb push $HEXAGON_SDK_ROOT/libs/run_main_on_hexagon/ship/android_aarch64/run_main_on_hexagon /vendor/bin

subdir=image_processing
lib=qhdsp_hvx/$subdir

clean_tests $lib
push_tests $lib $subdir

adb push $lib/test_data /$DSP_OUT
adb shell "mv $DSP_OUT/test_data $DSP_OUT/$lib/"

for tsttype in $TEST_TYPE; do
    # For every test in output folder run
    echo Execute $tsttype for library: $lib ... | tee -a $REPORT
    echo Execute $tst with argumetns: 1920 1080 test_data/football1920x1080.bin | tee -a $REPORT
    adb shell "cd $DSP_OUT/$lib && for ff in ${tsttype}_*_qhl.so; do ./run_main_on_hexagon ${DSP_DOMAIN}  \$ff 1920 1080 test_data/football1920x1080.bin ; done" | tee -a $REPORT
done

clean_tests $lib

echo -----------------------------------------------------------
subdir=fft
lib=qhdsp_hvx/$subdir

clean_tests $lib
push_tests $lib $subdir

adb push $lib/test_data /$DSP_OUT
adb shell "mv $DSP_OUT/test_data $DSP_OUT/$lib/"

adb shell "cd $DSP_OUT/$lib && ./run_main_on_hexagon ${DSP_DOMAIN}  test_bench_fxp_fft_real_HVX_qhl.so" | tee -a $REPORT
adb shell "cd $DSP_OUT/$lib && ./run_main_on_hexagon ${DSP_DOMAIN}  test_bench_float_fft_real_HVX_qhl.so" | tee -a $REPORT
adb shell "cd $DSP_OUT/$lib && ./run_main_on_hexagon ${DSP_DOMAIN}  test_bench_half_float_fft_real_HVX_qhl.so" | tee -a $REPORT
adb shell "cd $DSP_OUT/$lib && ./run_main_on_hexagon ${DSP_DOMAIN}  test_bench_fxp_fft_complex_HVX_qhl.so" | tee -a $REPORT
adb shell "cd $DSP_OUT/$lib && ./run_main_on_hexagon ${DSP_DOMAIN}  test_bench_float_fft_complex_HVX_qhl.so" | tee -a $REPORT
adb shell "cd $DSP_OUT/$lib && ./run_main_on_hexagon ${DSP_DOMAIN}  test_bench_half_float_fft_complex_HVX_qhl.so" | tee -a $REPORT


WIN_SIZES="128 256 512 1024 2048"

# 2D fixed point 8x32 FFT/IFFT tests
for window_size in $WIN_SIZES; do
    adb shell "cd $DSP_OUT/$lib && ./run_main_on_hexagon ${DSP_DOMAIN}  test_bench_fxp_2d_fft_real_HVX_qhl.so -itest_data/2D_FFT_inputs/fft2D_fxp_real_test_data_HVX_in$window_size\x$window_size.dat -rtest_data/2D_FFT_refs/fft2D_fxp_real_test_data_HVX_out$window_size\x$window_size\_REF.dat -w$window_size" | tee -a $REPORT
done

# 2D single-precision float-point FFT/IFFT tests
for window_size in $WIN_SIZES; do
    adb shell "cd $DSP_OUT/$lib && ./run_main_on_hexagon ${DSP_DOMAIN}  test_bench_float_2d_fft_real_HVX_qhl.so -itest_data/2D_FFT_inputs/fft2D_float_real_test_data_HVX_in$window_size\x$window_size.dat -rtest_data/2D_FFT_refs/fft2D_float_real_test_data_HVX_out$window_size\x$window_size\_REF.dat -w$window_size" | tee -a $REPORT
done

# 2D half-precision float-point FFT/IFFT tests
for window_size in $WIN_SIZES; do
    adb shell "cd $DSP_OUT/$lib && ./run_main_on_hexagon ${DSP_DOMAIN}  test_bench_half_float_2d_fft_real_HVX_qhl.so -itest_data/2D_FFT_inputs/fft2D_float_real_test_data_HVX_in$window_size\x$window_size.dat -rtest_data/2D_FFT_refs/fft2D_float_real_test_data_HVX_out$window_size\x$window_size\_REF.dat -w$window_size" | tee -a $REPORT
done

clean_tests $lib
echo -----------------------------------------------------------

subdir=filters
lib=qhdsp_hvx/$subdir

clean_tests $lib
push_tests $lib $subdir

TAP_SIZES="256 512 1024 2048 4096 8192"

# 1D block FIR tests
for tap_size in $TAP_SIZES; do
    adb shell "cd $DSP_OUT/$lib && ./run_main_on_hexagon ${DSP_DOMAIN}  bench_bkfir_af_qhl.so 4096 $tap_size" | tee -a $REPORT
done

adb shell "cd $DSP_OUT/$lib && ./run_main_on_hexagon ${DSP_DOMAIN}  bench_bkfir_ah_qhl.so" | tee -a $REPORT
adb shell "cd $DSP_OUT/$lib && ./run_main_on_hexagon ${DSP_DOMAIN}  bench_bkfir_aw_qhl.so" | tee -a $REPORT

clean_tests $lib
echo -----------------------------------------------------------

echo "Done." | tee -a $REPORT

echo
echo "Report is generated in" $REPORT
