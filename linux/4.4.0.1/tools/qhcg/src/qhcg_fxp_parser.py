import math
import numpy as np

# Num signed bit in fixed-point representation
num_signed_bit = 1
number_of_segments = 16

def __find_fmt(value, fxp_fmt_size):

    epsilon = 0.0000001
    value -= epsilon

    q_fmt = fxp_fmt_size - num_signed_bit

    if value > 1:
        q_fmt_int_part = math.ceil(math.log2(value))
        q_fmt -= q_fmt_int_part

    return q_fmt


# Find fixed-point format for array
def __find_fmt_arr(data_array, fxp_fmt_size):

    max_value = np.max(np.abs(data_array))
    return __find_fmt(max_value, fxp_fmt_size)


# Brief: Get fixed-point format for desired range
#
#     a0: Start point of input range - float value
#     b0: End point of input range -  float value
#     fxp_size: Number of bits of input type, e.g for 'int16_t' type fxp_size_in = 16
#
# return: qfmt : Returns suggested Q format representing the input
def find_qfmt_in_range(a0, b0, fxp_fmt_size):

    return __find_fmt_arr(np.array([a0, b0]), fxp_fmt_size)


# Brief: Get fixed-point format for input/output data
#
#     function: Desired function specified as string expression e.g 'exp(2*x) + x'
#     a0: Start point of input range - float value
#     b0: End point of input range -  float value
#     fxp_size_in: Number of bits of input type, e.g for 'int16_t' type fxp_size_in = 16
#     fxp_size_out: Number of bits of output type, e.g for 'int16_t' type fxp_size_in = 16
#
# return: [qfmt_in, qfmt_out] <=> qfmt_in: Input fixed-point format in range [0, fxp_size_in - 1]
#                                 qfmt_out: Output fixed-point format in range [0, fxp_size_out - 1]
def find_qfmt_in_out(computable_function, a0, b0, fxp_size_in, fxp_size_out):

    qfmt_in = __find_fmt_arr(np.array([a0, b0]), fxp_size_in)

    test_input = np.linspace(a0, b0, 10000)
    func_output = computable_function(test_input)
    qfmt_out = __find_fmt_arr(func_output, fxp_size_out)

    assert (qfmt_in > 0.0), 'Invalid input format!!!'
    assert (qfmt_out > 0.0), 'Invalid output format!!!'

    return [qfmt_in, qfmt_out]


def __find_fmt_per_seg(x, coeffs, coeffs_num_bits, in_fmt, poly_order):

    coeffs_fmt = np.zeros(poly_order + 1)

    y_curr = x * coeffs[0]
    coeff_fmt_curr = __find_fmt(coeffs[0], coeffs_num_bits)

    y_curr_fmt = __find_fmt_arr(y_curr, coeffs_num_bits)

    coeffs_fmt[0] = min(y_curr_fmt - in_fmt + 15, coeff_fmt_curr)

    y_curr = y_curr + coeffs[1]
    coeff_fmt_curr = __find_fmt(coeffs[1], coeffs_num_bits)

    y_curr_fmt = __find_fmt_arr(y_curr, coeffs_num_bits)

    coeffs_fmt[1] = min(y_curr_fmt - in_fmt + 15, coeff_fmt_curr)

    start_idx = 2
    for i in range(start_idx, poly_order + 1):
        y_curr = y_curr * x

        y_curr_fmt = __find_fmt_arr(y_curr, coeffs_num_bits)

        # If next multiply produce overflow, some refinement previous coefficient need to do
        if y_curr_fmt < in_fmt - coeffs_fmt[i - 1] + 15:
            coeffsFMT_i_prev = coeffs[i - 1]
            coeffs_fmt[i - 1] = y_curr_fmt - in_fmt + 15

            # Check is the overflow occured during first multiply
            if i == 2:
                coeffs[i - 2] -= coeffsFMT_i_prev - coeffs[i - 1]

        curr_coeff_fmt = __find_fmt(coeffs[i], coeffs_num_bits)

        y_curr = np.add(y_curr, coeffs[i])

        y_curr_fmt = __find_fmt_arr(y_curr, coeffs_num_bits)

        coeffs_fmt[i] = min(y_curr_fmt, curr_coeff_fmt)

    coeffs_fmt = np.array(coeffs_fmt, np.int32)

    return coeffs_fmt


def find_qfmt_coeffs_full_range(a0, b0, coeffs, in_fmt, poly_order, coeffs_num_bits):

    coeffs_fmt_mat = np.ndarray(shape=(16, poly_order + 1))
    coeffs_fmt_init = np.ndarray(shape=(16, poly_order + 1))
    sub_seg_width = (b0 - a0)/16
    coeffs_final = np.zeros(poly_order + 1)
    N_subseg = 1000

    start_seg = a0
    for i in range(16):
        x_sub_seg = np.linspace(start_seg, start_seg + sub_seg_width, N_subseg)
        coeffs_fmt_mat[i, :] = __find_fmt_per_seg(x_sub_seg, coeffs[i], coeffs_num_bits, in_fmt, poly_order)
        start_seg += sub_seg_width

    for i in range(16):
        for j in range(poly_order + 1):
            if abs(coeffs[i][j]) > 1.0:
                coeffs_fmt_init[i, j] = coeffs_num_bits - math.floor(math.log2(abs(coeffs[i][j]) + 1)) - 1
            else:
                coeffs_fmt_init[i, j] = coeffs_num_bits - 1

    coeffs_fmt_mat = np.minimum(coeffs_fmt_mat, coeffs_fmt_init)

    for i in range(poly_order + 1):
        coeffs_final[i] = np.min(coeffs_fmt_mat[:, i])

    return coeffs_final


# Brief: Convert float value to fixed-point in desired format
#
#     input_val: Input floating-point value
#     qfmt: Desired format for fixed-point
#
# return: Converted fixed-point value in qfmt format
#
def float_2_qfxp(input_val, qfmt):

    output = round(input_val * (1 << qfmt))
    output = int(output)

    return output


# Brief: Convert floating-point array to fixed-point array in desired format
#
#     input_arr: Input floating-point array
#     qfmt: Desired format for fixed-point
#
# return: Converted fixed-point array in qfmt format
#
def float_2_qfxp_arr(input_arr, qfmt):

    output_arr = input_arr * abs(float(2**qfmt))
    output_arr = np.array(output_arr, np.int64)

    return output_arr


# Brief: Convert fixed-point array  to floating-point array
#
#     input_arr: Input fixed-point array in specified format
#     qfmt: Desired format for fixed-point
#
# return: Converted floating-point value
#
def qfxp_2_float_arr(input_arr, qfmt):

    output_arr = input_arr / (1 << qfmt)
    return output_arr


# Helper function to calculate index for VLUT
# Used Q6_Vh_vmpy_VhRh_s1_rnd_sat intrinsinc
def __calc_idx(X, A0, scaled_factor, range_over_sign=False):

    X_sub = X - A0
    X_idx = X_sub * scaled_factor
    X_idx += 0x8000  # round mechanism like Q6_Vh_vmpy_VhRh_s1_rnd_sat instruction
    idx = X_idx >> 23
    if range_over_sign is True:
        idx >>= 1
    return idx


### Calculate scale factor
def calc_scaled_factor(a0, b0, in_fmt, bit_cnt=16):
    ''' return: A0, B0, scaled_factor, first_shift '''

    A0 = float_2_qfxp(a0, in_fmt)
    B0 = float_2_qfxp(b0, in_fmt)

    range_width = b0 - a0
    range_width_fxp = float_2_qfxp(range_width, in_fmt)

    range_over_sign = range_width_fxp >= 2**(bit_cnt - 1)
    additional_lsr = 1 if range_over_sign else 0

    if bit_cnt == 32:
        A0 >>= 16
        B0 >>= 16
        range_width_fxp = range_width_fxp >> 16

    if range_width_fxp < number_of_segments:
        raise "Error: Input range underflow exception - Provided narrow input range is not supported by the QHCG tool!"

    scaled_factor_f = number_of_segments / range_width_fxp

    # Index for VLUT is stored from the 24th position in the result of multiply
    # 23= 24-1 : The instruction for multiply (Q6_Vh_vmpy_VhRh_s1_rnd_sat) shift one more bit
    scaled_factor_f *= 2**(23 + additional_lsr)

    scaled_factor = int(scaled_factor_f)

    # Test scaled factor for (number_of_segments) segments
    # Maximal returned number must be (number_of_segments - 1)
    while True:
        idx = __calc_idx(B0, A0, scaled_factor, range_over_sign)
        if idx < number_of_segments:
            break
        scaled_factor -= 1

    # instruction requirements: multiply with signed short
    while scaled_factor >= 2**(16 - 1):
        scaled_factor >>= 1

    return A0, B0, scaled_factor, additional_lsr
