# Qualcomm Hexagon Libraries

QHL is a set of libraries for the Qualcomm Hexagon DSPs with a v66 or v68 instruction set.  These libraries provide a set of functions performing various mathematical computations using scalar instructions.

Since performance, alongside with accuracy, is crucial to each mathematical operation, most of the QHL implementations are written in assembly language or in C with intrinsics.

The QHL libraries are organized in the following groups:

* Qualcomm Hexagon Math (QHMATH)
    * Common mathematical computations on real numbers

* Qualcomm Hexagon Complex (QHCOMPLEX)
    * Common mathematical computations on complex numbers

* Qualcomm Hexagon Basic Linear Algebra Subprograms (QHBLAS)
    * Basic linear algebra operations

* Qualcomm Hexagon Digital Signal Processing (QHDSP)
    * Digital signal processing operations

## Naming convention

Naming convention used for QHL library functions is as below:

    <group_name>[_<output_format>]_<function description>_<input_format>
    <output_format> is optional and will be used only if input and output formats differ in a function.
    <function description> describes the function in brief

|Variant| Allowed values |
|--|--|
|group_name|qhdsp, qhmath, qhcomplex, qhblas|
|output_format|af,ah,ach,acf,f,h|
|input_format|af,ah,ach,acf,f,h|

The desciption for the short forms used in input and output formats is expanded in the table below:
When a function takes multiple input arguments, the input format only refers to the format of the main argument.

|Abbreviation| Description |
|--|--|
|a|array|
|c|complex|
|f|single-precision float|
|hf|half-precision float|
|h|fixed-point or integer type, usually but not always 16-bit|

Example: qhdsp_c1dfft_ach: This function falls under the sub-group qhdsp. It takes an array of complex 16-bit integer inputs(ach) and computes Complex 1-D FFT(c1dfft) outputs stored in an array of complex 16-bit integers(ach).


## Build instructions

In order to build all the libraries, run the following command:

    cd $HEXAGON_SDK_ROOT/libs/qhl
    ./cmake_build.<bash/cmd> hexagon DSP_ARCH=<v66/v68>

In order to remove all binaries, run the following command:

    ./cmake_build.<bash/cmd> hexagon_clean DSP_ARCH=<v66/v68>

## Running an example

In order to see an example of how to invoke the libraries, please see $HEXAGON_SDK_ROOT/examples/qhl.

## Running unit tests

The QHL library comes with a set of unit tests that measures the accuracy and performance of each function.  The following instructions demonstrate how to run the unit tests.

    cd $HEXAGON_SDK_ROOT/libs/qhl/tests

### Build and run accuracy tests on simulator

    ./run_accuracy_tests_simulator.<bash/cmd>  <v66/v68>

### Build and run benchmarks on simulator

    ./run_benchmarks_simulator.<bash/cmd>  <v66/v68>

### Build and run accuracy tests on device

    ./run_tests_dsp.<bash/cmd> <v66/v68>

***Note:*** A v66 or v68 target must be connected to execute this command.

### Performance Numbers

The [following table](./performance/QHL_Stats.xlsx) provides performance numbers for the QHL library functions.  These numbers may vary depending on the exact system configuration.