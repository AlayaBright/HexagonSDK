import numpy as np
import qhcg_fxp_parser as fxp_parser
from qhcg_config import Input_types, Output_types, Modes_process_output

# TODO: Number of elements needs to be changed not to be global
num_elements = 0.0
np.random.seed(9001)
# Hard limit of number of fixed points.
LIMIT_THE_NUMBER_OF_INPUT = 100000

# Brief: Generate fixed-point input points based on input range (in floating-point) and
#        and fixed-point format
#
#     a0_flp: Start point in fixed-point format
#     b0_flp: End point in fixed-point format
#     fxp_fmt: Fixed-point format for input points
#
# return: Array of fixed-point elements
#
def gen_fxp_input(a0_flp, b0_flp, qfmt, max_val):

    # Determine start and end point of input range in fixed-point format
    start_fxp_range = fxp_parser.float_2_qfxp(a0_flp, qfmt)
    end_fxp_range = fxp_parser.float_2_qfxp(b0_flp, qfmt)

    if end_fxp_range > max_val:
        end_fxp_range = max_val

    N = end_fxp_range - start_fxp_range + 2
    # TODO: Works forbit 16 -> 16bit but 32bit -> 32bit are in milions
    # allocated almost 15GB of RAM which is too much
    if N > LIMIT_THE_NUMBER_OF_INPUT:
        N = LIMIT_THE_NUMBER_OF_INPUT
    input_points = np.sort(np.random.random_integers(start_fxp_range, end_fxp_range, N))
    return input_points


# Brief: Generate fixed-point output based on input/output format and specified function
#
#     a0_fxp: Start point in fixed-point format
#     b0_fxp: End point in fixed-point format
#     input_fxp_fmt: Input fixed-point format
#     output_fxp_fmt: Output fixed-point format
#     func: Desired function for generating the outputs of fixed-point elements
#
# return: Array of fixed-point elements
#
def gen_fxp_output(input_fxp, input_fxp_fmt, output_fxp_fmt, computable_function):

    # Convert to floating-point input array, calculate function and revert to fixed-point array
    input_flp = fxp_parser.qfxp_2_float_arr(input_fxp, input_fxp_fmt)
    output_flp = computable_function(input_flp)
    output_fxp = fxp_parser.float_2_qfxp_arr(output_flp, output_fxp_fmt)

    return output_fxp


def gen_float_output(input_arr, computable_function):

    return computable_function(input_arr)


def gen_float_output_from_fxp(input_arr, input_fxp_fmt, output_fxp_fmt, computable_function):
    # The input array comes from the fixed-point implementation that will call the QHCG-generated HVX approximation.
    # The values are needed to be divided by  (2 ** input_fxp_fmt) to change to their actual representation.
    input_scale = fxp_parser.qfxp_2_float_arr(input_arr, input_fxp_fmt)
    output_raw = computable_function(input_scale)

    # multiply by a factor to match HVX code's output
    return output_raw * (2 ** output_fxp_fmt)


# Brief: Generate fixed-point output based on input/output format and specified function
#
#     a0_fxp: Start point in fixed-point format
#     b0_fxp: End point in fixed-point format
#     in_type: Data type: (int16, int32, 32f, 16f)
#
# return: Array of fixed-point elements
#
def save_input(outputer, a0_flp, b0_flp, in_type):

    global num_elements

    fxp_format = fxp_parser.find_qfmt_in_range(a0_flp, b0_flp, Input_types.input_type_to_bit_size(in_type))
    if in_type == Input_types.int16:
        hvx_vect_len = int((128 * 8) / Input_types.input_type_to_bit_size(in_type))
        input_arr = gen_fxp_input(a0_flp, b0_flp, fxp_format, 2**15-1)
        num_rows = int(len(input_arr) / hvx_vect_len)
        path = path_generator(mode=Modes_process_output.read_input, input_types=in_type)
        np.savetxt(outputer.get_file_path(path), np.reshape(input_arr[0:num_rows * hvx_vect_len], (num_rows, hvx_vect_len)), '%d', newline='\n', delimiter=',')
        num_elements = num_rows * hvx_vect_len
        # print('NUM ELEMENTS NEW: ' + str(num_elements))
        np.savetxt(outputer.get_file_path(path), np.reshape(input_arr[0:num_rows * hvx_vect_len], (num_rows, hvx_vect_len)), '%d', newline='\n', delimiter=',')

    elif in_type == Input_types.int32:
        num_elements = 2**14   # Generate 16384 input points
        input_arr = np.zeros(num_elements, dtype=np.int32)
        hvx_vect_len = int((128 * 8) / Input_types.input_type_to_bit_size(in_type))
        num_rows = int(len(input_arr) / hvx_vect_len)
        path = path_generator(mode=Modes_process_output.read_input, input_types=in_type)

        start_point = int(a0_flp * (1 << fxp_format))
        subseg_width_fxp = int(((b0_flp - a0_flp) / 16) * (1 << fxp_format))
        for i in range(16):
            if i < 15:
                input_arr[i * 1024:(i + 1) * 1024] = np.sort(np.random.random_integers(start_point, start_point + subseg_width_fxp, 1024))
            else:
                input_arr[i * 1024:(i + 1) * 1024] = np.sort(np.random.random_integers(start_point, start_point + subseg_width_fxp - 1, 1024))

            start_point += subseg_width_fxp
        np.savetxt(outputer.get_file_path(path), np.reshape(input_arr[0:num_rows * hvx_vect_len], (num_rows, hvx_vect_len)), '%d', newline='\n', delimiter=',')
    elif  in_type == Input_types.float16 or in_type == Input_types.float32:
        # print('generate 32f inputs')
        num_elements = 2**14  # Generate 16384 input points
        input_arr = np.zeros(num_elements, dtype=float)
        hvx_vect_len = int((128 * 8) / Input_types.input_type_to_bit_size(in_type))
        num_rows = int(len(input_arr) / hvx_vect_len)
        path = path_generator(mode=Modes_process_output.read_input, input_types=in_type)

        start_point = a0_flp
        subseg_width = (b0_flp - a0_flp) / 16.0
        for i in range(16):
            input_arr[i * 1024:(i + 1) * 1024] = np.sort(np.random.uniform(start_point, start_point + subseg_width, 1024))

            start_point += subseg_width

        np.savetxt(outputer.get_file_path(path), np.reshape(input_arr[0:num_rows * hvx_vect_len], (num_rows, hvx_vect_len)), '%.10e', newline='\n', delimiter=',')
    return input_arr

# TODO: Finish function and add description
def save_output_fxp(outputer, input_arr, vectorized_func, in_type, out_type, in_fmt, out_fmt):

    hvx_vect_len = int((128 * 8) / Output_types.output_type_to_bit_size(out_type))
    num_rows = int(len(input_arr) / hvx_vect_len)

    output_arr = gen_fxp_output(input_arr, in_fmt, out_fmt, vectorized_func)
    path = path_generator(mode=Modes_process_output.read_np, output_types=out_type)
    np.savetxt(outputer.get_file_path(path), np.reshape(output_arr[0:num_rows * hvx_vect_len], (num_rows, hvx_vect_len)), '%d', newline='\n', delimiter=',')

    return output_arr


def save_output_float(outputer, input_arr, vectorized_func, in_type, out_type, in_fmt, out_fmt):

    hvx_vect_len = int((128 * 8) / Output_types.output_type_to_bit_size(out_type))
    num_rows = int(len(input_arr) / hvx_vect_len)

    if in_type == Input_types.int16 or in_type == Input_types.int32:
        output_arr = gen_float_output_from_fxp(input_arr, in_fmt, out_fmt, vectorized_func)
    else:
        output_arr = gen_float_output(input_arr, vectorized_func)

    path = path_generator(mode=Modes_process_output.read_np, output_types=out_type)
    np.savetxt(outputer.get_file_path(path), np.reshape(output_arr[0:num_rows * hvx_vect_len], (num_rows, hvx_vect_len)), '%.10e', newline='\n', delimiter=',')

    return output_arr


def path_generator(mode, input_types=Input_types.int16, output_types=Output_types.int16) -> str:

    path_start = mode.value
    if mode == Modes_process_output.read_input:
        path_end = Input_types.input_type_to_bit_size_str(input_types)
        path_bit_size = str(Input_types.input_type_to_bit_size(input_types))
    else:
        path_end = Output_types.output_type_to_bit_size_str(output_types)
        path_bit_size = str(Output_types.output_type_to_bit_size(output_types))
    return path_start + path_bit_size + path_end


def get_num_elements():

    global num_elements
    return num_elements
