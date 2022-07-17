# Compile HVX code

# %%
from pathlib import Path
import subprocess
import os


HEXAGON_CC = 'hexagon-clang'
HEXAGON_SIM = 'hexagon-sim'
HEXAGON_OBJDUMP = 'hexagon-llvm-objdump'
INCLUDE = '-I {inc} '
CFLAGS = '-m{hvx_arch} -O2 -G0 -Wall -mhvx -c'
LDFLAGS = '-m{hvx_arch} -mhvx'
LIBS = '-lhexagon'

DBG_QHCG = False  # Enable HexagonToolChain debug prints

class HexagonToolchain(object):
    '''
    Hexagon toolchain
    '''

    def __init__(self, outputer, hvx_arch, toolchain_path=None):
        self.outputer = outputer
        self.toolchain_path = toolchain_path
        self.hvx_arch = hvx_arch
        self.cc = HEXAGON_CC
        self.sim = HEXAGON_SIM
        self.objdump = HEXAGON_OBJDUMP
        self.CFLAGS = CFLAGS.format(hvx_arch=hvx_arch)
        self.LDFLAGS = LDFLAGS.format(hvx_arch=hvx_arch)
        self.version_info = None
        self.includes = ''
        self.compiler_exists = self.__compile_init()

    def compile(self):

        self.__create_makefile()

        if not self.compiler_exists:
            print('No Hexagon toolchain found!')
            return

        self.__compile_func()
        self.__disassembly()
        self.__compile_test()
        self.__compile_bench()
        self.__link_test()
        self.__link_bench()

    def execute_test(self):

        self.__execute('test_elf')

    def execute_bench(self):

        exec_ok, msg = self.__execute('bench_elf', True)
        if (exec_ok == True):
            with open(self.outputer.get_file_path('report_perf'), "w") as report_perf:
                report_perf.writelines(msg)

    def check_compiler(self):

        '''
        Test if Hexagon toolchain executable exist.
        Prints version of the compiler.
        '''
        print('\nHexagon toolchain version:')

        cmd = [self.cc, '--version']
        compiler_found, msg = self.__print_and_run_cmd(cmd)
        if compiler_found:
            self.version_info = msg
        else:
            self.version_info = 'No Hexagon toolchain found!'
            print(self.version_info)
            print("It's important to provide toolchain path to get referenced output values for report.")
        print('---')

        return compiler_found

    def get_version_str(self) -> str:

        return '# Hexagon toolchain version:\n' + self.version_info + '\n'

    def __print_and_run_cmd(self, cmd, prn_stdout= True) -> bool: # , str:

        '''
        Print and execute command with the error check
        '''

        if type(cmd) == list:
            cmd_str = ' '.join(cmd)
        else:
            cmd_str = str(cmd)

        if DBG_QHCG:
            print(">#> " + cmd_str)

        # Python 3.5 and later
        # res = subprocess.run(cmd_str, capture_output=True) # shell=True
        # if DBG_QHCG: print(res)
        # self.outputer.stdout(res.stdout.decode("utf-8"))
        # self.outputer.stderr(res.stderr.decode("utf-8"))
        # exec_ok = True if res.returncode is 0 else False

        # patch for Python 3.4
        try:
            msg = subprocess.check_output(cmd_str, stderr=subprocess.STDOUT, shell=True)
            msg_str = msg.decode("utf-8")
            if prn_stdout:
                self.outputer.stdout(msg_str)
            exec_ok = True
        except subprocess.CalledProcessError as e:
            msg_str = e.output.decode("utf-8")
            self.outputer.stderr(msg_str)
            exec_ok = False

        return exec_ok, msg_str

    def __path(self, file_type: str) -> str:

        return self.outputer.get_file_path(file_type)

    def __compile_init(self):

        if DBG_QHCG:
            print('compile_init')

        self.includes = INCLUDE.format(inc=self.__path('inc'))

        if self.toolchain_path is None:
            hexagon_tools_root = os.environ.get('HEXAGON_TOOLS_ROOT')
            if hexagon_tools_root is None:
                default_hexagon_tools_root = os.environ.get('DEFAULT_HEXAGON_TOOLS_ROOT')
                if default_hexagon_tools_root is not None:
                    hexagon_tools_root = default_hexagon_tools_root
            self.toolchain_path = hexagon_tools_root

        if (self.toolchain_path is not None) and (self.toolchain_path is not ""):
            toolchain_bin_path = Path(self.toolchain_path, 'Tools/bin')
            self.cc = str(Path(toolchain_bin_path, HEXAGON_CC))
            self.sim = str(Path(toolchain_bin_path, HEXAGON_SIM))
            self.objdump = str(Path(toolchain_bin_path, HEXAGON_OBJDUMP))

        return self.check_compiler()

    def __compile_func(self, func_type='c'):

        if DBG_QHCG:
            print('\n\n\n\n__compile_func for:' + func_type)
        cmd = [self.cc, self.CFLAGS, self.__path(func_type), self.includes, '-o', self.__path(func_type + '_out')]
        self.__print_and_run_cmd(cmd)

    def __compile_test(self):

        if DBG_QHCG:
            print('\n\n\n\n__compile_test')
        self.__compile_func('test')

    def __compile_bench(self):

        if DBG_QHCG:
            print('\n\n\n\n__compile_bench')
        self.__compile_func('bench')

    def __link_test(self):

        if DBG_QHCG:
            print('\n\n\n\n__link_test')
        cmd = [self.cc, self.LDFLAGS, self.__path('c_out'), self.__path('test_out'), '-o', self.__path('test_elf'), LIBS]
        self.__print_and_run_cmd(cmd)

    def __link_bench(self):

        if DBG_QHCG:
            print('\n\n\n\n__link_bench')
        cmd = [self.cc, self.LDFLAGS, self.__path('c_out'), self.__path('bench_out'), '-o', self.__path('bench_elf'), LIBS]
        self.__print_and_run_cmd(cmd)

    def __execute(self, elf_name: str, use_timing: bool = False):

        if not self.compiler_exists:
            print('No Hexagon toolchain found!')
            return
        args = '--timing' if use_timing else ''
        cmd = [self.sim, '-q', args, self.__path(elf_name)]
        exec_ok, msg = self.__print_and_run_cmd(cmd)

        msg = msg.replace("member", "element")
        msg = msg.replace("CPC", "processor cycles")
        print('Done.')

        return exec_ok, msg

    def __disassembly(self):

        cmd = self.objdump + ' -r -d -C -S -print-imm-hex -no-show-raw-insn ' + self.__path('c_out')
        exec_stat, msg = self.__print_and_run_cmd(cmd, False)
        if exec_stat:
            with open(self.__path('objdump'), 'w') as file:
                file.write(msg)

    def __create_makefile(self):

        makefile_path = str(Path(__file__).resolve().parent.joinpath('templates/makefile.txt'))
        with open(makefile_path, 'r') as file:
            makefile_txt = file.read()
            makefile_txt = makefile_txt.format(cc=self.cc, sim=self.sim, name=self.outputer.get_hvx_func_name(), hvx_arch=self.hvx_arch)
        # patch for Python 3.4.3: no write_text() method
        with open(self.__path('mk'), "w") as makefile:
            makefile.writelines(makefile_txt)
