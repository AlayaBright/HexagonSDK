import os
import sys
import Device_configuration as dev_conf

Lparser=""

# List of supported targets
def target_list():
    target_info = ["qcs605","sm8150","sm6150","qcs405","sm7150","sm6125","sm8250","lahaina","rennell","saipan","kamorta","bitra",'agatti',"qrb5165","qcs403","qcs610","cedros","kodiak","mannar","sxr2130","strait","monaco","divar"]
    return target_info

def call_parser(parser):
    global Lparser
    parser.add_argument("-L", dest="linux_env", action="store_true", help="Build If your device HLOS is Linux Embedded (e.g APQ8096)  Don't choose this option if your device HLOS is Linux Android")
    parser.add_argument('-T', dest="target", default='', help="Specify target name  <qcs605, sm8150, sm6150, qcs405, sm7150, sm6125, sm8250, lahaina, rennell, saipan, kamorta, divar, bitra, agatti, monaco, qrb5165, qcs403, qcs610, cedros,kodiak, mannar, sxr2130, strait>")
    parser.add_argument('-d', action="store", dest="domain", default='', help="Domain value is optional. Default is 0 for Agatti and 3 for other targets. Options=<0/1/2/3> (0: ADSP, 3: CDSP, 2: SDSP, 1: MDSP)")
    parser.add_argument('-s', dest="serial_num", default='', help="Specify the adb serial number for the device.")
    parser.add_argument('-U', dest="unsigned", action="store", help="Run on specified PD: 1 for Unsigned PD and 0 for Signed PD. Default is 1")
    parser.add_argument('-M', dest="no_rebuild", action="store_true", help="Does not recompile hexagon and android variants")
    parser.add_argument("-N", dest="no_signing", action="store_true", help="Skips test signature installation")
    parser.add_argument('-32', dest="thirty_two_bit", action="store_true", help="Specify only if the target is 32 bit")
    parser.add_argument('-dest', dest="hlos_root_dest", help="Use this to override the default HLOS directory path <vendor, usr, data>")
    parser.add_argument('-libdest', dest="hlos_lib_dest", help="Use this to override the default HLOS lib directory path <vendor/lib, usr/lib, data/lib> <vendor/lib64, usr/lib64, data/lib64>")

    Lparser = parser
    return parser


class get_config:
    device_object=""
    dsp_arch=os.getenv('DEFAULT_DSP_ARCH')
    HLOS_ARCH=os.getenv('DEFAULT_HLOS_ARCH')
    LocalParser=""
    variant="android"
    build_flavor=os.getenv('DEFAULT_BUILD')
    dsp_dir=""
    hlos_dir=""
    tools_versions = ['8.3']      #List of Hexagon Tools versions supported by SDK, except the default Tools version
    HEXAGON_TOOLS_ROOT=os.getenv('HEXAGON_TOOLS_ROOT')
    tools_variant=os.getenv('DEFAULT_TOOLS_VARIANT')
    target_info = ""
    if HEXAGON_TOOLS_ROOT != None:
        for version in tools_versions:
            if version in HEXAGON_TOOLS_ROOT:
                tools_variant = 'toolv8'+version[-1]
                break
    global Lparser
    def __init__(self):
        if "-h" in sys.argv:
            self.device_object = dev_conf.device_config(False)      #creating an object for device_config class in Device_configuration script
        else:
            self.device_object = dev_conf.device_config(True)      #creating an object for device_config class in Device_configuration script
        self.LocalParser = Lparser.parse_args()
        self.target_info = target_list()
        self.LocalParser.target = self.LocalParser.target.lower()
        if self.LocalParser.thirty_two_bit:
            self.HLOS_ARCH="32"
        if self.LocalParser.serial_num != '' and self.LocalParser.target != "monaco":
            self.LocalParser.target = dev_conf.get_target_name_using_serial_num(self.device_object, self.LocalParser.serial_num)
        if self.LocalParser.target not in self.target_info:
            print("Error! Target name is not in list \nPlease pass serial number using -s or target name using -T for the targets supported below:")
            for target_name in self.target_info: print("\t"+target_name)
            sys.exit()
        if self.LocalParser.serial_num == '':
            self.LocalParser.serial_num = dev_conf.dev_dict(self.device_object, self.LocalParser.target)
            if self.LocalParser.serial_num == '':
                print("Error! device not connected!")
                sys.exit()
        if self.LocalParser.domain == '':
            if self.LocalParser.target in dev_conf.DEVICES_WITH_ONLY_ADSP:
                self.LocalParser.domain = '0'
            else:
                self.LocalParser.domain = '3'
        if self.LocalParser.unsigned not in ['0','1']:
            self.LocalParser.unsigned = '1'
        if self.LocalParser.target in ["sm7150","qcs605"] :
            self.dsp_arch="v65"
        elif self.LocalParser.target in ["sm8150", "sm6150", "qcs405", "sm6125", "sm8250", "rennell", "saipan", "kamorta", "divar", "bitra", 'agatti', 'monaco', "qrb5165", "qcs403", "qcs610", "mannar","strait","sxr2130"]:
            self.dsp_arch="v66"
        elif self.LocalParser.target in ["lahaina", "cedros","kodiak"]:
            self.dsp_arch="v68"
        self.hlos_dir=self.variant+'_'+self.build_flavor
        if self.LocalParser.linux_env:           #If -L specified, UbuntuARM_ReleaseG_aarch64
            self.variant="ubuntuARM"
            self.hlos_dir=self.variant[:1].upper() + self.variant[1:] +'_'+self.build_flavor
        if self.HLOS_ARCH=="64":
            self.hlos_dir+="_aarch64"
        self.dsp_dir="hexagon_"+self.build_flavor+"_"+self.tools_variant+"_"+self.dsp_arch
    def get_parameters(self):
        if self.LocalParser.target in dev_conf.DEVICES_WITH_ONLY_ADSP and self.LocalParser.domain!='0':
            print("Error! Domain "+ self.LocalParser.domain +" is not supported on Target!")
            sys.exit()
        elif self.LocalParser.domain not in ['0','1','2','3']:
            print("Error! Domain "+ self.LocalParser.domain +" is not valid!")
            sys.exit()
        return self.LocalParser, self.dsp_arch, self.variant, self.HLOS_ARCH, self.dsp_dir, self.hlos_dir


# Function is accepting 2 arguments, object_config as 'object of get_config class' And HEXAGON_SDK_ROOT as 'String'
def get_DST_PARAMS(object_config, HEXAGON_SDK_ROOT):
    Local_parser = object_config.LocalParser
    prior_sm8250_targets = ["qcs605","sm8150","sm6150","qcs405","sm7150","sm6125", "qcs403", "qcs610"]

    testsig_output = '{}/utils/signer'.format(HEXAGON_SDK_ROOT)
    testsig_command = 'python ' + HEXAGON_SDK_ROOT + '/utils/scripts/signer.py -o ' + testsig_output + ' -d ' + Local_parser.domain + ' -s ' +Local_parser.serial_num
    if Local_parser.hlos_root_dest:
        call_test_sig=testsig_command + ' -dest ' + Local_parser.hlos_root_dest
    else:
        call_test_sig=testsig_command

    ROOT_DST='/vendor/'
    if Local_parser.linux_env:           #If -L specified, UbuntuARM_Debug_aarch64
        if Local_parser.hlos_root_dest:
            call_test_sig=testsig_command + ' -LE -dest ' + Local_parser.hlos_root_dest
        elif Local_parser.thirty_two_bit:
            call_test_sig=testsig_command + ' -LE'
        else:
            call_test_sig=testsig_command + ' -LE -64'

        ROOT_DST='/usr/'

    if Local_parser.hlos_root_dest:
        ROOT_DST='/'+Local_parser.hlos_root_dest+'/'
    APPS_DST=ROOT_DST+'bin/'
    DSP_DST=ROOT_DST+'lib/rfsa/dsp/sdk/'
    if Local_parser.thirty_two_bit:
        LIB_DST=ROOT_DST+'lib/'
    else:
         if Local_parser.target in ["qrb5165", "qcs610", "sxr2130", "monaco"]:
             LIB_DST=ROOT_DST+'lib/'
         else:
             LIB_DST=ROOT_DST+'lib64/'

    if Local_parser.hlos_lib_dest:
        LIB_DST = Local_parser.hlos_lib_dest
        LIB_DST = LIB_DST.replace("\\","/")
        if not LIB_DST.startswith('/'):
            LIB_DST = '/' + LIB_DST
        if not LIB_DST.endswith('/'):
            LIB_DST = LIB_DST + '/'

    DSP_LIB_SEARCH_PATH="DSP_LIBRARY_PATH"
    DSP_LIB_PATH="\""+ROOT_DST+"lib/rfsa/dsp/sdk;\"";

    if Local_parser.target in prior_sm8250_targets:
        DSP_LIB_SEARCH_PATH = "ADSP_LIBRARY_PATH"
        DSP_LIB_PATH="\""+ROOT_DST+"lib/rfsa/dsp/sdk\;"+ROOT_DST+"lib/rfsa/adsp;\""

    return call_test_sig, APPS_DST, DSP_DST, LIB_DST, DSP_LIB_PATH, DSP_LIB_SEARCH_PATH
