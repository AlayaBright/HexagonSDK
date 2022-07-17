#!/usr/bin/env python
# child python script

### Imports ###
import os
import sys
import argparse
import platform
from os import path
from argparse import SUPPRESS

#******************************************************************************
# Parser for cmd line options
#******************************************************************************
parser = argparse.ArgumentParser(prog='debug_exceptions.py', usage=SUPPRESS, epilog='Note: Error code is displayed as part of crash signature in Lahaina and onward targets only.')
group1 = parser.add_argument_group('Debug PD exceptions by parsing crash signature in log file', "Usage: %(prog)s --debug_crash log_file [--lib crashed_library]")
group1.add_argument("--debug_crash", action="store", dest="log_file", default=False, help="path of log file containing the crash signature.")
group1.add_argument("--lib", action="store", dest="crashed_library", default=False, help="path of crashed shared library.")
group2 = parser.add_argument_group('Parse QuRT error code returned during PD crash', "Usage: %(prog)s --parse_error error_code")
group2.add_argument("--parse_error", action="store", dest="error_code", default=False, help="QuRT Error code to to parsed.")
options = parser.parse_args()

# check Python version
# generate a warning message for Python2 or Python 3 below 3.7
if sys.version_info.major < 3 or sys.version_info.minor < 7:
    python = 'python3'
    print("Python 3.7 is recommended. Please upgrade to 3.7 as soon as possible.")
else:
    python = 'python'


# initialize variables
reason = ", Reason: "
run_qurt_script = python + " qurt_error_info.py"
print(run_qurt_script)

HEXAGON_SDK_ROOT = os.getenv('HEXAGON_SDK_ROOT')
if not HEXAGON_SDK_ROOT:
    sys.exit("\nSDK Environment not set up -> Please run setup_sdk_env script from SDK's root directory.")
HEXAGON_TOOLS_ROOT = os.getenv('HEXAGON_TOOLS_ROOT')
if not HEXAGON_TOOLS_ROOT:
    HEXAGON_TOOLS_ROOT = os.getenv('DEFAULT_HEXAGON_TOOLS_ROOT')
    if not os.path.exists(HEXAGON_TOOLS_ROOT):
        sys.exit("\nHexagon Tools Root not found -> Please check Hexagon Tools setup.")

# find cause details and update global variable "reason"
def find_reason(line2):
    global reason
    L = line2.split()
    for i, j in enumerate(L):
        if j == 'code':
            ErrCode= L[i+2]
            os.system(run_qurt_script + " > temp_1.txt " + ErrCode)
            file3 = open('temp_1.txt','r+')
            for line3 in file3:
                if 'Cause details:' in line3:
                    L2 = line3.split()
                    lock = 0
                    for x,y in enumerate(L2):
                            #print(x,y)
                            if lock == 1:
                                reason += ' ' + L2[x]
                            if y == 'details:':
                                lock = 1
                                x = x + 2


# read crash signature and update crash reason
def replace_crash_reason(file):
        print ("\n\nParsing the user input log file : " + file + "\n")
        global reason
        if (os.path.exists(file) != False):
            file1 = open(file,'r+')
        else:
            print ("Failed to locate the log file at provided input: " + file)
            sys.exit(1)
        for line in file1:
            if "unknown reason" in line:
                file2 = open(file,'r+')
                for line2 in file2:
                    if 'Error code' in line2:
                       find_reason(line2)
            if reason != ", Reason: ":
               txt = line
               x1 = txt.replace("for unknown reason", reason)
               print ("\t" + x1)                 # replace unknown reason and prints logs with exact reason on console
               reason  = ", Reason: "
            else:
               print ("\t" +line)               # prints exception logs on console


# find line number in the source file where the crash occurred
def process_crashed_library(file):
        print ("\n\nParsing the user input crashed shared object :" + options.crashed_library)
        if (os.path.exists(file) != False):
            file1 = open(file,'r+')
        else:
            print ("Failed to locate the crashed library: " + file)
            sys.exit(1)
        fault_pc = 0
        load_address = 0
        for line in file1:
            if "Shared Object" in line:
                L = line.split()
                for i, j in enumerate(L):
                   if j == 'address':
                      load_address = L[i+2]
            if "Fault PC" in line:
                L = line.split()
                for i, j in enumerate(L):
                   if j == 'PC':
                      fault_pc = L[i+2]
        if fault_pc and load_address:
            fault_pc = int(fault_pc, 16)
            load_address = int(load_address, 16)
            offset = hex(fault_pc - load_address)
            print ("\nLine number in the source file where the crashed occured :")
            if platform.system() == "Windows":
               add2line = HEXAGON_TOOLS_ROOT + "\Tools\\bin\hexagon-addr2line.exe"
            else:
               add2line = HEXAGON_TOOLS_ROOT + "/Tools/bin/hexagon-addr2line"
            if (os.path.exists(add2line) != False):
                os.system( add2line + " -e " + options.crashed_library + " " + offset)
            else:
                print ("Failed to locate hexagon-add2line in Hexagon Tools")
                sys.exit(1)
        else:
            print ("Failed to retrieve Crashed shared object Load address or Fault PC from the log file")
            sys.exit(1)

# find crash reason from error code
def parse_error_code(error_code):
        print ("\n\nParsing error code: " + error_code + "\n")
        os.system(run_qurt_script + " " + error_code)

# main
if __name__ == '__main__':

    if options.log_file:
        replace_crash_reason(options.log_file)
        if options.crashed_library:
            process_crashed_library(options.log_file)
        sys.exit(0)

    if options.error_code:
        parse_error_code(options.error_code)
        sys.exit(0)

    parser.print_help()
    sys.exit(1)
