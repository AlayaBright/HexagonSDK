# Hexagon SDK examples

Each example illustrates one or more SDK features as summarized in the [List of examples](#list-of-examples) section.

All examples are designed to run on both the simulator and target.

## Setup

Follow the [setup instructions](../tools/setup.md) before running any example. Remember to always run the [setup_sdk_env](../tools/setup.md#sdk-environment) script to set up the SDK local environment.

## Walkthrough scripts

Most example projects come with a walkthrough python script that automates the following steps:

* [Sign](../tools/sign.md#sign-a-device) the device so that libraries can be loaded on it.
* [Build](../tools/build.md) the executable that runs on the application processor.
* [Build](../tools/build.md) the Hexagon DSP library, which implements the functions that the application processor will invoke.
* [Run](../tools/run.md) the example on the simulator.
* Push the executable, DSP library, and, if required, any test data on the target.
* [Run](../tools/run.md) the example on the target.

Usage of the script is as follows, which is displayed by using the -h option:

    <walkthrough script> [-h] [-L] [-T TARGET] [-s SERIAL_NUM] [-d DOMAIN] [-M] [-N] [-32] [-dest HLOS_ROOT_DEST] [-libdest HLOS_LIB_DEST]

    Optional arguments:
      -h, --help                Show the help message
      -L                        Build the executable for Linux Embedded HLOS (Default is Linux Android)
      -T TARGET                 Build the DSP library for TARGET: <sm8150, sm6150, qcs405, sm7150, sm6125, sm8250, lahaina, rennell, saipan, kamorta, bitra, agatti, qrb5165, qcs403, qcs610, cedros, mannar>
      -s SERIAL_NUM             Specify the adb serial number of device.
      -d DOMAIN                 Use the DSP subsystem DOMAIN: Options=<0/1/2/3> (0: ADSP, 3: CDSP, 2: SDSP, 1: MDSP). Domain value is optional. Default is 0 for agatti and 3 for other targets.
      -M                        Do not recompile Hexagon and Android variants
      -N                        Skip test signature installation
      -32                       Build executable for the 32-bit target
      -dest HLOS_ROOT_DEST      Override the default HLOS root directory <vendor, usr, data>. Default is /vendor for LA and /usr for LE targets.
      -libdest HLOS_LIB_DEST    Override the default directory for HLOS library <vendor/lib, usr/lib, data/lib> OR <vendor/lib64, usr/lib64, data/lib64>

One of the most useful options is -`N`: it skips the step of signing the device, which only needs to be performed on a newly flashed device.

Out of -`T`(TARGET) and -`s`(SERIAL_NUM), only one is mandatory.

### Default device paths used by example walkthrough scripts

For LA targets::

* HLOS exe destination: `/vendor/bin`
* HLOS lib destination: `/vendor/lib` (32 bit) OR `/vendor/lib64` (64 bit)
* DSP lib destination: `/vendor/lib/rfsa/dsp/sdk`

For LE targets::

* HLOS exe destination: `/usr/bin`
* HLOS lib destination: `/usr/lib` (32 bit) OR `/usr/lib64` (64 bit)
* DSP lib destination: `/usr/lib/rfsa/dsp/sdk`


### Usage examples

    python calculator_walkthrough.py -T sm8150 -32

The command above will:

* sign the `SM8150` device
* build the calculator 32-bit executable and libraries for the LA HLOS(Android)
* build the calculator Hexagon libraries
* run the example on the simulator as specified in the `hexagon.min` rules
* push the built binaries and run the example on target

The following configurations will be used:

* DSP used for offloading: cDSP
* HLOS exe destination: `/vendor/bin`
* HLOS lib destination: `/vendor/lib`
* cDSP lib destination: `/vendor/lib/rfsa/dsp/sdk`

        python calculator_walkthrough.py -T lahaina -d 0 -dest data

The command above will:

* sign the `Lahaina` device
* build the calculator 64-bit executable and libraries for the LA HLOS(Android)
* build the calculator Hexagon libraries
* run the example on the simulator as specified in the `hexagon.min` rules
* push the built binaries and run the example on target

The following configurations will be used:

* DSP used for offloading: aDSP
* HLOS exe destination: `/data/bin`
* HLOS lib destination: `/data/lib64`
* aDSP lib destination: `/data/lib/rfsa/dsp/sdk`


        python calculator_walkthrough.py -T qcs405 -L -N -dest data -libdest data/lib

The command above will:
* skip the device signing
* build the calculator 64-bit executable and libraries for the LE HLOS(UbuntuARM)
* build the calculator Hexagon libraries
* run the example on the simulator as specified in the `hexagon.min` rules
* push the built binaries and run the example on target

The following configurations will be used:

* DSP used for offloading: cDSP
* HLOS exe destination: `/data/bin`
* HLOS lib destination: `/data/lib`
* cDSP lib destination: `/data/lib/rfsa/dsp/sdk`

***NOTE:*** For building Hexagon libraries, the Hexagon architecture is determined based on the target_name or serial_number passed as arguments.

***NOTE:*** The option `-T` can be used to identify the required device if multiple devices are connected simultaneously. However, if multiple devices of the same type are connected, for example if there are two SM8250 devices connected simultaneously, then use the `-s` option to differentiate between devices based on the ADB serial numbers.

The ADB serial number of a device is a string created by ADB to uniquely identify the device by its port number.

    adb devices -l
	e76a39c6               device product:kona model:Kona_for_arm64 device:kona
	f68b78d7               device product:kona model:Kona_for_arm64 device:kona

    python calculator_walkthrough.py -s e76a39c6 -N

The command above will:

* skip the device signing
* build the calculator 64-bit executable and libraries for the LA HLOS(Android)
* build the calculator Hexagon libraries
* run the example on the simulator as specified in the `hexagon.min` rules
* push the built binaries and run the example on target

## List of examples

The following table lists all the examples that are included in the base SDK and the various features that they illustrate.

| Example | Features illustrated in the example |
|---|---|
| [asyncdspq_example](asyncdspq_example/README.md) | Demonstrate the usage of the asynchronous message queue |
| [calculator](calculator/README.md) | [FastRPC](../software/ipc/rpc.md)<br>[Building](../tools/build.md)<br>[Simulating](../tools/run.md)<br>[Target testing](../tools/run.md#run-applications-on-target)<br>[Multi-domain/multi-DSP support](../software/ipc/rpc.md#multi-domain)<br>[Unsigned PD](../software/system_integration.md#unsigned-pds) |
| [calculator_c++](calculator_c++/README.md) | C++ on the DSP|
| [calculator_c++_apk](calculator_c++_apk/README.md) | C++ on the application processor|
| [dspqueue](dspqueue/README.md) | [Asynchronous DSP Packet Queue](../software/ipc/dspqueue.md) |
| [gtest](gtest/README.md) | Demonstrate usage of the Google test framework |
| [HAP example](hap_example/README.md) | [HAP APIs](../software/system_libraries/index.md#dsp-side-apis)<br>[Remote DSP capabilities APIs](../doxygen/remote/index.html){target=_blank} |
| [LPI example](lpi_example/README.md) | LPI mode |
| [multithreading](multithreading/README.md) | [Multithreading using QuRT threads, barriers and mutexes](../software/os/os_support_dsp.md)<br>[Debugging](../tools/debug.md)<br>IDE |
| [profiling](profiling/README.md) | [CPU and DSP timers and cycle counters](../tools/profile.md#timers)<br>[Simulator profiling](../tools/profile.md#simulator-profiling)<br>[sysMon](../tools/profile.md#sysmon)<br>[Hexagon Trace Analyzer](../tools/profile.md#hexagon-trace-analyzer) |
| [qhl](qhl/README.md) | [QHL](../doxygen/qhl/index.html){target=_blank} (Qualcomm Hexagon Libraries) |
| [qhl_hvx](qhl_hvx/README.md) | [QHL HVX](../doxygen/qhl_hvx/index.html){target=_blank} (Qualcomm Hexagon Libraries for HVX)<br>[HVX](../index.md#hexagon-hvx-unit) |
| [qprintf](qprintf/README.md) | [qprintf](../doxygen/qprintf/index.html){target=_blank} (printf library extension for HVX and assembly) |
