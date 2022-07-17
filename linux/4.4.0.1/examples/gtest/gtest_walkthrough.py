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

unsupported_target_info = ["sm7150", "sm6125", "sm6150", "nicobar", "qcs405", "qrb5165", "qcs403", "qcs610","sxr2130","agatti","monaco"]

#******************************************************************************
# Parser for cmd line options
#******************************************************************************
parser = argparse.ArgumentParser(prog='gtest_walkthrough.py', description=__doc__, formatter_class=argparse.RawTextHelpFormatter)
CW.call_parser(parser)

# run gtest
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
	gtest_exe='{}/examples/gtest/{}/gtest'.format(HEXAGON_SDK_ROOT,hlos_dir)
	libgtest='{}/examples/gtest/{}/libgtest.so'.format(HEXAGON_SDK_ROOT,hlos_dir)
	libgtest_skel='{}/examples/gtest/{}/libgtest_skel.so'.format(HEXAGON_SDK_ROOT,dsp_dir)

	if sys.platform == "win32":
		LIBCPP_SHARED_PATH=HEXAGON_SDK_ROOT+'/tools/android-ndk-r19c/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/lib/aarch64-linux-android/'
		clean_variant = 'cmake_build.cmd '+variant+'_clean HLOS_ARCH='+arch+' VERBOSE=1 || exit /b'
		build_variant = 'cmake_build.cmd '+variant+' HLOS_ARCH='+arch+' VERBOSE=1 || exit /b'
		clean_hexagon_variant = 'cmake_build.cmd hexagon_clean DSP_ARCH='+hex_variant+' VERBOSE=1 || exit /b'
		build_hexagon_variant = 'cmake_build.cmd hexagon DSP_ARCH='+hex_variant+' VERBOSE=1 || exit /b'
	else:
		LIBCPP_SHARED_PATH=HEXAGON_SDK_ROOT+'/tools/android-ndk-r19c/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android/'
		clean_variant = '/bin/bash ./cmake_build.bash '+variant+'_clean HLOS_ARCH='+str(arch)+' VERBOSE=1 || exit 1'
		build_variant = '/bin/bash ./cmake_build.bash '+variant+' HLOS_ARCH='+str(arch)+' VERBOSE=1 || exit 1'
		clean_hexagon_variant = '/bin/bash ./cmake_build.bash hexagon_clean DSP_ARCH='+hex_variant+' VERBOSE=1 || exit 1'
		build_hexagon_variant = '/bin/bash ./cmake_build.bash hexagon DSP_ARCH='+hex_variant+' VERBOSE=1 || exit 1'

	if not options.no_rebuild:
		old_pwd = os.getcwd()
		os.chdir(HEXAGON_SDK_ROOT + '/examples/gtest')
		print("---- Build gtest example for both Android and Hexagon ----")
		dev_conf.print_and_run_cmd(clean_variant)
		dev_conf.print_and_run_cmd(build_variant)
		dev_conf.print_and_run_cmd(clean_hexagon_variant)
		dev_conf.print_and_run_cmd(build_hexagon_variant)
		os.chdir(old_pwd)
	else:
		print("---- Skip rebuilding gtest example for both Android and Hexagon ----")

	call_test_sig , APPS_DST, DSP_DST, LIB_DST, DSP_LIB_PATH, DSP_LIB_SEARCH_PATH = CW.get_DST_PARAMS(object_new, HEXAGON_SDK_ROOT)

	os.system('adb -s '+device_number+'  logcat -c')
	if not options.no_signing :
		os.system(call_test_sig)

	print("---- root/remount device ----")
	dev_conf.mount_device(device_number, parser)

	print("---- Push Android components ----")
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell mkdir -p '+APPS_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+gtest_exe+' '+APPS_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell chmod 777 '+APPS_DST+'/gtest')
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+libgtest+' '+LIB_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+LIBCPP_SHARED_PATH+'/libc++_shared.so'+' '+LIB_DST)

	print(" ---- Push Hexagon Components ----")
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell mkdir -p '+DSP_DST)
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device push '+libgtest_skel+' '+DSP_DST)

	print("---- Direct dsp messages to logcat ---")
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell "echo 0x1f > '+DSP_DST+'gtest.farf"')

	# dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device reboot')
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device')

	if sys.platform == "win32":
		print("---- Launch logcat window to see aDSP diagnostic messages")
		dev_conf.print_and_run_cmd('start cmd.exe /c adb -s '+device_number+'  logcat -s adsprpc')
		dev_conf.print_and_run_cmd('sleep 2')

	print("---- Run gtest Example on Android and DSP ----")
	dev_conf.print_and_run_cmd('adb -s '+device_number+'  wait-for-device shell export LD_LIBRARY_PATH='+LIB_DST+':$LD_LIBRARY_PATH '+DSP_LIB_SEARCH_PATH+'='+DSP_LIB_PATH+' '+APPS_DST+'/gtest')
	print("Done")

if __name__ == '__main__':
	run_example()                                                                                           # call function to initialize debug_agent
	sys.stdout.flush()                                                                              # show output immediately in command prompt
