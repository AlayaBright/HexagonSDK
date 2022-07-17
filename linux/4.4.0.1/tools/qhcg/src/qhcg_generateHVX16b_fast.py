import numpy as np
import qhcg_fxp_parser as fxp_parser
import qhcg_gen_data as gen_data
from qhcg_config import Input_types, Output_types, Modes_process_output
from pathlib import Path
import math as m
import sys
from qhcg_helper import replace_words, format_table_str

# Number of bits for coefficients
coeffs_size = 16

def check_shift_req(coeffs, in_fmt, out_fmt, poly_order):
    max_coeffs = np.max(np.abs(coeffs))

    if max_coeffs > 1:
        coeff_fmt = coeffs_size - int(m.floor(m.log2(max_coeffs) + 1)) - 1
    else:
        coeff_fmt = coeffs_size - 1

    if (coeff_fmt - poly_order * (15 - in_fmt)) < (out_fmt - 2):
        return True
    else:
        return False


class GeneratorHVX16b_fast(object):

    def __init__(self, function, computable_function, outputer, a0, b0, coeffs, poly_order):

        input_path = gen_data.path_generator(mode=Modes_process_output.read_input, input_types=Input_types.int16)
        output_path = gen_data.path_generator(mode=Modes_process_output.read_HVX, output_types=Output_types.int16)

        self.outputer = outputer
        self.a0 = a0
        self.b0 = b0
        self.coeffs = coeffs
        self.coeffs_arr = []
        self.order = poly_order
        self.segments = 16
        self.shift_req = False

        # Determine input/output format
        (self.in_fmt, self.out_fmt) = fxp_parser.find_qfmt_in_out(computable_function, a0, b0, 16, 16)

        self.A0, self.B0, self.scaled_factor, self.first_shift = fxp_parser.calc_scaled_factor(a0, b0, self.in_fmt)

        if (check_shift_req(self.coeffs, self.in_fmt, self.out_fmt, self.order) == True):
            self.shift_req = True

            self.coeff_fmts = self.get_coeffs_fmt_with_sht()
        else:

            max_coeff = np.max(np.abs(self.coeffs))

            if max_coeff > 1:
                if (self.in_fmt < 15):
                    fmt_1st_coeff = coeffs_size - int(m.floor(m.log2(max_coeff) + 1)) - 1
                else:
                    fmt_1st_coeff = coeffs_size - int(m.floor(m.log2(max_coeff) + 1)) - 2
            else:
                if (self.out_fmt == 15):
                    fmt_1st_coeff = 15 # If all coefficients are less than 1.0 set format to q15
                else:
                    fmt_1st_coeff = self.out_fmt

            # Decrease fixed-point format in order to ensure nooverflow
            self.coeffs_fmt = fmt_1st_coeff
            self.coeffs_fmt = fmt_1st_coeff - self.order * (15 - self.in_fmt)
            self.out_fmt = self.coeffs_fmt

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
            "QHCG_KEY_HVX_VECTOR_DECL": self.getQHCG_KEY_HVX_VECTOR_DECL,
            "QHCG_KEY_LOAD_COEFFS": self.getQHCG_KEY_HVX_LOAD_COEFFS,
            "QHCG_KEY_SUB_LSR": self.getQHCG_KEY_SUB_LSR,
            "QHCG_KEY_COEFFS_FXP": self.getQHCG_KEY_COEFFS_FXP,
            "QHCG_KEY_SPLIT_COEFF": self.getQHCG_KEY_SPLIT_COEFF,
            "QHCG_KEY_VLUT": self.getQHCG_KEY_VLUT,
            "QHCG_KEY_COMPOSE_COEFF": self.getQHCG_KEY_COMPOSE_COEFF,
            "QHCG_KEY_POLY_EVAL": self.getQHCG_KEY_POLY_EVAL,
            "QHCG_KEY_INPUT_FILENAME": self.outputer.get_file_path(input_path).replace('\\', '/'),
            "QHCG_KEY_OUTPUT_FILENAME": self.outputer.get_file_path(output_path).replace('\\', '/'),
            "QHCG_KEY_INPUT_TYPE": self.getQHCG_KEY_HVX_INPUT(),  # TODO: can be uint16_t, uint32_t or float
            "QHCG_KEY_OUTPUT_TYPE": self.getQHCG_KEY_HVX_OUTPUT(),  # TODO: can be uint16_t, uint32_t or float
            "QHCG_KEY_STR_TO_NUMBER_FUNC": 'atoi',  # atoi or atof
            "QHCG_KEY_OUTPUT_PRINTF_FORMAT": 'i', # TODO: format for uh
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

        # print("Generate: '{}'->'{}'".format(template_path, output_path))

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
            "c": "qhcg_hvx_poly16b_fast.c",
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
        one_a = indent_spaces + 'HVX_Vector c{0}_coeff_v;\n' + \
                indent_spaces + 'HVX_DV c{0}_coeff_dv;\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)
        return out_str

    def getQHCG_KEY_HVX_LOAD_COEFFS(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'c{0}_coeff_v = *((HVX_Vector *)(c{0}_coeffs));\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)
        return out_str

    def getQHCG_KEY_HVX_A0_VECT(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'input_min_dv.V.lo = Q6_Vh_vsplat_R({0});\n' + \
                indent_spaces + 'input_min_dv.V.hi = Q6_Vh_vsplat_R({0});\n'

        return one_a.format(self.A0)

    def getQHCG_KEY_COEFFS_FXP(self, indent_spaces: str = ''):
        if self.shift_req == True:
            for i in range(self.order + 1):
                coeff_seg = np.zeros(2*self.segments)
                # Use paddign with zeros between two elements
                for j in range(2 * self.segments):
                    if (j % 2) == 0:
                        coeff_seg[j] = self.coeffs[int(j/2)][self.order - i]
                    else:
                        coeff_seg[j] = 0
                try:
                    assert (self.coeff_fmts[self.order - i] > 0), "Shift coefficients are less then zero. Abort!"
                except AssertionError:
                    sys.exit("Shift coefficients are less then zero. Abort!")
                coeff_fxp = np.array(np.round(coeff_seg * (1 << self.coeff_fmts[self.order - i])), np.int)
                coeff_fxp_sat = coeff_fxp.clip(-2**15, 2**15 - 1)
                self.coeffs_arr.append(coeff_fxp_sat)
        else:
            # Start coefficients format
            coeffs_fmt = self.coeffs_fmt

            for i in range(self.order + 1):
                coeff_seg = np.zeros(2*self.segments)
                # Use paddign with zeros between two elements
                for j in range(2 * self.segments):
                    if (j % 2) == 0:
                        coeff_seg[j] = self.coeffs[int(j/2)][self.order - i]
                    else:
                        coeff_seg[j] = 0
                try:
                    assert (coeffs_fmt > 0), "Shift coefficients are less then zero. Abort!"
                except AssertionError:
                    sys.exit("Shift coefficients are less then zero. Abort!")
                coeff_fxp = np.array(np.round(coeff_seg * (1 << coeffs_fmt)), np.int)
                coeff_fxp_sat = coeff_fxp.clip(-2**15, 2**15 - 1)
                self.coeffs_arr.append(coeff_fxp_sat)
                coeffs_fmt += 15 - self.in_fmt

        one_a = 'static const int16_t c{}_coeffs[{}] __attribute__((aligned(VLEN))) =\n' \
                '{{' \
                '{}\n' \
                '}};\n'
        out_str = ''
        for i in range(self.order + 1):
            coeff_arr_idx = format_table_str(self.coeffs_arr[i])
            out_str += one_a.format(i, 2 * self.segments, coeff_arr_idx)

        return out_str

    def getQHCG_KEY_SPLIT_COEFF(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'c{0}_coeff_d_vp = Q6_Wuw_vzxt_Vuh(*((HVX_Vector *)(c{0}_coeffs)));\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)
        return out_str

    def getQHCG_KEY_VLUT(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'c{0}_coeff_dv.VV = Q6_Wh_vlut16_VbVhR(vlut_indexes_v, c{0}_coeff_v, 0);\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)
        return out_str

    def getQHCG_KEY_COMPOSE_COEFF(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'c{0}_even = Q6_Vh_vshuffe_VhVh(Q6_V_lo_W(c{0}_coeff_h_vp), Q6_V_lo_W(c{0}_coeff_l_vp));\n' + \
                indent_spaces + 'c{0}_odd = Q6_Vh_vshuffo_VhVh(Q6_V_lo_W(c{0}_coeff_h_vp), Q6_V_lo_W(c{0}_coeff_l_vp));\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)
            out_str += '\n'
        return out_str

    def getQHCG_KEY_POLY_EVAL(self, indent_spaces: str = ''):
        out_str = ''
        if self.shift_req == True:
            sht_fact = np.zeros(self.order, np.int)
            for j in range(self.order):
                sht_fact[j] = self.coeff_fmts[j + 1]  - (self.coeff_fmts[j] - (15 - self.in_fmt))

            for i in reversed(range(self.order)):
                mpy_1st_op = 'c{}_coeff_dv.V.lo'.format(i + 1) if i == (self.order - 1) else 'y_1st_v'
                mpy_2nd_op = 'c{}_coeff_dv.V.hi'.format(i + 1) if i == (self.order - 1) else 'y_2nd_v'

                if sht_fact[self.order - i - 1] > 0:
                    shift_str = indent_spaces + 'y_1st_v = Q6_Vh_vasl_VhR(y_1st_v, ' + str(sht_fact[self.order - i - 1]) + ');\n' + \
                                indent_spaces + 'y_2nd_v = Q6_Vh_vasl_VhR(y_2nd_v, ' + str(sht_fact[self.order - i - 1]) + ');\n'
                elif sht_fact[self.order - i - 1] < 0:
                    shift_str = indent_spaces + 'y_1st_v = Q6_Vh_vasr_VhR(y_1st_v, ' + str(abs(sht_fact[self.order - i - 1])) + ');\n' + \
                                indent_spaces + 'y_2nd_v = Q6_Vh_vasr_VhR(y_2nd_v, ' + str(abs(sht_fact[self.order - i - 1])) + ');\n'
                else:
                    shift_str = ''

                one_a = indent_spaces + 'y_1st_v = Q6_Vh_vmpy_VhVh_s1_rnd_sat({mpy_1st_1stOp}, input_dv.V.lo);\n' + \
                indent_spaces + 'y_2nd_v = Q6_Vh_vmpy_VhVh_s1_rnd_sat({mpy_2nd_1stOp}, input_dv.V.hi);\n' + \
                indent_spaces + '{sht_fact}' + \
                indent_spaces + 'y_1st_v = Q6_Vh_vadd_VhVh_sat(y_1st_v, c{idx_2}_coeff_dv.V.lo);\n' + \
                indent_spaces + 'y_2nd_v = Q6_Vh_vadd_VhVh_sat(y_2nd_v, c{idx_2}_coeff_dv.V.hi);\n' + \
                '\n'

                out_str += one_a.format(mpy_1st_1stOp=mpy_1st_op, mpy_2nd_1stOp=mpy_2nd_op, idx=self.order, idx_2=i, sht_fact=shift_str)
        else:
            one_a = indent_spaces + 'y_1st_v = Q6_Vh_vmpy_VhVh_s1_rnd_sat(c{idx}_coeff_dv.V.lo, input_dv.V.lo);\n' + \
                    indent_spaces + 'y_2nd_v = Q6_Vh_vmpy_VhVh_s1_rnd_sat(c{idx}_coeff_dv.V.hi, input_dv.V.hi);\n' + \
                    indent_spaces + 'y_1st_v = Q6_Vh_vadd_VhVh_sat(y_1st_v, c{idx_2}_coeff_dv.V.lo);\n' + \
                    indent_spaces + 'y_2nd_v = Q6_Vh_vadd_VhVh_sat(y_2nd_v, c{idx_2}_coeff_dv.V.hi);\n' + \
                    '\n'
            out_str = ''

            out_str += one_a.format(idx = self.order, idx_2 = (self.order - 1))

            one_a = indent_spaces + 'y_1st_v = Q6_Vh_vmpy_VhVh_s1_rnd_sat(y_1st_v, input_dv.V.lo);\n' + \
                    indent_spaces + 'y_2nd_v = Q6_Vh_vmpy_VhVh_s1_rnd_sat(y_2nd_v, input_dv.V.hi);\n' + \
                    indent_spaces + 'y_1st_v = Q6_Vh_vadd_VhVh_sat(y_1st_v, c{idx}_coeff_dv.V.lo);\n' + \
                    indent_spaces + 'y_2nd_v = Q6_Vh_vadd_VhVh_sat(y_2nd_v, c{idx}_coeff_dv.V.hi);\n' + \
                    '\n'

            for i in reversed(range(self.order -  1)):
                # Decision whether is need to shift right or left
                out_str += one_a.format(idx=i)

        return out_str

    def getQHCG_KEY_SCALE_FACT(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'input_scaled_1_v = Q6_Vh_vmpy_VhRh_s1_rnd_sat(input_shifted_dv.V.lo, {scaled_factor});\n' + \
                indent_spaces + 'input_scaled_2_v = Q6_Vh_vmpy_VhRh_s1_rnd_sat(input_shifted_dv.V.hi, {scaled_factor});\n'

        scaled_factor_str = str('0x{0:0{1}x}{0:0{1}x}'.format(self.scaled_factor, 4))

        return one_a.format(scaled_factor=scaled_factor_str)

    def getQHCG_KEY_SUB_LSR(self, indent_spaces: str = ''):
        if self.first_shift is not 0:
            one_a = indent_spaces + '/* Divide shifted input range when it is out-of-boundary */\n' + \
                    indent_spaces + 'input_shifted_dv.V.lo = Q6_Vuh_vlsr_VuhR(input_shifted_dv.V.lo, {arg});\n' + \
                    indent_spaces + 'input_shifted_dv.V.hi = Q6_Vuh_vlsr_VuhR(input_shifted_dv.V.hi, {arg});\n'
            return one_a.format(arg=self.first_shift)
        else:
            return ''

    def getQHCG_KEY_HVX_INPUT(self):
        one_a = 'qhl_q{}_t'

        return one_a.format(str(self.in_fmt))

    def getQHCG_KEY_HVX_OUTPUT(self):
        one_a = 'qhl_q{}_t'

        return one_a.format(str(self.out_fmt))

    def getQHCG_KEY_NUM_BENCH_ELEM(self):
        return str(gen_data.get_num_elements())

    # Get output format
    def getOutputFMT(self):
        return self.out_fmt

    def getInputFMT(self):
        return self.in_fmt

    def get_coeffs_fmt_with_sht(self):
        coeff_seg = np.zeros(self.segments)
        # Formats for each coefficients particularly
        coeff_fmts = np.zeros((self.order + 1), np.int)
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
