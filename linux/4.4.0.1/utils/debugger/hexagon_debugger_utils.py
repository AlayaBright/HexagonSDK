
from __future__ import print_function
import lldb
import xml.dom.minidom as dom
from xml.dom.minidom import parse
import os
import subprocess
import re
import webbrowser
import sys
if sys.version_info[0] < 3:
    import urllib
else:
    import urllib.request

def __lldb_init_module(debugger, dict):
    debugger.HandleCommand(
        'command script add -f hexagon_debugger_utils.device_connect device_connect')
    print("Hexagon SDK device_connect command loaded")

def run_cmd(cmd):
    if os.system(cmd) != 0: sys.exit(2)

def print_and_get_user_pd_list():

    error = ''
    portList = []

    print("\n- Reading list of active user processes ...")

    if sys.version_info[0] < 3:
        opener = urllib.FancyURLopener({})
    else:
        opener = urllib.request.FancyURLopener({})
    try:
        f = opener.open("http://localhost:5555/pslist.xml")
    except (Exception) as e:
        error = str(e)

    if not error:
        doc = parse(f)
        if doc == None:
            error = "No XML file exists on the local host for parsing"

    if not error:
        print("\n- Please select the process to debug (if you don't see your process ensure you application is running):")
        i = 0
        while (i < doc.getElementsByTagName("item").length) and not error:
            process = doc.getElementsByTagName("item")[i]
            if (5 != process.getElementsByTagName("column").length):
                error = "column length not correct"
                break;

            command = process.getElementsByTagName("column")[3]
            if (not command.getAttribute("name") == "command"):
                error = "command not found"
                break;

            print(str(i + 1) + "\t" + str(command.firstChild.data))

            debug_port = process.getElementsByTagName("column")[4]
            if (not debug_port.getAttribute("name") == "debugport"):
                error = "failed to find debug port"
                break;

            debug_port_int = int(debug_port.firstChild.data)
            portList.append(debug_port_int)

            if (debug_port_int < 1 or debug_port_int > 99999):
                error = "debug port " + str(debug_port_int) + "out of bounds"
                break;

            i = i + 1

        if i == 0 and not error:
            error = "Unable to find any running processes"

    if error:
        error = error + "\nRun $HEXAGON_SDK_ROOT/utils/debugger/verify_debugger.py to verify that the debugger is setup correctly"

    return error, portList

def device_connect(debugger, register, result, dict):
    result.Clear()
    print('device_connect command called', file=result)
    if not os.getenv('SDK_SETUP_ENV'):
        result.SetError("SDK Environment not set up -> run setup_sdk_env script from SDK's root directory")
        return
    sign_dsp_name = ''
    subSyst = int(input("\n- Select the DSP you will be debugging:\n1\tADSP\n2\tCDSP\n>"))
    if(subSyst == 1):
        dsp_name = 'ADSP'
    elif(subSyst == 2):
        dsp_name = 'CDSP'
    else:
        result.SetError("Unknown DSP, select valid one from the list")
        return

    if(dsp_name == "CDSP"):
        pd_selection = int(input("\n- Select 1 for debugging on Unsigned PD and 2 for debugging on Signed PD\n>"))
        if(pd_selection == 1):
            sign_dsp_name = 'UnsignedCDSP'
        else:
            sign_dsp_name = 'SignedCDSP'

    print('\n- Configuring the DSP to stop at user process start ...')
    run_cmd('adb wait-for-device')
    run_cmd('adb wait-for-device root')
    run_cmd('adb wait-for-device shell setprop fastrpc.process.attrs 2')
    run_cmd('adb wait-for-device shell setprop vendor.fastrpc.process.attrs 2')

    script = os.getenv('HEXAGON_SDK_ROOT') + '/utils/debugger/run_remote_debug_agent.py'
    if not os.path.isfile(script):
        result.SetError("File not found: " + script)
        return

    try:
        if sys.version_info[0] < 3:
            with open(os.devnull, 'w') as devnull:
                procId1 = subprocess.Popen('python ' + script + ' -' + dsp_name, shell=True, stdout=subprocess.PIPE, stdin=subprocess.PIPE,stderr=devnull)
        else:
             procId1 = subprocess.Popen('python ' + script + ' -' + dsp_name, shell=True, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.DEVNULL)
        #Wait for remote_debug_agent to setup port
        while True:
            line = procId1.stdout.readline()
            if not line:
                print("Could not read line from remote debug agent or debug agent already running.")
                break
            if("Created Port" in str(line)):
                print("Port Created")
                break
            if procId1.poll() != None:
                print("Remote debug agent failed to setup correctly. Exiting.")
                sys.exit()
    except (Exception) as e:
        result.SetError(str(e))
        return

    print("\n- Open a new CLI shell and run your application in adb shell.\nExample:\n\tadb shell export LD_LIBRARY_PATH=/vendor/lib64/:$LD_LIBRARY_PATH ADSP_LIBRARY_PATH=\"/vendor/lib/rfsa/dsp/sdk\;/vendor/lib/rfsa/dsp/testsig;\" /vendor/bin/calculator 0 3 1000\n")
    if sys.version_info[0] < 3:
        raw_input("- Press ENTER once your application has started\n>")
    else:
        input("- Press ENTER once your application has started\n>")
    error,pd_list = print_and_get_user_pd_list()
    if error:
        result.SetError(error)
        return

    itemNum = input("> ")
    if(int(itemNum) < 1 or int(itemNum) > len(pd_list)):
        result.SetError("Unknown process, select valid one from the list")
        return
    req_port = pd_list[int(itemNum) - 1]

    run_cmd('adb forward tcp:6666 tcp:' + str(req_port))

    print("\n- Pull the user process shell module from target")
    errcode = 0
    FNULL = open(os.devnull, 'w')

    if(dsp_name == "ADSP"):
        dsp = 'adsp'
        frpcName = 'fastrpc_shell_0'
    if(dsp_name == "CDSP" and sign_dsp_name == "SignedCDSP"):
        dsp = 'cdsp'
        frpcName = 'fastrpc_shell_3'
    if(dsp_name == "CDSP" and sign_dsp_name == "UnsignedCDSP"):
        dsp = 'cdsp'
        frpcName = 'fastrpc_shell_unsigned_3'

    filedir = os.path.dirname(__file__)
    frpcPath = filedir + '/' + frpcName

    res = lldb.SBCommandReturnObject()
    if os.path.exists(frpcPath):
        # Check the target list for the existing targets
        command = 'target list'
        debugger.GetCommandInterpreter().HandleCommand(command, res)
        error = res.GetError()
        if error:
            result.SetError(error)
            result.SetStatus(lldb.eReturnStatusFailed)
            return
        output = res.GetOutput().split('\n')
        for target in output:
            if (frpcPath.replace("/","\\")) in target:
                index = re.search('target #(\d+)', target, re.IGNORECASE).group(1)
                command = 'target delete ' + index + ' --clean'
                debugger.GetCommandInterpreter().HandleCommand(command, res)
                error = res.GetError()
                if error:
                    result.SetError(error)
                    result.SetStatus(lldb.eReturnStatusFailed)
                    return
                else:
                    break

    errcode = subprocess.call('adb pull /vendor/dsp/' + dsp + '/' + frpcName + ' ' + filedir + '/.' ,shell = True,stdout=subprocess.PIPE, stdin=subprocess.PIPE,stderr=FNULL)
    if(errcode):
        errcode = subprocess.call('adb pull /dsp/' + dsp + '/' + frpcName + ' ' + filedir + '/.' ,shell = True,stdout=subprocess.PIPE, stdin=subprocess.PIPE,stderr=FNULL)
    if(errcode):
        errcode = subprocess.call('adb pull /dsp/' + frpcName + ' ' + filedir + '/.' ,shell = True,stdout=subprocess.PIPE, stdin=subprocess.PIPE,stderr=FNULL)
    if(errcode):
        errcode = subprocess.call('adb pull /vendor/dsp/' + frpcName + ' ' + filedir + '/.' ,shell = True,stdout=subprocess.PIPE, stdin=subprocess.PIPE,stderr=FNULL)
    if(errcode):
        result.SetError("Could not find shell file: " + frpcName + " || errcode:" + str(errcode))
        result.SetStatus(lldb.eReturnStatusFailed)
        return

    command = 'target create ' + frpcPath
    debugger.GetCommandInterpreter().HandleCommand(command, res)
    error = res.GetError()
    if error:
        result.SetError(error)
        result.SetStatus(lldb.eReturnStatusFailed)
        return
    output = res.GetOutput()
    print(output)

    command = 'b fastrpc_uprocess_exception'
    print("- Setting breakpoint in user process execption handler\n" + command)
    debugger.GetCommandInterpreter().HandleCommand(command, res)
    error = res.GetError()
    if error:
        result.SetError(error)
        result.SetStatus(lldb.eReturnStatusFailed)
        return
    output = res.GetOutput()
    print(output)
    print("- Specify your shared library search path (optional).\n- If lldb cannot find your shared object then it cannot setup breakpoints in it.\n- Example: C:/Qualcomm/Hexagon_SDK/<HEXAGON_SDK_VERSION>/examples/calculator/hexagon_Debug_toolv84_v66/ship/\n")
    if sys.version_info[0] < 3:
        so_search_path = raw_input("> ")
    else:
        so_search_path = input("> ")
    if so_search_path:
        solib_path = str(so_search_path)
        for letter in solib_path:
            if letter == "\\":
                solib_path = solib_path.replace(letter,"/")
        command = 'image search-paths add . ' + solib_path
        print(command)
        debugger.GetCommandInterpreter().HandleCommand(command, res)
        error = res.GetError()
        if error:
            result.SetError(error)
            result.SetStatus(lldb.eReturnStatusFailed)
            return
        output = res.GetOutput()
        print(output)

    print("\n- Stopping at beginning of process, once stopped:\n\t- Set any additional breakpoints (module must be found in provided source paths)\n\t- Enter 'c' when ready to continue\n\n")

    command = 'gdb-remote localhost:6666'
    debugger.GetCommandInterpreter().HandleCommand(command, res)
    error = res.GetError()
    if error:
        result.SetError(error)
        result.SetStatus(lldb.eReturnStatusFailed)
        return
    output = res.GetOutput()
    print(output)
