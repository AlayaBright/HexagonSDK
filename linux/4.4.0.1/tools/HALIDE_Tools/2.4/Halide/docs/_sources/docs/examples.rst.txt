
.. _HalideExamples:

****************
Halide examples
****************

The Halide directory provides some sample pipelines that demonstrate the use of
Halide.

The examples are structured in the following manner

::

   Halide/
        Examples/
        ├── include
        │   └── test_report.h
        ├── Makefile
        ├── offload
        │   ├── apps
        │   ├── Makefile
        │   └── Makefile.common
        ├── setup-env.sh
        ├── standalone
        │   ├── device
        │   ├── Makefile
        │   └── simulator
        └── unsetup-env.sh

``Halide/Examples/offload`` has pipelines that demonstrate the use of Device
and Simulator Offload modes.

``Halide/Examples/standalone`` has pipelines that demonstrate the use of Device
and Simulator Standalone modes.

The following sections provide steps for running examples on Linux. For Windows usage, see ``Halide/Examples.txt``.

.. _ExamplesSetUp:

=======
Setup
=======

First, set up the environment for the examples by editing
``Halide/Examples/setup-env.sh``. In particular, set up the path to
``HEXAGON_SDK_ROOT``, and then set up the environment as follows:

.. code-block:: shell

    $> source setup-env.sh

This process sets up a variable called ``RUN_DEVICE_ID`` in your environment.
This variable is a list of the DEVICE IDs of all the devices connected to the
host. All device-based examples (Device Standalone and Device Offload modes)
will now run on all the devices in ``RUN_DEVICE_ID``.

To limit the list to a subset of all the devices connected to your machine,
reset ``RUN_DEVICE_ID`` to the DEVICE ID of a specific device.

To unset the environment that was set up via ``setup-env.sh``, use
``unsetup-env.sh``.


--------------
Device setup
--------------

To run examples in Device Standalone or Device Offload mode, set up Halide
runtime libraries on all the devices in ``RUN_DEVICE_ID``. Run the following
commands:

.. code-block:: shell-sm

    $> adb push $HALIDE_ROOT/lib/arm-32-android/libhalide_hexagon_host.so /system/lib/
    $> adb push $HALIDE_ROOT/lib/arm-64-android/libhalide_hexagon_host.so /system/lib64/
    $> adb shell mkdir -p /system/lib/rfsa/adsp/
    $> adb push $HALIDE_ROOT/lib/v65/libhalide_hexagon_remote_skel.so /system/lib/rfsa/adsp/

    $> adb push $HALIDE_ROOT/lib/arm-32-android/libhalide_hexagon_host.so /vendor/lib/
    $> adb push $HALIDE_ROOT/lib/arm-64-android/libhalide_hexagon_host.so /vendor/lib64/
    $> adb shell mkdir -p /vendor/lib/rfsa/adsp/
    $> adb push $HALIDE_ROOT/lib/v65/libhalide_hexagon_remote_skel.so /vendor/lib/rfsa/adsp/

    $> sysroot=$HEXAGON_SDK_ROOT/tools/android-ndk-r19c/toolchains/llvm/prebuilt/linux-x86_64/sysroot
    $> adb push $sysroot/usr/lib/arm-linux-androideabi/libc++_shared.so /system/lib/
    $> adb push $sysroot/usr/lib/aarch64-linux-android/libc++_shared.so /system/lib64/
    $> adb push $sysroot/usr/lib/arm-linux-androideabi/libc++_shared.so /vendor/lib/
    $> adb push $sysroot/usr/lib/aarch64-linux-android/libc++_shared.so /vendor/lib64/


================
Run examples
================

All examples can now be run from the command line by executing the following
command from the ``Examples`` directory.

.. code-block:: shell

    $> make all


-------------------------------------
Run only a single execution mode
-------------------------------------

Instead of running all the examples, you can run each execution mode
individually as follows:

.. code-block:: shell

    $>  make <mode>

In this case, ``mode`` can be ``sim-standalone``, ``sim-offload``,
``dev-standalone``, or ``dev-offload``.
