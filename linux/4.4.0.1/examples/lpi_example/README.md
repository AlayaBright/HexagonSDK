# LPI example

This example illustrates how to generate a shared object that can be loaded in LPI mode and non-LPI mode.

This example should only be used as a reference to create LPI shared objects and run them in audio or sensor PD, but not in dynamic PD.


The objective of the LPI mode is to achieve power optimizations for audio/sensors/voice use-cases running on the aDSP and sDSP.  In Low Power Island (LPI) mode, shared objects will be loaded into TCM (tightly coupled memory) instead of DDR because DDR access will be disabled.

LPI example is only supported on SM8250 and beyond.

## Instructions

All step-by-step instructions for building and loading the LPI shared object on target are captured in the `lpi_walkthrough.py' script. Please review the generic [setup](../README.md#setup) and [walkthrough_scripts](../README.md#walkthrough-scripts) instructions to learn more about setting up your device and using walkthrough scripts.

You can also execute each step manually as follows.

###Build

	make hexagon DSP_ARCH=v66 LPI=1


Shared objects that need to be loaded in LPI mode must be created with a separate note section containing text **uimg.dl.ver.2.0.0**.

This is accomplished by setting the `LPI` flag (`LPI=1`) as part of the make command building the DSP shared object.
Setting this flag triggers two actions in hexagon.min:

* Compile src/uimage_dl_v2.c, which defines the note section of the shared .so with the string (uimg.dl.ver.2.0.0).
* Invoke a custom linker script at link time, src/uimage_v2.lcs, which will add the new note section in the shared object.


Do not specify `LPI=1` flag to build the shared object for non-LPI mode.

### Push test-signature file

Follow the steps listed in the [Use signer.py](../../tools/sign.md#use-signerpy) section of the signing documentation to push test-signature file to device.

Note: This step only needs to be done once as the same test signature will enable loading any module.


### Push required files to device

    adb root
    adb remount
    adb push $(HEXAGON_SDK_ROOT)/libs/run_main_on_hexagon/ship/android_Debug_aarch64/run_main_on_hexagon /vendor/bin
    adb shell chmod 774 /vendor/bin/run_main_on_hexagon
    adb shell mkdir -p /vendor/lib/rfsa/dsp/sdk/
    adb push $(HEXAGON_SDK_ROOT)/libs/run_main_on_hexagon/ship/hexagon_Debug_dynamic_toolv83_v66/librun_main_on_hexagon_skel.so /vendor/lib/rfsa/dsp/sdk/
    adb push $(HEXAGON_SDK_ROOT)examples/lpi_example/hexagon_Debug_dynamic_toolv83_v66/ship/libLPI.so /vendor/lib/rfsa/dsp/sdk/

### Direct dsp messages to logcat

    adb shell "echo 0x1f > /vendor/lib/rfsa/dsp/sdk/run_main_on_hexagon.farf"

### Run example on device

    adb shell export DSP_LIBRARY_PATH="/vendor/lib/rfsa/dsp/sdk\;/vendor/lib/rfsa/dsp/testsig;" /vendor/bin/run_main_on_hexagon 0 libLPI.so

The above command loads `libLPI.so` in the dynamic PD on ADSP using `run_main_on_hexagon` executable.
The first argument of run_main_on_hexagon is domain id that specifies which hexagon DSP to use.
`0` in the above command makes run_main_on_hexagon to run on ADSP. Domain id for CDSP is `3`.

Fore more details on run_main_on_hexagon, please refer to the [Run application on Hexagon Simulator section](../../tools/run.md#run-applications-on-simulator)

All the above steps remain the same for loading a non-LPI shared object with the exception of the build command.

## Confirming usage of the LPI mode

Shared object that need to be loaded in LPI mode must be created with a separate note section containing text **uimg.dl.ver.2.0.0**.
You can confirm whether a shared object is compiled for LPI mode or not by using the command below.

	readelf --notes <shared_object>
If your output is similar to the excerpt below, then the specified shared object is compiled for LPI mode.

	Notes at offset 0x00001000 with length 0x00000030:
	  Owner                         Data size       Description
	  uimg.dl.ver.2.0.0             0x0000000c      Unknown note type: (0x00000000)


The GNU/LLVM linker generally uses a linker script that is compiled into the linker executable to control the memory layout of the ELF. It is possible to supply a custom [command file or linker script](https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_chapter/ld_3.html){target=_blank}, which in our case will add a new note section in the shared object.

`src/uimage_v2.lcs` is used as the linker script in this example and `src/uimage_dl_v2.c` is compiled as part of the shared object to put the constant string **uimg.dl.ver.2.0.0** in the new section created using `uimage_v2.lcs`.

FastRPC uses this note section **uimg.dl.ver.2.0.0** to decide whether to load in TCM (in LPI mode) or in DDR (in non-LPI mode).

After running this example, you can see the loaded address of the shared object as below in logcat by running `adb logcat -s adsprpc`

	adsprpc : so_source.c:31:0x9b:8: load address of shared object is 0x8bddf000

Below are the TCM ranges for SM8250 and Saipan

	Target 			     tcm_range

	Saipan (aDSP)  	    0x02C00000 - 0x02CD0000
	SM8250 (aDSP)       0x02C00000 - 0x02D80000

From the load address in logcat and above TCM ranges you can confirm whether the shared object is loaded in TCM or not.

*** Note ***: LPI shared objects should only be loaded in audio and sensors PDs. Loading LPI shared objects into dynamic PD is currently not supported.
