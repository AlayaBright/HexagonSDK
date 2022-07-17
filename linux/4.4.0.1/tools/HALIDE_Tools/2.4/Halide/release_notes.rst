===================================
Release notes for Version |release|
===================================

Following are the changes made in this release (Qualcomm\ |reg| Halide
|release|) in comparison to the previous major release (Qualcomm\ |reg|
Halide 2.3.0).

-----------------------------
Vector floating point support
-----------------------------

Halide provides support for HVX floating point on the Hexagon DSP for both
``float16_t`` half-precision (16-bit) and ``float_t`` single precision (32-bit)
data types as well as conversion between floating point and integer data types.
In the previous release, Qualcomm |reg| Halide 2.3.0,
vectorized floating point pipelines had to completely operate in
floating point and operate in only one selected precision (no conversions).
Floating point vector support is only available on HVX ISA v68 or later.

To enable this feature, add both of the Halide target features, ``hvx`` and
``hvx_v68``, to the target when running your generator.

The following functions support both 16- and 32-bit floating point vectors:
``sin(x)``,  ``cos(x)``,  ``tan(x)``, ``asin(x)``, ``acos(x)``, ``atan(x)``,
``sinh(x)``, ``cosh(x)``, ``tanh(x)``,
``exp(x)``, ``log(x)``, ``pow(x,y)``, ``sqrt(x)``, ``fast_inverse_sqrt(x)``,
``hypot(x,y)``, ``floor(x)``, ``ceil(x)``, ``trunc(x)``, ``round(x)``,
``is_inf(x)``, ``is_nan(x)``, ``is_finite(x)``

The following functions support only 32-bit floating point vectors:
``fast_exp(x)``, ``fast_log(x)``

Using other functions will result in scalar code generation.
Support for more functions will be added in a future release.

To get a complete list of supported functions, please see
:ref:`VectorFloatingPoint`.

---------------------------------------------------------------
``rotate_native`` Halide intrinsic for HVX ``vror`` instruction
---------------------------------------------------------------

In this release, we have added a new function called ``rotate_native`` allowing
users to generate vror instruction for HVX.

---------------------
Gather for 8-bit data
---------------------

In this release, we have added the ability to generate ``vgather`` instruction
when trying to gather 8-bit data. Users no longer need to upcast data to 16-bit
to use ``vgather`` instructions.

----------------------------------
Deprecation of 64-byte HVX support
----------------------------------

This release removes support for 64 byte HVX vectors. ``hvx_64`` is no
longer a valid target feature and the native vector size for HVX is now
assumed to always be 128 bytes. Consequently, we have introduced a new
target feature ``hvx`` that can be used interchangeably with ``hvx_128``.

----------------------------------------------
User specified offloading while Autoscheduling
----------------------------------------------

This release adds the ability to let the user specify which stage of a pipeline
should be offloaded to Hexagon by the Autoscheduler. Prior to this release,
in case of offload mode, the Autoscheduler generated a schedule such that
execution of the entire pipeline is offloaded to Hexagon.

-------------------------------------
Deprecation of hvx_v65 Target feature
-------------------------------------

``hvx_v65`` is no longer a valid target feature and the default ISA is now
assumed to always be HVX v65. Using any of target features ``hvx`` &``hvx_128``
now will instruct the Halide compiler to generate code for HVX v65 ISA.

--------------------------
Autotuning on Windows host
--------------------------

In this release, the autotuner shell script has been replaced by a Python based
autotuner, which adds the ability to autotune on a Windows machine. In the previous
release, autotuning was only supported on Linux based systems. Minimum Python version
required for autotuning is 3.3.

-----------------------------------------------------
Autoscheduling/Autotuning support for standalone mode
-----------------------------------------------------

In this release, we have added the ability to autotune pipelines in standalone mode.
This release also includes support in the Autoscheduler for emitting the ``prefetch``
scheduling directives in standalone mode.

--------------------------
DMA-BUF heap for Android-S
--------------------------

In this release, we have added support for the new Android-S DMA-BUF heap. DMA-BUF
heap replaces the older ION heap mechanism. To be able to run Halide apps on newer
Android-S devices, the latest ``libhalide_hexagon_host.so`` (for offload mode)
or ``ion_allocation.cpp`` in ``Halide/Examples/standalone/device/utils/ion`` (for
standalone mode) must be used. ION heap is still supported when run on older
devices.

-------------------
System Requirements
-------------------

Minimum system requirements are as follows

- Clang 7.0 with libc++ as the native (x86) compiler and C++ runtime library
  respectively.
- Ubuntu 16.04
- SDK 4.2.0.2

.. note::

    SDK 4.2.0.2 contains an updated QAIC IDL compiler that includes important
    security fixes. Note that while these fixes are recommended for all devices, on
    SM8450 and newer devices the QAIC generated skel shared objects will also be
    version checked when run. On SM8450 and newer devices, only skel objects that
    have been built with all the QAIC security fixes are allowed to run on the DSP.
