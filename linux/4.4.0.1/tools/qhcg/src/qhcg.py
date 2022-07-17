# %%
from qhcg_toolbox import approximation_toolbox as at
from qhcg_function_parser import function_parser as fp
from qhcg_argument_parser import argument_parser as ap
import qhcg_gen_data as gen_data
import qhcg_fxp_parser as fxp_parser
from qhcg_generate_hvx import GeneratorHVX_generic
from qhcg_outputer import Outputer
from qhcg_hexagon_toolchain import HexagonToolchain
from qhcg_process_output import process_output as op
from qhcg_config import Input_types, Output_types, Function_parser_methods, Optimize_by
# %%
# create system parser
argument_parser = ap()
# parse data from cmd line
argument_parser.parse_cmd_line_input()
# %%
# create outputer
outputer = Outputer(output_path=argument_parser.get_output_path(), function_name=argument_parser.get_function_name(), bundle=argument_parser.get_bundle_flag())
outputer.set_args(argument_parser.get_all_parsed_functions())

# TODO: Create additional parser options
N_seg = 16

# %%
# data_range : list with lowest and highest points of range in which approximation is done
data_range = argument_parser.get_input_range()
start_of_range = data_range[0]
end_of_range = data_range[1]

# N : number of elements for generating larger
N_per_seg = 10000
N = N_seg * N_per_seg

# N_sample : number of elements for sampling / number of chebyshev nodes

N_sampling_per_seg = 100

# input_data generating
function_name = argument_parser.get_input_string_function()
computable_function = argument_parser.get_computable_input_function()
function_class = fp(input_string_function=function_name, computable_input_function = computable_function, list_range=data_range, number_of_points=N, number_of_segments=N_seg, method=Function_parser_methods.random, number_of_samples_per_seg=N_sampling_per_seg)
input_data = function_class.get_input_data()
output_data = function_class.get_output_data()
input_data_resampled_seg = function_class.get_input_resampled_data()
output_data_resampled_seg = function_class.get_output_resampled_data()
# # %%
input_type = argument_parser.get_input_type_enum()
output_type = argument_parser.get_output_type_enum()
# Generate test data in the agreed path
in_size = Input_types.input_type_to_bit_size(input_type)
out_size = Output_types.output_type_to_bit_size(output_type)

if input_type == Input_types.int16 or input_type == Input_types.int32:
    (in_fmt, out_fmt) = fxp_parser.find_qfmt_in_out(computable_function, start_of_range, end_of_range, in_size, out_size)
if input_type == Input_types.float16 or input_type == Input_types.float32:
    out_fmt = 0   # TODO:  Find better solution for for in_fmt and out_fmt story.

# %%
toolbox = at(num_seg=N_seg, input_data=input_data, output_data=output_data, input_data_resampled_seg=input_data_resampled_seg, output_data_resampled_seg=output_data_resampled_seg)
input_polynomial_order = argument_parser.get_polynomial_order()
test_passed, polynomial_order = toolbox.calculate_coefficient_floating_point_with_tolerance(polynomial_order_limit=15, output_type=output_type, LSB_ULP=argument_parser.get_tolerance(), out_format=out_fmt, input_polynomial_order=input_polynomial_order)
toolbox.print_error_report(per_segment_report=False)
print("\nChosen polynomial order: " + str(polynomial_order))
# %%
speed_flag = (argument_parser.get_optimise_by() == Optimize_by.speed)

# TODO: add part for floating number
# Generate input points related to input type, input range and input format (if talk about fixed-point)
in_test_arr = gen_data.save_input(outputer, start_of_range, end_of_range, input_type)
# %% coefficients
print('\nGenerate HVX code for approximated function ...')
gen_hvx = GeneratorHVX_generic(function_name, computable_function, outputer, start_of_range, end_of_range, toolbox.get_approximation_polynomial_coefficients(), polynomial_order, input_type, output_type, speed_flag)

gen_hvx.generate()
print('Done.')

# Generate input/output reference points
out_test_arr = gen_data.save_output_float(outputer, in_test_arr, computable_function, input_type, output_type, gen_hvx.getInputFMT(), gen_hvx.getOutputFMT())

# %%
# Hexagon toolchain for HVX code
hvx_arch = "v68"
hvx = HexagonToolchain(outputer, hvx_arch, argument_parser.get_toolchain())

outputer.save_version(hvx.get_version_str())

# %%
print('\nCompile HVX code ...')
hvx.compile()
print('Done.')

# %%
print('\nExecute functional test of generated HVX code ...')
hvx.execute_test()

# %%
process_output = op(input_types=input_type, output_types=output_type, input_format=gen_hvx.getInputFMT(), output_format=gen_hvx.getOutputFMT())
process_output.process_output_of_HVX(outputer=outputer)

# %%
if argument_parser.get_execute_benchmark_flag():
    print('\nExecute performance test of generated HVX code ...')
    hvx.execute_bench()
    print('Done.')

# %%
outputer.write_readme()

# %%
toolbox.create_report(outputer=outputer, 
                      process_output=process_output, 
                      summary=argument_parser.get_report_flag(), 
                      input_string_function=function_name)
# %%
