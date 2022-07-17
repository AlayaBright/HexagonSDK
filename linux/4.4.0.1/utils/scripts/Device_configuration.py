import os		# provide output to command prompt
import signal	# allow communication between child and parent processes
import time		# delay functions
import sys		# flush command prompt output
from sys import platform as _platform
import subprocess
from subprocess import check_output,CalledProcessError,Popen, PIPE, STDOUT
import platform
import re
import inspect
if sys.version_info.major == 2:
	from commands import getoutput
else:
	from subprocess import getoutput

# List of targets supporting only ADSP
DEVICES_WITH_ONLY_ADSP = ["agatti","monaco"]

if not os.getenv('SDK_SETUP_ENV'):
		sys.exit("\nSDK Environment not set up -> please run setup_sdk_env script from SDK's root directory.")

def fetchOutput(cmd):
	if platform.system()=="Windows":
		if sys.version_info[0] < 3:
			return check_output(cmd)
		else:
			return check_output(cmd).decode()
	if platform.system()=="Linux":
		return getoutput(cmd)

# signal handler
def signal_usr1(signum, frame):	# signum is signal used to call handler 'signal_usr1', frame is current stack frame
	print("Exiting...")			# print appropriate message
	sys.stdout.flush()			# flush output to command prompt
	sys.exit(0)					# exit child process

#print command and execute with the error check
def print_and_run_cmd(cmd):
	print(cmd)
	if os.system(cmd) != 0 : sys.exit(2) # in error stop execution and exit

def adb_version_check():
	try:
		adb_version_output = fetchOutput("adb version")
	except CalledProcessError as e:
		print("Error occured while calling adb version!")
		print(e.returncode)
		sys.exit()
	if platform.system()=="Windows":
		version_lines=adb_version_output.split('\r\n')[0]
	elif platform.system()=="Linux":
		version_lines=adb_version_output.split('\n')[0]
	#print(version_lines)
	ver_num=version_lines.split(' ')[4].split('.')[2]
	adb_version = int(ver_num)
	if(adb_version < 39):
		print("adb version below 1.0.39 not supported, please upgrade to higher version")
		sys.exit()
	return adb_version

# This function return the output from "adb devices" command
def adb_devices_output():
	try:
		adb_ouput_l = fetchOutput("adb devices -l")
	except CalledProcessError as e:
		print("Error occured while calling adb devices -l!")
		print(e.returncode)
		sys.exit()
	return adb_ouput_l

def mount_device(device_number, Lparser):
      LocalParser = Lparser.parse_args()
      cmd1='adb -s '+device_number+' wait-for-device root'
      print_and_run_cmd(cmd1)
      cmd2='adb -s '+device_number+' wait-for-device remount'
      print_and_run_cmd(cmd2)
      if LocalParser.linux_env:
        cmd3='adb -s '+device_number+' wait-for-device shell mount -o remount,rw,exec /'
        print_and_run_cmd(cmd3)

def get_dev_no(i):
	dev_no=i.split('               ')[0]
	dev_no=str(dev_no)
	return dev_no

def get_target_name_using_dev_num(dev_no):
	# List of Paths containing ver_info.txt
	VERINFO_PATH_LIST = ["/firmware/verinfo/Ver_Info.txt", "/vendor/firmware/verinfo/ver_info.txt", "/vendor/firmware_mnt/verinfo/ver_info.txt", "/vendor/firmware_mnt/verinfo/Ver_info.txt", "/firmware/verinfo/ver_info.txt"]

	cmd1 = "adb -s " +dev_no+" wait-for-device root"
	print_and_run_cmd(cmd1)
	for path in VERINFO_PATH_LIST:
		try:
			cmd = check_output(["adb","-s",dev_no,"wait-for-device","shell","ls",path])
			break
		except:
			continue
	#print(path)
	verinfo = fetchOutput("adb -s "+dev_no+" wait-for-device shell cat "+path)
	lines=verinfo.split('\n')
	target = 'none'
	for j in lines:
		if "Meta_Build_ID" in j:
			target=j.split(":")[1].strip()
			target=target.split('"')[1].split(".")[0].lower()
			if "sm8250" in target:
				target = "sm8250"
			if "sm8150" in target:
				target = "sm8150"
			if "nicobar" in target:
				target = "sm6125"
	return target

class device_config:
    adb_output=''
    device_dict={}
    def __init__(self, enableTarget=True):
        if enableTarget:
            adb_version_check()
            self.adb_output = adb_devices_output()
            devices = self.adb_output.split('\n')
            for i in devices:
                if '               ' in i:
                    i=str(i)
                    dev_no=get_dev_no(i)
                    target = get_target_name_using_dev_num(dev_no)
                    if target in list(self.device_dict.keys()):
                        self.device_dict[target.lower()].append(dev_no)
                    else:
                        self.device_dict[target.lower()]=[dev_no]

# Function is accepting 2 arguments, device_object as 'object of device_config class' and serial number of device.
def get_target_name_using_serial_num(device_object, serial_no):
	device_list = []
	devices = device_object.adb_output.split('\n')
	for i in devices:
		if 'List of devices' not in i:
			i=str(i)
			device_list.append(get_dev_no(i))

	if serial_no not in device_list :
		print("Error! Device with serial number ",serial_no," not connected...")
		sys.exit()
	else :
		return get_target_name_using_dev_num(serial_no)

# Function is accepting 2 arguments, device_object as 'object of device_config class' and target_name of device.
# Return Value:: device number for given target name
def dev_dict(device_object, target_name):
	devices_dict = device_object.device_dict
	if target_name == "qcs403" and "qcs405" in list(devices_dict.keys()) and "qcs403" not in list(devices_dict.keys()):
		target_name = "qcs405"
		print("QCS403 is not supported , using QCS405 instead of QCS403 as meta build info says QCS405")
	if target_name not in list(devices_dict.keys()):
		print("Device with target name ",target_name," not connected")
		device_number=""
		return device_number
	device_list=devices_dict[target_name]
	if len(device_list)>1:
		print("More than one device is connected with same target name! Provide the adb serial number of the device")
		print("target name: ",target_name)
		print("devices connected: ",device_list)
		sys.exit()
	device_number=device_list[0]
	print("returned device_number is" + device_number)
	return device_number
