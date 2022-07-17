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

unsupported_target_info = ["sm8150","sm6150","qcs405","sm7150","sm6125","agatti","monaco"]

#******************************************************************************
# Parser for cmd line options
#******************************************************************************
parser = argparse.ArgumentParser(prog='LPI_walkthrough.py', description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
CW.call_parser(parser)

if sys.platform == "win32":
    EXIT_COMMAND = 'exit /b'
else:
    EXIT_COMMAND = 'exit 1'
# run lpi_example
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

	hlos_dir_run_main = hlos_dir.replace("_ReleaseG","")

	dsp_dir_run_main  = dsp_dir.replace("_ReleaseG","")


	run_main_on_hexagon_exe='{}/libs/run_main_on_hexagon/ship/{}/run_main_on_hexagon'.format(HEXAGON_SDK_ROOT,hlos_dir_run_main)
	librun_main_on_hexagon_skel='{}/libs/run_main_on_hexagon/ship/{}/librun_main_on_hexagon_skel.so'.format(HEXAGON_SDK_ROOT,dsp_dir_run_main)
	lpi_example_SO = '{}/examples/lpi_example/{}/ship/libLPI.so'.format(HEXAGON_SDK_ROOT,dsp_dir)

	clean_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/lpi_example hexagon_clean DSP_ARCH='+hex_variant+' LPI=1 VERBOSE=1  || ' + EXIT_COMMAND
	build_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/lpi_example hexagon DSP_ARCH='+hex_variant+' LPI=1 VERBOSE=1  || ' + EXIT_COMMAND

	if not options.no_rebuild:
		print("---- Build lpi_example for both Android and Hexagon ----")
		dev_conf.print_and_run_cmd(clean_hexagon_variant)
		dev_conf.print_and_run_cmd(build_hexagon_variant)
	else:
		print("---- Skip rebuilding lpi_example for both Android and Hexagon ----")

	call_test_sig , APPS_DST, DSP_DST, LIB_DST, DSP_LIB_PATH, DSP_LIB_SEARCH_PATH = CW.get_DST_PARAMS(object_new, HEXAGON_SDK_ROOT)

	os.system('adb -s '+device_number+'  logcat -c')
	if not options.no_signing :
		os.system(call_test_sig)

	print("---- root/remount device ----")
	dev_conf.mount_device(device_number, parser)
	print("---- Push Required components ----")
	dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device shell mkdir -p '+APPS_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device push ' + run_main_on_hexagon_exe + ' ' + APPS_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device shell "chmod 774 ' + APPS_DST + '/run_main_on_hexagon"')
	dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device shell mkdir -p '+DSP_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device push ' +librun_main_on_hexagon_skel+' ' + DSP_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+' wait-for-device push ' +lpi_example_SO+' ' + DSP_DST)

	print("---- Direct dsp messages to logcat ---")
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell "echo 0x1f > '+DSP_DST+'run_main_on_hexagon.farf"')


	if sys.platform == "win32":
		print("---- Launch logcat window to see aDSP diagnostic messages")
		dev_conf.print_and_run_cmd('start cmd.exe /c adb -s '+device_number+'  logcat -s adsprpc')
		dev_conf.print_and_run_cmd('sleep 2')

	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/run_main_on_hexagon 0 libLPI.so')

	print("Done")

if __name__ == '__main__':
	run_example()												# call function to initialize debug_agent
	sys.stdout.flush()										# show output immediately in command prompt

