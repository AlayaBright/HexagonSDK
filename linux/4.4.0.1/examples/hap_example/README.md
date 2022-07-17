# HAP example

##  Overview

The hap_example illustrates the usage of the [remote APIs](../../doxygen/remote/index.html){target=_blank} API and the following HAP APIs:

* [HAP_compute_res](../../doxygen/HAP_compute_res/index.html){target=_blank}
* [HAP_farf](../../doxygen/HAP_farf/index.html){target=_blank}
* [HAP_mem](../../doxygen/HAP_mem/index.html){target=_blank}
* [HAP_perf](../../doxygen/HAP_perf/index.html){target=_blank}
* [HAP_power](../../doxygen/HAP_power/index.html){target=_blank}
* [sysmon_cachelock](../../doxygen/sysmon_cachelock/index.html){target=_blank}
* [HAP_vtcm_mgr](../../doxygen/HAP_vtcm_mgr/index.html){target=_blank}

##  Project structure


* `Makefile`

  Root makefile that invokes variant-specific min files to either build the application processor source code or the Hexagon DSP source code.

* `android.min`, `hexagon.min`

  Contains the make.d directives used to build the application processor and Hexagon DSP source code.

* `inc/hap_example.idl`

  IDL interface that defines the hap_example API.

  This [IDL](../../reference/idl.md) file is compiled by the QAIC IDL compiler into the following files:

  * `hap_example.h`: C/C++ header file
  * `hap_example_stub.c`: Stub source that needs to be built for the HLOS (Android, etc...)
  * `hap_example_skel.c`: Skel source that needs to be built for the Hexagon DSP

* `src_app/`  : Source files for the HLOS executable that offloads the task to DSP

  * `hap_example_main.c` : Contains main() function of the hap_example that runs on application processor
  * `hap_example.c` :  Invokes call to DSP functions that demonstrate the HAP APIs
  * `hap_unit_test.c` : Invokes unit test on DSP that validates some of the HAP APIs

* `src_dsp/`: Source files for the Hexagon-side implementation of the hap_example that is compiled into a shared object. Demonstrates usage of the HAP APIs and contains a unit test.

  * `hap_example_imp.c` : Contains function to open and close handle to a cdsp domain

  * `hap_example_compute_res_imp.c` : [Compute resource manager framework](../../doxygen/HAP_compute_res/index.html){target=_blank} APIs to request and release compute resources on CDSP

  * `hap_example_farf_runtime.c` : [FARF API](../../doxygen/HAP_farf/index.html){target=_blank} to control compile-time and run-time logging

  * `hap_example_mem_imp.c` : [HAP_mem](../../doxygen/HAP_mem/index.html){target=_blank}  APIs to control mapping of buffers to user PD heap memory

  * `hap_example_perf_imp.c` : [HAP_perf](../../doxygen/HAP_perf/index.html){target=_blank} APIs to measure the execution time without RPC overhead

  * `hap_example_power_imp.c` : [HAP_power](../../doxygen/HAP_power/index.html){target=_blank} APIs to vote for DSP core, bus clock frequencies and voltage corners

  * `hap_example_sysmon_cachelock_imp.c` : [Cache locking manager](../../doxygen/sysmon_cachelock/index.html){target=_blank} to lock a section of CDSP L2 cache and release it

  * `hap_example_vtcm_mgr_imp.c` : [VTCM Manager](../../doxygen/HAP_vtcm_mgr/index.html){target=_blank} to request or release VTCM memory on CDSP

  * `hap_example_unit_test_imp.c` : Unit test that validates some of the HAP APIs


##  Building

###  Using the walkthrough script

* The example comes with a walkthrough script called `hap_example_walkthrough.py` which builds the code and runs on target.

* The script must be run using the following command :

      python hap_example_walkthrough.py -T <Target Name>

Please review the generic [setup](../README.md#setup) and [walkthrough_scripts](../README.md#walkthrough-scripts) instructions to learn more about setting up your device and using walkthrough scripts.



###  Using the make commands

* To build your code without using the walkthrough script, you will need to build both the Android and Hexagon modules.  To build the Android module along with the dependencies, run the following command:

        make android VERBOSE=1

* To build the Hexagon module, run the following command:

        make hexagon DSP_ARCH=v66 VERBOSE=1

For more information on the build syntax, please refer to the [building reference instructions](../../tools/build.md).

##  Running on Target

If you want to run your code on target without using the walkthrough script, please use the following steps:

* Use ADB as root and remount system read/write

        adb root
        adb wait-for-device
        adb remount

* Push the HLOS side hap_example executable and the supporting  stub library to the device

        adb shell mkdir -p /vendor/bin/
        adb push android_ReleaseG_aarch64/hap_example /vendor/bin/
        adb shell chmod 777 /vendor/bin/hap_example
        adb push android_ReleaseG_aarch64/ship/libhap_example.so /vendor/lib64/

* Push the Hexagon Shared Object to the device's file system

        adb shell mkdir -p /vendor/lib/rfsa/dsp/sdk
        adb push hexagon_ReleaseG_toolv84_v66/ship/libhap_example_skel.so /vendor/lib/rfsa/dsp/sdk

* Generate a device-specific test signature based on the device's serial number
  Follow the steps listed in the [Use signer.py](../../tools/sign.md#use-signerpy) section of the signing documentation.

  Note: This step only needs to be done once as the same test signature will enable loading any module.

* Redirect DSP FARF messages to ADB logcat by creating a farf file

        adb shell "echo 0x1f > /vendor/lib/rfsa/dsp/sdk/hap_example.farf"

    Please refer to the page on [messaging resources](../../tools/messaging.md) which discusses the tools available for logging debug messages from the DSP.

* Launch a new CLI shell to view the DSP's diagnostic messages using logcat

    Open a new shell or command window and execute :

        adb logcat -s adsprpc

* Execute the `hap_example` binary as follows :

        adb shell export LD_LIBRARY_PATH=/vendor/lib64/:$LD_LIBRARY_PATH DSP_LIBRARY_PATH="/vendor/lib/rfsa/dsp/sdk\;/vendor/lib/rfsa/dsp/testsig;" /vendor/bin/hap_example <0/1/2/3/4/5/6 select HAP API>

    The command-line argument must be an integer ranging from 0 to 5 and selects the HAP APIs to be demonstrated:

    * 0 - [HAP_compute_res.h](../../doxygen/HAP_compute_res/index.html){target=_blank}
    * 1 - [HAP_farf.h](../../doxygen/HAP_farf/index.html){target=_blank}
    * 2 - [HAP_mem.h](../../doxygen/HAP_mem/index.html){target=_blank}
    * 3 - [HAP_perf.h](../../doxygen/HAP_perf/index.html){target=_blank}
    * 4 - [HAP_power.h](../../doxygen/HAP_power/index.html){target=_blank}
    * 5 - [sysmon_cachelock.h](../../doxygen/sysmon_cachelock/index.html){target=_blank}
    * 6 - [HAP_vtcm_mgr.h](../../doxygen/HAP_vtcm_mgr/index.html){target=_blank}

* Analyze the output

    The command window or shell should contain messages returned by the application processor when using the `printf` command.

    For example when we run the example on an sm8250 target :

        adb shell /vendor/bin/hap_example 1

    demonstrates FARF run-time logging and the following output is printed in the shell:

        -----Retrieving VTCM information of CDSP using FastRPC Capability API-----

        Result of capability query for VTCM_PAGE on CDSP is 262144 bytes
        Result of capability query for VTCM_COUNT on CDSP is 1

        -----------------------HAP API Example--------------------------------

        Demonstrating FARF run-time logging

        hap_example function PASSED

        Please look at the mini-dm logs or the adb logcat logs for DSP output

        --------------------------HAP Unit Test-------------------------------

        hap_unit_test PASSED

        hap_example PASSED

    while the FARF logs redirected to the adb logcat are :

        HAP_utils.c:303:0x1620ea:13: Logging mask set to 31
        mod_table.c:541:0x12f0d3:13: open_mod_table_open_dynamic: Module libhap_example_skel.so opened with handle 0xa52093d0
        HAP_utils.c:303:0x12f0d3:13: Logging mask set to 0
        HAP_utils.c:329:0x12f0d3:13: Logging enabled for file hap_example_farf_runtime.c
        hap_example_farf_runtime.c:67:0x12f0d3:13: Low    FARF message : Run-time logging disabled
        hap_example_farf_runtime.c:68:0x12f0d3:13: Medium FARF message : Run-time logging disabled
        hap_example_farf_runtime.c:69:0x12f0d3:13: High   FARF message : Run-time logging disabled
        hap_example_farf_runtime.c:70:0x12f0d3:13: Error  FARF message : Run-time logging disabled
        hap_example_farf_runtime.c:71:0x12f0d3:13: Fatal  FARF message : Run-time logging disabled
        HAP_utils.c:303:0x12f0d3:13: Logging mask set to 31
        HAP_utils.c:329:0x12f0d3:13: Logging enabled for file hap_example_farf_runtime.c
        hap_example_farf_runtime.c:44:0x12f0d3:13: Low    FARF message : Run-time logging enabled
        hap_example_farf_runtime.c:45:0x12f0d3:13: Medium FARF message : Run-time logging enabled
        hap_example_farf_runtime.c:46:0x12f0d3:13: High   FARF message : Run-time logging enabled
        hap_example_farf_runtime.c:47:0x12f0d3:13: Error  FARF message : Run-time logging enabled
        hap_example_farf_runtime.c:48:0x12f0d3:13: Fatal  FARF message : Run-time logging enabled
        hap_example_farf_runtime.c:50:0x12f0d3:13: Runtime Low FARF message    - Run-time logging enabled
        hap_example_farf_runtime.c:51:0x12f0d3:13: Runtime Medium FARF message - Run-time logging enabled
        hap_example_farf_runtime.c:52:0x12f0d3:13: Runtime High FARF message   - Run-time logging enabled
        hap_example_farf_runtime.c:53:0x12f0d3:13: Runtime Error FARF message  - Run-time logging enabled
        hap_example_farf_runtime.c:54:0x12f0d3:13: Runtime Fatal FARF message  - Run-time logging enabled
        mod_table.c:719:0x12f0d3:13: open_mod_table_close: unloaded libhap_example_skel.so
        HAP_utils.c:303:0x1380dd:13: Logging mask set to 31
        mod_table.c:541:0x1500e4:13: open_mod_table_open_dynamic: Module libhap_example_skel.so opened with handle 0x4c3093d0
        hap_unit_test_imp.c:108:0x1500e4:13: HAP_compute_res_attr_set_vtcm_param : TEST PASSED
        hap_unit_test_imp.c:109:0x1500e4:13: HAP_compute_res_acquire             : TEST PASSED
        hap_unit_test_imp.c:148:0x1500e4:13: HAP_compute_res_release             : TEST PASSED
        hap_unit_test_imp.c:194:0x1500e4:13: HAP_mmap                            : TEST PASSED
        hap_unit_test_imp.c:231:0x1500e4:13: HAP_query_request_VTCM              : TEST PASSED
        hap_unit_test_imp.c:266:0x1500e4:13: HAP_query_release_VTCM              : TEST PASSED
        hap_unit_test_imp.c:304:0x1500e4:13: HAP_query_request_async_VTCM        : TEST PASSED
        hap_unit_test_imp.c:326:0x1500e4:13:
        hap_example_unit_test PASSED