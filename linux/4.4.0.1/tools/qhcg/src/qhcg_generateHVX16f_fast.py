import numpy as np
import qhcg_gen_data as gen_data
from qhcg_config import Input_types, Output_types, Modes_process_output
from pathlib import Path
import math as m
from qhcg_helper import replace_words
from qhcg_helper import format_table_str
from qhcg_helper import float_to_fp16, float_to_fp16_hex, get_valid_scale_fp16

# Number of bits which represent coefficients, inputs & outputs respectively
coeffs_size = 16
input_size = 16
output_size = 16


class GeneratorHVX16f_fast(object):

    def __init__(self, function, outputer, a0, b0, coeffs, poly_order):

        input_path = gen_data.path_generator(mode=Modes_process_output.read_input, input_types=Input_types.float16)
        output_path = gen_data.path_generator(mode=Modes_process_output.read_HVX, output_types=Output_types.float16)

        self.outputer = outputer
        self.a0 = float_to_fp16(a0, False)
        self.b0 = float_to_fp16(b0, True)
        self.coeffs = coeffs
        self.coeffs_arr = []
        self.order = poly_order
        self.segments = 16
        self.eval_with_shift = False

        self.a0_fp16_hex = float_to_fp16_hex(self.a0)

        self.scale_fp16 = get_valid_scale_fp16(self.a0, self.b0)
        self.scale_fp16_hex = float_to_fp16_hex(self.scale_fp16)

        # print(f"Convert from input range:({a0})-({b0}) to fp16 input range: ({self.a0})-({self.b0})")

        self.key_maps = {
            'QHCG_KEY_FILENAME': self.outputer.get_hvx_file_name(),
            'QHCG_KEY_FUNCTNAME': self.outputer.get_hvx_func_name(),
            'QHCG_KEY_FUNCTION_STRING': str(function),
            'QHCG_KEY_INPUT_RANGE_MIN': str(self.a0),
            'QHCG_KEY_INPUT_RANGE_MAX': str(self.b0),
            "QHCG_KEY_NUM_SEGMENTS": str(self.segments),
            "QHCG_KEY_POLY_ORDER": str(self.order),
            "QHCG_KEY_A0": self.getQHCG_KEY_HVX_A0_VECT,
            "QHCG_KEY_HVX_VECTOR_DECL": self.getQHCG_KEY_HVX_VECTOR_DECL,
            "QHCG_KEY_COEFFS": self.getQHCG_KEY_COEFFS(),
            "QHCG_KEY_SPLIT_COEFF": self.getQHCG_KEY_SPLIT_COEFF,
            "QHCG_KEY_VLUT": self.getQHCG_KEY_VLUT,
            "QHCG_KEY_POLY_EVAL": self.getQHCG_KEY_POLY_EVAL,
            "QHCG_KEY_INPUT_FILENAME": self.outputer.get_file_path(input_path).replace('\\', '/'),
            "QHCG_KEY_OUTPUT_FILENAME": self.outputer.get_file_path(output_path).replace('\\', '/'),
            "QHCG_KEY_INPUT_TYPE": self.getQHCG_KEY_HVX_INPUT(),  # TODO: can be uint16_t, uint32_t, __fp or float
            "QHCG_KEY_OUTPUT_TYPE": self.getQHCG_KEY_HVX_OUTPUT(),  # TODO: can be uint16_t, uint32_t, __fp or float
            "QHCG_KEY_STR_TO_NUMBER_FUNC": 'atof',  # atoi or atof
            "QHCG_KEY_OUTPUT_PRINTF_FORMAT": '.10e', # TODO: format for uh

            "QHCG_KEY_SCALE_FACT": self.getQHCG_KEY_SCALE_FACT,
            "QHCG_KEY_LOAD_COEFF_ARR":self.getQHCG_KEY_LOAD_COEFF_ARR,
            "QHCG_KEY_SF_2_Q32":self.getQHCG_KEY_SF_2_Q32,

            # KEYs for bench file
            "QHCG_KEY_NUM_BENCH_ELEM": self.getQHCG_KEY_NUM_BENCH_ELEM(),
        }


    def generate_of(self, template_path, output_path):
        '''
        Use string for paths
        '''

        try:
            fin = open(template_path, "r")
            str_template = fin.read()
            fin.close()
        except FileNotFoundError:
            print("Error: File doesn't exist: " + template_path)
            return

        # call the function and get the changed text
        str_generated = replace_words(str_template, self.key_maps)

        try:
            # write changed text back out
            fout = open(output_path, "w")
            fout.write(str_generated)
            fout.close()
        except FileNotFoundError:
            print("Error: Problem with write into file: " + output_path)
            return

    def generate(self, file_type='all'):
        '''
        Use 'all' for all: 'c' or 'h' or 'test' for file_type
        '''

        if file_type is 'all':
            self.generate('c')
            self.generate('h')
            self.generate('qhcg_internal')
            self.generate('test')
            self.generate('bench')
            return

        template_paths = {
            "c": "qhcg_hvx_poly16f_fast.c",
            "h": "qhcg_gen_hvx_func_float.h",
            "test": "qhcg_test_gen_hvx_func.c",
            "bench": "qhcg_bench_gen_hvx_func.c",
            "qhcg_internal": "qhcg_internal.h"
        }

        f_rel_path = template_paths.get(file_type, 'Invalid')
        if f_rel_path == 'Invalid':
            print("File type is not supported.")
            return

        template_root = Path(__file__).resolve().parent.joinpath('templates')
        template_file = template_root.joinpath(f_rel_path)
        output_path = self.outputer.get_file_path(file_type)
        self.generate_of(str(template_file), str(output_path))

    def getQHCG_KEY_HVX_VECTOR_DECL(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'HVX_VectorPair c{0}_coeff_vp;\n' + \
                indent_spaces + 'HVX_Vector c{0}_coeff_v_hf;\n' + \
                indent_spaces + 'HVX_Vector c{0}_coeff_v_qf16;\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)
        return out_str

    def getQHCG_KEY_COEFFS(self):
        one_a = 'static const __fp16 c{}_coeffs[{}] __attribute__((aligned(VLEN))) =\n' \
                '{{' \
                '{}\n' \
                '}};\n'
        out_str = ''
        for i in range(self.order + 1):
            coeff_seg = np.zeros(2 * self.segments, dtype=float)
            for j in range(2 * self.segments):
                if (j % 2) == 0:
                    coeff_seg[j] = self.coeffs[int(j / 2)][self.order - i]
                else:
                    coeff_seg[j] = 0.0
            coeff_arr_idx = format_table_str(coeff_seg)
            out_str += one_a.format(i, 2 * self.segments, coeff_arr_idx)

        return out_str

    def getQHCG_KEY_LOAD_COEFF_ARR(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'c{0}_coeff_v_hf = *((HVX_Vector *)(c{0}_coeffs));\n'

        out_str = ''

        for i in range(self.order + 1):\
            out_str += one_a.format(i)

        return out_str
# TODO: clear execess
    def getQHCG_KEY_SF_2_Q32(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'c{0}_coeff_v_qf16 = Q6_Vqf16_vadd_VhfVhf(c{0}_coeff_v_hf, zero_v);\n'

        out_str = ''

        for i in range(self.order + 1):
            out_str += one_a.format(i)

        return out_str

    def getQHCG_KEY_SPLIT_COEFF(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'c{0}_coeff_d_vp.VV = Q6_Wuw_vzxt_Vuh(c{0}_coeff_v_qf16);\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)

        return out_str

    def getQHCG_KEY_VLUT(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'c{0}_coeff_vp = Q6_Wh_vlut16_VbVhI(indexes_v, c{0}_coeff_v_qf16, 0);\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)
        return out_str

    def getQHCG_KEY_SCALE_FACT(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'scale_v_hf = Q6_Vh_vsplat_R({});\n'

        return one_a.format(self.scale_fp16_hex)

    def getQHCG_KEY_HVX_A0_VECT(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'input_min_v_hf = Q6_Vh_vsplat_R({});\n'

        return one_a.format(self.a0_fp16_hex)

    def getQHCG_KEY_POLY_EVAL(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'output_dv = Q6_Vqf16_vmpy_Vqf16Vqf16(Q6_V_lo_W(c{idx_1}_coeff_vp), input_v_qf16);\n' + \
                indent_spaces + 'output_dv = Q6_Vqf16_vadd_Vqf16Vqf16(output_dv, Q6_V_lo_W(c{idx_2}_coeff_vp));\n'

        out_str = ''

        dot_VV = ''
        for i in reversed(range(self.order)):
            if (i == (self.order - 1)):
                out_str += one_a.format(idx_1=str(self.order), idx_2=str(i))
            else:
                one_a = indent_spaces + 'output_dv = Q6_Vqf16_vmpy_Vqf16Vqf16(output_dv, input_v_qf16);\n' + \
                        indent_spaces + 'output_dv = Q6_Vqf16_vadd_Vqf16Vqf16(output_dv, Q6_V_lo_W(c{}_coeff_vp));\n'
                out_str += one_a.format(i)

        return out_str

    def getQHCG_KEY_HVX_INPUT(self):

        return '__fp16'

    def getQHCG_KEY_HVX_OUTPUT(self):

        return '__fp16'

    def getQHCG_KEY_NUM_BENCH_ELEM(self):
        return str(gen_data.get_num_elements())

    # Get oup8ut format
    def getOutputFMT(self):

        return 15

    def getInputFMT(self):

        return 15
