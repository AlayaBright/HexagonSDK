#!/usr/bin/env python
# child python script

### Imports ###
import os               # provide output to command prompt
import sys              # flush command prompt output
import argparse

# Check if SDK_SETUP_ENV is set or not for checking whether environment is set or not.
# Exit walkthrough if SDK_SETUP_ENV is not set.
if not os.getenv('SDK_SETUP_ENV'):
                sys.exit("\nSDK Environment not set up -> please run setup_sdk_env script from SDK's root directory.")
HEXAGON_SDK_ROOT=os.getenv('HEXAGON_SDK_ROOT')
script_dir = HEXAGON_SDK_ROOT + '/utils/scripts/'
sys.path.append(script_dir)
import Common_Walkthrough as CW
import Device_configuration as dev_conf

pid = os.getpid()       # return the current process ID
received = False        # initialize signal received to false

unsupported_target_info = []
#******************************************************************************
# Parser for cmd line options
#******************************************************************************
parser = argparse.ArgumentParser(prog='hap_example_walkthrough.py', description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
CW.call_parser(parser)

# run hap_example
def run_example():
    global options
    object_new = CW.get_config() #creating an object for get_config class in Common_Walkthrough script
    #parsing the command line parameters and subsystem Flags
    options, hex_variant, variant, arch, dsp_dir, hlos_dir = object_new.get_parameters()
    device_number = options.serial_num

    if options.target in unsupported_target_info:
        print("Error! "+ options.target+" is not supported.")
        sys.exit()

    print("hex_variant = "+hex_variant)

    hap_example_exe='{}/examples/hap_example/{}/ship/hap_example'.format(HEXAGON_SDK_ROOT,hlos_dir)
    libhap_example='{}/examples/hap_example/{}/ship/libhap_example.so'.format(HEXAGON_SDK_ROOT,hlos_dir)
    libhap_example_skel='{}/examples/hap_example/{}/ship/libhap_example_skel.so'.format(HEXAGON_SDK_ROOT,dsp_dir)

    if sys.platform == "win32":
            clean_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/hap_example '+variant+'_clean HLOS_ARCH='+arch+' VERBOSE=1 || exit /b'
            build_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/hap_example '+variant+' HLOS_ARCH='+arch+' VERBOSE=1 || exit /b'
            clean_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/hap_example hexagon_clean DSP_ARCH='+hex_variant+' VERBOSE=1 || exit /b'
            build_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/hap_example hexagon DSP_ARCH='+hex_variant+' VERBOSE=1 || exit /b'
    else:
            clean_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/hap_example '+variant+'_clean HLOS_ARCH='+arch+' VERBOSE=1 || exit 1'
            build_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/hap_example '+variant+' HLOS_ARCH='+arch+' VERBOSE=1 || exit 1'
            clean_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/hap_example hexagon_clean DSP_ARCH='+hex_variant+' VERBOSE=1 || exit 1'
            build_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/hap_example hexagon DSP_ARCH='+hex_variant+' VERBOSE=1 || exit 1'

    if not options.no_rebuild:
        print("---- Build hap_example for both Android and Hexagon ----")
        dev_conf.print_and_run_cmd(clean_variant)
        dev_conf.print_and_run_cmd(build_variant)
        dev_conf.print_and_run_cmd(clean_hexagon_variant)
        dev_conf.print_and_run_cmd(build_hexagon_variant)
    else:
        print("---- Skip rebuilding hap_example for both Android and Hexagon ----")

    call_test_sig , APPS_DST, DSP_DST, LIB_DST, DSP_LIB_PATH, DSP_LIB_SEARCH_PATH = CW.get_DST_PARAMS(object_new, HEXAGON_SDK_ROOT)

    if not options.no_signing :
            os.system(call_test_sig)

    print("---- root/remount device ----")
    dev_conf.mount_device(device_number, parser)

    print("---- Push Android components ----")
    dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell mkdir -p '+APPS_DST)
    dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+hap_example_exe+' '+APPS_DST)
    dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell chmod 777 '+APPS_DST+'/hap_example')
    dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+libhap_example+' '+LIB_DST)

    print(" ---- Push Hexagon Components ----")
    dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell mkdir -p '+DSP_DST)
    dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+libhap_example_skel+' '+ DSP_DST)

    print("---- Direct dsp messages to logcat ---")
    dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell "echo 0x1f > '+DSP_DST+'hap_example.farf"')

    dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device')

    if sys.platform == "win32":
            print("---- Launch logcat window to see aDSP diagnostic messages")
            dev_conf.print_and_run_cmd('start cmd.exe /c adb -s '+device_number+'  logcat -s adsprpc')
            dev_conf.print_and_run_cmd('sleep 2')

    print("---- Run hap_example Example ----")

    if options.target in ["agatti", "monaco"]:
	    domain = "0"
    else:
	    domain = options.domain

    # HAP APIs supported on both CDSP and ADSP
    dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/hap_example -f 1 -U '+options.unsigned)
    dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/hap_example -f 3 -U '+options.unsigned)
    if not options.target in ["qcs403","qcs405"]:
            dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/hap_example -f 2 -U '+options.unsigned)
            dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/hap_example -f 4 -U '+options.unsigned)

    # HAP APIs supported only on CDSP
    if domain != "0":
	    if not options.target in ["qcs403","qcs405"]:
		    dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/hap_example -f 5 -U 0')
	    dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/hap_example -f 6 -U '+options.unsigned)
	    dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/hap_example -f 0 -U '+options.unsigned)


    print("Done")

if __name__ == '__main__':
    run_example()												# call function to initialize debug_agent
    sys.stdout.flush()										# show output immediately in command prompt
