import math as m
from pathlib import Path
import numpy as np
import qhcg_fxp_parser as fxp_parser
import qhcg_gen_data as gen_data
from qhcg_config import Input_types, Output_types, Modes_process_output
from qhcg_helper import replace_words, format_table_str

# Number of bits which represent coefficients, inputs & outputs respectively
coeffs_size = 32
input_size = 32
output_size = 32

def check_implement_with_shift(coeffs, in_fmt, out_fmt, poly_order):

    max_coeff = np.max(np.abs(coeffs))

    if max_coeff > 1.0:
        coeff_fmt = coeffs_size - int(m.floor(m.log2(max_coeff) + 1)) - 1
    else:
        coeff_fmt = coeffs_size - 1

    if (coeff_fmt - poly_order*(input_size - 1 - in_fmt)) < (out_fmt - 5):
        return True
    else:
        return False


class GeneratorHVX32b(object):

    def __init__(self, function, computable_function, outputer, a0, b0, coeffs, poly_order):

        input_path = gen_data.path_generator(mode=Modes_process_output.read_input, input_types=Input_types.int32)
        output_path = gen_data.path_generator(mode=Modes_process_output.read_HVX, output_types=Output_types.int32)

        self.outputer = outputer
        self.a0 = a0
        self.b0 = b0
        self.coeffs = coeffs
        self.coeffs_arr = []
        self.order = poly_order
        self.segments = 16
        self.eval_with_shift = False
        self.coeff_fmts = None

        # Determine input/output format
        (self.in_fmt, self.out_fmt) = fxp_parser.find_qfmt_in_out(computable_function, a0, b0, 32, 32)

        self.A0, self.B0, self.scaled_factor, self.first_shift = fxp_parser.calc_scaled_factor(a0, b0, self.in_fmt, 32)

        max_coeff = np.max(np.abs(self.coeffs))

        if max_coeff > 1:
            if self.in_fmt < 31:
                fmt_1st_coeff = coeffs_size - int(m.floor(m.log2(max_coeff) + 1)) - 1
            else:
                fmt_1st_coeff = coeffs_size - int(m.floor(m.log2(max_coeff) + 1)) - 2
        else:
            fmt_1st_coeff = 31 # If all coefficients are less than 1.0 set format to q31

        if check_implement_with_shift(self.coeffs, self.in_fmt, self.out_fmt, self.order) == True:
            self.coeff_fmts = self.get_coeffs_fmt_with_sht()
            self.eval_with_shift = True
        else:
            self.eval_with_shift = False
            # Decrease fixed-point format in order to ensure nooverflow
            self.coeffs_fmt = fmt_1st_coeff

            self.coeffs_fmt = fmt_1st_coeff - self.order * (31 - self.in_fmt)

            # Fix overflow issue when coeff_fmt is greater than output_fmt required.
            # Overflow occurs for exp2 function with input range between 0 and 1
            # where calculated coeffs_fmt is 31 and output_fmt required is 30.

            if self.out_fmt < self.coeffs_fmt:
                self.coeffs_fmt = self.out_fmt
            else:
                self.out_fmt = self.coeffs_fmt

        # print('Input format acc: ' + str(self.in_fmt))
        # print('Output format acc: ' + str(self.out_fmt))

        self.key_maps = {
            'QHCG_KEY_FILENAME': self.outputer.get_hvx_file_name(),
            'QHCG_KEY_FUNCTNAME': self.outputer.get_hvx_func_name(),
            'QHCG_KEY_FUNCTION_STRING': str(function),
            'QHCG_KEY_INPUT_RANGE_MIN': str(self.a0),
            'QHCG_KEY_INPUT_RANGE_MAX': str(self.b0),
            'QHCG_KEY_INPUT_FORMAT': self.getInputFMT(),
            'QHCG_KEY_OUTPUT_FORMAT': self.getOutputFMT(),
            "QHCG_KEY_NUM_SEGMENTS": str(self.segments),
            "QHCG_KEY_POLY_ORDER": str(self.order),
            "QHCG_KEY_A0": self.getQHCG_KEY_HVX_A0_VECT,
            "QHCG_KEY_SUB_LSR": self.getQHCG_KEY_SUB_LSR,
            "QHCG_KEY_HVX_VECTOR_DECL": self.getQHCG_KEY_HVX_VECTOR_DECL,
            "QHCG_KEY_COEFFS_FXP": self.getQHCG_KEY_COEFFS_FXP(),
            "QHCG_KEY_SPLIT_COEFF": self.getQHCG_KEY_SPLIT_COEFF,
            "QHCG_KEY_VLUT": self.getQHCG_KEY_VLUT,
            "QHCG_KEY_COMPOSE_COEFF": self.getQHCG_KEY_COMPOSE_COEFF,
            "QHCG_KEY_POLY_EVAL": self.getQHCG_KEY_POLY_EVAL,
            "QHCG_KEY_INPUT_FILENAME": self.outputer.get_file_path(input_path).replace('\\', '/'),
            "QHCG_KEY_OUTPUT_FILENAME": self.outputer.get_file_path(output_path).replace('\\', '/'),
            "QHCG_KEY_INPUT_TYPE": self.getQHCG_KEY_HVX_INPUT(),  # TODO: can be uint16_t, uint32_t or float
            "QHCG_KEY_OUTPUT_TYPE": self.getQHCG_KEY_HVX_OUTPUT(),  # TODO: can be uint16_t, uint32_t or float
            "QHCG_KEY_STR_TO_NUMBER_FUNC": 'atoi',  # atoi or atof
            "QHCG_KEY_OUTPUT_PRINTF_FORMAT": 'li', # TODO: format for uh

            "QHCG_KEY_SCALE_FACT": self.getQHCG_KEY_SCALE_FACT,

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
            "c": "qhcg_hvx_poly32b.c",
            "h": "qhcg_gen_hvx_func_fxp.h",
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
        one_a = indent_spaces + 'HVX_VectorPair c{0}_coeff_l_vp, c{0}_coeff_h_vp, c{0}_coeff_d_vp;\n' + \
                indent_spaces + 'HVX_VectorPair c{0}_even_odd_vp;\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)
        return out_str

    def getQHCG_KEY_COEFFS_FXP(self):

        if self.eval_with_shift == True:
            for i in range(self.order + 1):
                coeff_seg = np.zeros(self.segments)
                for j in range(self.segments):
                    coeff_seg[j] = self.coeffs[j][self.order - i]

                coeff_fxp = np.array(coeff_seg * (2**int(self.coeff_fmts[self.order - i]) - 1), np.int64)
                coeff_fxp_sat = coeff_fxp.clip(-2**31, 2**31 - 1)
                self.coeffs_arr.append(coeff_fxp_sat)
        else:
            coeffs_fmt_curr = self.coeffs_fmt
            for i in range(self.order + 1):
                coeff_seg = np.zeros(self.segments)
                for j in range(self.segments):
                    coeff_seg[j] = self.coeffs[j][self.order - i]

                coeff_fxp = np.array(np.round(coeff_seg * ((1 << coeffs_fmt_curr) - 1)), np.int64)
                coeff_fxp_sat = coeff_fxp.clip(-2**31, 2**31 - 1)
                self.coeffs_arr.append(coeff_fxp_sat)
                coeffs_fmt_curr += (input_size - self.in_fmt - 1)

        one_a = 'static const int32_t c{}_coeffs[{}] __attribute__((aligned(VLEN))) =\n' \
                '{{' \
                '{}\n' \
                '}};\n'
        out_str = ''
        for i in range(self.order + 1):
            coeff_arr_idx = format_table_str(self.coeffs_arr[i])
            out_str += one_a.format(i, self.segments, coeff_arr_idx)

        return out_str

    def getQHCG_KEY_SPLIT_COEFF(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'c{0}_coeff_d_vp = Q6_Wuw_vzxt_Vuh(*((HVX_Vector *)(c{0}_coeffs)));\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)

        return out_str

    def getQHCG_KEY_VLUT(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'c{0}_coeff_l_vp = Q6_Wh_vlut16_VbVhR(vlut_indexes_v, Q6_V_lo_W(c{0}_coeff_d_vp), 0);\n' + \
                indent_spaces + 'c{0}_coeff_h_vp = Q6_Wh_vlut16_VbVhR(vlut_indexes_v, Q6_V_hi_W(c{0}_coeff_d_vp), 0);\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)
            out_str += '\n'
        return out_str

    def getQHCG_KEY_COMPOSE_COEFF(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'c{0}_even_odd_vp = Q6_W_vdeal_VVR(Q6_V_hi_W(c{0}_coeff_h_vp), Q6_V_hi_W(c{0}_coeff_l_vp), 2);\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)
            out_str += '\n'
        return out_str

    def getQHCG_KEY_POLY_EVAL(self, indent_spaces: str = ''):

        # Get all coefficients by number
        if self.eval_with_shift == True:
            for i in range(self.segments):
                sht_fact = np.zeros(self.order, np.int)
                for j in range(self.order):
                    sht_fact[j] = self.coeff_fmts[j + 1]  - (self.coeff_fmts[j] - (input_size - 1 - self.in_fmt))

        one_a = indent_spaces + 'y_even_odd_dv.V.lo = Q6_Vw_vmpye_VwVuh(Q6_V_lo_W({in_c}_even_odd_vp{dot_V_l_c}), sline1);\n' + \
                indent_spaces + 'y_even_odd_dv.V.lo = Q6_Vw_vmpyoacc_VwVwVh_s1_rnd_sat_shift(y_even_odd_dv.V.lo, Q6_V_lo_W({in_c}_even_odd_vp{dot_VV_c}), sline1);\n' + \
                indent_spaces + 'y_even_odd_dv.V.hi = Q6_Vw_vmpye_VwVuh(Q6_V_hi_W({in_c}_even_odd_vp{dot_V_l_c}), sline2);\n' + \
                indent_spaces + 'y_even_odd_dv.V.hi = Q6_Vw_vmpyoacc_VwVwVh_s1_rnd_sat_shift(y_even_odd_dv.V.hi, Q6_V_hi_W({in_c}_even_odd_vp{dot_VV_c}), sline2);\n' + \
                '{sht_fact}' + \
                indent_spaces + 'y_even_odd_dv.VV = Q6_Ww_vadd_WwWw_sat(y_even_odd_dv.VV, c{idx}_even_odd_vp);\n' + \
                '\n'

        one_a_2nd_pattern = indent_spaces + 'y_tmp_even_v = Q6_Vw_vmpye_VwVuh(Q6_V_lo_W({in_c}_even_odd_dv{dot_V_l_c}), sline1);\n' + \
            indent_spaces + 'y_even_odd_dv.V.lo = Q6_Vw_vmpyoacc_VwVwVh_s1_rnd_sat_shift(y_tmp_even_v, Q6_V_lo_W({in_c}_even_odd_dv{dot_VV_c}), sline1);\n' + \
            indent_spaces + 'y_tmp_odd_v = Q6_Vw_vmpye_VwVuh(Q6_V_hi_W({in_c}_even_odd_dv{dot_V_l_c}), sline2);\n' + \
            indent_spaces + 'y_even_odd_dv.V.hi = Q6_Vw_vmpyoacc_VwVwVh_s1_rnd_sat_shift(y_tmp_odd_v, Q6_V_hi_W({in_c}_even_odd_dv{dot_VV_c}), sline2);\n' + \
            '{sht_fact}' + \
            indent_spaces + 'y_even_odd_dv.VV = Q6_Ww_vadd_WwWw_sat(y_even_odd_dv.VV, c{idx}_even_odd_vp);\n'\
            '\n'

        out_str = ''

        dot_V_l = ''
        dot_V_h = ''
        dot_VV = ''
        for i in reversed(range(self.order)):
            if self.eval_with_shift == True:
                if sht_fact[self.order - i - 1] < 0.0:
                    shift_str = indent_spaces + 'y_even_odd_dv.V.lo = Q6_Vw_vasr_VwR(y_even_odd_dv.V.lo,' + str(abs(sht_fact[self.order - i - 1])) + ');\n' + \
                                indent_spaces + 'y_even_odd_dv.V.hi = Q6_Vw_vasr_VwR(y_even_odd_dv.V.hi,' + str(abs(sht_fact[self.order - i - 1])) + ');\n'
                elif sht_fact[self.order - i - 1] > 0.0:
                    shift_str = indent_spaces + 'y_even_odd_dv.V.lo = Q6_Vw_vasl_VwR(y_even_odd_dv.V.lo,' + str(sht_fact[self.order - i - 1]) + ');\n' + \
                                indent_spaces + 'y_even_odd_dv.V.hi = Q6_Vw_vasl_VwR(y_even_odd_dv.V.hi,' + str(sht_fact[self.order - i - 1]) + ');\n'
                else:
                    # print('Shift are not neccessary')
                    shift_str = ''
            else:
                shift_str = ''

            # Decision whether is need to shift right or left
            in_val = "c{}".format(i+1) if i is self.order-1 else 'y'

            if i == (self.order-1):
                out_str += one_a.format(in_c=in_val, idx=i, sht_fact=shift_str, dot_V_l_c=dot_V_l, dot_V_h_c=dot_V_h, dot_VV_c=dot_VV)
            else:
                out_str += one_a_2nd_pattern.format(in_c=in_val, idx=i, sht_fact=shift_str, dot_V_l_c=dot_V_l, dot_V_h_c=dot_V_h, dot_VV_c=dot_VV)

            dot_V_h = '.VV'
            dot_V_l = '.VV'
            dot_VV = '.VV'

        return out_str

    def getQHCG_KEY_SCALE_FACT(self, indent_spaces: str = ''):

        one_a = indent_spaces + 'vlut_indexes_v = Q6_Vh_vmpy_VhRh_s1_rnd_sat(input_shifted_v, {scaled_factor});\n'

        scaled_factor_str = str('0x{0:0{1}x}{0:0{1}x}'.format(self.scaled_factor, 4))

        return one_a.format(scaled_factor=scaled_factor_str)

    def getQHCG_KEY_SUB_LSR(self, indent_spaces: str = ''):

        if self.first_shift is not 0:
            one_a = indent_spaces + 'input_shifted_v = Q6_Vuh_vlsr_VuhR(input_shifted_v, {arg});\n'
            return one_a.format(arg=self.first_shift)
        else:
            return ''

    def getQHCG_KEY_HVX_A0_VECT(self, indent_spaces: str = ''):

        one_a = indent_spaces + 'input_min_v = Q6_Vh_vsplat_R({});\n'

        return one_a.format(self.A0)

    def getQHCG_KEY_HVX_INPUT(self):

        one_a = 'qhlw_q{}_t'

        return one_a.format(str(self.in_fmt))

    def getQHCG_KEY_HVX_OUTPUT(self):

        one_a = 'qhlw_q{}_t'

        return one_a.format(str(self.out_fmt))

    def getQHCG_KEY_NUM_BENCH_ELEM(self):
        return str(gen_data.get_num_elements())

    # Get oup8ut format
    def getOutputFMT(self):

        return self.out_fmt

    def getInputFMT(self):

        return self.in_fmt

    def get_coeffs_fmt_with_sht(self):
        coeff_seg = np.zeros(self.segments)
        # Formats for each coefficients particularly
        coeff_fmts = np.zeros((self.order + 1), np.int64)
        # Get initial coefficient format
        for i in range(self.order + 1):
            for j in range(self.segments):
                coeff_seg[j] = self.coeffs[j][i]

            max_coeff = np.max(np.abs(coeff_seg))

            if max_coeff > 1.0:
                coeff_fmts[i] = coeffs_size - int(m.floor(m.log2(max_coeff) + 1)) - 1
            else:
                coeff_fmts[i] = coeffs_size - 1

        # print('Coeff FMTs: ' + str(coeff_fmts))

        subseg_width = (self.b0 - self.a0)/self.segments
        for i in range(self.segments):
            x = np.linspace(self.a0 + i*subseg_width, self.a0 + (i+1)*subseg_width, 1024)
            y = np.zeros(1024)
            for j in range(self.order):
                if j == 0:
                    y = x*self.coeffs[i][j]

                    if (np.max(np.abs(y)) >= (1 << (coeffs_size - 1 - coeff_fmts[j + 1]))):
                        coeff_fmts[j + 1] -= 1

                    y += self.coeffs[i][j + 1]

                    if (np.max(np.abs(y)) >= (1 << (coeffs_size - 1 - coeff_fmts[j + 1]))):
                        coeff_fmts[j + 1] -= 1

                else:
                    y = y * x

                    if (np.max(np.abs(y)) >= (1 << (coeffs_size - 1 - coeff_fmts[j + 1]))):
                        coeff_fmts[j + 1] -= 1

                    y += self.coeffs[i][j + 1]

                    if (np.max(np.abs(y)) >= (1 << (coeffs_size - 1 - coeff_fmts[j + 1]))):
                        coeff_fmts[j + 1] -= 1



        self.out_fmt = coeff_fmts[self.order]

        return coeff_fmts
