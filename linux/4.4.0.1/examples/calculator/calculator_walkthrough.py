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

unsupported_target_info = []

#******************************************************************************
# Parser for cmd line options
#******************************************************************************
parser = argparse.ArgumentParser(prog='calculator_walkthrough.py', description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
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
	calculator_exe='{}/examples/calculator/{}/ship/calculator'.format(HEXAGON_SDK_ROOT,hlos_dir)
	libcalculator='{}/examples/calculator/{}/ship/libcalculator.so'.format(HEXAGON_SDK_ROOT,hlos_dir)
	libcalculator_skel='{}/examples/calculator/{}/ship/libcalculator_skel.so'.format(HEXAGON_SDK_ROOT,dsp_dir)

	if sys.platform == "win32":
		clean_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/calculator '+variant+'_clean HLOS_ARCH='+arch+' VERBOSE=1 || exit /b'
		build_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/calculator '+variant+' HLOS_ARCH='+arch+' VERBOSE=1 || exit /b'
		clean_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/calculator hexagon_clean DSP_ARCH='+hex_variant+' VERBOSE=1 || exit /b'
		build_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/calculator hexagon DSP_ARCH='+hex_variant+' VERBOSE=1 || exit /b'
	else:
		clean_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/calculator '+variant+'_clean HLOS_ARCH='+str(arch)+' VERBOSE=1 || exit 1'
		build_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/calculator '+variant+' HLOS_ARCH='+str(arch)+' VERBOSE=1 || exit 1'
		clean_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/calculator hexagon_clean DSP_ARCH='+hex_variant+' VERBOSE=1 || exit 1'
		build_hexagon_variant = 'make -C ' + HEXAGON_SDK_ROOT + '/examples/calculator hexagon DSP_ARCH='+hex_variant+' VERBOSE=1 || exit 1'

	if not options.no_rebuild:
		print("---- Build calculator example for both Android and Hexagon ----")
		dev_conf.print_and_run_cmd(clean_variant)
		dev_conf.print_and_run_cmd(build_variant)
		dev_conf.print_and_run_cmd(clean_hexagon_variant)
		dev_conf.print_and_run_cmd(build_hexagon_variant)
	else:
		print("---- Skip rebuilding calculator example for both Android and Hexagon ----")

	call_test_sig , APPS_DST, DSP_DST, LIB_DST, DSP_LIB_PATH, DSP_LIB_SEARCH_PATH = CW.get_DST_PARAMS(object_new, HEXAGON_SDK_ROOT)

	os.system('adb -s '+device_number+'  logcat -c')
	if not options.no_signing :
		os.system(call_test_sig)

	print("---- root/remount device ----")
	dev_conf.mount_device(device_number, parser)

	print("---- Push Android components ----")
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell mkdir -p '+APPS_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+calculator_exe+' '+APPS_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell chmod 777 '+APPS_DST+'/calculator')
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+libcalculator+' '+LIB_DST)

	print(" ---- Push Hexagon Components ----")
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell mkdir -p '+DSP_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+libcalculator_skel+' '+DSP_DST)

	print("---- Direct dsp messages to logcat ---")
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell "echo 0x1f > '+DSP_DST+'calculator.farf"')

	# dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device reboot')
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device')

	if sys.platform == "win32":
		print("---- Launch logcat window to see aDSP diagnostic messages")
		dev_conf.print_and_run_cmd('start cmd.exe /c adb -s '+device_number+'  logcat -s adsprpc')
		dev_conf.print_and_run_cmd('sleep 2')

	print("---- Run Calculator Example Locally on Android ----")
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH=\"'+LIB_DST+';\" '+APPS_DST+'/calculator -r 1 -d 0 -n 1000')
	if options.domain == '0':
		print("---- Run Calculator Example on aDSP ----")
		dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/calculator -r 0 -d 0 -n 1000')
	elif options.target != "agatti" and options.domain == '3':
		if options.target not in ['qcs403','qcs405']:
			print("---- Run Calculator Example on cDSP Unsigned PD ----")
			dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/calculator -r 0 -d 3 -n 1000')
		print("---- Run Calculator Example on cDSP Signed PD----")
		dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/calculator -r 0 -d 3 -n 1000 -U 0')
	#print("---- Run Calculator Example on mDSP ----")
	#dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/calculator 0 1 1000')
	elif options.domain == '2':
		if options.target == "sdm845":
			print("---- Calculator Example is not supported on SLPI because SLPI does not have enough memory to support fastrpc user mode invocation")
			print("---- So walk-through script does not run this example on SLPI")
		elif options.target == "sm8150" or options.target == "sm8250" or options.target == "lahaina":
			print("---- Run Calculator Example on sDSP ----")
			dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/calculator -r 0 -d 2 -n 1000')
		else:
			print("Error! SLPI is not supported on Target!")
	else:
		print("---- Run Calculator Example on mDSP ----")
		dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/calculator -r 0 -d 1 -n 1000')

	print("Done")

if __name__ == '__main__':
	run_example()												# call function to initialize debug_agent
	sys.stdout.flush()										# show output immediately in command prompt

