# %%
import struct
import warnings
import numpy as np
import os
import matplotlib as mpl
if os.environ.get('DISPLAY','') == '':
    print('no display found. Using non-interactive Agg backend')
    mpl.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from qhcg_config import Input_types, Output_types, Approximation_methods, Approximation_norm, list_of_enum_options, list_of_enum_options_in_str
import sys

# %%


def err_handler(type_str, flag):
    print("Floating point error (%s), with flag %s" % (type_str, flag))
    print("Check does function and given input_range. There is a undefined Examples log(x) when x < 0, div by zero etc...")
    sys.exit(2)

def ilogb(arg: float) -> int:
    '''
    Extracts the value of the unbiased exponent from the single precission IEEE754 floating-point
    ilogb (0) isn't allowed.
    '''

    if arg == 0.0:
        raise Exception("Error: Not supported value {} for ilogb() !".format(arg))

    # Handle float IEEE754 number as integer
    sx = struct.pack('>f', abs(arg))
    ix = struct.unpack('>l', sx)[0]
    exponent = ix >> 23  # strip (23 bits) mantissa, in 32-bit single precission representation

    if exponent == 0:
        exponent = -128  # de-normalized IEEE754
    else:
        exponent -= 127  # IEEE754 exponent bias

    return int(exponent)


def ULP(computed: float, expected: float, bits: int = 32) -> float:
    ''' Calculate relative ULP between two floating-point numbers '''

    diff = abs(computed - expected)

    # 16 bit float: 1|5|10
    # 32 bit float: 1|8|23
    MANT_DIG = 10 if bits is 16 else 23
    num_exp_calc = -MANT_DIG
    if expected != 0.0:
        num_exp_calc += ilogb(expected)

    eps = 2**(num_exp_calc)
    result_ulp = diff / eps

    return result_ulp


def ULP_array(computed_array, expected_array, float_bits=32):
    ''' Calculate ULP for given arrays '''

    len_computed = len(computed_array)
    len_expected = len(expected_array)

    if len_computed != len_expected:
        print("Error: different input arrays sizes!")
        return

    result_ulp_a = np.zeros(len_expected)

    for i in range(len_expected):
        result_ulp_a[i] = ULP(computed_array[i], expected_array[i], float_bits)

    return result_ulp_a


# error handling
saved_handler = np.seterrcall(err_handler)
save_err = np.seterr(all='call')

# %%


# class approximation_toolbox will for given number of segments, input data, output data, polyOrder

class approximation_toolbox():
    def __init__(self,
                 num_seg: int,
                 input_data: np.ndarray,
                 output_data: np.ndarray,
                 input_data_resampled_seg: np.ndarray,
                 output_data_resampled_seg: np.ndarray,
                 polyOrder: int = 4,
                 norm: Approximation_norm = Approximation_norm.min_norm):
        self.input_data_seg = np.array_split(input_data, num_seg)                   # spliting of input data into given number of segments
        self.output_data_seg = np.array_split(output_data, num_seg)                 # spliting of output data into given number of segments
        self.approx_out_data_seg = [[] for i in range(num_seg)]                     # for each segment output data
        self.error_max_seg = [[] for i in range(num_seg)]                           # max error per segment
        self.error_rms_seg = [[] for i in range(num_seg)]                           # rms error per segment
        self.error_mean_seg = [[] for i in range(num_seg)]                          # mean error per segment
        self.error_values_seg = [[] for i in range(num_seg)]                        # error y - y_approx per segment
        self.list_of_methods = [[] for i in range(num_seg)]                         # list of methods done for each segment
        self.approximation_polynomial = [[] for i in range(num_seg)]                # np.poly1d of approximation
        self.approximation_polynomial_coefficients = [[] for i in range(num_seg)]   # ceoficients of approximation
        self.num_seg = num_seg                                                      # number of segments
        self.polyOrder = 0                                                          # polynomial order
        self.set_polyOrder(polyOrder=polyOrder)
        self.input_data = input_data                                                # input data into given number of segments
        self.output_data = output_data                                              # output data into given number of segments
        self.approx_out_data = None                                                 # for each segment output data
        self.error_max = 0.0                                                        # max error per segment
        self.error_rms = 0.0                                                        # rms error per segment
        self.error_mean = 0.0                                                       # mean error per segment
        self.error_values = None                                                    # error y - y_approx per segment
        self.list_of_supported_methods_str = list_of_enum_options_in_str(Approximation_methods)  # List of supported methods, , "constraints" are not supported
        self.list_of_supported_methods = list_of_enum_options(Approximation_methods)# List of supported methods, , "constraints" are not supported
        self.norm = norm
        self.rank_warning = True
        self.output_data_resampled_seg = output_data_resampled_seg
        self.input_data_resampled_seg = input_data_resampled_seg

    # Name:Get polyinomial order from toolbox class
    def get_polyOrder(self) -> int:
        return self.polyOrder

    # Name:Get polyinomial order from toolbox class
    def set_polyOrder(self, polyOrder: int):
        if (polyOrder < 1):
            raise(ValueError("Polynomial order needs to be at least 1 or greater."))
        else:
            self.polyOrder = polyOrder

    # Name: get_list_of_methods returns list of all methods used for every segment
    def get_list_of_methods(self) -> list:
        return self.list_of_methods

    # Name: get_num_seg returns number of segments used in
    def get_num_seg(self) -> int:
        return self.num_seg

    # Name: calculate_mean_error_on_segment
    # Inputs: none
    # Outputs: max_error, error_rms and mean error
    def get_error_report(self):
        return(self.error_max, self.error_rms, self.error_mean)

    # get_approximation_polynomial_coefficients returns a list of arrays.
    # One array in list represents one polynomial approximation.
    # Elements in arrays are coefficients of polynomial of given order
    def get_approximation_polynomial_coefficients(self):
        return self.approximation_polynomial_coefficients

    # Name: __approximation_polynomial_on_segment
    # Inputs: segment_i - (int), method - Approximation_methods (Enum)
    # Outputs: None.
    # Description: Method finds polynomial approximation on segment_i of input_data_seg and output_data_seg.
    # self.approximation_polynomial_coefficients will be populated with coefficients from approximation
    # self.approximation_polynomial will be polynomial functions
    def __approximation_polynomial_on_segment(self, segment_i: int, method: Approximation_methods):
        with warnings.catch_warnings():
            warnings.filterwarnings('error')
            try:
                if method == Approximation_methods.polyfit:
                    self.approximation_polynomial_coefficients[segment_i] = np.polyfit(x=self.input_data_resampled_seg[segment_i],
                                                                                       y=self.output_data_resampled_seg[segment_i],
                                                                                       deg=self.polyOrder)
                if method == Approximation_methods.chebfit:
                    fit = np.polynomial.Chebyshev.fit(x=self.input_data_resampled_seg[segment_i],
                                                      y=self.output_data_resampled_seg[segment_i],
                                                      deg=self.polyOrder)
                    self.approximation_polynomial_coefficients[segment_i] = np.flip(np.polynomial.chebyshev.cheb2poly(fit.convert().coef), axis=0)
                if method == Approximation_methods.legfit:
                    fit = np.polynomial.Legendre.fit(x=self.input_data_resampled_seg[segment_i],
                                                     y=self.output_data_resampled_seg[segment_i],
                                                     deg=self.polyOrder)
                    self.approximation_polynomial_coefficients[segment_i] = np.flip(np.polynomial.legendre.leg2poly(fit.convert().coef), axis=0)
                if method == Approximation_methods.lagfit:
                    fit = np.polynomial.Laguerre.fit(x=self.input_data_resampled_seg[segment_i],
                                                     y=self.output_data_resampled_seg[segment_i],
                                                     deg=self.polyOrder)
                    self.approximation_polynomial_coefficients[segment_i] = np.flip(np.polynomial.laguerre.lag2poly(fit.convert().coef), axis=0)
                if method == Approximation_methods.hermfit:
                    fit = np.polynomial.Hermite.fit(x=self.input_data_resampled_seg[segment_i],
                                                    y=self.output_data_resampled_seg[segment_i],
                                                    deg=self.polyOrder)
                    self.approximation_polynomial_coefficients[segment_i] = np.flip(np.polynomial.hermite.herm2poly(fit.convert().coef), axis=0)
                if method == Approximation_methods.fit:
                    fit = np.polynomial.polynomial.Polynomial.fit(x=self.input_data_resampled_seg[segment_i],
                                                                  y=self.output_data_resampled_seg[segment_i],
                                                                  deg=self.polyOrder)
                    self.approximation_polynomial_coefficients[segment_i] = np.flip(fit.convert().coef, axis=0)
                self.approximation_polynomial[segment_i] = np.poly1d(self.approximation_polynomial_coefficients[segment_i])
                self.list_of_methods[segment_i] = method
            except (np.RankWarning, np.polynomial.polyutils.RankWarning):
                if self.rank_warning:
                    self.rank_warning = False
                    print("Approximation with polynomial order = " + str(self.polyOrder) + " has started to overfit. This is not last polynomial order! ")

    # Name: approximation_polynomial_calculation
    # Inputs: list_of_methods - (list of strings) default value None, method - (Enum) Approximation_methods with default value Approximation_methods.polyfit
    # Outputs: None.
    # Description: For each segment calculate coefficients P(x) per list of method. If list_of_methods is given then method will not be usedself.
    # if list_of_methods is not given, method will be applayed for each segment.
    def approximation_polynomial_calculation(self, list_of_methods=None, method: Approximation_methods = Approximation_methods.polyfit):
        if list_of_methods is None:
            list_of_methods = [method] * self.num_seg
        else:
            list_of_methods = self.list_of_methods
        for (i, method_iter) in zip(range(0, self.num_seg), list_of_methods):
            self.__approximation_polynomial_on_segment(segment_i=i, method=method_iter)

    # Name: __calculate_best_approximation_on_segment
    # Inputs: segment_i (int)
    # Outputs: None.
    # Description: For each of possible approximations choose best one with
    # lowest minmax error is norm.
    def __calculate_best_approximation_on_segment(self, segment_i: int):
        error_list = np.zeros(len(self.list_of_supported_methods))
        for method, k in zip(self.list_of_supported_methods, range(len(self.list_of_supported_methods))):
            self.__approximation_polynomial_on_segment(segment_i=segment_i, method=method)
            self.__calculate_polynomial_output_data_on_segment(segment_i=segment_i)
            self.calculate_error_value_on_segment(segment_i=segment_i)
            if (self.norm == Approximation_norm.min_norm):
                self.calculate_max_error_on_segment(segment_i=segment_i)
                error_list[k] = self.error_max_seg[segment_i]
            elif (self.norm == Approximation_norm.rms_norm):
                self.calculate_rms_error_on_segment(segment_i=segment_i)
                error_list[k] = self.error_rms_seg[segment_i]
            elif (self.norm == Approximation_norm.mean_norm):
                self.calculate_mean_error_on_segment(segment_i=segment_i)
                error_list[k] = self.error_mean_seg[segment_i]
            else:
                print("Norm for the best approximation can only be " + list_of_enum_options_in_str(Approximation_norm))
        min_index = np.argmin(error_list)
        self.list_of_methods[segment_i] = self.list_of_supported_methods[min_index]

    # Name: calculate_best_approximations
    # Inputs: None.
    # Outputs: None.
    # Description: Find best approximation for each segment.
    def calculate_best_approximations(self):
        for i in range(0, self.num_seg):
            self.__calculate_best_approximation_on_segment(segment_i=i)

    # Name: __calculate_polynomial_output_data_on_segment
    # Inputs: segment_i (int)
    # Outputs: None.
    # Description: For given segment_i calculate P(x) values on given segment.
    # Output error message will be printed out if P(x) is not set.
    def __calculate_polynomial_output_data_on_segment(self, segment_i: int):
        if not (self.approximation_polynomial[segment_i]):
            print("On segment i= " + str(segment_i) + " there is no approximation polynom")
        else:
            self.approx_out_data_seg[segment_i] = [self.approximation_polynomial[segment_i](x) for x in self.input_data_seg[segment_i]]

    # Name: calculate_error_value_on_segment
    # Inputs: segment_i (int)
    # Outputs: None.
    # Description: For given segment_i calculate absolute error values on given segment.
    # Output error message will be printed out if values of P(x) are not set.
    def calculate_error_value_on_segment(self, segment_i: int):
        if not (self.approx_out_data_seg[segment_i]):
            print("On segment i= " + str(segment_i) + " there is no approximation data")
        else:
            self.error_values_seg[segment_i] = abs(self.approx_out_data_seg[segment_i] - self.output_data_seg[segment_i])

    # Name: calculate_max_error_on_segment
    # Inputs: segment_i (int)
    # Outputs: None.
    # Description: Finds and stores maximum of absolute error on given segment.
    def calculate_max_error_on_segment(self, segment_i: int):
        if (len(self.error_values_seg[segment_i]) == 0):
            print("On segment i= " + str(segment_i) + " there is no error data")
        else:
            self.error_max_seg[segment_i] = np.max(self.error_values_seg[segment_i])

    # Name: calculate_rms_error_on_segment
    # Inputs: segment_i (int)
    # Outputs: None.
    # Description: Finds and stores RMS (root mean squared) of absolute error on given segment.
    def calculate_rms_error_on_segment(self, segment_i: int):
        if (len(self.error_values_seg[segment_i]) == 0):
            print("On segment i= " + str(segment_i) + " there is no error data")
        else:
            self.error_rms_seg[segment_i] = np.std(self.error_values_seg[segment_i])

    # Name: calculate_mean_error_on_segment
    # Inputs: segment_i (int)
    # Outputs: None.
    # Description: Finds and stores mean of absolute error on given segment.
    def calculate_mean_error_on_segment(self, segment_i: int):
        if (len(self.error_values_seg[segment_i]) == 0):
            print("On segment i=" + str(segment_i) + "there is no error data")
        else:
            self.error_mean_seg[segment_i] = np.mean(self.error_values_seg[segment_i])

    # Name: print_error_report_on_segment
    # Inputs: segment_i (int)
    # Outputs: None.
    # Description: Prints Maximum, RMS (root mean squared) and mean absolute error on given segment.
    def print_error_report_on_segment(self, segment_i: int):
        print("Maximum error on segment " + str(segment_i) + ": " + str(self.error_max_seg[segment_i]))
        print("RMS error on segment " + str(segment_i) + ": " + str(self.error_rms_seg[segment_i]))
        print("Mean error on segment " + str(segment_i) + ": " + str(self.error_mean_seg[segment_i]))

    # Name: print_error_report
    # Inputs: per_segment_report (boolean) default value False
    # Outputs: None.
    # Description: Prints Maximum, RMS (root mean squared) and mean of absolute error.
    # If per_segment_report is True then additionally prints maximum, RMS, and mean of absolute error of each segment as well.
    def print_error_report(self, per_segment_report: bool = False):
        print("\nPython polynomial approximation (NumPy polynomials package):\n\nAccuracy:")
        if (per_segment_report is True):
            for i in range(0, self.num_seg):
                self.print_error_report_on_segment(i)
        print("Max error [float]: " + str(self.error_max))
        print("RMS error [float]: " + str(self.error_rms))
        print("Mean error [float]: " + str(self.error_mean))

    # Name: calculate_errors
    # Inputs: segment_i (int)
    # Outputs: None.
    # Description: Prints Maximum, RMS (root mean squared) and mean of absolute error on given segment.
    def calculate_errors(self):
        for i in range(0, self.num_seg):
            self.calculate_max_error_on_segment(i)
            self.calculate_rms_error_on_segment(i)
            self.calculate_mean_error_on_segment(i)
        self.calculate_max_error()
        self.calculate_rms_error()
        self.calculate_mean_error()

    # Name: calculate_polynomial_output_data
    # Inputs: None.
    # Outputs: None.
    # Description: For each segment calculates P(x) and merges all data into one array for additional handling.
    def calculate_polynomial_output_data(self):
        for i in range(0, self.num_seg):
            self.__calculate_polynomial_output_data_on_segment(i)
        self.__merge_approx_out_data()

    # Name: calculate_max_error
    # Inputs: None.
    # Outputs: None.
    # Description: On merged errors array finds maximum.
    def calculate_max_error(self):
        if (self.error_values is None):
            print("Errors have not been merged or calculated!")
        else:
            self.error_max = np.max(self.error_values)

    # Name: calculate_rms_error
    # Inputs: None.
    # Outputs: None.
    # Description: On merged errors array finds RMS (root mean squared).
    def calculate_rms_error(self):
        if (self.error_values is None):
            print("Errors have not been merged or calculated!")
        else:
            self.error_rms = np.std(self.error_values)

    # Name: calculate_mean_error
    # Inputs: None.
    # Outputs: None.
    # Description: On mearged errors array finds mean.
    def calculate_mean_error(self):
        if (self.error_values is None):
            print("Errors have not been merged or calculated!")
        else:
            self.error_mean = np.mean(self.error_values)

    # Name: calculate_error_value
    # Inputs: None.
    # Outputs: None.
    # Description: Calculates all absolute error values for each segment and merging them into one array.
    def calculate_error_value(self):
        for i in range(0, self.num_seg):
            self.calculate_error_value_on_segment(i)
        self.__merge_error_values()

    # Name: __merge_approx_out_data
    # Inputs: None.
    # Outputs: None.
    # Description: Helper function for merging approximation output data for easy manipulation.
    def __merge_approx_out_data(self):
        self.approx_out_data = np.concatenate(self.approx_out_data_seg)

    # Name: __merge_error_values
    # Inputs: None.
    # Outputs: None.
    # Description: Helper function for merging error values data for easy manipulation.
    def __merge_error_values(self):
        self.error_values = np.concatenate(self.error_values_seg)

    def add_pdf_graph_page(self, pdf, plt_data, title, legends):
        '''
            Name: add_pdf_graph_page
            Inputs: data to plot, plot attributes
            Outputs: None.
            Description: adds single graph plot to the pdf
        '''
        plt.plot(*plt_data)
        plt.title(title)
        if legends:
            plt.legend(legends)
        plt.grid()
        plt.savefig(pdf, format='pdf')
        plt.clf()

    def create_report(self, outputer, process_output, summary, input_string_function):
        '''
            Name: create_report
            Inputs: path - path to folder, summary boolean that signals to create summary or detailed report.
            Outputs: None.
            Description: Creates one PDF with all information with .txt with coeficients. Long report has error for each segment.
        '''
        print('\nCreating report ...')
        pdf_variant = 'pdf_summary' if summary is True else 'pdf_detailed'
        pp = PdfPages(outputer.get_file_path(pdf_variant))
        input_string_function = "(" + input_string_function + ")"

        if process_output.check_report():
            # function vs. approximation vs. HVX execution
            plot_data = (self.input_data,
                         self.output_data,
                         self.input_data,
                         self.approx_out_data,
                         process_output.get_input_data_float(),
                         process_output.get_output_hvx_float())

            self.add_pdf_graph_page(pp,
                                    plot_data,
                                    "Function vs. Polynomial approximations",
                                    ["Function " + input_string_function, "Numpy polynomial approximation p(x)", "HVX polynomial function h(x)"])
        else:
            # function vs. approximation vs. HVX execution
            plot_data = (self.input_data,
                         self.output_data,
                         self.input_data,
                         self.approx_out_data)

            self.add_pdf_graph_page(pp,
                                    plot_data,
                                    "Function vs. Polynomial approximation",
                                    ["Function " + input_string_function, "Numpy polynomial approximation p(x)"])

        # plot error_from approximation
        plot_data = (self.input_data,
                     self.error_values)

        self.add_pdf_graph_page(pp,
                                plot_data,
                                "Numpy polynomial approximation error",
                                [])

        if process_output.check_report():
            # plot error_from_HVX
            plot_data = (process_output.get_input_data_float(),
                          process_output.get_error_float())

            self.add_pdf_graph_page(pp,
                                    plot_data,
                                    "HVX polynomial approximation error",
                                    [])

        # print all segments
        if summary is False:
            for segment_i in range(0, self.get_num_seg()):
                plot_data = (self.input_data_seg[segment_i],
                             self.error_values_seg[segment_i])

                self.add_pdf_graph_page(pp,
                                        plot_data,
                                        "Polynomial approximation error - segement " + str(segment_i + 1),
                                        ["Segment approximation error " + str(segment_i + 1)])
        # close PDF file
        pp.close()
        # create report with coefficients
        coeff_txt = outputer.get_file_path('coefficients')
        np.savetxt(coeff_txt, self.get_approximation_polynomial_coefficients())
        print('Done.')
        print("Report created in " + outputer.get_file_path('reports_dir') + " directory.")

    def calculate_coefficient_floating_point(self):
        '''
            Name: calculate_coefficient_floating_point
            Inputs: None.
            Outputs: None.
            Description: Creates one PDF with all information
        '''
        self.calculate_best_approximations()
        self.approximation_polynomial_calculation(list_of_methods=self.get_list_of_methods())
        self.calculate_polynomial_output_data()
        self.calculate_error_value()
        self.calculate_errors()

    def calculate_coefficient_floating_point_with_tolerance(self, polynomial_order_limit: int = 10, output_type: Output_types = Output_types.int16, LSB_ULP: int = 2, out_format: int = 15, input_polynomial_order: int = None):
        '''
            Name: calculate_coefficient_floating_point_with_tolerance
            Inputs: polynomial_order_limit maximum order to choose.  rtol and atol are relative and absolute tolerances.
            Outputs: None.
            Description: Creates one PDF with all information
            TODO: Enchance this part for other formats. This will get an upgrade.
        '''
        if (input_polynomial_order is None):
            polynomial_order = 0
        else:
            polynomial_order = input_polynomial_order
        test_passed = False

        while ((polynomial_order < polynomial_order_limit) and not (test_passed) and (self.rank_warning)):
            if (input_polynomial_order is None):
                polynomial_order = polynomial_order + 1
            self.set_polyOrder(polyOrder=polynomial_order)
            self.calculate_coefficient_floating_point()

            array_ULP = ULP_array(self.approx_out_data, self.output_data, float_bits=Output_types.output_type_to_bit_size(Output_types))
            if np.max(array_ULP) <= LSB_ULP:
                test_passed = True

            if (input_polynomial_order is not None):
                test_passed = True

        # if overfit is bound to happends. At least reverse it's effects.
        if (not (self.rank_warning)) and (input_polynomial_order is None):
            polynomial_order = polynomial_order - 1
            self.set_polyOrder(polyOrder=polynomial_order)
            self.calculate_coefficient_floating_point()

        return test_passed, polynomial_order
