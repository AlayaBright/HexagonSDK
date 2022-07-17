import re
import struct
from inspect import isfunction, ismethod

# ---

# read a text file, replace multiple words specified in a dictionary
# write the modified text back to a file
def replace_words(text, word_dic):
    """
    take a text and replace words that match a key in a dictionary with
    the associated value, return the changed text
    """

    rc = re.compile('|'.join(map((lambda s: '( )*' + str(s)), map(re.escape, word_dic))))

    def translate(match):
        keyword = match.group(0).lstrip(' ')
        indent_spaces_cnt = len(match.group(0)) - len(keyword)
        indent_spaces = ' ' * indent_spaces_cnt

        f_replace = word_dic[keyword]

        if isfunction(f_replace) or ismethod(f_replace):
            result = f_replace(indent_spaces)
        else:
            result = indent_spaces + str(f_replace)

        return str(result).rstrip('\n')

    return rc.sub(translate, text)


def format_table_str(data_arr, num_of_col: int = 8):
    out_t = ""
    for i in range(0, len(data_arr)):
        if i % num_of_col == 0:
            out_t += "\n"
        out_t += "{:>10},".format(data_arr[i])
    return out_t


# ---


def float_to_fp16(f32: float, round_up: bool = True) -> float:
    ''' return: float with fp16 representation of value '''

    f32_int = struct.unpack('I', struct.pack('f', f32))[0]

    use_round = (f32 < 0) != round_up
    if use_round:
        round_diff = 0x1FFF  # for rounding purpose
        f32_int += round_diff

    fp32_for_fp16 = f32_int & 0xFFFFE000  # cut mantissa for 23 - 10 = 13 bits
    fp16_value = struct.unpack('f', struct.pack('I', fp32_for_fp16))[0]

    return fp16_value


def float_to_fp16_hex(f: float) -> str:
    ''' Represent floating-point 32bit number in 16bit hexadecimal format. Without rounding. '''

    f_int = struct.unpack('I', struct.pack('f', f))[0]
    conversion_to_fp16 = ((f_int >> 16) & 0x8000) | (
        (((f_int & 0x7f800000) - 0x38000000) >> 13) & 0x7c00) | ((f_int >> 13) & 0x03ff)

    return hex(conversion_to_fp16)


def get_valid_scale_fp16(A0: float, B0: float) -> float:
    X = B0 - A0

    scale = 16.0/(B0 - A0)

    scale = float_to_fp16(scale)

    q_scale = scale / 2**10  # quant to get next number

    while True:
        test = X * scale
        test_fp16 = float_to_fp16(test)
        scale -= q_scale
        scale = float_to_fp16(scale)
        # print(f"X-A0= {X} x scale= {scale} > test= {test} ||fp16: {test_fp16}")
        if test_fp16 < 16:
            break

    return scale


# Represent floating-point number in hexadecimal format
def float_to_fp32_hex(f):
    return hex(struct.unpack('<I', struct.pack('<f', f))[0])


def get_valid_scale_fp32(A0: float, B0: float) -> float:
    X = B0 - A0

    scale = 16.0/(B0 - A0)

    q_scale = scale / 2**23  # quant to get next number

    while True:
        test = X * scale
        # print(f"X-A0= {X} x scale= {scale} > test= {test}")
        if test < 16:
            break
        scale -= q_scale

    return scale
