import os
import sys
import subprocess
import argparse

HEXAGON_SDK_ROOT = os.getenv('HEXAGON_SDK_ROOT')
if not HEXAGON_SDK_ROOT:
    sys.exit('\n SDK Environment not setup. Please run setup_sdk_env script from SDK root directory')

script_dir = HEXAGON_SDK_ROOT + '/utils/scripts/'
sys.path.append(script_dir)
import Common_Walkthrough as CW

parser = argparse.ArgumentParser(prog='target_profile_setup.py',formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument("-S",dest="trace_size",default="0x4000000",help="Trace buffer size (bytes) in hexadecimal format. Default is 0x4000000, i.e. 64MB.")
parser.add_argument("-O",dest="one_time_setup",action="store_true",default=False,help="Perform one-time profiling setup. This is required on a newly flashed device.")
CW.call_parser(parser)
parser = parser.parse_args()

def run_cmd(cmd):
     if os.system(cmd) != 0 : sys.exit(2)

def enable_trace():
    if options.trace_size.startswith("0x") == False:
        trace_size = "0x"+options.trace_size
    else:
        trace_size = options.trace_size
    run_cmd('sleep 2 && adb -s '+device_number+' wait-for-device remount')
    print("Configuring and enabling tracing on device "+device_number)
    run_cmd('adb -s '+device_number+' shell "echo 1 > /sys/bus/coresight/reset_source_sink"')
    if os.system('adb -s '+device_number+' shell ls /sys/bus/coresight/devices/coresight-tmc-etr/buffer_size') == 0 :
        run_cmd('adb -s '+device_number+' shell "echo '+trace_size+' > /sys/bus/coresight/devices/coresight-tmc-etr/buffer_size"')
        run_cmd('echo -n "Updated Buffer Size: " && adb -s '+device_number+' shell "cat /sys/bus/coresight/devices/coresight-tmc-etr/buffer_size"')
    else :
        run_cmd('adb -s '+device_number+' shell "echo '+trace_size+' > /sys/bus/coresight/devices/coresight-tmc-etr/mem_size"')
        run_cmd('echo -n "Updated Mem Size: " && adb -s '+device_number+' shell "cat /sys/bus/coresight/devices/coresight-tmc-etr/mem_size"')
    run_cmd('adb -s '+device_number+' shell "echo 0 > /sys/bus/coresight/devices/coresight-stm/hwevent_enable"')
    run_cmd('adb -s '+device_number+' shell "echo mem > /sys/bus/coresight/devices/coresight-tmc-etr/out_mode"')
    run_cmd('adb -s '+device_number+' shell "echo 1 > /sys/bus/coresight/devices/coresight-tmc-etr/enable_sink"')
    run_cmd('adb -s '+device_number+' shell "echo 1 > /sys/bus/coresight/devices/coresight-turing-etm0/enable_source"')
    run_cmd('adb -s '+device_number+' shell setprop vendor.fastrpc.process.attrs 17')
    run_cmd('adb -s '+device_number+' shell /data/local/tmp/sysMonApp etmTrace --command etm --q6 CDSP --etmType ca_pc')
    print('Tracing enabled')

def one_time_setup():
    print("Run one-time profiling setup for device "+device_number)
    binary_dir = os.path.join(HEXAGON_SDK_ROOT,"tools","debug","hexagon-trace-analyzer","binaries",device_number)
    run_cmd('adb -s '+device_number+' wait-for-device remount')
    print('*** Push sysmon on device ***')
    run_cmd('adb -s '+device_number+' push '+os.path.join(HEXAGON_SDK_ROOT,"tools","utils","sysmon","sysMonApp")+' /data/local/tmp/sysMonApp')
    run_cmd('adb -s '+device_number+' shell chmod 777 /data/local/tmp/sysMonApp')
    print('***Write to '+binary_dir+' the binaries needed for tracing analysis ***')
    run_cmd('mkdir -p '+binary_dir)
    run_cmd('adb -s '+device_number+' pull /vendor/dsp/cdsp/fastrpc_shell_3 '+binary_dir)
    run_cmd('adb -s '+device_number+' pull /vendor/lib/rfsa/adsp/libdspCV_skel.so '+binary_dir)
    run_cmd('adb -s '+device_number+' pull /vendor/firmware_mnt/image/ '+binary_dir)

if __name__ == "__main__":
    global options
    device_info = CW.get_config()
    options = device_info.get_parameters()[0]
    device_number = options.serial_num
    if options.one_time_setup == True:
        one_time_setup()
    enable_trace()
