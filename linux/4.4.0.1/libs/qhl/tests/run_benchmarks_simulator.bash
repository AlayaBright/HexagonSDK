#!/bin/bash

Q6VERSION=$1
make clean V=${Q6VERSION}
make V=${Q6VERSION}
LIBS="qhmath qhblas qhcomplex qhdsp/fft"
# with arguments: qhdsp/filters qhdsp/interpolation qhdsp/decimation

TEST_TYPE=bench

# Hexagon simulator Path 
HEXAGON_SIM=$DEFAULT_HEXAGON_TOOLS_ROOT/Tools/bin/hexagon-sim

REPORTS_DIR=test_results
mkdir -p $REPORTS_DIR

REPORT_NAME=report-benchmarks-simulator.txt
REPORT=$REPORTS_DIR/$REPORT_NAME

# Start with the new report every time
rm -f $REPORT

echo "Running benchmarks on simulator ..."

if [[ ${Q6VERSION} = "v68" ]];
then
    HEX_SIM_ARG="-m${Q6VERSION}n_1024 --timing --quiet"
elif [[ ${Q6VERSION} = "v66" ]];
then
    HEX_SIM_ARG="-m${Q6VERSION}g_1024 --timing --quiet"
else
    HEX_SIM_ARG="-m${Q6VERSION}a_1024 --timing --quiet"
fi

for tsttype in $TEST_TYPE; do
    for tstlib in $LIBS; do
        for tst in output/sim/$tstlib/${tsttype}_*; do
            echo ---------------------------------------------------
            echo Execute $tst
            $HEXAGON_SIM $HEX_SIM_ARG $tst 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
        done
    done
done


# Execution with arguments
mkdir -p qhdsp/decimation/test_data/outputs/
mkdir -p qhdsp/interpolation/test_data/outputs/
mkdir -p qhdsp/filters/test_data/outputs/

# qhdsp/decimation
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/decimation/test_bench_fir_decim_qhl -- -iqhdsp/decimation/test_data/inputs/sine_multitone_1s_1ch_48000_16bPCM.wav -rqhdsp/decimation/test_data/refs/sine_multitone_1s_1ch_24000_16bPCM_FIR_deci_REF.wav -dqhdsp/decimation/test_data/outputs/sine_multitone_1s_1ch_24000_16bPCM_FIR_deci_DUT.wav -f2 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/decimation/test_bench_iir_decim_qhl -- -iqhdsp/decimation/test_data/inputs/sine_multitone_1s_1ch_48000_16bPCM.wav -rqhdsp/decimation/test_data/refs/sine_multitone_1s_1ch_24000_16bPCM_IIR_deci_REF.wav -dqhdsp/decimation/test_data/outputs/sine_multitone_1s_1ch_24000_16bPCM_IIR_deci_DUT.wav -f2 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/decimation/test_bench_fir_decim_qhl -- -iqhdsp/decimation/test_data/inputs/sine_multitone_1s_1ch_48000_16bPCM.wav -rqhdsp/decimation/test_data/refs/sine_multitone_1s_1ch_12000_16bPCM_FIR_deci_REF.wav -dqhdsp/decimation/test_data/outputs/sine_multitone_1s_1ch_12000_16bPCM_FIR_deci_DUT.wav -f4 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/decimation/test_bench_iir_decim_qhl -- -iqhdsp/decimation/test_data/inputs/sine_multitone_1s_1ch_48000_16bPCM.wav -rqhdsp/decimation/test_data/refs/sine_multitone_1s_1ch_12000_16bPCM_IIR_deci_REF.wav -dqhdsp/decimation/test_data/outputs/sine_multitone_1s_1ch_12000_16bPCM_IIR_deci_DUT.wav -f4 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/decimation/test_bench_fir_decim_qhl -- -iqhdsp/decimation/test_data/inputs/sine_multitone_1s_1ch_48000_float.wav -rqhdsp/decimation/test_data/refs/sine_multitone_1s_1ch_24000_float_FIR_deci_REF.wav -dqhdsp/decimation/test_data/outputs/sine_multitone_1s_1ch_24000_float_FIR_deci_DUT.wav -f2 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/decimation/test_bench_iir_decim_qhl -- -iqhdsp/decimation/test_data/inputs/sine_multitone_1s_1ch_48000_float.wav -rqhdsp/decimation/test_data/refs/sine_multitone_1s_1ch_24000_float_IIR_deci_REF.wav -dqhdsp/decimation/test_data/outputs/sine_multitone_1s_1ch_24000_float_IIR_deci_DUT.wav -f2 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/decimation/test_bench_fir_decim_qhl -- -iqhdsp/decimation/test_data/inputs/sine_multitone_1s_1ch_48000_float.wav -rqhdsp/decimation/test_data/refs/sine_multitone_1s_1ch_12000_float_FIR_deci_REF.wav -dqhdsp/decimation/test_data/outputs/sine_multitone_1s_1ch_12000_float_FIR_deci_DUT.wav -f4 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/decimation/test_bench_iir_decim_qhl -- -iqhdsp/decimation/test_data/inputs/sine_multitone_1s_1ch_48000_float.wav -rqhdsp/decimation/test_data/refs/sine_multitone_1s_1ch_12000_float_IIR_deci_REF.wav -dqhdsp/decimation/test_data/outputs/sine_multitone_1s_1ch_12000_float_IIR_deci_DUT.wav -f4 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
# qhdsp/interpolation
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/interpolation/test_bench_iir_interp_qhl -- -iqhdsp/interpolation/test_data/inputs/sine_multitone_1s_1ch_48000_16bPCM.wav -rqhdsp/interpolation/test_data/refs/sine_multitone_1s_1ch_96000_16bPCM_IIR_interp_REF.wav -dqhdsp/interpolation/test_data/outputs/sine_multitone_1s_1ch_96000_16bPCM_IIR_interp_DUT.wav 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/interpolation/test_bench_iir_interp_qhl -- -iqhdsp/interpolation/test_data/inputs/sine_multitone_1s_1ch_48000_float.wav -rqhdsp/interpolation/test_data/refs/sine_multitone_1s_1ch_96000_float_IIR_interp_REF.wav -dqhdsp/interpolation/test_data/outputs/sine_multitone_1s_1ch_96000_float_IIR_interp_DUT.wav 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
# qhdsp/filters/iir
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/filters/test_bench_iir_qhl -- -iqhdsp/filters/test_data/inputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM.wav -rqhdsp/filters/test_data/refs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_IIR_LPF_2kHz_REF.wav -dcqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_IIR_LPF_2kHz_DUT_C.wav -daqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_IIR_LPF_2kHz_DUT_ASM.wav -fLP 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/filters/test_bench_iir_qhl -- -iqhdsp/filters/test_data/inputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM.wav -rqhdsp/filters/test_data/refs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_IIR_HPF_10kHz_REF.wav -dcqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_IIR_HPF_10kHz_DUT_C.wav -daqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_IIR_HPF_10kHz_DUT_ASM.wav -fHP 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/filters/test_bench_iir_qhl -- -iqhdsp/filters/test_data/inputs/White_noise_m6dBFs_2s_1ch_48000_float.wav -rqhdsp/filters/test_data/refs/White_noise_m6dBFs_2s_1ch_48000_float_IIR_LPF_2kHz_REF.wav -dcqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_IIR_LPF_2kHz_DUT_C.wav -daqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_IIR_LPF_2kHz_DUT_ASM.wav -fLP 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/filters/test_bench_iir_qhl -- -iqhdsp/filters/test_data/inputs/White_noise_m6dBFs_2s_1ch_48000_float.wav -rqhdsp/filters/test_data/refs/White_noise_m6dBFs_2s_1ch_48000_float_IIR_HPF_10kHz_REF.wav -dcqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_IIR_HPF_10kHz_DUT_C.wav -daqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_IIR_HPF_10kHz_DUT_ASM.wav -fHP 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
# qhdsp/filters/fir
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/filters/test_bench_fir_qhl -- -iqhdsp/filters/test_data/inputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM.wav -rqhdsp/filters/test_data/refs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_FIR_LPF_2kHz_REF.wav -dcqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_FIR_LPF_2kHz_DUT_C.wav -daqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_FIR_LPF_2kHz_DUT_ASM.wav -fLP 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/filters/test_bench_fir_qhl -- -iqhdsp/filters/test_data/inputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM.wav -rqhdsp/filters/test_data/refs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_FIR_HPF_10kHz_REF.wav -dcqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_FIR_HPF_10kHz_DUT_C.wav -daqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_16bPCM_FIR_HPF_10kHz_DUT_ASM.wav -fHP 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/filters/test_bench_fir_qhl -- -iqhdsp/filters/test_data/inputs/White_noise_m6dBFs_2s_1ch_48000_float.wav -rqhdsp/filters/test_data/refs/White_noise_m6dBFs_2s_1ch_48000_float_FIR_LPF_2kHz_REF.wav -dcqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_FIR_LPF_2kHz_DUT_C.wav -daqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_FIR_LPF_2kHz_DUT_ASM.wav -fLP 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT
$HEXAGON_SIM $HEX_SIM_ARG output/sim/qhdsp/filters/test_bench_fir_qhl -- -iqhdsp/filters/test_data/inputs/White_noise_m6dBFs_2s_1ch_48000_float.wav -rqhdsp/filters/test_data/refs/White_noise_m6dBFs_2s_1ch_48000_float_FIR_HPF_10kHz_REF.wav -dcqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_FIR_HPF_10kHz_DUT_C.wav -daqhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_float_FIR_HPF_10kHz_DUT_ASM.wav -fHP 2>&1 | grep -v "hexagon-sim" | tee -a $REPORT


echo "Done." | tee -a $REPORT

# Delete pmustats file generated by simulator
rm -f pmu_statsfile.txt

echo
echo "Report is generated in" $REPORT
