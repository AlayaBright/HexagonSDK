=====================
DIRECTORY STRUCTURE
=====================

Tree of this directory is as the following:

.
├── generated_hvx_func_coefficients.txt
├── HVX_code
│   ├── build
│   │   ├── bench_generated_hvx_func.elf
│   │   ├── bench_generated_hvx_func.o
│   │   ├── generated_hvx_func.o
│   │   ├── test_generated_hvx_func.elf
│   │   └── test_generated_hvx_func.o
│   ├── inc
│   │   └── generated_hvx_func.h
│   ├── Makefile
│   ├── src
│   │   ├── disassembly
│   │   │   └── generated_hvx_func.S.dump
│   │   └── c
│   │       └── generated_hvx_func.c
│   └── test
│       ├── bench_generated_hvx_func.c
│       ├── test_data
│       │   ├── inputs_16f.txt
│       │   ├── outputs_16f_hvx.txt
│       │   └── outputs_16f_np.txt
│       └── test_generated_hvx_func.c
├── qhcg_args.txt
├── qhcg_console.log
├── README
├── report
│   ├── generated_hvx_func_accuracy_report.txt
│   ├── generated_hvx_func_long_report.pdf
|   └── generated_hvx_func_performance_report.txt
└── versions.log

<qhcg_args.txt> file lists arguments that were passed to QHCG during generation of this output.

<generated_hvx_func_coefficients.txt> file contains coefficients of polynomial approximation.

<qhcg_console.log> file is log from console during generation of this output.

<versions.log> file contains versions of OS and tools that are used from QHCG.

<HVX_code> directory contains generated code. It consist of few subdirectories and files.
    <src> subdirectory is further divided into <disassembly> and <c> directories.
        <c> directory contains generated HVX implementation.
        <disassembly> directory contains output from the compiler for generated HVX implementation in C.
              NOTE: This S.dump file is used just to verify optimality of the C code, it is not intended to be used as a source file.
    <inc> subdirectory consist of header file corresponding to generated HVX implementation in C.
    <test> subdirectory contains functional and performance tests used to calculate accuracy and measure speed of generated implementation.
           As reference in functional test it is used appropriate numpy implementation.
           <test_data> subdirectory contains outputs from HVX implementation and corresponding numpy function for the same inputs.
    <build> subdirectory contains object and executable files of generated implementation, as well as functional and performance tests.
    Makefile file can be used to re-build generated code manually.

<report> directory consist of report files.
    <generated_hvx_func_report.pdf> gives graphical comparison of generated HVX implementation and corresponding numpy function.
    <generated_hvx_func_accuracy_report.txt> file lists Max, Mean and RMS errors of generated HVX implementation
    <generated_hvx_func_performance_report.txt> file shows performance results of generated HVX implementation.
                                                Note: This file is generated only if --exec_bench argument was active.

=================
REBUILD THE CODE
=================

If user wants to manually add some changes to the generated HVX implementation, code can be easily rebuilt using the generated Makefile:

cd HVX_code
make all

=================
RE-RUN THE TESTS
=================

If user wants to manually re-run the functional and performance tests, i.e after modifications in generated HVX code, it can be done using the following commands:

functional test: hexagon-sim build/test_generated_hvx_func.elf
performance test: hexagon-sim --timing HVX_code/build/bench_generated_hvx_func.c

Note: Running the Makefile, alongside with generated code, tests will also be rebuilt.
      Functional test will just produce new outputs_hvx.txt file in HVX_code/test/test_data directory, i.e Max/Mean/RMS error will not be recalculated.
