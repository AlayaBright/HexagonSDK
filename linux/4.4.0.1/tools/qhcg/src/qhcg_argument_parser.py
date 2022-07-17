# %%
import argparse
import sys
import sympy as sym
from sympy.parsing.sympy_parser import (
    parse_expr, standard_transformations, implicit_application)
import numpy as np
from qhcg_config import Input_types, Output_types, Optimize_by, list_of_enum_options_in_str

# %%


def optimize_by_str_to_enum(input_string: str) -> Optimize_by:
    if (input_string in list_of_enum_options_in_str(Optimize_by)):
        return Optimize_by[input_string]


class argument_parser():
    '''
    Performs the following tasks:
       --- build ineractive environment to allow command line arguments
       --- validate command line arguments
       --- define getters
    '''
    def __init__(self):
        self.list_of_input_types = list_of_enum_options_in_str(Input_types)
        self.list_of_output_types = list_of_enum_options_in_str(Output_types)
        self.parser = argparse.ArgumentParser(description="Approximation Toolbox \
                                              for HVX code generation",
                                              usage="\n\nExample inputs --input_range -2.5 -0.5 " +
                                                    "--function '1/x' --input_type int16 \n " +
                                                    "See --help for more information")
        self.__add_required_args()
        self.__add_optional_args()
        self.args = None
        self.argv = ''

    # Name: parse_cmd_line_input
    # Input: System cmd line
    # Output: None
    # Description:
    def parse_cmd_line_input(self, argv: list = sys.argv):
        self.argv = argv
        # remove name_of_script.py if in command line.
        if ((argv[0][-3:] == '.py')):
            argv = argv[1:]
        self.args = self.parser.parse_args(argv)
        self.__validate_user_inputs()

    def get_argv(self) -> list:
        return self.argv

    def get_input_range(self) -> list:
        return self.args.input_range

    # get input function as string
    def get_input_string_function(self) -> str:
        return self.args.function

    # get numpy computable input function
    def get_computable_input_function(self) -> sym.function:
        '''
           generate sympy equivalent function from string expression
           that can be used in compuatation
        '''
        t = standard_transformations + (implicit_application,)
        z = parse_expr(self.get_input_string_function(), transformations=t)
        x = sym.Symbol('x')
        function = sym.simplify(z)
        return sym.lambdify(x, function, 'numpy')

    def get_input_type_enum(self) -> 'Input_types':
        return Input_types[self.args.input_type]

    def get_input_type_str(self) -> str:
        return self.args.input_type

    def get_output_type_enum(self) -> 'Output_types':
        return Output_types[self.output_type]

    def get_output_type_str(self) -> str:
        return self.output_type

    def get_tolerance(self) -> int:
        return self.args.tolerance

    def get_toolchain(self) -> str:
        return self.args.toolchain

    def get_output_path(self) -> str:
        return self.args.output_path

    def get_function_name(self) -> str:
        return self.args.func_name

    def get_polynomial_order(self) -> int:
        return self.args.polynomial_order

    def get_execute_benchmark_flag(self) -> bool:
        return self.args.exec_bench

    def get_bundle_flag(self) -> bool:
        return self.args.bundle

    def get_report_flag(self) -> bool:
        return self.args.summary

    # function to return all inputs which are parsed
    # input range, function, input_type, and tolerance
    def get_all_parsed_functions(self):
        return self.args.input_range, self.args.function, self.args.input_type, self.output_type, self.args.tolerance

    def get_optimise_by(self) -> Optimize_by:
        return optimize_by_str_to_enum(self.args.optimize_by)

    def __add_required_args(self):
        ''' adds all mandatory command line arguments that must
            be passed when invoking QHCG tool.
        '''
        Required_group = self.parser.add_argument_group(title="Required inputs")

        Required_group.add_argument("--input_range",
                                    nargs=2,
                                    metavar=('input_range_start', 'input_range_end'),
                                    type=float,
                                    default=None,
                                    help="Float input range for given function. Required input.\n",
                                    required=True)

        Required_group.add_argument("--function",
                                    metavar=('input_function'),
                                    default=None,
                                    help="String of function. Required input.\n",
                                    required=True)

        Required_group.add_argument("--input_type",
                                    metavar=('pick one type'),
                                    default=Input_types.int16.value,
                                    help="input_types: " + str(self.list_of_input_types) + " Required input\n",
                                    required=True)

    def __add_optional_args(self):
        ''' adds all optional  command line arguments that may
            be passed when invoking QHCG tool.
        '''
        optional_group = self.parser.add_argument_group("optional arguments")

        optional_group.add_argument("--tolerance",
                                    metavar=('float value'),
                                    type=float,
                                    default=1.0,
                                    help="Float value describing LSB or ULP depending on input \
                                         and output types.\n",
                                    required=False)

        optional_group.add_argument("--toolchain",
                                    metavar=('path'),
                                    default=None,
                                    help="Set custom toolchain path for Hexagon Tools.\n",
                                    required=False)

        optional_group.add_argument("--output_path",
                                    metavar=('path'),
                                    default=None,
                                    help="Set custom output directory path.\n",
                                    required=False)

        optional_group.add_argument("--func_name",
                                    metavar=('string'),
                                    default='qhcg_approximation',
                                    help="Set custom function name. Default is 'generated_hvx_func'\n",
                                    required=False)

        optional_group.add_argument("--optimize_by",
                                    metavar=('string'),
                                    type=str,
                                    choices=list_of_enum_options_in_str(Optimize_by),
                                    default="accuracy",
                                    help="Choises are accuracy and speed. Default value is accuracy. \
                                         Give QHCG an option to hasten code but accuracy will be worst in \
                                         this case. Default is False. \n",
                                    required=False)

        optional_group.add_argument("--polynomial_order",
                                    type=int,
                                    metavar=('int'),
                                    default=None,
                                    help="Positive integer input. Forces QHCG to search for best polynomial \
                                         approximation for given polynomial order. If not given QHCG will \
                                         search for best polynomial order that is suits all accuracy bounds. \n",
                                    required=False)

        optional_group.add_argument("--exec_bench",
                                    action='store_true',
                                    help="Execute benchmark on Hexagon simulator (long process). \n",
                                    required=False)

        optional_group.add_argument("--bundle",
                                    action='store_true',
                                    help="Execute QHCG in bundle mode. The destination folder will \
                                         not be deleted initially.\n",
                                    required=False)

        optional_group.add_argument("--summary",
                                    action='store_true',
                                    help="Create summary report. Otherwise, QHCG will create detailed report. \n",
                                    required=False)

    def __validate_user_inputs(self):
        '''
           validates command line arguments if they meet
           QHCG design requirements.
        '''
        if (self.args.input_range and self.args.function):
            if (self.args.input_range[0] >= self.args.input_range[1]):
                self.parser.error("Range [a, b] needs to be a < b!")

        if (self.args.input_type not in self.list_of_input_types):
            self.parser.error("Available types for input types are :" + str(self.list_of_input_types))
        else:
            if self.args.input_type == 'int16':
                self.output_type = 'int16'
            elif self.args.input_type == 'int32':
                self.output_type = 'int32'
            elif self.args.input_type == 'float16':
                self.output_type = 'float16'
            elif self.args.input_type == 'float32':
                self.output_type = 'float32'

        if (self.args.tolerance < 0.0):
            self.parser.error("Tolerance input needs to be positive numbers ...")

        if (self.args.polynomial_order is not None) and (self.args.polynomial_order < 1):
            self.parser.error("Polynomial_order input needs to be positive integer number ...")

        if (self.args.optimize_by not in list_of_enum_options_in_str(Optimize_by)):
            self.parser.error("Available types for input types are :" + list_of_enum_options_in_str(Optimize_by))

        if (self.args.optimize_by == str(Optimize_by.speed)):
            if (self.args.input_type == str(Input_types.float32)):
                print("INFO: Using flag --optimize_by speed in float32->float32 will have same output as --optimize_by accuracy")
            elif (self.args.input_type == str(Input_types.int32)):
                print("INFO: Using flag --optimize_by speed in int32->int32 will have same output as --optimize_by accuracy")
