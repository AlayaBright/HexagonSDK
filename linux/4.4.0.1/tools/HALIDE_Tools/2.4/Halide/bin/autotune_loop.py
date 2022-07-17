#!/usr/bin/env python3


import sys
import subprocess
import os
from argparse import ArgumentParser
from re import search
from time import time, sleep
from shutil import copyfile
from glob import glob
from multiprocessing import cpu_count, Pool

def uses_android(target):
    features = target.split('-')
    return 'android' in features

def is_hexagon_standalone(target):
    features = target.split('-')
    return 'hexagon' == features[0]

def get_hexagon_tools_dir(hexagon_sdk_root):
    hexagon_tools_base = os.path.join(hexagon_sdk_root, 'tools/HEXAGON_Tools')
    if 'HEXAGON_TOOLS_VER' in os.environ.keys():
        hexagon_tools_ver = os.path.join(hexagon_tools_base, os.environ['HEXAGON_TOOLS_VER'])
    else:
        hexagon_tools_ver = glob(hexagon_tools_base + "/*/")[-1]
    hexagon_tools_dir = os.path.join(hexagon_tools_ver, 'Tools')
    print('Using Hexagon tools: {0}'.format(hexagon_tools_dir))
    return hexagon_tools_dir

def purge(dir, pattern):
    for f in os.listdir(dir):
        if search(pattern, f):
            os.remove(os.path.join(dir, f))

def host_os(os_name):
    if os_name == 'win':
        return os.name == 'nt'
    if os_name == 'linux':
        return os.name == 'posix'

def build_schedule_independent_objs(bin_dir):
    # Build rungen.o and rungen_skel.o here.
    print('Building hexagon standalone mode rungen objects...', end="")
    sys.stdout.flush()
    rungen_standalone_dir = os.path.join(halide_root, 'tools/hexagon_standalone')
    rungen_main_file = os.path.join(halide_root, 'tools/RunGenMain.cpp')
    rungen_skel_source = os.path.join(rungen_standalone_dir, 'rungen_skel.c')

    if not os.path.exists(rungen_skel_source):
        print("Autotuning not supported for hexagon standalone mode! Exiting...")
        exit(1)
    rungen_skel_file = os.path.join(bin_dir, 'rungen_skel.o')
    rungen_object_file = os.path.join(bin_dir, 'rungen.o')

    rungen_includes = app_cxx_includes + [
            '-I{0}'.format(rungen_standalone_dir),
            '-I{0}/rtos/qurt/computev{1}/include/qurt'
                .format(hexagon_sdk_root, args.q6_version),
            '-I{0}/rtos/qurt/computev{1}/include/posix'
                .format(hexagon_sdk_root, args.q6_version)
    ]

    rungen_defines = ['-DHL_HEXAGON_STANDALONE', '-DHALIDE_NO_PNG', '-DHALIDE_NO_JPEG']

    build_rungen_skel_cmd = [
        hexagon_cc,
        '-fPIC', '-c',
        rungen_skel_source,
        '-o', rungen_skel_file
    ]

    build_rungen_skel_cmd += hexagon_cflags
    build_rungen_skel_cmd += hexagon_includes

    build_rungen_object_cmd = [
        hexagon_cxx,
        '-std=c++11',
        '-fPIC', '-c',
        rungen_main_file,
        '-o', rungen_object_file
    ]

    build_rungen_object_cmd += hexagon_cflags
    build_rungen_object_cmd += hexagon_includes
    build_rungen_object_cmd += rungen_includes
    build_rungen_object_cmd += rungen_defines

    if (subprocess.call(build_rungen_skel_cmd) or
        subprocess.call(build_rungen_object_cmd)):
        print("Error building rungen objects. Exiting...")
        exit(1)
    else:
        print(" done.")

def build_librungen_skel(registration_file, halide_object_file, output_dir, stdout, stderr):
    registration_object_file = os.path.join(output_dir, 'registration.o')

    build_registration_obj_cmd = [
        hexagon_cxx,
        registration_file,
        '-std=c++11',
        '-fPIC', '-c',
        '-o', registration_object_file
    ]
    build_registration_obj_cmd += hexagon_cflags + app_cxx_includes + hexagon_includes

    shared_lib_flags = ('-mv{0} -mG0lib -G0 -fPIC -shared -lc -lc++ -lc++abi'
                        .format(args.q6_version).split())

    bin_dir = os.path.join(args.bld_dir, 'bin')
    rungen_skel_file = os.path.join(bin_dir, 'rungen_skel.o')
    rungen_object_file = os.path.join(bin_dir, 'rungen.o')

    files_to_link = [
        rungen_skel_file,
        rungen_object_file,
        registration_object_file,
        halide_object_file
    ]
    linker_group = ['-Wl,--start-group'] + files_to_link + ['-Wl,--end-group']

    librungen_file = os.path.join(output_dir, 'librungen_skel.so')

    build_librungen_cmd = [
        hexagon_cc,
        '-o', librungen_file
    ]
    build_librungen_cmd += shared_lib_flags + linker_group


    subprocess.call(build_registration_obj_cmd, stdout=stdout, stderr=stderr)
    subprocess.call(build_librungen_cmd, stdout=stdout, stderr=stderr)


def make_featurization(output_dir, seed, filename, generator, extra_generator_args):
    os.makedirs(output_dir, exist_ok=True)

    featurization_file = os.path.join(output_dir, filename + '.featurization')
    sample_file = os.path.join(output_dir, filename + '.sample')
    registration_file = os.path.join(output_dir, filename + '.registration.cpp')
    object_file = os.path.join(output_dir, filename + obj_ext)

    if os.path.exists(featurization_file):
        os.remove(featurization_file)
    if os.path.exists(sample_file):
        os.remove(sample_file)

    # Sample 0 in each batch is best effort beam search, with no randomness
    if search('/0$', output_dir):
        dropout = '100'
        beam = '32'
    # The other samples are random probes biased by the cost model
    else:
        dropout = '1'     # 1% chance of operating entirely greedily
        beam = '1'

    env = os.environ.copy()

    new_env = {
       'HL_WEIGHTS_DIR': weights,
       'HL_SEED': seed,
       'HL_RANDOM_DROPOUT': dropout,
       'HL_BEAM_SIZE': beam,
       'HL_PERMIT_FAILED_UNROLL': '1',
       'HL_PREFETCHING': '1',
       'HL_MACHINE_PARAMS': machine_params
    }


    env.update(new_env)

    if host_os('linux'):
        libauto = os.path.join(autosched_bin, 'libauto_schedule.so')
        output_option = ['-o', os.path.join(output_dir, 'bench')]
    if host_os('win'):
        libauto = os.path.join(autosched_bin, 'auto_schedule.dll')
        if uses_android(args.target):
            output_option = ['-o', os.path.join(output_dir, 'bench')]
        else:
            output_option = [
                '-Fe' + os.path.join(output_dir, 'bench'),
                '-Fo' + output_dir + '\\'
            ]

    run_generator_cmd = [
        generator, '-g', filename,
        '-o', output_dir,
        '-e', 'o,h,schedule,featurization,registration',
        '-f', filename,
        '-p', libauto,
        '-s', 'Adams2019',
        'target=' + args.target,
        'auto_schedule=true'
    ]
    run_generator_cmd += extra_generator_args

    build_executable_cmd = [
        app_cxx, '-DHALIDE_NO_PNG', '-DHALIDE_NO_JPEG',
        '-I', output_dir,
        os.path.join(halide_root,'tools/RunGenMain.cpp'),
        registration_file,
        object_file,
    ]

    build_executable_cmd += app_cxx_includes
    build_executable_cmd += app_cxx_flags
    build_executable_cmd += app_ld_flags
    build_executable_cmd += output_option

    with open(os.path.join(output_dir, 'stdout.txt'), 'w') as stdout, open(
      os.path.join(output_dir, 'stderr.txt'), 'w') as stderr:
        ret = subprocess.call(run_generator_cmd,
            env=env, timeout=compilation_timeout, stdout=stdout, stderr=stderr)
        if ret == 0:
            if is_hexagon_standalone(args.target):
                build_librungen_skel(registration_file, object_file,
                                     output_dir, stdout, stderr)
            else:
                subprocess.call(build_executable_cmd, stdout=stdout)

    if 'DEBUG_AUTOTUNE' in os.environ.keys() and os.environ['DEBUG_AUTOTUNE'] == '1':
        pass
    else:
        # remove files that are not needed anymore.
        if os.path.exists(registration_file):
            os.remove(registration_file)
        if os.path.exists(object_file):
            os.remove(object_file)

    # remove intermediate .obj files produced by windows.
    obj_files = glob(output_dir + '\*.obj')
    for obj_file in obj_files:
        os.remove(obj_file)

    print(".", end="")
    sys.stdout.flush()
    return 0

# device setup function to be run after every device restart (android only).
def device_setup(hexagon_standalone=False):
    print("Performing initial device(s) setup...")

    for device in devices:
        root_cmd = 'adb -s {0} root'.format(device)
        wait_for_device_cmd = 'adb -s {0} wait-for-device'.format(device)
        remount_cmd = 'adb -s {0} remount'.format(device)
        make_device_path_cmd = 'adb -s {0} shell mkdir -p {1}'.format(device, device_path)
        copy_testsig_cmd = ('adb -s {0} shell cp '
            '/vendor/lib/rfsa/adsp/testsig*.so {1}'.format(device, device_path))
        copy_remote_skel_cmd = ('adb -s {0} shell cp '
            '/vendor/lib/rfsa/adsp/libhalide_hexagon_remote_skel.so {1}'.format(device, device_path))

        subprocess.call(root_cmd, shell=True)
        subprocess.call(wait_for_device_cmd, shell=True)
        subprocess.call(remount_cmd, shell=True)
        subprocess.call(wait_for_device_cmd, shell=True)
        subprocess.call(make_device_path_cmd, shell=True)
        subprocess.call(copy_testsig_cmd, shell=True)
        subprocess.call(copy_remote_skel_cmd, shell=True)

        if hexagon_standalone:
            # In hexagon standalone mode, push the rungen_host executable, which drives
            # the rungen dsp library, as 'bench' once.

            # Hardcoding this to cdsp for now. Seems like a reasonable assumption.
            dsp_type = 'cdsp'
            rungen_host_file = os.path.join(halide_root,
                        'tools/hexagon_standalone/{0}/rungen_host'.format(dsp_type))
            librungen_stub_file = os.path.join(halide_root,
                        'tools/hexagon_standalone/host/librungen_stub.so'.format(dsp_type))
            push_rungen_host_cmd = ('adb -s {0} push {1} {2}/bench'
                                        .format(device, rungen_host_file, device_path))
            change_permission_cmd = ('adb -s {0} shell chmod +x {1}/bench'
                                        .format(device, device_path))
            push_librungen_stub_cmd = ('adb -s {0} push {1} /vendor/lib64'
                                        .format(device, librungen_stub_file, device_path))
            subprocess.call(push_rungen_host_cmd, shell=True)
            subprocess.call(change_permission_cmd, shell=True)
            subprocess.call(push_librungen_stub_cmd, shell=True)

    print("Device setup completed for devices: {0}".format(' '.join(devices)))

def run_android(bin_dir, device, fp):
    make_device_path_cmd = 'adb -s {0} shell mkdir -p {1}'.format(device, device_path)
    push_bench_cmd = 'adb -s {0} push {1}/bench {2}'.format(device, bin_dir, device_path)
    change_permission_cmd = 'adb -s {0} shell chmod +x {1}/bench'.format(device, device_path)
    execute_bench_cmd = ('adb -s {0} shell ADSP_LIBRARY_PATH={1} '
                            '{2}/bench --estimate_all --benchmarks=all'
                            .format(device, device_path, device_path))
    subprocess.call(make_device_path_cmd, shell=True)
    subprocess.call(push_bench_cmd, shell=True)
    subprocess.call(change_permission_cmd, shell=True)
    subprocess.call(execute_bench_cmd, shell=True, timeout=benchmarking_timeout,
        stderr=subprocess.STDOUT, stdout=fp)

def run_hexagon(bin_dir, device, fp):
    make_device_path_cmd = ('adb -s {0} shell mkdir -p {1}'
                                .format(device, device_path))
    push_librungen_skel_cmd = ('adb -s {0} push {1}/librungen_skel.so {2}'
                                .format(device, bin_dir, device_path))
    execute_bench_cmd = ('adb -s {0} shell \'cd {1} && '
                            'LD_LIBRARY_PATH=/vendor/lib64 '
                            './bench --estimate_all --benchmarks=all\''
                            .format(device, device_path))

    subprocess.call(make_device_path_cmd, shell=True)
    subprocess.call(push_librungen_skel_cmd, shell=True)
    subprocess.call(execute_bench_cmd, shell=True, timeout=benchmarking_timeout,
        stderr=subprocess.STDOUT, stdout=fp)


def run_host(bin_dir, fp):
    if host_os('linux'):
        execute_bench_cmd = '{0}/bench --estimate_all --benchmarks=all'.format(bin_dir)
    elif host_os('win'):
        execute_bench_cmd = '{0}\\bench.exe --estimate_all --benchmarks=all'.format(bin_dir)
    subprocess.call(execute_bench_cmd, shell=True, timeout=benchmarking_timeout,
        stderr=subprocess.STDOUT, stdout=fp)

def benchmark_sample(output_dir, seed, pipeline_id, filename, device):
    featurization_file = os.path.join(output_dir, filename + '.featurization')
    sample_file = os.path.join(output_dir, filename + '.sample')
    # windows produces bench.exe instead of bench
    if not uses_android(args.target) and host_os('win'):
        bench_file = os.path.join(output_dir, 'bench.exe')
    elif is_hexagon_standalone(args.target):
        # Use librungen_skel.so as a proxy for 'bench'.
        bench_file = os.path.join(output_dir, 'librungen_skel.so')
    else:
        bench_file = os.path.join(output_dir,'bench')
    # Give CPU clocks a chance to spin back up if we're thermally throttling
    sleep(1)
    if os.path.exists(bench_file):
        with open(os.path.join(output_dir,'bench.txt'), 'w') as fp:
            if (uses_android(args.target)):
                run_android(output_dir, device, fp)
            elif (is_hexagon_standalone(args.target)):
                run_hexagon(output_dir, device, fp)
            else:
                run_host(output_dir, fp)
    else:
        print("Not benchmarking {0} because compilation failed!".format(output_dir))
        return

    # Add the runtime, pipeline id, and schedule id to the feature file
    with open(os.path.join(output_dir, 'bench.txt'), 'r') as fp:
        for line in fp:
            if search('Benchmark for', line):
                run_time = line.split()[7]
                build_sample_cmd = [
                    os.path.join(autosched_tools,'featurization_to_sample'),
                    featurization_file,
                    run_time,
                    pipeline_id,
                    seed,
                    sample_file
                ]
                subprocess.call(build_sample_cmd)
                break

    if 'DEBUG_AUTOTUNE' in os.environ.keys() and os.environ['DEBUG_AUTOTUNE'] == '1':
        pass
    else:
        # Remove files not needed anymore
        if os.path.exists(featurization_file):
            os.remove(featurization_file)
        if os.path.exists(bench_file):
            os.remove(bench_file)


def retrain_model():
    print("Retraining model...")
    halide_lib_path = os.path.join(halide_root, 'lib')
    env = os.environ.copy()
    if host_os('win'):
        if 'PATH' in env.keys():
            env['PATH'] += os.pathsep + halide_lib_path
        else:
            env['PATH'] = halide_lib_path
    elif host_os('linux'):
        if 'LD_LIBRARY_PATH' in env.keys():
            env['LD_LIBRARY_PATH'] += os.pathsep + halide_lib_path
        else:
            env['LD_LIBRARY_PATH'] = halide_lib_path

    sample_files = ' '.join(glob(os.path.join(samples, '**/*.sample'), recursive=True))
    best_benchmark_file = os.path.join(samples, 'best.{0}.benchmark.txt'.format(args.pipeline))
    best_schedule_file = os.path.join(samples, 'best.{0}.schedule.h'.format(args.pipeline))
    stdin_array = bytearray(sample_files, 'utf-8')
    retrain_model_cmd = [
        os.path.join(autosched_tools, 'retrain_cost_model'),
        '--epochs=' + str(args.batch_size),
        '--rates="0.0001"',
        '--num_cores=32',
        '--initial_weights=' + weights,
        '--weights_out=' + weights,
        '--best_benchmark={0}'.format(best_benchmark_file),
        '--best_schedule={0}'.format(best_schedule_file),
    ]
    with open(os.path.join(samples, 'retrain.log'), 'a') as fp:
        process = subprocess.Popen(retrain_model_cmd, stderr=subprocess.STDOUT,
            stdin=subprocess.PIPE, stdout=fp, env=env)
        process.stdin.write(stdin_array)
        process.stdin.close()
        process.communicate()



dir_path = os.getcwd()
file_path = os.path.dirname(os.path.abspath(__file__))
default_halide_root = os.path.abspath(os.path.join(file_path, os.pardir))
default_weights = os.path.join(file_path, 'baseline.weights')
default_target = 'host'

parser = ArgumentParser(
    description='This script will be autotuning the autoscheduler\'s cost model training pipeline.'
)

parser.add_argument('generator',
                    help='Path to pipeline generator which needs to be autotuned.')
parser.add_argument('pipeline',
                    help='Name of the pipeline.')
parser.add_argument('--weights', '-w', default=default_weights,
                    help='Location of starting weights file.')
parser.add_argument('--halide-root', '-hr', default=default_halide_root,
                    help='Location of installed Halide.')
parser.add_argument('--target', '-t', default=default_target,
                    help='Target to autotune for. Defaults to "host".')
parser.add_argument('--bld-dir', '-b', default=dir_path,
                    help='Location for any built artifacts. '
                    'Samples directory will be created inside.')
parser.add_argument('--batch-size', '-s', type=int, default=32,
                    help='Batch of samples to compile in parallel. Defaults to 32')
parser.add_argument('--num-batches', '-n', type=int, default=100,
                    help='Number of batches to train for. Defaults to 100.')
parser.add_argument('--parallel-factor', '-p', type=int, default=16,
                    help='Parallelization factor to be used for schedules. Defaults to 16.')
parser.add_argument('--hexagon-sdk-root', '-sdk',
                    help='Location of Hexagon sdk. Required for an android target.')
parser.add_argument('--run-device-id', '-r', nargs='+',
                    help='Space separated list of devices to run for.'
                    ' Required for an android target.')
parser.add_argument('--q6-version', '-v', type=int, default=65,
                    help='Hexagon (Q6) version of the device. Defaults to 65 for v65.')
parser.add_argument('--generator-arg-sets', nargs='+',
                    help='Space separated list of generator-arg sets. '
                    'Arguments within each set delimited by ";" '
                    'and wrapped in quotes(""). '
                    'For e.g. "set1arg1=1;set1arg2=foo" set2=bar '
                    '"set3arg1=3.14;set4arg2=42"')

args = parser.parse_args()


if not os.path.exists(args.halide_root):
    print("Specified Halide location {0} not found!".format(args.halide_root))
    exit(1)

halide_root =  args.halide_root

if host_os('win') and not uses_android(args.target):
    obj_ext = '.obj'
else:
    obj_ext = '.o'


if uses_android(args.target) or is_hexagon_standalone(args.target):
    if not args.hexagon_sdk_root:
        print("Location of hexagon sdk is required for android target."
            " Use the option --hexagon-sdk-root/-sdk to specify it.")
        exit(1)
    if not args.run_device_id:
        print("Please specify the device(s) to run the benchmarking on"
            " by using the option --run-device-id/-r.")
        exit(1)

    hexagon_sdk_root =  args.hexagon_sdk_root

    if not os.path.exists(hexagon_sdk_root):
        print("Specified Hexagon sdk location {0} not found!".format(hexagon_sdk_root))
        exit(1)
    app_target = 'android'

    if host_os('linux'):
        # assume Linux. Note, this might not be a reasonable assumption.
        android_arm64_toolchain = os.path.join(hexagon_sdk_root,
            'tools/android-ndk-r19c/toolchains/llvm/prebuilt/linux-x86_64')
    elif host_os('win'):
        android_arm64_toolchain = os.path.join(hexagon_sdk_root,
            'tools/android-ndk-r19c/toolchains/llvm/prebuilt/windows-x86_64')
    app_cxx = os.path.join(android_arm64_toolchain, "bin/clang++")
    app_cxx_flags = "-target aarch64-linux-android21 -std=c++11 -stdlib=libc++".split()
    app_ld_flags = "-llog -fPIE -pie".split()
    app_cxx_includes = [
        '-I{0}'.format(os.path.join(halide_root, 'tools')),
        '-I{0}'.format(os.path.join(halide_root, 'include')),
        '-I{0}'.format(os.path.join(halide_root, 'support'))
    ]

    hexagon_tools_dir = get_hexagon_tools_dir(hexagon_sdk_root)
    if not os.path.exists(hexagon_tools_dir):
        print('Error: Hexagon tools directory not found: {0}'.format(hexagon_tools_dir))
        exit(1)
    hexagon_cxx = os.path.join(hexagon_tools_dir, 'bin/hexagon-clang++')
    hexagon_cc = os.path.join(hexagon_tools_dir, 'bin/hexagon-clang')
    if not os.path.exists(hexagon_cxx):
        print('Error: Cannot find hexagon-clang++:{0}'.format(hexagon_cxx))
        exit(1)
    if not os.path.exists(hexagon_cc):
        print('Error: Cannot find hexagon-clang:{0}'.format(hexagon_cc))
        exit(1)
    hexagon_includes = [
        '-I{0}/incs'.format(hexagon_sdk_root),
        '-I{0}/incs/stddef'.format(hexagon_sdk_root)
    ]
    hexagon_cflags = "-mhvx -mhvx-length=128B -mv{0} -O2".format(args.q6_version).split()

    devices = args.run_device_id
    num_devices = len(devices)

else:
    app_target = "host"
    app_cxx_includes = [
        '-I{0}'.format(os.path.join(halide_root, 'include')),
        '-I{0}'.format(os.path.join(halide_root, 'support'))
    ]
    if host_os('linux'):
        app_cxx = "clang++"
        app_cxx_flags = "-std=c++11 -stdlib=libc++".split()
        app_ld_flags = "-ldl -lpthread".split()
    elif host_os('win'):
        app_cxx = 'cl.exe'
        app_cxx_flags = ['/EHsc', '/nologo']
        app_ld_flags = []

    devices = ["host"]
    num_devices = 1

compilation_timeout = 300
benchmarking_timeout = 180

autosched_bin = os.path.join(halide_root, 'lib')
autosched_tools = os.path.join(halide_root, 'bin')

samples = os.path.join(args.bld_dir, 'samples')
os.makedirs(samples, exist_ok=True)

weights = os.path.join(samples, 'updated.weights')
device_path = '/vendor/bin/tmp/autotune'

machine_params = str(args.parallel_factor) + ',24000000,40'


if __name__ == "__main__":

    if 'DEBUG_AUTOTUNE' in os.environ.keys() and os.environ['DEBUG_AUTOTUNE'] == '1':
        print('Autotuning in debug mode!')

    if args.generator_arg_sets:
        generator_arg_sets = [argset.split(';') for argset in args.generator_arg_sets]
    else:
        # Make sure the list has a size of atleast one.
        generator_arg_sets = ['']

    if os.path.exists(weights):
        print("Using existing weights {0}".format(weights))
    else:
        # Only copy over the weights if we don't have any already,
        # so that restarted jobs can continue from where they left off
        if os.path.exists(args.weights):
            print('Copying starting weights from {0} to {1}'.format(args.weights, weights))
            copyfile(args.weights, weights)
        else:
            print("Invalid starting weights file {0}. Exiting!".format(args.weights))
            exit(1)

    hexagon_standalone = is_hexagon_standalone(args.target)
    if hexagon_standalone:
        bin_dir = os.path.join(args.bld_dir, 'bin')
        os.makedirs(bin_dir, exist_ok=True)
        # Build schedule independent object files required for librungen_skel.so.
        build_schedule_independent_objs(bin_dir)
    if uses_android(args.target) or hexagon_standalone:
        device_setup(hexagon_standalone)

    # Don't clobber existing samples
    first_sample = max(
        [int(batch.split('_')[-2]) for batch in glob(samples + '/batch_*')],
        default=0) + 1

    print("First sample is {0}".format(first_sample))
    local_cores = cpu_count()
    print("Local number of cores detected as {0}".format(local_cores))


    # Main execution loop.
    for batch_id in range(first_sample, first_sample + args.num_batches):
        start = time()
        for extra_args_idx, extra_generator_args in enumerate(generator_arg_sets):
            if extra_generator_args:
                print("Adding extra generator args {0} for batch_{1}"
                    .format(extra_generator_args, batch_id))

            # compile a batch of samples using the generator in parallel
            output_dir = os.path.join(samples, 'batch_{0}_{1}'.format(batch_id, extra_args_idx))

            # Copy the weights being used into the batch folder so that we can repro failures
            os.makedirs(output_dir, exist_ok=True)
            copyfile(weights, output_dir + '/used.weights')

            with open(os.path.join(output_dir, 'extra_generator_args.txt'), 'w') as fp:
                fp.write('\n'.join(extra_generator_args))

            # Do parallel compilation in batches, so that machines with fewer than BATCH_SIZE cores
            # don't get swamped and timeout unnecessarily
            print("Compiling {0} samples".format(args.batch_size), end = " ")
            sys.stdout.flush()

            filename = args.pipeline

            # Single-threaded loop for debug.
            if 'DEBUG_AUTOTUNE' in os.environ.keys() and os.environ['DEBUG_AUTOTUNE'] == '1':
                for sample_id in range(args.batch_size):
                    seed = '{0:04d}{1:04d}'.format(batch_id, sample_id)
                    make_featurization(os.path.join(output_dir, str(sample_id)), seed,
                    filename, args.generator, extra_generator_args)
            else:
                pool = Pool(processes=local_cores)
                for sample_id in range(args.batch_size):
                    seed = '{0:04d}{1:04d}'.format(batch_id, sample_id)

                    result = pool.apply_async(
                        make_featurization,
                        args=(os.path.join(output_dir, str(sample_id)), seed,
                            filename, args.generator, extra_generator_args)
                    )

                pool.close()
                try:
                    pool.join()
                except KeyboardInterrupt:
                    print("Caught KeyboardInterrupt. Terminating workers!")
                    pool.terminate()
                    pool.join()
                    exit(1)

            print(" done.")

            # benchmark them serially using rungen and in parallel across devices.
            print("Benchmarking pipelines...")
            # Single-threaded loop for debug.
            if 'DEBUG_AUTOTUNE' in os.environ.keys() and os.environ['DEBUG_AUTOTUNE'] == '1':
                for sample_id_psuedo in range(int(args.batch_size/num_devices)):
                    for n in range(num_devices):
                        sample_id = sample_id_psuedo * num_devices + n
                        device = devices[n]
                        seed = '{0:04d}{1:04d}'.format(batch_id, sample_id)
                        benchmark_sample(os.path.join(output_dir,str(sample_id)),
                            seed, str(batch_id), filename, device)

            else:
                for sample_id_psuedo in range(int(args.batch_size/num_devices)):
                    num_pool = min(num_devices, local_cores)
                    pool = Pool(processes=num_pool)
                    for n in range(num_devices):
                        sample_id = sample_id_psuedo * num_devices + n
                        device = devices[n]
                        seed = '{0:04d}{1:04d}'.format(batch_id, sample_id)

                        result = pool.apply_async(
                            benchmark_sample,
                            args=(os.path.join(output_dir, str(sample_id)),
                                seed, str(batch_id), filename, device)
                        )
                    pool.close()
                    try:
                        pool.join()
                    except KeyboardInterrupt:
                        print("Caught KeyboardInterrupt. Terminating workers!")
                        pool.terminate()
                        pool.join()
                        exit(1)

            print("done.")
            retrain_model()

            stop = time()
            duration = stop - start
            print("Batch {0} took {1} seconds to compile, benchmark and retrain".format(
                batch_id, int(duration)))
