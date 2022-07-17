# simple python script for running all tests for 2D FFT in simulator

import os
import platform

create_DUT = False

if platform.system() == 'Linux':
    dir_separator = "/"
else:
    dir_separator = "\\"

script_dir_path = os.path.dirname(os.path.realpath(__file__))
test_exe_dir = script_dir_path + dir_separator  + ".." + dir_separator + ".." + dir_separator + "output" + dir_separator + "sim" + dir_separator + "qhdsp_hvx" + dir_separator + "fft"
hexagon_sim_cmd_line = "hexagon-sim -mv68g_1024 "
FFT_window_sizes = ["128", "256", "512", "1024", "2048"]

os.chdir(test_exe_dir)

test_outputs_dir = script_dir_path + dir_separator + "test_data" + dir_separator + "2D_FFT_dut"

if(create_DUT == True):
    if not os.path.exists(test_outputs_dir):
        os.mkdir(test_outputs_dir)
        os.chdir(test_exe_dir)

for fft_window_size in FFT_window_sizes:
    input_file = "../../../../qhdsp_hvx/fft/test_data/2D_FFT_inputs/fft2D_float_real_test_data_HVX_in"
    input_file += fft_window_size + "x" + fft_window_size + ".dat"
    ref_file = "../../../../qhdsp_hvx/fft/test_data/2D_FFT_refs/fft2D_float_real_test_data_HVX_out"
    ref_file += fft_window_size + "x" + fft_window_size + "_REF.dat"
    dut_file = "../../../../qhdsp_hvx/fft/test_data/2D_FFT_dut/fft2D_float_real_test_data_HVX_out"
    dut_file += fft_window_size + "x" + fft_window_size + "_DUT.dat"

    test_cmd_line = hexagon_sim_cmd_line + "test_bench_half_float_2d_fft_real_HVX_qhl -- " + \
                        "-i" + input_file + \
                        " -r" + ref_file + \
                        " -w" + fft_window_size
    if(create_DUT == True):
        test_cmd_line += " -d" + dut_file

    os.system(test_cmd_line)
