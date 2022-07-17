#!/usr/bin/env python
# child python script

### Imports ###
import os	       # provide output to command prompt
import sys	      # flush command prompt output
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
received = False	# initialize signal received to false

unsupported_target_info = []
#******************************************************************************
# Parser for cmd line options
#******************************************************************************
parser = argparse.ArgumentParser(prog='multithreading_walkthrough.py', description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
CW.call_parser(parser)

# run multithreading
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

	multithreading_exe='{}/examples/multithreading/{}/ship/multithreading'.format(HEXAGON_SDK_ROOT,hlos_dir)
	libmultithreading='{}/examples/multithreading/{}/ship/libmultithreading.so'.format(HEXAGON_SDK_ROOT,hlos_dir)
	libmultithreading_skel='{}/examples/multithreading/{}/ship/libmultithreading_skel.so'.format(HEXAGON_SDK_ROOT,dsp_dir)

	print("---- Build multithreading example for both Android and Hexagon ----")
	if sys.platform == "win32":
		clean_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/multithreading '+variant+'_clean HLOS_ARCH='+arch+' VERBOSE=1 || exit /b'
		build_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/multithreading '+variant+' HLOS_ARCH='+arch+' VERBOSE=1 || exit /b'
		clean_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/multithreading hexagon_clean DSP_ARCH='+hex_variant+' VERBOSE=1 || exit /b'
		build_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/multithreading hexagon DSP_ARCH='+hex_variant+' VERBOSE=1 || exit /b'
	else:
		clean_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/multithreading '+variant+'_clean HLOS_ARCH='+arch+' VERBOSE=1 || exit 1'
		build_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/multithreading '+variant+' HLOS_ARCH='+arch+' VERBOSE=1 || exit 1'
		clean_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/multithreading hexagon_clean DSP_ARCH='+hex_variant+' VERBOSE=1 || exit 1'
		build_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/multithreading hexagon DSP_ARCH='+hex_variant+' VERBOSE=1 || exit 1'

	if not options.no_rebuild:
		print("---- Build multithreading example for both Android and Hexagon ----")
		dev_conf.print_and_run_cmd(clean_variant)
		dev_conf.print_and_run_cmd(build_variant)
		dev_conf.print_and_run_cmd(clean_hexagon_variant)
		dev_conf.print_and_run_cmd(build_hexagon_variant)
	else:
		print("---- Skip rebuilding multithreading example for both Android and Hexagon ----")

	call_test_sig , APPS_DST, DSP_DST, LIB_DST, DSP_LIB_PATH, DSP_LIB_SEARCH_PATH = CW.get_DST_PARAMS(object_new, HEXAGON_SDK_ROOT)

	if not options.no_signing :
		os.system(call_test_sig)

	print("---- root/remount device ----")
	dev_conf.mount_device(device_number, parser)

	print("---- Push Android components ----")
	dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device shell mkdir -p '+APPS_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device push '+multithreading_exe+' '+APPS_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device shell chmod 777 '+APPS_DST+'/multithreading')
	dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device push '+libmultithreading+' '+LIB_DST)

	print(" ---- Push Hexagon Components ----")
	dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device shell mkdir -p '+DSP_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device push '+libmultithreading_skel+' '+ DSP_DST)

	print("---- Direct dsp messages to logcat ---")
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell "echo 0x1f > '+DSP_DST+'multithreading.farf"')


	dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device')

	if sys.platform == "win32":
		print("---- Launch logcat window to see aDSP diagnostic messages")
		dev_conf.print_and_run_cmd('start cmd.exe /c adb -s '+device_number+' logcat -s adsprpc')
		dev_conf.print_and_run_cmd('sleep 2')

	print("---- Run multithreading Example on Domain "+options.domain+" ----")
	sys.stdout.flush()
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/multithreading -d '+options.domain+' -U '+options.unsigned)

	print("Done")

if __name__ == '__main__':
	run_example()												# call function to initialize debug_agent
	sys.stdout.flush()										# show output immediately in command prompt
