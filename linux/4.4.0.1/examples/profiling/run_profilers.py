#!/usr/bin/env python

### Imports ###
from __future__ import print_function
import sys
import os
import subprocess
import argparse
import glob

if not os.getenv('SDK_SETUP_ENV'):
        sys.exit("\nSDK Environment not set up -> please run setup_sdk_env script from SDK's root directory.")

hexagon_sdk_root=os.getenv('HEXAGON_SDK_ROOT')
hexagon_tools_root=os.getenv('HEXAGON_TOOLS_ROOT')
if hexagon_tools_root == None:
    hexagon_tools_root=os.getenv('DEFAULT_HEXAGON_TOOLS_ROOT')
script_dir = hexagon_sdk_root + '/utils/scripts/'
sys.path.append(script_dir)
import Common_Walkthrough as CW
import Device_configuration as dev_conf
generate_config_script_dir = hexagon_sdk_root + '/tools/debug/hexagon-trace-analyzer/'
sys.path.append(generate_config_script_dir)
import generate_config as gen_config
example_name="profiling"
example_path=os.path.join(hexagon_sdk_root, "examples", example_name, "")
hexta_path=os.path.join(hexagon_sdk_root, "tools", "debug", "hexagon-trace-analyzer","")
dsp_dir=""
hlos_dir=""
sim_v_arch=""
binFile=""
pid = os.getpid()
eeceived = False

#******************************************************************************
# Parser for cmd line options
#******************************************************************************
parser = argparse.ArgumentParser(prog='run_profilers.py', description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument("-r", action="store", dest="profilers", default="all", nargs="?", help="Run the specified profiler only. Default setting is run all profilers")
parser.add_argument("-c", action="store_true", dest="skip_script", help="Skip running the one_time_script.py for Hexagon Trace Analyzer")
parser.add_argument("-S",dest="trace_size",default="0x4000000",help="Trace buffer size (bytes) in hexadecimal format. Default is 0x4000000, i.e. 64MB.")
CW.call_parser(parser)
parser = parser.parse_args()

def hexagon_profiler():
    print("Running Hexagon profiler")
    generate_json = hexagon_tools_root + '/Tools/bin/hexagon-sim -m' +sim_v_arch+ ' --timing --packet_analyze hexagon_profiler.json --uarchtrace uarch.txt --simulated_returnval --usefs '+example_path +'/' +dsp_dir+ ' --cosim_file ' +example_path +'/'+dsp_dir+ '/q6ss.cfg --l2tcm_base 0xd800 --rtos ' +example_path +'/' +dsp_dir+ '/osam.cfg ' +hexagon_sdk_root+ '/rtos/qurt/compute' +device_info.dsp_arch+ '/sdksim_bin/runelf.pbn -- ' +hexagon_sdk_root+ '/libs/run_main_on_hexagon/ship/hexagon_'+device_info.tools_variant+'_' +device_info.dsp_arch+ '/run_main_on_hexagon_sim -- ./'+example_path +'/' +dsp_dir+ '/' + binFile +' -f timers'
    cmdOutput = gen_config.run_command(generate_json,example_path)
    loadAddress = gen_config.get_loadaddress(cmdOutput,binFile)
    print("=====Running hexagon profiler assuming the "+ binFile+" is loaded at location " +'0x'+loadAddress+ " on the simulator. Please ensure this address is correct.")
    print("=====This address is referred as vaddr in the console logs and its value is provided below the text that includes the shared object name, "+binFile+". For further information please refer to the profiling documentation.")
    generate_html = hexagon_tools_root + '/Tools/bin/hexagon-profiler --packet_analyze --elf=' +hexagon_sdk_root+ '/libs/run_main_on_hexagon/ship/hexagon_'+device_info.tools_variant+'_' +device_info.dsp_arch+ '/run_main_on_hexagon_sim,' +dsp_dir+ '/' +binFile+ ':0x' +loadAddress+ ' --json=hexagon_profiler.json -o hexagon_profiler.html'
    dev_conf.print_and_run_cmd(generate_html)
    print("Hexagon Profiler run has ended.")

def hexagon_trace_analyzer():
    #Runs only on Linux system
    if sys.platform == "win32":
        print("Hexagon Trace Analyzer not supported on Windows")
        return
    if sys.version_info > (3, 0):
        print ("=====Flamegraph and Catapult don't support Python 3. The Hexagon Trace Analyzer results will be incomplete")
    if os.path.isdir(hexta_path+'FlameGraph'):
        print ("Directory framegraph exists, not downloading it")
    else:
        print ("Directory framegraph does not exist, downloading it to" +hexta_path)
        dev_conf.print_and_run_cmd('wget https://github.com/brendangregg/FlameGraph/archive/master.zip && unzip master.zip && rm master.zip && mv FlameGraph-master ' +hexta_path+'FlameGraph')

    if os.path.isdir(hexta_path+'catapult'):
        print ("Directory catapult exists")
    else:
        print ("Directory catapult does not exist, downloading it to" +hexta_path)
        dev_conf.print_and_run_cmd('wget https://github.com/catapult-project/catapult/archive/master.zip && unzip master.zip -x *.json "*tracing/test_data/*" && rm master.zip && mv catapult-master ' +hexta_path+'catapult')
    print("example_path :  "+example_path)

    if os.path.isdir(example_path + dsp_dir):
        print("Running Hexagon Trace Analyzer on simulator")
        simulate = hexagon_tools_root + '/Tools/bin/hexagon-sim -m' +sim_v_arch+ ' --timing --pctrace_nano pctrace.txt --simulated_returnval --usefs '+example_path +'/' +dsp_dir+ ' --pmu_statsfile '+example_path +'/' +dsp_dir+ '/pmu_stats.txt --cosim_file '+example_path +'/' +dsp_dir+ '/q6ss.cfg --l2tcm_base 0xd800 --rtos '+example_path +'/' +dsp_dir+ '/osam.cfg ' +hexagon_sdk_root+ '/rtos/qurt/compute' +device_info.dsp_arch+ '/sdksim_bin/runelf.pbn -- ' +hexagon_sdk_root+ '/libs/run_main_on_hexagon/ship/hexagon_'+device_info.tools_variant+'_' +device_info.dsp_arch+ '/run_main_on_hexagon_sim -- ./'+example_path +'/' +dsp_dir+ '/' + binFile +' -f timers'
        cmdOutput = gen_config.run_command(simulate,example_path)
        loadAddress = gen_config.get_loadaddress(cmdOutput,binFile)
        print("=====Running Hexagon Trace Analyzer assuming the "+binFile+" is loaded at location " +'0x'+loadAddress+ " on the simulator. Please ensure this address is correct.")
        print("=====This address is referred as vaddr in the console logs and its value is provided below the text that includes the shared object name, "+ binFile+". For further information please refer to the profiling documentation.")
        gen_config.generate_config_simulator('0x'+loadAddress,device_info,dsp_dir,binFile,example_path)
        dev_conf.print_and_run_cmd(hexta_path+'hexagon-trace-analyzer '+example_path+'config_simulator.py '+example_path+'result_hexta_simulator '+example_path+'pctrace.txt')
    else:
        print("Running Hexagon Trace analyzer on simulator only if hexagon_"+device_info.build_flavor+"_"+device_info.tools_variant+"_"+device_info.dsp_arch+" variant is built. Edit the config_simulator.py file to run for other variants.")
    print("Running Hexagon Trace Analyzer on target")
    print("Running the application on target. Make sure the binary exists on the device. If not, run profiling_walkthrough.py and try again.")
    dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+example_name+' -U 0 -f timers')

    if options.skip_script:
        dev_conf.print_and_run_cmd('python '+hexta_path+'target_profile_setup.py -T '+options.target+' -s '+device_number+' -S '+options.trace_size)
    else:
        dev_conf.print_and_run_cmd('python '+hexta_path+'target_profile_setup.py -O -T '+options.target+' -s '+device_number+' -S '+options.trace_size)
    #This runs the timer sections of the profiling example
    dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+example_name+' -U 0 -f timers')
    dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device shell "cat /dev/coresight-tmc-etr > /data/local/tmp/trace.bin" ')
    dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device pull /data/local/tmp/trace.bin '+example_path+'trace.bin ')
    sysmonOutput = gen_config.run_command('adb -s '+device_number+' wait-for-device shell /data/local/tmp/sysMonApp etmTrace --command dll --q6 cdsp',example_path)
    gen_config.generate_config_target(sysmonOutput,device_number,dsp_dir,options.target,example_path)
    dev_conf.print_and_run_cmd(hexta_path+'hexagon-trace-analyzer '+example_path+'config_example.py '+example_path+'results_hexta_target '+example_path+'trace.bin')

def sysMonApp_test_cdsp():
    dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device push ' +hexagon_sdk_root+ '/tools/utils/sysmon/sysMonApp /data/local/tmp/')
    dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device shell chmod 777 /data/local/tmp/sysMonApp')
    command_sysmon = 'adb -s '+device_number+' shell /data/local/tmp/sysMonApp profiler --defaultSetEnable 1 --q6 cdsp'
    print(command_sysmon)
    #This is needed as we need to run sysmonapp in a separate process that stops working only when Return key is pressed. This is done after the process being profiled is completed
    sysMonApp_proc = subprocess.Popen(command_sysmon, shell=True, stdin=subprocess.PIPE, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
    print("Running the application on target. Make sure the binary exists on the device. If not, run profiling_walkthrough.py and try again.")
    dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+example_name+' -f default')

    sysmon_output=sysMonApp_proc.communicate(input=("\n").encode())
    print("From sysMonApp: ", sysmon_output)

    pull_cmd_1 = 'adb -s '+device_number+' wait-for-device pull /sdcard/sysmon_cdsp.bin ./'
    pull_cmd_2 = 'adb -s '+device_number+' wait-for-device pull /data/sysmon_cdsp.bin ./'
    try:
        print(pull_cmd_1)
        output = subprocess.check_output(pull_cmd_1, stderr = subprocess.STDOUT, shell = True).decode('utf-8')
    except Exception as e:
        output = str(e)
        print("Error pulling from sysmon_cdsp.bin from /sdcard/ : "+ output)
        print("Attempting to pull sysmon_cdsp.bin from /data/ instead")
        try:
            print(pull_cmd_2)
            output1 = subprocess.check_output(pull_cmd_2, stderr = subprocess.STDOUT, shell = True).decode('utf-8')
        except Exception as e:
            output = str(e)
            print("Error pulling from sysmon_cdsp.bin from /data/ : "+ output)
            print("sysMonApp failed.")
            return

    #Generate HTML file and CSV from the .bin
    if sys.platform.startswith('linux'):
        dev_conf.print_and_run_cmd(hexagon_sdk_root+ '/tools/utils/sysmon/parser_linux_v2/HTML_Parser/sysmon_parser ./sysmon_cdsp.bin --outdir sysmon_parsed_output')
    elif sys.platform == "win32":
        dev_conf.print_and_run_cmd(hexagon_sdk_root+ '/tools/utils/sysmon/parser_win_v2/HTML_Parser/sysmon_parser.exe ./sysmon_cdsp.bin --outdir sysmon_parsed_output')

    print("sysMonApp run has ended.")

if __name__ == "__main__":
    global options
    if os.getcwd() != hexagon_sdk_root+'/examples/profiling/':
        os.chdir(hexagon_sdk_root+'/examples/profiling')
    device_info = CW.get_config() #creating an object for get_config class in Common_Walkthrough script
    #parsing the command line parameters and subsystem Flags
    options, hex_variant, variant, arch, dsp_dir, hlos_dir = device_info.get_parameters()
    device_number = options.serial_num
    call_test_sig , APPS_DST, DSP_DST, LIB_DST, DSP_LIB_PATH, DSP_LIB_SEARCH_PATH = CW.get_DST_PARAMS(device_info, hexagon_sdk_root)

    if options.profilers == "clean":
        dev_conf.print_and_run_cmd('rm -rf hexagon_profiler.json hexagon_profiler.html uarch* sysmon_cdsp.bin trace.bin sysmon_output* pctrace.txt *.pyc ')
        sys.exit()

    if options.target != "simulator":
        #Hexagon Trace Analyzer and sysMonApp might not work on the following targets. Refer the Feature Matrix to check for support. The user might try to run the profilers on these targets separately, but results are not guaranteed.
        unsupported_target_info = ["qcs405", "sm7150", "sm6125", "sm6150", "sxr1130", "sxr2130", "kamorta", "rennell", "bitra", 'agatti', "qrb5165", "qcs403","qcs610", "mannar","divar"]

    if options.target in unsupported_target_info:
        print("Error! "+ options.target+" is not supported.")
        sys.exit()

    if os.path.isdir(hexagon_sdk_root+'/examples/profiling/'+dsp_dir):
        #Taking maked and cmake binaries into account
        if sys.platform == "win32":
            binFile = glob.glob('{}/*{}_q.so'.format(os.path.join(example_path,dsp_dir),example_name))[0].split("\\").pop()
        else:
            binFile = glob.glob('{}/*{}_q.so'.format(os.path.join(example_path,dsp_dir),example_name))[0].split("/").pop()
        print("The dsp directory "+dsp_dir+" exists. ")
    else:
        print("The dsp directory "+dsp_dir+" does not exist. Please build the variant.")
        sys.exit()

    if device_info.dsp_arch == "v65":
        sim_v_arch = device_info.dsp_arch+ "a_1024"
    elif device_info.dsp_arch == "v68":
        sim_v_arch = device_info.dsp_arch+ "n_1024"
    elif device_info.dsp_arch == "v66":
        sim_v_arch = device_info.dsp_arch+ "g_1024"
    else:
        print("Variant not supported")
        sys.exit()

    if options.profilers == "hexagon_profiler":
        hexagon_profiler()
    elif options.profilers == "hexagon_trace_analyzer":
        if sys.platform.startswith("linux"):
            hexagon_trace_analyzer()
        else:
            print("Cannot run hexagon_trace_analyzer on " + sys.platform)
    elif options.profilers == "sysmon_app":
        if options.target != "simulator":
            sysMonApp_test_cdsp()
        else:
            print("Cannot run Sysmon App on simulator. Can only run on device")
    elif options.profilers == "all":
        print("Platform is: "+sys.platform)
        hexagon_profiler()
        if sys.platform.startswith('linux'):
            hexagon_trace_analyzer()
        if options.target != "simulator":
            sysMonApp_test_cdsp()
    print("Run Profilers script successful.")
