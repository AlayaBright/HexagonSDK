# %%
import numpy as np
import os
from qhcg_config import Input_types, Output_types, Modes_process_output
# class for reading output results
from qhcg_toolbox import ULP_array


class process_output():
    def __init__(self, input_types, output_types, input_format=15, output_format=15):
        self.input_data_raw = None
        self.output_hvx_raw = None
        self.output_np_raw = None
        self.input_data_float = None
        self.output_hvx_float = None
        self.output_np_float = None
        self.error_float_LSB = None
        self.input_types = input_types
        self.output_types = output_types
        self.input_format = input_format
        self.output_format = output_format
        self.error_raw = None
        self.error_float = None
        self.error_mean_raw = 0.0
        self.error_rms_raw = 0.0
        self.error_max_raw = 0.0
        self.error_mean_float = 0.0
        self.error_rms_float = 0.0
        self.error_max_float = 0.0
        self.error_float_LSB_mean = 0.0
        self.error_float_LSB_rms = 0.0
        self.error_float_LSB_max = 0.0
        self.input_file_found = False
        self.output_hvx_found = False
        self.output_np_found = False
        self.completed_report = False
        self.lsbulp_str = 'LSB'  # 'LSB' or 'ULP'

    # TODO: set robust / modular path from give type and mode
    def read_input_file(self, outputer):
        path = self.path_generator(mode=Modes_process_output.read_input)
        path_to_file = outputer.get_file_path(path)
        if os.path.exists(path_to_file):
            self.input_data_raw = np.reshape(np.genfromtxt(path_to_file, delimiter=','), (1, -1))[0]
            self.input_file_found = True

    def read_output_hvx_file(self, outputer):
        path = self.path_generator(mode=Modes_process_output.read_HVX)
        path_to_file = outputer.get_file_path(path)
        if os.path.exists(path_to_file):
            self.output_hvx_raw = np.reshape(np.genfromtxt(path_to_file, delimiter=','), (1, -1))[0]
            self.output_hvx_found = True

    def read_output_np_file(self, outputer):
        path = self.path_generator(mode=Modes_process_output.read_np)
        path_to_file = outputer.get_file_path(path)
        if os.path.exists(path_to_file):
            self.output_np_raw = np.reshape(np.genfromtxt(path_to_file, delimiter=','), (1, -1))[0]
            self.output_np_found = True

    def read_files(self, outputer):
        self.read_input_file(outputer=outputer)
        self.read_output_hvx_file(outputer=outputer)
        self.read_output_np_file(outputer=outputer)

    def get_input_data_raw(self):
        return self.input_data_raw

    def get_output_hvx_raw(self):
        return self.output_hvx_raw

    def get_output_np_raw(self):
        return self.output_np_raw

    def get_input_data_float(self):
        return self.input_data_float

    def get_output_hvx_float(self):
        return self.output_hvx_float

    def get_output_np_float(self):
        return self.output_np_float

    def get_error_raw(self):
        return self.error_raw

    def get_error_float(self):
        return self.error_float

    def get_error_float_LSB(self):
        return self.error_float_LSB

    def get_error_report(self):
        return self.error_max_float, self.error_rms_float, self.error_mean_float

    def print_error_report(self, outputer):
        err_report_f = "HVX polynomial approximation:\n\n" \
        "Accuracy:\n" \
        "Max error [float]: {}\n" \
        "RMS error [float]: {}\n" \
        "Mean error [float]: {}\n\n" \
        "Max error [{lsbulp}]: {}\n" \
        "RMS error [{lsbulp}]: {}\n" \
        "Mean error [{lsbulp}]: {}\n"

        err_report = err_report_f.format( \
            str(self.error_max_float), str(self.error_rms_float), str(self.error_mean_float), \
            str(self.error_float_LSB_max), str(self.error_float_LSB_rms), str(self.error_float_LSB_mean), \
            lsbulp=self.lsbulp_str)

        if (self.input_types == Input_types.int16) or (self.input_types == Input_types.int32):
            err_report = err_report + "\nNote: Multiply by 2^-{} to get the actual float Max, RMS and Mean errors.\n".format(str(self.output_format))

        print(err_report)

        with open(outputer.get_file_path('report_accr'), "w") as report_accr:
            report_accr.writelines(err_report)

    def covert_raw_to_float(self):
        # Do not convert raw to float if raw is float
        if (self.input_types == Input_types.int16) or (self.input_types == Input_types.int32):
            input_modifier = 1.0 / (2.0**self.input_format)
            output_modifier = 1.0 / (2.0**self.output_format)
            self.input_data_float = self.input_data_raw * input_modifier
            self.output_np_float = self.output_np_raw * output_modifier
            self.output_hvx_float = self.output_hvx_raw * output_modifier
        else:
            self.input_data_float = self.input_data_raw
            self.output_np_float = self.output_np_raw
            self.output_hvx_float = self.output_hvx_raw

    def calculate_error_values(self):
        self.error_raw = abs(self.output_hvx_raw - self.output_np_raw)

        if (self.input_types == Input_types.int16) or (self.input_types == Input_types.int32):
            self.error_float = self.error_raw
        else:
            self.error_float = abs(self.output_hvx_float - self.output_np_float)

        # based on the output files within output/HVX_code/test/test_data folder
        self.error_mean_float = np.mean(self.error_raw)
        self.error_rms_float = np.std(self.error_raw)
        self.error_max_float = np.max(self.error_raw)

        self.error_float_LSB = ULP_array(self.output_hvx_float, self.output_np_float, Input_types.input_type_to_bit_size(self.input_types))
        self.lsbulp_str = 'ULP'

        self.error_float_LSB_mean = np.mean(self.error_float_LSB)
        self.error_float_LSB_rms = np.std(self.error_float_LSB)
        self.error_float_LSB_max = np.max(self.error_float_LSB)

    def process_output_of_HVX(self, outputer):
        self.read_files(outputer=outputer)
        if self.check_data():
            self.covert_raw_to_float()
            self.calculate_error_values()
            self.print_error_report(outputer=outputer)
            self.completed_report = True

    def path_generator(self, mode) -> str:
        path_start = mode.value
        if (mode == Modes_process_output.read_input):
            path_end = Input_types.input_type_to_bit_size_str(self.input_types)
            path_bit_size = str(Input_types.input_type_to_bit_size(self.input_types))
        else:
            path_end = Output_types.output_type_to_bit_size_str(self.output_types)
            path_bit_size = str(Output_types.output_type_to_bit_size(self.output_types))
        return path_start + path_bit_size + path_end

    def check_data(self):
        return (self.output_np_found and self.input_file_found and self.output_hvx_found)

    def check_report(self):
        return self.completed_report
