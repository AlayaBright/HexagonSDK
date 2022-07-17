# simple python script for running all tests for FIR decimation

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
output_file_suffixes = "24000_", "12000_"
decimation_factor = "2", "4"
decimator_variants = "_FIR", "_IIR"

os.chdir(test_exe_dir)

test_outputs_dir = script_dir_path + dir_separator + "test_data" + dir_separator + "outputs"

if not os.path.exists(test_outputs_dir):
    os.mkdir(test_outputs_dir)
    os.chdir(test_exe_dir)

for input_type in types_of_inputs:
    for j in range(len(decimation_factor)):
        for filter_type in decimator_variants:
            input_file = "../../qhdsp/decimation/test_data/inputs/sine_multitone_1s_1ch_48000_"
            input_file += input_type + ".wav"
            ref_file = "../../qhdsp/decimation/test_data/refs/sine_multitone_1s_1ch_"
            ref_file += output_file_suffixes[j] + input_type + filter_type + "_deci_REF.wav"
            dut_file = "../../qhdsp/decimation/test_data/outputs/sine_multitone_1s_1ch_"
            dut_file += output_file_suffixes[j] + input_type + filter_type + "_deci_DUT.wav"
            dec_factor = decimation_factor[j]

            test_cmd_line = hexagon_sim_cmd_line + "test_bench" + filter_type.lower() + "_decim_qhl -- " + \
                                "-i" + input_file + \
                                " -r" + ref_file + \
                                " -d" + dut_file + \
                                " -f" + dec_factor
            os.system(test_cmd_line)
