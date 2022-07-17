#!/usr/bin/env python
# child python script

### Imports ###
import os		# provide output to command prompt
import sys		# flush command prompt output
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

pid = os.getpid()	# return the current process ID
received = False	# initialize signal received to false

unsupported_target_info = ["sm8150","sm6150","qcs405","sm7150","sm6125","sm8250","rennell","saipan","kamorta","bitra","agatti", "qcs403", "qcs610","qrb5165","mannar","sxr2130","strait","monaco","divar","qcs605"]

#******************************************************************************
# Parser for cmd line options
#******************************************************************************
parser = argparse.ArgumentParser(prog='dspqueue_walkthrough.py', description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
CW.call_parser(parser)

# run calculator
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

	HEXAGON_SDK_ROOT=os.getenv('HEXAGON_SDK_ROOT')
	dspqueue_sample_exe='{}/examples/dspqueue/{}/ship/dspqueue_sample'.format(HEXAGON_SDK_ROOT,hlos_dir)
	libdspqueue_sample_skel='{}/examples/dspqueue/{}/ship/libdspqueue_sample_skel.so'.format(HEXAGON_SDK_ROOT,dsp_dir)

	if sys.platform == "win32":
		clean_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/dspqueue '+variant+'_clean HLOS_ARCH='+arch+' VERBOSE=1 || exit /b'
		build_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/dspqueue '+variant+' HLOS_ARCH='+arch+' VERBOSE=1 || exit /b'
		clean_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/dspqueue hexagon_clean DSP_ARCH='+hex_variant+' VERBOSE=1 || exit /b'
		build_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/dspqueue hexagon DSP_ARCH='+hex_variant+' VERBOSE=1 || exit /b'
	else:
		clean_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/dspqueue '+variant+'_clean HLOS_ARCH='+str(arch)+' VERBOSE=1 || exit 1'
		build_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/dspqueue '+variant+' HLOS_ARCH='+str(arch)+' VERBOSE=1 || exit 1'
		clean_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/dspqueue hexagon_clean DSP_ARCH='+hex_variant+' VERBOSE=1 || exit 1'
		build_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/dspqueue hexagon DSP_ARCH='+hex_variant+' VERBOSE=1 || exit 1'

	if not options.no_rebuild:
		print("---- Build dspqueue example for both Android and Hexagon ----")
		dev_conf.print_and_run_cmd(clean_variant)
		dev_conf.print_and_run_cmd(build_variant)
		dev_conf.print_and_run_cmd(clean_hexagon_variant)
		dev_conf.print_and_run_cmd(build_hexagon_variant)
	else:
		print("---- Skip rebuilding dspqueue example for both Android and Hexagon ----")

	call_test_sig , APPS_DST, DSP_DST, LIB_DST, DSP_LIB_PATH, DSP_LIB_SEARCH_PATH = CW.get_DST_PARAMS(object_new, HEXAGON_SDK_ROOT)

	os.system('adb -s '+device_number+'  logcat -c')

	print("---- root/remount device ----")
	dev_conf.mount_device(device_number, parser)

	print("---- Push Android components ----")
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell mkdir -p '+APPS_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+dspqueue_sample_exe+' '+APPS_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell chmod 777 '+APPS_DST+'/dspqueue_sample')

	print(" ---- Push Hexagon Components ----")
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell mkdir -p '+DSP_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+libdspqueue_sample_skel+' '+DSP_DST)

	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device')

	print("---- Run Asynchronous Packet Queue Example on cDSP ----")
	sys.stdout.flush()
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/dspqueue_sample -U '+options.unsigned)
	print("Done")

if __name__ == '__main__':
	run_example()												# call function to initialize debug_agent
	sys.stdout.flush()										# show output immediately in command prompt
