# simple python script for running all tests for IIR interpolation

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

os.chdir(test_exe_dir)

test_outputs_dir = script_dir_path + dir_separator + "test_data" + dir_separator + "outputs"

if not os.path.exists(test_outputs_dir):
    os.mkdir(test_outputs_dir)
    os.chdir(test_exe_dir)

# tested only interpolation factor of 2
for input_type in types_of_inputs:
    input_file = "../../qhdsp/interpolation/test_data/inputs/sine_multitone_1s_1ch_48000_"
    input_file += input_type + ".wav"
    ref_file = "../../qhdsp/interpolation/test_data/refs/sine_multitone_1s_1ch_96000_"
    ref_file += input_type + "_IIR_interp_REF.wav"
    dut_file = "../../qhdsp/interpolation/test_data/outputs/sine_multitone_1s_1ch_96000_"
    dut_file += input_type + "_IIR_interp_DUT.wav"

    test_cmd_line = hexagon_sim_cmd_line + "test_bench_iir_interp_qhl -- " + \
                        "-i" + input_file + \
                        " -r" + ref_file + \
                        " -d" + dut_file
    os.system(test_cmd_line)
