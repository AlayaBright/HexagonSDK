# simple python script for running all tests for IIR filter

import os
import platform

if platform.system() == 'Linux':
    dir_separator = "/"
else:
    dir_separator = "\\"

script_dir_path = os.path.dirname(os.path.realpath(__file__))
test_exe_dir = script_dir_path + dir_separator  + ".." + dir_separator + ".." + dir_separator + "output" + dir_separator + "qhl"

hexagon_sim_cmd_line = "hexagon-sim -mv66 --timing "

types_of_inputs = "16bPCM", "float"
output_file_suffixes = "_IIR_LPF_2kHz_", "_IIR_HPF_10kHz_"
types_of_filters = "LP", "HP"

os.chdir(test_exe_dir)

test_outputs_dir = script_dir_path + dir_separator + "test_data" + dir_separator + "outputs"

if not os.path.exists(test_outputs_dir):
    os.mkdir(test_outputs_dir)
    os.chdir(test_exe_dir)

for input_type in types_of_inputs:
    for j in range(len(types_of_filters)):
        input_file = "../../qhdsp/filters/test_data/inputs/White_noise_m6dBFs_2s_1ch_48000_"
        input_file += input_type + ".wav"
        ref_file = "../../qhdsp/filters/test_data/refs/White_noise_m6dBFs_2s_1ch_48000_"
        ref_file += input_type + output_file_suffixes[j] +"REF.wav"
        dut_file_C = "../../qhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_"
        dut_file_C += input_type + output_file_suffixes[j] +"DUT_C.wav"
        dut_file_ASM = "../../qhdsp/filters/test_data/outputs/White_noise_m6dBFs_2s_1ch_48000_"
        dut_file_ASM += input_type + output_file_suffixes[j] +"DUT_ASM.wav"
        filter_type = types_of_filters[j]

        test_cmd_line = hexagon_sim_cmd_line + "test_bench_iir_qhl -- " + \
                            "-i" + input_file + \
                            " -r" + ref_file + \
                            " -dc" + dut_file_C + \
                            " -da" + dut_file_ASM + \
                            " -f" + filter_type
        os.system(test_cmd_line)
