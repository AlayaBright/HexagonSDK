# DSP Worker Pool

The DSP worker pool available in $HEXAGON_SDK_ROOT/libs/worker_pool offers thread worker pool and synchronization APIs.

## Usage of worker pool library

The following steps illustrate how to use the DSP worker pool:

* Partition your workload into callback functions that can be executed in parallel.
* During run-time initialization, initialize a worker pool instance and obtain the handle.
* Submit a multi-threaded job by passing the worker pool your callback function pointers and associated data context pointers.
* Wait on a token for all of the submitted jobs to complete.
* Repeat job submissions and waiting on tokens as many times as needed until ready to tear down the worker pool.
* De-initialize the worker pool to free its resources.

Example usage of worker pool library can be found in the benchmark example from the SDK compute addon: $HEXAGON_SDK_ROOT/addons/compute/examples/benchmark.