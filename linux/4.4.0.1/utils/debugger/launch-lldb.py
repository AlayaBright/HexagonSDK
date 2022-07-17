
# Description:
# The launch-lldb script simplifies the process of debugging an executable that was previously simulated.
# The script parses a simulator command passed through the command line or stored in a sim_cmd_line.txt file and launches hexagon-lldb to run the same code within the debugger

import os
import sys
import re
import argparse
import platform
from argparse import RawTextHelpFormatter

#******************************************************************************
# Parser for cmd line options
#******************************************************************************
parser=argparse.ArgumentParser(prog="launch-lldb.py", description="Providing the lldb command by taking the simulator command or the pm_stats.txt file as arguments", formatter_class=RawTextHelpFormatter)#, usage="launch-lldb.py [-f <filename>]" )

HEXAGON_SDK_ROOT = os.getenv('HEXAGON_SDK_ROOT')
if not HEXAGON_SDK_ROOT:
    sys.exit("\nSDK Environment not set up -> Please run setup_sdk_env script from SDK's root directory.")
DEFAULT_HEXAGON_TOOLS_ROOT = os.getenv('DEFAULT_HEXAGON_TOOLS_ROOT')

def print_and_run_cmd(cmd):
    print(cmd)
    return os.system(cmd)

args_filename = sys.argv[1]
fp=open(args_filename)
sim_command=fp.read()

environment = platform.system()

if environment == "Windows":
    lldb_exe_root=DEFAULT_HEXAGON_TOOLS_ROOT + '/Tools/bin/hexagon-lldb.exe '
    if args_filename.rfind('\\') != -1:
        absolute_file_path = (args_filename[:args_filename.rfind('\\')])
        replace_file_name = absolute_file_path[absolute_file_path.rfind('\\')+1:]
    else:
        absolute_file_path = (args_filename[:args_filename.rfind('/')])
        replace_file_name = absolute_file_path[absolute_file_path.rfind('/')+1:]
else:
    lldb_exe_root=DEFAULT_HEXAGON_TOOLS_ROOT + '/Tools/bin/hexagon-lldb '
    absolute_file_path = args_filename[:args_filename.rfind('/')]
    replace_file_name = absolute_file_path[absolute_file_path.rfind('/')+1:]

#Get run_main_on_hexagon from the simulator command
st_ind = sim_command.find('run_main_on_hexagon_sim')
las_ind = sim_command.rfind(' ',0,st_ind)
program_to_debug = sim_command[las_ind:sim_command.find(' ',st_ind)]

start_index=sim_command.find(' ')
last_index = sim_command.find('osam.cfg')
sim_args = sim_command[start_index:sim_command.find(' ',last_index)]

first_index=sim_command.rfind('.so')
filename_index=sim_command.rfind(' ',0,first_index)
program_args = sim_command[filename_index+1:first_index+3]

variant_arg_index = sim_command.find('--usefs')
variant_index = sim_command.find(' ',variant_arg_index)
variant_arg = sim_command[variant_index +1: sim_command.find(' ',variant_index+1)]

#for examples without run_main_on_hexagon
if environment=='Windows':
    begin_index = sim_command.rfind(' ',0,sim_command.rfind('/'))
    Program_to_debug_with_no_hexagon = sim_command[begin_index+1:sim_command.rfind(' ')]
else:
    begin_index = sim_command.rfind(' ',0,sim_command.rfind(' '))
    Program_to_debug_with_no_hexagon = sim_command[begin_index+1:sim_command.rfind(' ')]

program_index=sim_command.find(' ')
program_args_with_no_hexagon_main = sim_command[program_index+1:begin_index]


if '.pbn' in sim_command:
    os.environ['LLDB_HEXAGON_BOOTER_PATH'] = HEXAGON_SDK_ROOT + '/rtos/qurt/computev65/sdksim_bin/runelf.pbn'
    command_with_hexagon_main = lldb_exe_root + program_to_debug + " -o \"image search-paths add . " + replace_file_name + "\"" +' --' + sim_args + ' -- -- ' +'./' + variant_arg +'/' + program_args
    command_with_hexagon_main = command_with_hexagon_main.replace(replace_file_name,absolute_file_path)
    print_and_run_cmd(command_with_hexagon_main)
else:
    os.unsetenv('LLDB_HEXAGON_BOOTER_PATH')
    abs_path = os.path.abspath(args_filename)
    if environment == "Windows":
        exe_index = abs_path.rfind('\\',0,abs_path.rfind('\\'))
    else:
        exe_index = abs_path.rfind('/',0,abs_path.rfind('/'))

    exe_file_path = abs_path[:exe_index]
    program_args_with_no_hexagon_main = program_args_with_no_hexagon_main.replace(replace_file_name,absolute_file_path)
    command_with_no_hexagon_main = lldb_exe_root + exe_file_path + '/' + Program_to_debug_with_no_hexagon +'   -- '  + program_args_with_no_hexagon_main
    print_and_run_cmd(command_with_no_hexagon_main)