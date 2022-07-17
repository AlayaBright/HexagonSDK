#!/usr/bin/env python

### Imports ###
import os
import sys
import argparse

if not os.getenv('SDK_SETUP_ENV'):
        sys.exit("\nSDK Environment not set up -> please run setup_sdk_env script from SDK's root directory.")

HEXAGON_SDK_ROOT=os.getenv('HEXAGON_SDK_ROOT')
script_dir = HEXAGON_SDK_ROOT + '/utils/scripts/'
sys.path.append(script_dir)
import Common_Walkthrough as CW
import Device_configuration as dev_conf
import csv

pid = os.getpid()

unsupported_target_info = []
HEAP_ID = 25

parser = argparse.ArgumentParser(prog='profiling.py', description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument("-p", action="store_true", dest="power_boost", default=False, help="Use power boost to run rpcperf. Default setiing is OFF. Run power boost using the request type HAP_power_set_DCVS_v3 with HAP_DCVS_V2_PERFORMANCE_MODE")
parser.add_argument("-n", action="store", dest="iters", nargs="?", default='1000', help="Iterations to run. Default setting is 1000.")
parser.add_argument("-i", action="store_true", dest="ion_disable", default=False, help="Turn ION memory off. Default setting is ON")
parser.add_argument("-m", action="store_true", dest="check_mem", default=False, help="Verify memory operations for rpc performance tests by passing a fixed pattern to the DSP function which copies it to the output. The caller then confirms the correct pattern is received. Default setting is OFF")
parser.add_argument("-u", action="store_true", dest="uncached", default=False, help="Use uncached buffers on the HLOS. Default setting is OFF")
parser.add_argument("-q", action="store", dest="fastrpc_qos", nargs="?", default='300', help="FastRPC QoS is turned ON by default. 0 turns QoS OFF. Positive numbers set the QoS latency in usecs (300 is default)")
parser.add_argument("-y", action="store", dest="sleep_latency", nargs="?", default='0', help="Set Sleep Latency. 0 is default and means DCVS V3 vote will not be made. Minimum value is 10. Maximum value is 65535.")

CW.call_parser(parser)

tests = [
    [ 'noop',                   0,   '0K'],
    [ 'inbuf',          32 * 1024,  '32K'],
    [ 'routbuf',        32 * 1024,  '32K'],
    [ 'inbuf',          64 * 1024,  '64K'],
    [ 'routbuf',        64 * 1024,  '64K'],
    [ 'inbuf',         128 * 1024, '128K'],
    [ 'routbuf',       128 * 1024, '128K'],
    [ 'inbuf',        1024 * 1024,   '1M'],
    [ 'routbuf',      1024 * 1024,   '1M'],
    [ 'inbuf',    4 * 1024 * 1024,   '4M'],
    [ 'routbuf',  4 * 1024 * 1024,   '4M'],
    [ 'inbuf',    8 * 1024 * 1024,   '8M'],
    [ 'routbuf',  8 * 1024 * 1024,   '8M'],
    [ 'inbuf',   16 * 1024 * 1024,  '16M'],
    [ 'routbuf', 16 * 1024 * 1024,  '16M'],
    [ 'default',                0,    '0']
]

def run_performance_tests(device_number):
    global tests
    full_app_path = 'LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/profiling'
    for test in tests:
        dev_conf.print_and_run_cmd("adb -s "+device_number+" wait-for-device shell export RPCPERF_HEAP_ID={} RPCPERF_HEAP_FLAGS=1 {} -f {} -n {} {} -s {} {} {} {} -q {} -d {} -U {} -y {}".
                            format(HEAP_ID, full_app_path, test[0], options.iters, options.power_boost, test[1], options.uncached,
                                    options.ion_disable, options.check_mem, options.fastrpc_qos, options.domain, options.unsigned, options.sleep_latency))

global options
object_new = CW.get_config()     #creating an object for get_config class in Common_Walkthrough script
#parsing the command line parameters and subsystem Flags
options, hex_variant, variant, arch, dsp_dir, hlos_dir = object_new.get_parameters()
device_number = options.serial_num

if options.target in unsupported_target_info:
    print("Error! "+ options.target+" is not supported.")
    sys.exit()

if not options.domain in ['0','3']:
    print("Error! Domain not supported \nPlease pass 0 for ADSP or 3 for CDSP")
    sys.exit()

if options.power_boost:
    options.power_boost = "-p"
else:
    options.power_boost = ""
if options.ion_disable:
    options.ion_disable = "-i"
else:
    options.ion_disable = ""
if options.check_mem:
    options.check_mem = "-m"
else:
    options.check_mem = ""
if options.uncached:
    options.uncached = "-u"
else:
    options.uncached = ""

profiling_exe='{}/examples/profiling/{}/ship/profiling'.format(HEXAGON_SDK_ROOT,hlos_dir)
libprofiling='{}/examples/profiling/{}/ship/libprofiling.so'.format(HEXAGON_SDK_ROOT,hlos_dir)
libprofiling_skel='{}/examples/profiling/{}/ship/libprofiling_skel.so'.format(HEXAGON_SDK_ROOT,dsp_dir)

if sys.platform == "win32":
    clean_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/profiling '+variant+'_clean HLOS_ARCH='+arch+' VERBOSE=1 || exit /b'
    build_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/profiling '+variant+' HLOS_ARCH='+arch+' VERBOSE=1 || exit /b'
    clean_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/profiling hexagon_clean DSP_ARCH='+hex_variant+' VERBOSE=1 || exit /b'
    build_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/profiling hexagon DSP_ARCH='+hex_variant+' VERBOSE=1 || exit /b'
else:
    clean_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/profiling '+variant+'_clean HLOS_ARCH='+arch+' VERBOSE=1 || exit 1'
    build_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/profiling '+variant+' HLOS_ARCH='+arch+' VERBOSE=1 || exit 1'
    clean_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/profiling hexagon_clean DSP_ARCH='+hex_variant+' VERBOSE=1 || exit 1'
    build_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/profiling hexagon DSP_ARCH='+hex_variant+' VERBOSE=1 || exit 1'

if not options.no_rebuild:
    print("---- Build profiling example for both Android and Hexagon ----")
    dev_conf.print_and_run_cmd(clean_variant)
    dev_conf.print_and_run_cmd(build_variant)
    dev_conf.print_and_run_cmd(clean_hexagon_variant)
    dev_conf.print_and_run_cmd(build_hexagon_variant)
else:
    print("---- Skip rebuilding profiling example for both Android and Hexagon ----")

call_test_sig , APPS_DST, DSP_DST, LIB_DST, DSP_LIB_PATH, DSP_LIB_SEARCH_PATH = CW.get_DST_PARAMS(object_new, HEXAGON_SDK_ROOT)

os.system('adb -s '+device_number+'  logcat -c')
if not options.no_signing :
    os.system(call_test_sig)

print("---- root/remount device ----")
dev_conf.mount_device(device_number, parser)

print("---- Push Android components ----")
dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell mkdir -p '+APPS_DST)
dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+profiling_exe+' '+APPS_DST)
dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell chmod 777 '+APPS_DST+'/profiling')
dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+libprofiling+' '+LIB_DST)

print(" ---- Push Hexagon Components ----")
dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell mkdir -p '+DSP_DST)
dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+libprofiling_skel+' '+DSP_DST)

print("---- Direct dsp messages to logcat ---")
dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell "echo 0x1f > '+DSP_DST+'profiling.farf"')

if sys.platform == "win32":
    print("---- Launch logcat window to see DSP diagnostic messages")
    dev_conf.print_and_run_cmd('start cmd.exe /c adb -s '+device_number+'  logcat -s adsprpc')
    dev_conf.print_and_run_cmd('sleep 2')
elif sys.platform.startswith("linux"):
    print("---- Launch logcat window to see DSP diagnostic messages")

print(("---- Run profiling Example ----"))
sys.stdout.flush()
#Run the timer demonstration only
dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/profiling -f timers -d '+options.domain+' '+ options.power_boost)
#Run Fastrpc perfromance tests
run_performance_tests(device_number)
print("Done")
sys.stdout.flush()