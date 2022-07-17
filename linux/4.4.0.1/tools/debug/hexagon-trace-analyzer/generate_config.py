# The script contains functions to automate generation of config.py for profiling on simulator and target
from __future__ import print_function
import os
import sys
import re
import subprocess
from collections import OrderedDict
import glob
if not os.getenv('SDK_SETUP_ENV'):
        sys.exit("\nSDK Environment not set up -> please run setup_sdk_env script from SDK's root directory.")

hexagon_sdk_root=os.getenv('HEXAGON_SDK_ROOT')
hexta_path = hexagon_sdk_root + '/tools/debug/hexagon-trace-analyzer'
v66OrLesserDevices = ['sm8150','sm7150','sm6150','sm6125','sm8250','rennell','saipan','kamorta','bitra','mannar','qcs405','qcs403','qcs610','qrb5165','sxr2130']

def run_command(command,example_path):    #Function to run the command specified on the command line and print output line by line 
    data=''
    print(command)
    process = subprocess.Popen(command,cwd=example_path,shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT, universal_newlines=True)
    while True:
        output = process.stdout.readline()
        print(output,end='')
        data=data+output
        if not output:
            returncode = process.poll()
            if returncode is not None:
                break
    if returncode!=0:
        print('Error on Execution')
        sys.exit(returncode)
    return data

def get_loadaddress(cmdoutput,binFile):    #Function to return the load address of the simulator binary being executed
    mapped_string = ""
    splitdata = cmdoutput.split("\n")
    for i in range(len(splitdata)):
        if splitdata[i].__contains__("mapped"):
            mapped_string = splitdata[i]
            break
    if not mapped_string:
        sys.exit('The load address of '+binFile+' was not found in the simulator logs. Please ensure the simulator runs properly')
    load_address_list = re.findall(r'\b[a-f]\w+',mapped_string)
    return load_address_list[0]

def generate_config_simulator(loadAddress,device_info,dsp_dir,binFile,example_path):    #Function to generate config_simulator.py
    f=open(os.path.join(example_path,'config_simulator.py'),"w+")
    f.write("LLVM_TOOLS_PATH = \""+os.path.join(os.getenv('DEFAULT_HEXAGON_TOOLS_ROOT'),'Tools','bin/')+"\" \n")
    f.write("elfList=[\""+os.path.join(hexagon_sdk_root,'libs','run_main_on_hexagon','ship','hexagon_'+device_info.tools_variant+'_'+device_info.dsp_arch,'run_main_on_hexagon_sim'+"\" , \n" ))
    f.write("\""+os.path.join(example_path,dsp_dir,binFile)+"\", \n")
    f.write("]\n\n")
    f.write("elfOffsets=[0x00000000,\n")
    f.write(loadAddress+" \n")
    f.write("]")

def generate_config_target(output,device_number,dsp_dir,target,example_path):    #Function to generate config_example.py
    unordered_elf_and_address = []
    list_of_files = []
    filepaths = glob.glob('{}/*.so'.format(os.path.join(example_path,dsp_dir,'ship')))
    for i in range(len(filepaths)):
        list_of_files.append(filepaths[i].split('/').pop())
    splitdata = output.split("\n")
    for i in range(len(splitdata)):
        if splitdata[i].__contains__("fastrpc_shell_3") or (any(ele in splitdata[i] for ele in list_of_files) is True) :
            if(splitdata[i+1].__contains__("LOAD_ADDRESS")):
                unordered_elf_and_address.append((splitdata[i].split(" = ")[1],splitdata[i+1].split(" = ")[1]))
            elif(splitdata[i+2].__contains__("LOAD_ADDRESS")):
                unordered_elf_and_address.append((splitdata[i].split(" = ")[1],splitdata[i+2].split(" = ")[1]))
    countLibs = 0
    countFrpcShell = 0
    elf_and_address_list = list(OrderedDict.fromkeys(unordered_elf_and_address))
    for i in range(len(elf_and_address_list)):
        if any(ele in elf_and_address_list[i] for ele in list_of_files) is True:
            countLibs = countLibs+1
        elif elf_and_address_list[i].__contains__("fastrpc_shell_3"):
            countFrpcShell = countFrpcShell + 1
    if countLibs == 0 or countFrpcShell == 0:
        sys.exit("Project skel not present. Please reboot the device and re-run the script")
    f=open(os.path.join(example_path,'config_example.py'),"w+")
    f.write("LLVM_TOOLS_PATH = \""+os.path.join(os.getenv('DEFAULT_HEXAGON_TOOLS_ROOT'),'Tools','bin/')+"\" \n")
    if v66OrLesserDevices.count(target) > 0:
        f.write("ver=\"V67\" \n")
    else:
        f.write("ver=\"V68\" \n")
    f.write("elfList=[\""+os.path.join(hexta_path,'binaries',device_number,'image','cdsp.mdt')+"\",\n")
    elf_names = [names for names,address in elf_and_address_list]
    elf_address = [address for names,address in elf_and_address_list]
    for i in range(len(elf_names)):
        if elf_names[i].__contains__("fastrpc"):
            f.write("\""+os.path.join(hexta_path,'binaries',device_number,elf_names[i])+"\",\n" )
        else:
            f.write("\""+os.path.join(example_path,dsp_dir,elf_names[i])+"\",\n")
    f.write("]\n\n")
    f.write("elfOffsets=[0x00000000,\n")
    for i in range(len(elf_address)):
        f.write(elf_address[i]+", \n")
    f.write("]")

