import numpy as np
import qhcg_fxp_parser as fxp_parser
import qhcg_gen_data as gen_data
from qhcg_config import Input_types, Output_types, Modes_process_output
from pathlib import Path
import math as m
from qhcg_helper import replace_words, format_table_str

# Number of bits which represent coefficients
coeffs_size = 32


class GeneratorHVX16b_acc(object):

    def __init__(self, function, comuptable_function, outputer, a0, b0, coeffs, poly_order):

        input_path = gen_data.path_generator(mode=Modes_process_output.read_input, input_types=Input_types.int16)
        output_path = gen_data.path_generator(mode=Modes_process_output.read_HVX, output_types=Output_types.int16)

        self.outputer = outputer
        self.a0 = a0
        self.b0 = b0
        self.coeffs = coeffs
        self.coeffs_arr = []
        self.order = poly_order
        self.segments = 16
        self.eval_with_shift = False

        # Determine input/output format
        (self.in_fmt, self.out_fmt) = fxp_parser.find_qfmt_in_out(comuptable_function, a0, b0, 16, 16)

        self.A0, self.B0, self.scaled_factor, self.first_shift = fxp_parser.calc_scaled_factor(a0, b0, self.in_fmt)

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
            "QHCG_KEY_CALC_EVENODD": self.getQHCG_KEY_CALC_EVENODD,
            "QHCG_KEY_INPUT_FILENAME": self.outputer.get_file_path(input_path).replace('\\', '/'),
            "QHCG_KEY_OUTPUT_FILENAME": self.outputer.get_file_path(output_path).replace('\\', '/'),
            "QHCG_KEY_INPUT_TYPE": self.getQHCG_KEY_HVX_INPUT(),  # TODO: can be uint16_t, uint32_t or float
            "QHCG_KEY_OUTPUT_TYPE": self.getQHCG_KEY_HVX_OUTPUT(),  # TODO: can be uint16_t, uint32_t or float
            "QHCG_KEY_STR_TO_NUMBER_FUNC": 'atoi',  # atoi or atof
            "QHCG_KEY_OUTPUT_PRINTF_FORMAT": 'i',  # TODO: format for uh

            "QHCG_KEY_ROUND_VECT": self.getQHCG_KEY_ROUND_VECT,
            "QHCG_KEY_SCALE_FACT": self.getQHCG_KEY_SCALE_FACT,
            "QHCG_KEY_OUTPUT_SHT": self.getQHCG_KEY_OUTPUT_SHT,

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
            "c": "qhcg_hvx_poly16b_acc.c",
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
                indent_spaces + 'HVX_Vector c{0}_even_v, c{0}_odd_v;\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)
        return out_str

    def getQHCG_KEY_COEFFS_FXP(self):

        if self.check_shift_condition() == True:

            coeffs_fmt_final = np.array(fxp_parser.find_qfmt_coeffs_full_range(self.a0, self.b0, self.coeffs, self.in_fmt, self.order, 32), np.int32)

            # Decrease fixed-point format in order to ensure nooverflow
            self.coeffs_fmt = coeffs_fmt_final - 1

            for i in range(self.order + 1):
                coeff_seg = np.zeros(self.segments)
                for j in range(self.segments):
                    coeff_seg[j] = self.coeffs[j][self.order - i]

                coeff_fxp = np.array(np.round(coeff_seg * (1 << self.coeffs_fmt[self.order - i])), np.int64)
                coeff_fxp_sat = coeff_fxp.clip(-2**31, 2**31 - 1)
                self.coeffs_arr.append(coeff_fxp_sat)

        else:
            max_coeffs = 0.0
            for j in range(self.segments):
                if max_coeffs < np.max(np.abs(self.coeffs[j])):
                    max_coeffs = np.max(np.abs(self.coeffs[j]))

            self.coeffs_fmt = coeffs_size - int(m.floor(m.log2(max_coeffs) + 1)) - 2
            self.coeffs_fmt -= self.order * (15 - self.in_fmt)

            # print('Start coeffs format: ' + str(self.coeffs_fmt))
            coeffs_fmt_curr = self.coeffs_fmt

            for i in range(self.order + 1):
                coeff_seg = np.zeros(self.segments)
                for j in range(self.segments):
                    coeff_seg[j] = self.coeffs[j][self.order - i]

                coeff_fxp = np.array(np.round(coeff_seg * (1 << coeffs_fmt_curr)), np.int64)
                coeff_fxp_sat = coeff_fxp.clip(-2**31, 2**31 - 1)
                self.coeffs_arr.append(coeff_fxp_sat)
                coeffs_fmt_curr += (15 - self.in_fmt)

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
        one_a = indent_spaces + 'c{0}_coeff_d_vp = Q6_Wuw_vzxt_Vuh(*((HVX_Vector *)c{0}_coeffs));\n'
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
        one_a = indent_spaces + 'c{0}_even_v = Q6_Vh_vshuffe_VhVh(Q6_V_hi_W(c{0}_coeff_h_vp), Q6_V_hi_W(c{0}_coeff_l_vp));\n' + \
                indent_spaces + 'c{0}_odd_v = Q6_Vh_vshuffo_VhVh(Q6_V_hi_W(c{0}_coeff_h_vp), Q6_V_hi_W(c{0}_coeff_l_vp));\n'
        out_str = ''
        for i in range(self.order + 1):
            out_str += one_a.format(i)
            out_str += '\n'
        return out_str

    def getQHCG_KEY_CALC_EVENODD(self, indent_spaces: str = ''):

        if self.check_shift_condition() == True:
            shift_fact = np.zeros(self.order + 1)

            # This shift factor simulates real hexagon instruction vmpyo 32 x 16
            shift_right = 15

            for i in range(self.order):
                shift_fact[i] = self.coeffs_fmt[i + 1] - (self.coeffs_fmt[i] + self.in_fmt - shift_right)

            # Absolute value for shift factor
            shift_fact_abs = np.array(np.abs(shift_fact), np.int32)

        one_a = indent_spaces + 'y_even_v = Q6_Vw_vmpyo_VwVh_s1_rnd_sat({in_c}_even_v, x_even_v);\n' + \
                indent_spaces + 'y_odd_v = Q6_Vw_vmpyo_VwVh_s1_rnd_sat({in_c}_odd_v, sline);\n' \
                '\n' \
                + '{sht_fact}' + \
                indent_spaces + 'y_even_v = Q6_Vw_vadd_VwVw_sat(y_even_v, c{idx}_even_v);\n' + \
                indent_spaces + 'y_odd_v = Q6_Vw_vadd_VwVw_sat(y_odd_v, c{idx}_odd_v);\n' \
                '\n'
        out_str = ''
        for i in reversed(range(self.order)):
            # Decision whether is need to shift right or left
            in_val = "c{}".format(i+1) if i is self.order-1 else 'y'

            if self.check_shift_condition() == True:
                if shift_fact[self.order - i - 1] < 0.0:
                    shift_str = indent_spaces + 'y_even_v = Q6_Vw_vasr_VwR(y_even_v,' + str(shift_fact_abs[self.order - i - 1]) + ');\n' + \
                                indent_spaces + 'y_odd_v = Q6_Vw_vasr_VwR(y_odd_v,' + str(shift_fact_abs[self.order - i - 1]) + ');\n'
                elif shift_fact[self.order - i - 1] > 0.0:
                    shift_str = indent_spaces + 'y_even_v = Q6_Vw_vasl_VwR(y_even_v,' + str(shift_fact_abs[self.order - i - 1]) + ');\n' + \
                                indent_spaces + 'y_odd_v = Q6_Vw_vasl_VwR(y_odd_v,' + str(shift_fact_abs[self.order - i - 1]) + ');\n'
                else:
                    shift_str = ''

                out_str += one_a.format(in_c=in_val, idx=i, sht_fact=shift_str)
            else:
                out_str += one_a.format(in_c=in_val, idx=i, sht_fact='')

        return out_str

    def getQHCG_KEY_ROUND_VECT(self, indent_spaces: str = ''):
        one_a = indent_spaces + 'round_v = Q6_V_vsplat_R({});\n'

        if self.check_shift_condition() == True:
            pos = self.coeffs_fmt[self.order] - self.out_fmt
        else:
            pos = self.coeffs_fmt - self.out_fmt

        if (pos > 1):
            rnd_fact =  1 << (pos - 1)
        else:
            rnd_fact = 0

        str_rnd_fact = str('0x{0:0{1}x}'.format(rnd_fact,8))
        return one_a.format(str_rnd_fact)

    def getQHCG_KEY_SUB_LSR(self, indent_spaces: str = ''):

        if self.first_shift is not 0:
            one_a = indent_spaces + '/* Divide shifted input range when it is out-of-boundary */\n' + \
                    indent_spaces + 'input_shifted_v = Q6_Vuh_vlsr_VuhR(input_shifted_v, {arg});\n'
            return one_a.format(arg=self.first_shift)
        else:
            return ''

    def getQHCG_KEY_SCALE_FACT(self, indent_spaces: str = ''):

        one_a = indent_spaces + 'vlut_indexes_v = Q6_Vh_vmpy_VhRh_s1_rnd_sat(input_shifted_v, {scaled_factor});\n'

        scaled_factor_str = str('0x{0:0{1}x}{0:0{1}x}'.format(self.scaled_factor, 4))

        return one_a.format(scaled_factor=scaled_factor_str)

    def getQHCG_KEY_OUTPUT_SHT(self, indent_spaces: str = ''):

        if self.check_shift_condition() == True:
            pos = self.coeffs_fmt[self.order] - self.out_fmt
        else:
            pos = self.coeffs_fmt - self.out_fmt

        if pos > 0:
            one_a = indent_spaces + 'y_even_v = Q6_Vw_vasr_VwR(y_even_v, {pos});\n' + \
                    indent_spaces + 'y_odd_v = Q6_Vw_vasr_VwR(y_odd_v, {pos});\n'
        else:
            one_a = indent_spaces + 'y_even_v = Q6_Vw_vasl_VwR(y_even_v, {pos});\n' + \
                    indent_spaces + 'y_odd_v = Q6_Vw_vasl_VwR(y_odd_v, {pos});\n'

        pos_str = str(abs(pos))
        return one_a.format(pos=pos_str)

    def getQHCG_KEY_HVX_A0_VECT(self, indent_spaces: str = ''):

        one_a = indent_spaces + 'input_min_v = Q6_Vh_vsplat_R({0});\n'

        return one_a.format(self.A0)

    def getQHCG_KEY_HVX_INPUT(self):

        one_a = 'qhl_q{}_t'

        return one_a.format(str(self.in_fmt))

    def getQHCG_KEY_HVX_OUTPUT(self):

        one_a = 'qhl_q{}_t'

        return one_a.format(str(self.out_fmt))

    def getQHCG_KEY_NUM_BENCH_ELEM(self):
        return str(gen_data.get_num_elements())

    def getOutputFMT(self):

        return self.out_fmt

    def getInputFMT(self):

        return self.in_fmt

    # Check is there any needed for shift instruction in polynomial evaluation
    def check_shift_condition(self):

        max_coeffs = np.max(np.abs(self.coeffs))

        if max_coeffs > 1.0:
            coeffs_fmt = coeffs_size - int(m.floor(m.log2(max_coeffs + 1))) - 1
            if (coeffs_fmt - self.order * (15 - self.in_fmt)) < self.out_fmt + 6:
                return True
            else:
                return False
        else:
            return False
