# Qualcomm Hexagon Libraries Example

This example illustrates how to use the Qualcomm Hexagon Libraries (QHL) located under `$HEXAGON_SDK_ROOT/libs/qhl`.

Currently, QHL is only supported for V66 and V68 targets. Please refer to the complete [documentation on the QHL library](../../doxygen/qhl/index.html){target=_blank} for more details on each supported function and the unit test framework provided to test the performance and accuracy of all functions.

## Overview

The example calls a few functions from the qhmath and qhblas libraries and sends their output to the console. The list of functions invoked in this example are listed below:

* qhmath_abs_af
* qhmath_abs_ah
* qhblas_vector_add_af
* qhblas_vector_add_ah

This example uses a different build approach from the other example projects in the SDK: it uses a standalone Makefile and invokes CMake to build the required QHL libs. The standalone Makefile in this project does not have dependencies on the make.d build system.

## Instructions

### Walkthrough

All step-by-step instructions for building and running the test both on simulator and on target are captured in the qhl_walkthrough.py script.

You may run the script directly.  To do so, simply run 'python qhl_walkthrough.py'. To see the messages sent to the DSP run 'adb wait-for-device logcat -s adsprpc' in a new window.

You can also execute manually each instruction present in the script to reproduce separately the build, run on simulator, and run on target steps.

### Standalone Makefile

Unlike most Hexagon SDK examples, this example uses a standalone Makefile to build the project and its dependencies: no hexagon.min and android.min are provided to enable make.d support.  A similar approach may be used for building custom projects that do not come readily equipped with make.d or CMake support.

To build the example and its dependencies for a v66 target, simply type the following command:

    make V=v66

***Note:*** In order to build the QHL libraries, the Makefile invokes a CMake command as part of the command above: `cmake_build.bash hexagon DSP_ARCH=v66` from within the QHL library project.

