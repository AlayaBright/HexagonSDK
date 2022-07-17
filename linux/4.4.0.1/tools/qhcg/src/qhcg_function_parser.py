# %%
import numpy as np
import matplotlib.pyplot as plt
import math as m
from qhcg_config import Function_parser_methods, list_of_enum_options
# %%


class function_parser():
    '''
       Class peforms the following:
         --- generate input dataset within input range
         --- convert user input function into a symbolized form
         --- generate resampled input and output data needed  for approximation
    '''
    def __init__(self,
                 input_string_function: str,
                 computable_input_function,
                 list_range,
                 number_of_points: int,
                 method: Function_parser_methods = Function_parser_methods.linspace,
                 number_of_segments: int = 16,
                 number_of_samples_per_seg: int = 100):
        self.input_string_function = input_string_function
        self.computable_function = computable_input_function
        self.list_range = list_range
        self.number_of_samples_per_seg = number_of_samples_per_seg
        self.number_of_points = int(number_of_points)
        self.output_data = None
        self.input_data = None
        self.list_of_methods = list_of_enum_options(Function_parser_methods)
        self.number_of_segments = number_of_segments
        self.output_data_resampled_seg = None
        self.input_data_resampled_seg = None
        self.method = method
        np.random.seed(9001)

    def get_input_data(self):
        '''
            split up user input range into a number of segments
            and generate N= 10000 sample points for each segment.
            Generated data follows a uniform or random
            distribution model
        '''
        if (self.input_data is None):
            self.__calculate_input_data()
        return self.input_data

    def get_input_resampled_data(self):
        '''
           generate input  sample data that is need to
           find the best fit polynomial approximated function.
        '''
        if (self.input_data_resampled_seg is None):
            self.__calculate_input_resampled_data()
        return self.input_data_resampled_seg

    def get_output_data(self):
        '''
           generate golden reference by passing input data
           through user defined function.
        '''
        if (self.output_data_resampled_seg is None):
            self.__calculate_output_data()
        return self.output_data

    def get_output_resampled_data(self):
        '''
           generate  resampled output data by passing input
           resampled data through user defined function.
        '''
        if (self.output_data_resampled_seg is None):
            self.__calculate_output_resampled_data()
        return self.output_data_resampled_seg

    def get_list_of_methods(self):
        return self.list_of_methods

    def __calculate_input_data(self):
        self.input_data = np.zeros(self.number_of_points)
        if (self.method == Function_parser_methods.linspace):
            self.input_data = np.linspace(self.list_range[0], self.list_range[1], self.number_of_points)
        if (self.method == Function_parser_methods.random):
            self.input_data = np.sort(np.random.uniform(self.list_range[0], self.list_range[1], self.number_of_points))

    def __calculate_input_resampled_data(self):
        self.input_data_resampled_seg = [[] for i in range(self.number_of_segments)]
        # Calling self.get_input_data() ensures that we have all the data
        temp_input_data = np.array_split(self.get_input_data(), self.number_of_segments)
        for i in range(0, self.number_of_segments):
            start_range = temp_input_data[i][0]
            end_range = temp_input_data[i][-1]
            ChebyshevInputTransformation = lambda x: 0.5 * (start_range + end_range) + \
                                                  0.5 * (end_range - start_range) * m.cos((2 * x - 1) * \
                                                  m.pi / (2 * self.number_of_samples_per_seg))

            self.input_data_resampled_seg[i] = [ChebyshevInputTransformation(x + 1)
                                              for x in range(0, self.number_of_samples_per_seg)]

    def __calculate_output_data(self):
        self.output_data = self.computable_function(self.get_input_data())

    def __calculate_output_resampled_data(self):
        self.output_data_resampled_seg = [[] for i in range(self.number_of_segments)]
        input_data = self.get_input_resampled_data()
        for i in range(0, self.number_of_segments):
            self.output_data_resampled_seg[i] = self.computable_function(np.array(input_data[i]))

