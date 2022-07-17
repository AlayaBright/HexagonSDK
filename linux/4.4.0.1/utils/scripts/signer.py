#!/usr/bin/env python3
# child python script

### Imports ###
import os
import signal
import time
import sys
import subprocess
import argparse
import platform
import Device_configuration as dev_conf

#******************************************************************************
# Parser for cmd line options
#******************************************************************************
parser = argparse.ArgumentParser(prog='signer.py', description="Retrieve serial number from device, generate a test signature, or sign a shared objects.", formatter_class=argparse.RawTextHelpFormatter, usage='signer.py <sign/getserial> [-T <target>] [-s <serial_num] [-d <domain>] [-LE] [-dest <hlos_destination>] [-t <serial_number>] [-i <input_file>] [-o <output_dir>] [-l] [-64]  \
                                 \nIf saving testsig locally, do provide -t option along with -l option.')
parser.add_argument("action", choices=['sign', 'getserial'], nargs='?', default='sign', help="sign: Generate a test signature (TestSig) or sign a library\ngetserial: Retrieve device serial number")
parser.add_argument("-T", dest="target", help="Specify target name  <qcs605, sm8150, sm6150, qcs405, sm7150, sm6125, sm8250, lahaina, rennell, saipan, kamorta, bitra, agatti, qrb5165, qcs403, qcs610, cedros, kodiak, mannar, sxr2130, monaco>")
parser.add_argument("-s", dest="serial_num", help="Specify the adb serial number for the device.")
parser.add_argument("-d", dest="domain", action="store", default='3', help="Domain is <0/3>, 0: ADSP, 3: CDSP. Default is 0 for agatti and 3 for other targets.")
parser.add_argument("-LE", dest="linux_env", action="store_true", help="Use if device HLOS is Linux Embedded. Do not use with Linux Android.")
parser.add_argument('-dest', dest="hlos_root_dest", help="Use this to override the default HLOS directory path <vendor, usr, data>")
parser.add_argument("-t", dest="serial_number", help="Use provided serial number instead of retrieving it from device")
parser.add_argument("-i", dest="input_file", help="Path to library that needs to be signed. Generate a test signature if no path specified.")
parser.add_argument("-o", dest="output_dir", help="Directory where to store signed object.")
parser.add_argument("-l", "--local", dest="local", action="store_true", help="Save generated testsig locally.")
parser.add_argument("-64", dest="aarch64", action="store_true", help="Use 64-bit getserial binary to retrieve serial number, only for Linux Embedded.")
parser.add_argument("--no_disclaimer", dest="no_disclaimer", action="store_true", help=argparse.SUPPRESS)

# Check Python version
# Generate a warning message for Python2 or Python 3 below 3.7
if sys.version_info.major < 3 or sys.version_info.minor < 7:
    print("Python 3.7 is recomended. Please upgrade to 3.7 as soon as possible.")


HEXAGON_SDK_ROOT = os.getenv('HEXAGON_SDK_ROOT')
if not HEXAGON_SDK_ROOT:
    sys.exit("\nSDK Environment not set up -> Please run setup_sdk_env script from SDK's root directory.")

# Initializing Global variables
def set_global_vars():
    global LE_DEVICE,HLOS_DST,adb_ouput,dev_num
    dev_num = ""
    adb_ouput = ""
    LE_DEVICE = 0
    HLOS_DST = '/vendor/'

    # signer.py -LE
    if options.linux_env:
        LE_DEVICE = 1
        HLOS_DST = '/usr/'

    if options.hlos_root_dest:
        HLOS_DST = '/'+options.hlos_root_dest+'/'


# signal handler
# signum is signal used to call handler 'signal_usr1', frame is current stack frame
def signal_usr1(signum, frame):
    print("Exiting...")
    sys.stdout.flush()
    sys.exit(0)

# print and run input command
def print_and_run_cmd(cmd):
    print(cmd)
    return os.system(cmd)

# disable verity check and remount device
def root_remount_device(dev_num):
    print_and_run_cmd('adb -s '+dev_num+' wait-for-device root')
    print ("checking disable-verity...")
    verity_output = subprocess.check_output("adb -s "+dev_num+" wait-for-device disable-verity", shell=True).decode('utf-8')
    if "already" not in verity_output and "disable-verity" not in verity_output and verity_output != "":
        print ("\n---- Successfully disabled verity ----\nDevice rebooting...")
        print_and_run_cmd("adb -s "+dev_num+" wait-for-device reboot")
        print_and_run_cmd("adb -s "+dev_num+" wait-for-device root")
    else :
        print ("verity is already disabled")
    if LE_DEVICE:
        print_and_run_cmd('adb -s '+dev_num+' wait-for-device remount')
        print_and_run_cmd('adb -s '+dev_num+' wait-for-device shell mount -o,remount rw /')
    else:
        print_and_run_cmd('adb -s '+dev_num+' wait-for-device remount')

# get device number
def get_dev_num():
    global options
    global dev_num
    if options.serial_num:
        dev_num = options.serial_num
    else:
        device_object = dev_conf.device_config()    #creating an object for device_config class in Device_configuration script
        if options.target:
            dev_num = dev_conf.dev_dict(device_object, options.target)
        else:
            count = 0
            adb_ouput = device_object.adb_output
            devices = adb_ouput.split('\n')
            for device in devices:
                if '               ' in device:
                    count += 1
                    if count > 1:
                        print("More than one target connected, please specify the target name using -T or serial number of target using -s!")
                        sys.exit()
                    device = str(device)
                    dev_num = dev_conf.get_dev_no(device)
    return dev_num

# get serial number
def get_serial_num(dev_num):
    print("\n---- Root/Remount device ----")
    root_remount_device(dev_num)

    # if -LE is passed as a argument - testsig is used for Ubuntu target otherwise Android targets
    dest = HLOS_DST + "bin/"
    build_variant = "android_Release"
    if LE_DEVICE:
        if options.aarch64:
            build_variant = "UbuntuARM_Release_aarch64"
        else:
            build_variant = "UbuntuARM_Release"

    # get target chipset name using the device serial number
    target_name = dev_conf.get_target_name_using_dev_num(dev_num)

    # check domain passed by the user
    if target_name in dev_conf.DEVICES_WITH_ONLY_ADSP or options.domain == "0":
        dsp = "ADSP"
    else:
        dsp = "CDSP" # Using CDSP as default domain

    print("\n---- Retrieve serial number from {} ----".format(dsp))
    print_and_run_cmd('adb -s '+dev_num+' wait-for-device shell mkdir -p {}'.format(dest))
    print_and_run_cmd('adb -s '+dev_num+' wait-for-device push {}/tools/elfsigner/getserial/{}/{}/getserial {}'.format(HEXAGON_SDK_ROOT,dsp,build_variant,dest))
    print_and_run_cmd('adb -s '+dev_num+' wait-for-device shell chmod 777 {}/getserial'.format(dest))

    getSerialNum = 'adb -s '+dev_num+' wait-for-device shell {}/getserial | grep -o \"0.*\" | tr -d "\\n\\r"'.format(dest)
    serial_number = subprocess.check_output(getSerialNum, shell=True)
    if isinstance(serial_number, bytes):
        serial_number = serial_number.decode("utf-8")
    if serial_number >= "0":
        return serial_number
    return "-1"

# generate testsig and save to output_path. Push to device
def generate_testsig(serial_number,output_path,no_disclaimer):
    if serial_number == "0":
        print("\n--- Warning: serial_number = 0 ---")
    elif serial_number == "-1":
        sys.exit("\n--- Failed to retrieve serial number, please check DSP logs for more information. ---")


    print("\n--- Generate testsig ---")
    if no_disclaimer:
        generate_testsig_cmd = ' python {}/tools/elfsigner/elfsigner.py -t {} -o {}/testsigs --no_disclaimer'.format(HEXAGON_SDK_ROOT,serial_number,output_path)
    else:
        generate_testsig_cmd = 'python {}/tools/elfsigner/elfsigner.py -t {} -o {}/testsigs'.format(HEXAGON_SDK_ROOT,serial_number,output_path)

    # if testsig generation failed exit the script with 2 exit status
    if print_and_run_cmd(generate_testsig_cmd) != 0 : sys.exit(2)

    testsig = '{}/testsigs/testsig-{}.so'.format(output_path,serial_number)
    if os.path.exists(testsig): print("\n--- Testsig generated successfully ---")
    else : print("\n--- Error while generating testsig - {} ---".format(testsig))

    sys.stdout.flush()
    return testsig

# push generated testsig to the device
def push_testsig(dev_num, testsig):
    # push the testsig to common path accessible for all LA/LE devices
    dsp_path = HLOS_DST+"lib/rfsa/adsp"

    print("\n--- Push Test Signature ---")
    print_and_run_cmd('adb -s '+dev_num+' wait-for-device shell mkdir -p '+dsp_path)
    print_and_run_cmd('adb -s '+dev_num+' wait-for-device push {} {}'.format(testsig,dsp_path))
    print_and_run_cmd('adb -s '+dev_num+' wait-for-device shell sync')
    print_and_run_cmd('adb -s '+dev_num+' wait-for-device')
    print("Done")

# sign library input_file and save output to output_path
def sign_library(input_file,output_path,no_disclaimer):
    if input_file == "":
        sys.exit("\n--- Error: Please check input arguments ---")

    print("\n--- Signing input library ---")

    if no_disclaimer:
        generate_signing_cmd = 'python {}/tools/elfsigner/elfsigner.py -i {} -o {}/output --no_disclaimer'.format(HEXAGON_SDK_ROOT,input_file,output_path)
    else:
        generate_signing_cmd = 'python {}/tools/elfsigner/elfsigner.py -i {} -o {}/output'.format(HEXAGON_SDK_ROOT,input_file,output_path)

    # if signing failed exit the script with 2 exit status
    if print_and_run_cmd(generate_signing_cmd) != 0 : sys.exit(2)

    print("\n--- Library signed successfully ---")
    sys.stdout.flush()

# main entry point for child process
if __name__ == '__main__':

    options = parser.parse_args()
    set_global_vars()

    # register signal handler 'signal.SIGINT' to function handler 'signal_usr1'
    signal.signal(signal.SIGINT, signal_usr1)

    # signer.py getserial
    if options.action == 'getserial':
        # get device number and return if no device is connected
        dev_num = get_dev_num()
        if not dev_num:
            sys.exit("\n--- No device connected! ---")

        # get serial number from the device and display it
        serial_number = get_serial_num(dev_num)
        print("Serial number returned by connected device: " + serial_number)

    # signer.py sign
    elif options.action == 'sign':

        if options.output_dir:
            output_path = options.output_dir
        else:
            output_path = '.'

        if options.no_disclaimer:
            no_disclaimer = 1
        else:
            no_disclaimer = 0

        # signer.py sign -i <input_file>
        if options.input_file:
            input_file = options.input_file

            # sign input library
            sign_library(input_file,output_path,no_disclaimer)
        else:
            if not options.local:
                # get device number and return if no device is connected
                dev_num = get_dev_num()
                if not dev_num:
                    sys.exit("\n--- No device connected! ---")

            # signer.py sign -t <serial_number>
            if options.serial_number:
                serial_number = options.serial_number
                if serial_number.find("0x") == -1:
                    serial_number = hex(int(serial_number)).rstrip('L')
            else:
                # get serial number from the device
                serial_number = get_serial_num(dev_num)


            # generate testsig and flush output to console
            testsig = generate_testsig(serial_number,output_path,no_disclaimer)

            if not options.local:
                # push generated testsig to device
                push_testsig(dev_num, testsig)

    sys.stdout.flush()
