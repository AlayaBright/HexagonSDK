# Qprintf example

This example illustrates how to use the [Qualcomm printf library (qprintf)](../../doxygen/qprintf/index.html){target=_blank} located under `$HEXAGON_SDK_ROOT/libs/qprintf`.

## Instructions

All step-by-step instructions for building and running the test both on simulator and on target are captured in the qprintf_walkthrough.py script.

You may run the script directly.  To do so, simply run 'python qprintf_walkthrough.py'. To see the messages sent to the DSP run 'adb wait-for-device logcat -s adsprpc' in a new window.

The example demonstrates how the library displays the contents of scalar and vector registers from assembly and provides various options for displaying HVX_Vector contents from C.