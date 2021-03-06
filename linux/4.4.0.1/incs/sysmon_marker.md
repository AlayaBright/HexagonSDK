# sysMon marker

The sysMon marker API profiles a specific code region to study its own load on the processor compute resources and the bus bandwidth, and captures various other profiling metrics associated to that specific code region.
This approach is useful when measuring performance, debugging performance-related issues or identifying possible optimizations for a specific code region instead of profiling the entire application.

This API is not supported in unsigned PD and CPZ.

## Collect profiling data

Once the code has been instrumented with the sysMon marker APIs to enable and disable profiling of specific code regions, the [sysMonApp profiler](../../tools/sysmon_app.html#profiler-service)
must run to collect sysMon marker data.

If DCVS is enabled by the user, the decisions taken by DCVS with profiling markers enabled might not be the same as without markers.

## Parsing profiling data with sysMon marker

Profiling data captured using the [sysMonApp profiler](../../tools/sysmon_app.html#profiler-service) can be parsed using the [sysMon parser](../../tools/sysmon_parser.html).
Refer [STID and markers data](../../tools/sysmon_parser.html#stid-and-markers-data) section for the output files generated by the sysMon parser when sysMon markers are enabled.

## Limitations

* Nested markers are not supported

    For example, the following piece of code does not produce the expected profiling data; its behavior is undefined.

        HP_profile(10, 1);
        // ... user code ...
        HP_profile(11, 1);
        // ... user code ...
        HP_profile(11, 0);
        // ... user code ...
        HP_profile(10, 0);

* Enabling profiling markers forces collection of profiling data on all hardware threads.

    Profiling statistics are collected for any entity running in parallel with the piece of code where markers are defined.
