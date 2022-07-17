# See in the README file for output data structure

import json
from pathlib import Path
import sys
import subprocess
import platform
import shutil


class Tee(object):
    def __init__(self, name, mode):
        self.file = open(name, mode)
        self.stdout = sys.stdout
        sys.stdout = self

    def __del__(self):
        sys.stdout = self.stdout
        self.file.close()

    def write(self, data):
        self.file.write(data)
        self.stdout.write(data)
        self.flush()

    def flush(self):
        self.file.flush()
        self.stdout.flush()


class Outputer(object):

    def __init__(self, output_path: str = None, function_name: str = 'qhcg_approximation', bundle: bool = False):
        self.bundle = bundle  # generate multiple functions ready for pack
        self.args = "args"  # set later with set_args()
        self.hvx_func_name = function_name
        self.hvx_file_name = function_name
        self.file_paths = {
            "readme": "README",
            "h": "HVX_code/inc/" + function_name + ".h",
            "c": "HVX_code/src/c/" + function_name + ".c",
            "c_out": "HVX_code/build/" + function_name + ".o",
            "objdump": "HVX_code/src/disassembly/" + function_name + ".S.dump",
            "s": "HVX_code/src/disassembly/" + function_name + ".S",
            "s_out": "HVX_code/build/" + function_name + ".o",
            "test": "HVX_code/test/test_" + function_name + ".c",
            "test_out": "HVX_code/build/test_" + function_name + ".o",
            "bench": "HVX_code/test/bench_" + function_name + ".c",
            "bench_out": "HVX_code/build/bench_" + function_name + ".o",
            "mk": "HVX_code/Makefile",
            "qhcg_internal": "HVX_code/inc/qhcg_internal.h",
            # ---
            "test_elf": "HVX_code/build/test_" + function_name + ".elf",
            "bench_elf": "HVX_code/build/bench_" + function_name + ".elf",
            # --- dir
            "build": "HVX_code/build",
            "inc": "HVX_code/inc",
            # --- reports
            "reports_dir": "report/",
            "txt_summary": "report/" + function_name + "_summary_report.txt",
            "pdf_summary": "report/" + function_name + "_summary_report.pdf",
            "txt_detailed": "report/" + function_name + "_detailed_report.txt",
            "pdf_detailed": "report/" + function_name + "_detailed_report.pdf",
            "report_accr": "report/" + function_name + "_acc_report.txt",
            "report_perf": "report/" + function_name + "_perf_report.txt",
            "stdout": function_name + "_console.log",
            "stderr": function_name + "_stderr.log",
            "args": function_name + "_args.txt",
            "coefficients": function_name + "_coefficients.txt",
            "versions": "versions.log",
            # --- data
            "in_16b": "HVX_code/test/test_data/inputs_16b.txt",
            "in_16f": "HVX_code/test/test_data/inputs_16f.txt",
            "in_32b": "HVX_code/test/test_data/inputs_32b.txt",
            "in_32f": "HVX_code/test/test_data/inputs_32f.txt",
            "hvx_16b": "HVX_code/test/test_data/outputs_16b_hvx.txt",
            "np_16b": "HVX_code/test/test_data/outputs_16b_np.txt",
            "hvx_16f": "HVX_code/test/test_data/outputs_16f_hvx.txt",
            "np_16f": "HVX_code/test/test_data/outputs_16f_np.txt",
            "hvx_32b": "HVX_code/test/test_data/outputs_32b_hvx.txt",
            "np_32b": "HVX_code/test/test_data/outputs_32b_np.txt",
            "hvx_32f": "HVX_code/test/test_data/outputs_32f_hvx.txt",
            "np_32f": "HVX_code/test/test_data/outputs_32f_np.txt",
        }

        self.output_paths = [
            'HVX_code/build',
            'HVX_code/inc',
            'HVX_code/src/disassembly',
            'HVX_code/src/c',
            'HVX_code/test/test_data',
            'report',
        ]
        self.src_path = Path(__file__).resolve().parent
        self.root_path = self.src_path.parent
        if (output_path is None) or (output_path is ''):
            output_path = 'output'
        self.output_path = Path(self.root_path, output_path)

        if not self.bundle:
            self.__empty_output()
        self.__create_subfolders()
        self.tee = Tee(self.get_file_path('stdout'), "w")
        self.versions = self.__init_versions()

    def __del__(self):
        try:
            del self.tee
        except: # pylint: disable=W0702
            pass

        try:
            self.versions.close()
        except: # pylint: disable=W0702
            pass

    def set_args(self, args: tuple):
        self.args = list()
        for i in args:
            if type(i) is list:
                for l in i:
                    self.args.append(l)
            else:
                self.args.append(i)

        # patch for Python 3.4.3: no write_text() method
        args_f = open(self.get_file_path('args'), "w")
        args_f.writelines(json.dumps(self.args))
        args_f.close()

    def get_hvx_func_name(self) -> str:
        return self.hvx_func_name

    def get_hvx_file_name(self) -> str:
        return self.hvx_file_name

    def get_file_path(self, file_type: str) -> str:
        '''
        Get absolute path for a given file type
        '''

        f_rel_path = self.file_paths.get(file_type, None)
        if f_rel_path == None:
            f_rel_path = Path(file_type)  # append to output root

        f_rel_path = Path(self.output_path, f_rel_path)

        return str(f_rel_path)

    def save_version(self, version_info: str):
        # append in file version
        self.versions.writelines(version_info)

    def write_readme(self):
        readme_src = str(Path(__file__).resolve().parent.joinpath('templates/qhcg_readme.txt'))
        shutil.copyfile(readme_src, self.get_file_path('readme'))

    def stdout(self, msg: str):
        if len(msg) > 0:
            self.tee.write(msg)

    def stderr(self, msg: str):
        if len(msg) > 0:
            self.tee.write('[stderr] ' + msg)

    def __empty_output(self):
        dir_dir = Path(self.output_path)
        if not dir_dir.exists():
            return

        is_ok_to_rm = False

        is_qhcg_folder = Path(dir_dir, 'HVX_code').exists() and Path(dir_dir, 'report').exists()
        if is_qhcg_folder:
            print('\n[Warning] This action will delete directory: [{}]'.format(str(dir_dir)))
            need_yes = input("Do you really want to do that [Y/n]? ")
            is_ok_to_rm = True if need_yes is '' or need_yes is 'y' else False
        else:
            print('\n[Critical warning] This action will delete directory: [{}]'.format(str(dir_dir)))
            need_yes = input("Do you really want to do that [y/N]? ")
            is_ok_to_rm = True if need_yes is 'y' else False

        if not is_ok_to_rm:
            print('QHCG execution aborted.')
            sys.exit(0)

        try:
            shutil.rmtree(str(dir_dir), ignore_errors=True)
        except PermissionError:
            print("[Error Permission] Cannot empty destination directory: " +
                  str(dir_dir) + " because it is being used by another process!")
            sys.exit(1)
        except: # pylint: disable=W0702
            print("[Error] Cannot empty destination directory: " + str(dir_dir))
            sys.exit(1)

    def __create_subfolders(self):
        try:
            dir_dir = Path(self.output_path)
            # patch for Python 3.4.3: no exist_ok argument ! (exist_ok=True)
            if not dir_dir.exists():
                dir_dir.mkdir(parents=True)
        except: # pylint: disable=W0702
            print("[ERROR] Problem with creating directory: " + str(dir_dir))
            sys.exit(1)

        for subf in self.output_paths:
            # create sub-folders!
            try:
                dir_sub = Path(self.output_path, subf)
                dir_sub_str = str(dir_sub)
                # Create target Directory
                dir_sub.mkdir(parents=True)  # exist_ok=False
                # print(f"Directory {dir_sub_str} Created ")
            except FileExistsError:
                if not self.bundle:
                    print("[ERROR] Directory ", dir_sub_str,  " already exists")
            except FileNotFoundError:
                print("[ERROR] Problem with creating directory: " + dir_sub_str)
                sys.exit(1)

    def __init_versions(self):
        file_ptr = open(self.get_file_path('versions'), "w")
        file_ptr.writelines('# System information:\n')
        file_ptr.writelines(platform.system() + ' | ' + platform.machine() + '\n')
        file_ptr.writelines(platform.platform() + '\n')
        file_ptr.writelines('# Python information:\n')
        file_ptr.writelines(sys.version + '\n')
        file_ptr.writelines('API version: ' + str(sys.api_version) + '\n')
        file_ptr.writelines('# Python modules information:\n')
        pip_mods = subprocess.check_output('pip3 freeze', shell=True)
        file_ptr.writelines(pip_mods.decode('utf-8') + '\n')
        return file_ptr
