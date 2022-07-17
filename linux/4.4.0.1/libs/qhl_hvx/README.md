# Qualcomm Hexagon Libraries for HVX

QHL HVX is a set of libraries for the HVX extensions of the Qualcomm Hexagon DSPs with a v66 or v68 instruction set. These libraries provide a set of functions performing various mathematical computations using HVX instructions.  Floating-point libraries are only supported on v68.

Since performance, alongside with accuracy, is crucial to each mathematical operation, most of the QHL HVX implementations are written in C with intrinsics.

The QHL HVX libraries are organized in the following groups:

* Qualcomm Hexagon Math HVX (QHMATH HVX)
    * Common mathematical computations on vectors of fixed-point or floating-point data

* Qualcomm Hexagon Basic Linear Algebra Subprograms HVX (QHBLAS HVX)
    * Basic linear algebra operations on vectors of fixed-point or floating-point data

* Qualcomm Hexagon Digital Signal Processing HVX (QHDSP HVX)
    * Digital signal processing operations on vectors of fixed-point or floating-point data

## Dependencies

QHL HVX libraries are dependent on QHL libraries, that can be found under libs/qhl.
In case of using QHL HVX libraries, user needs to be sure that QHL libraries are built
and added to a linking line.

## Naming convention

Naming convention used for QHL HVX library functions is as below:

    <group_name>[_<output_format>]_<function description>_<input_format>
    <output_format> is optional and will be used only if input and output formats differ in a function.
    <function description> describes the function in brief

|Variant| Allowed values |
|--|--|
|group_name|qhdsp_hvx, qhmath_hvx, qhcomplex_hvx, qhblas_hvx|
|output_format|af,ahf,ab,ah,aw,aub,auh,auw,ach,acw,acf,achf,aqf16,aqf32,f,hf,h,w,i|
|input_format|af,ahf,ab,ah,aw,aub,auh,auw,acub,ach,acw,acf,achf,aqf16,aqf32|

The desciption for the short forms used in input and output formats is expanded in the table below:
When a function takes multiple input arguments, the input format only refers to the format of the main argument.

|Abbreviation| Description |
|--|--|
|a|array|
|c|complex|
|u|unsigned values|
|i|64-bit integer|
|f|single-precision float|
|hf|half-precision float|
|b|8-bit fixed-point or integer|
|h|16-bit fixed-point or integer|
|w|32-bit fixed-point or integer|
|qf16|16-bit QFLOAT|
|qf32|32-bit QFLOAT|

Example: qhdsp_hvx_acw_c1dfft_acub: This function falls under the sub-group qhdsp_hvx. It takes an array of complex unsigned byte inputs(acub) and computes Complex 1-D FFT(c1dfft) outputs stored in an array of complex 32-bit integers(acw).


## Build instructions

In order to build QHL HVX libraries, run the following command:

    cd $HEXAGON_SDK_ROOT/libs/qhl_hvx
    ./cmake_build.<bash/cmd> hexagon DSP_ARCH=<v66/v68>

In order to remove all binaries, run the following command:

    ./cmake_build.<bash/cmd> hexagon_clean DSP_ARCH=<v66/v68>

## Running an example

In order to see an example of how to invoke the libraries, please see $HEXAGON_SDK_ROOT/examples/qhl_hvx.

## Running unit tests

The QHL HVX library comes with a set of unit tests that measures the accuracy and performance of each function.  The following instructions demonstrate how to run the unit tests.

    cd $HEXAGON_SDK_ROOT/libs/qhl_hvx/tests

### Build and run accuracy tests on simulator

    ./run_accuracy_tests_simulator.<bash/cmd>  <v66/v68>

### Build and run benchmarks on simulator

    ./run_benchmarks_simulator.<bash/cmd>  <v66/v68>

### Build and run accuracy tests on device

    ./run_tests_dsp.<bash/cmd> <v66/v68>

***Note:*** A v66 or v68 target must be connected to execute this command.  HVX float tests require a v68 device.

### Performance Numbers

The [following table](./performance/QHL_HVX_Stats.xlsx) provides performance numbers for the QHL_HVX library functions.  These numbers may vary depending on the exact system configuration.