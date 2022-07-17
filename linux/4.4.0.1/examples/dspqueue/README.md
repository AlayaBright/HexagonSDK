# Asynchronous DSP packet queue example

##  Overview

The `dspqueue` example illustrates using the
[Asynchronous DSP packet queue](../../software/ipc/dspqueue.md) for
communication between the host CPU and the cDSP.

The API and the example are supported on Android target only, for
Lahaina and later products. See the [feature
matrix](../../reference/feature_matrix.md) for details on feature
support.


## Building and running the example

The `dspqueue` example includes a walkthrough script called
`dspqueue_walkthrough.py`. Please review the generic
[setup](../README.md#setup) and
[walkthrough_scripts](../README.md#walkthrough-scripts) instructions
to learn more about setting up your device and using walkthrough
scripts. Walkthrough script automates building and the example as
discussed in this section.

Without the walkthrough script, the example can be built like other
SDK examples, using `make` commands such as these:

    make android BUILD=Debug
    make hexagon BUILD=Debug DSP_ARCH=v68

For more information on the build syntax, please refer to the [building reference instructions](../../tools/build.md).

The packet queue API is only supported on target, so the example
cannot be run on the simulator. To run the example on a target, push
the generated binaries to the device:

    adb push android_Debug_aarch64/ship/dspqueue_sample /vendor/bin/
    adb shell chmod 777 /vendor/bin/dspqueue_sample
    adb push hexagon_Debug_toolv84_v68/ship/libdspqueue_sample_skel.so /vendor/lib/rfsa/adsp/

After this simply execute `dspqueue_sample` on the target:

    adb shell dspqueue_sample

Note that by default the example runs in an [unsigned
PD](../../software/system_integration.md#unsigned-pds).


## Example application details

The example application contains two separate test scenarios. The
following sections discuss these and any common elements in detail.


### Echo Test {#example-echo}

The echo test illustrates a basic `dspqueue` scenario: A host CPU
application sends requests to the DSP, which responds back by echoing
the same message. The test is driven by `echo_test()` in
`src/dspqueue_sample.c`. This section discusses the key steps in
detail.

~~~{.c}
dspqueue_create(DSPQUEUE_CDSP, 
                0, // Flags
                256, // Request queue size
                256, // Response queue size
                packet_callback,
                error_callback_fatal,
                (void*)c,  // Callback context
                &queue);
~~~

This call creates a new queue. This example uses a relatively small
queue (256 bytes for both requests and responses); for use cases with
larger messages a larger queue can yield better performance. The two
callback functions are used for handling responses from the DSP
(`packet_callback()`) and error handling (`error_callback_fatal()`). In
this example the error callback simply terminates the process with an
error.

~~~{.c}
dspqueue_export(queue, &dsp_queue_id);
dspqueue_sample_start(sample_handle, dsp_queue_id);
~~~

These calls export the queue for use on the DSP, and pass the queue ID
to the DSP side of the test application. The DSP side is implemented
in `src/dspqueue_sample_imp.c`, with the IDL interface defined in
`inc/dspqueue_sample.idl` as is typical for Hexagon SDK
applications. The example uses a regular synchronous FastRPC call to
pass the queue ID to the DSP and start the use case - this is common
for packet queue applications.

The DSP side implementation uses the queue ID to create a local handle
to the same queue:

~~~{.c}
dspqueue_import(dsp_queue_id, // Queue ID from dspqueue_export
                sample_packet_callback, // Packet callback
                sample_error_callback, // Error callback; no errors expected on the DSP
                (void*)c, // Callback context
                &c->queue);
~~~

The DSP implementation has its own set of callback functions. All
requests are processed in the packet callback
(`sample_packet_fallback()`). The current implementation does not use
a DSP-side error callback, but it is included for completeness.

After the queue has been created and connected, the host CPU sends a
sequence of requests to the DSP:


~~~{.c}
dspqueue_write(queue,
               0, // Flags
               0, NULL, // No buffer references in this packet
               2 * sizeof(uint32_t), (const uint8_t*)msg, // Message
               1000000); // Timeout
~~~

The packets consist of two 32-bit words, with no buffer
references. The call uses a one-second timeout to catch error
situations where the DSP may have stopped responding; alternatively
clients can use `DSPQUEUE_TIMEOUT_NONE`.

On the DSP side incoming request packets are handled in the packet
callback. The callback function structure is typical of most `dspqueue`
clients:

~~~{.c}
while ( 1 ) {
    //...
    err = dspqueue_read_noblock(queue,
                                &flags,
                                SAMPLE_MAX_PACKET_BUFFERS, // Maximum number of buffer references
                                &num_bufs, // Number of buffer references
                                bufs, // Buffer references
                                sizeof(msg), // Max message length
                                &msg_length, // Message length
                                (uint8_t*)msg); // Message
    if ( err == AEE_EWOULDBLOCK ) {
        return;
    }
    switch ( msg[0] ) {
        case SAMPLE_MSG_ECHO:
            resp_msg[0] = SAMPLE_MSG_ECHO_RESP;
            resp_msg[1] = msg[1];
            dspqueue_write(queue,
                           0, // Flags
                           0, NULL, // No buffers
                           8, (const uint8_t*)resp_msg, // Message
                           DSPQUEUE_TIMEOUT_NONE);
	    break;
            //...
    }
}
~~~

The callback repeatedly reads packets from the queue until it is empty
(`AEE_EWOULDBLOCK`). This ensures all packets are consumed - the
client does not necessarily get a separate callback for each packet,
and multiple new packets can arrive while a previous callback is being
handled.

In this example each received packet is handled directly in the
callback function; for the echo test the DSP simply sends the same
payload word back to the CPU. Real-world clients would likely start
processing in background worker threads, and eventually send a
response back to the CPU once all work is complete.

The host CPU receives response packets in a packet callback function
virtually identical to the DSP one discussed above.

Finally at the end of the test the queue is closed first on the DSP
followed by the host CPU:

~~~{.c}
dspqueue_sample_stop(sample_handle);
dspqueue_close(queue);
~~~


### Data Processing Test {#example-processing}

The second test in the `dspqueue` example illustrates using a packet
queue to send data processing requests to the DSP. Its structure is
very similar to the [echo test](#example-echo), so this section only
discusses the differences between the two.

The test uses a set of shared memory buffers to send input data and
receive output from the DSP. The buffers are allocated and mapped to
the DSP before processing can start:

~~~{.c}
buffers[i] = rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS,
                          BUFFER_SIZE);
fds[i] = rpcmem_to_fd(buffers[i]);
fastrpc_mmap(CDSP_DOMAIN_ID, fds[i], buffers[i], 0, BUFFER_SIZE, FASTRPC_MAP_FD);
~~~

`rpcmem_alloc()` allocates a shareable ION buffer using the [RPCMEM
library](../../doxygen/rpcmem/index.html), `rpcmem_to_fd()` retrieves its corresponding File Descriptor
number, and finally `fastrpc_mmap()` maps it to the DSP. In all calls
`BUFFER_SIZE` is the size of the buffer in bytes.

Queue creation is similar to the echo test, except the processing test
uses a larger queue to account for larger packets with buffer
references:

~~~{.c}
dspqueue_create(DSPQUEUE_CDSP,
                0, // Flags
                4096, // Request queue size
                4096, // Response queue size
                packet_callback,
                error_callback_fatal,
                (void*)c,  // Callback context
                &queue);
~~~

Each request packet sent to the DSP includes two buffer references:
One for input data, and one to hold output from the DSP:

~~~{.c}
struct dspqueue_buffer bufs[2];
memset(bufs, 0, sizeof(bufs));
bufs[0].fd = fds[input_buf];
bufs[0].flags = (DSPQUEUE_BUFFER_FLAG_REF | // Take a reference
                 DSPQUEUE_BUFFER_FLAG_FLUSH_SENDER | // Flush CPU
                 DSPQUEUE_BUFFER_FLAG_INVALIDATE_RECIPIENT); // Invalidate DSP
bufs[1].fd = fds[output_buf];
bufs[1].flags = (DSPQUEUE_BUFFER_FLAG_REF |
                 DSPQUEUE_BUFFER_FLAG_FLUSH_SENDER);
dspqueue_write(queue,
               0, // Flags - the framework will update this
               2, bufs, // Buffer references
               sizeof(msg), (const uint8_t*)msg, // Message
               1000000); // Timeout
~~~

On the host CPU side the example requests the framework to take
references to both buffers, flush the buffers on the CPU side to
ensure all input data is visible to other devices in the system and no dirty data remains in the caches for the output buffer, and
invalidate it on the DSP to ensure it sees an up to date version in
memory. The DSP will trigger cache maintenance operations on the
output buffer as a part of its response packet.

On the DSP side the packet callback function
(`sample_packet_callback()`) retrieves buffer information from the
incoming packet, runs the processing algorithm, and constructs a
response:

~~~{.c}
case SAMPLE_MSG_BYTE_SQUARE:
    len = bufs[0].size;
    byte_square(bufs[0].ptr, bufs[1].ptr, len);
    resp_msg[0] = SAMPLE_MSG_BYTE_SQUARE_RESP;
    memset(resp_bufs, 0, sizeof(resp_bufs));
    resp_bufs[0].fd = bufs[0].fd;
    resp_bufs[0].flags = DSPQUEUE_BUFFER_FLAG_DEREF; // Release reference
    // (If we had written to the input buffer, we'd also need to flush it)
    resp_bufs[1].fd = bufs[1].fd;
    resp_bufs[1].flags = (DSPQUEUE_BUFFER_FLAG_DEREF | // Release reference
                          DSPQUEUE_BUFFER_FLAG_FLUSH_SENDER | // Flush DSP
                          DSPQUEUE_BUFFER_FLAG_INVALIDATE_RECIPIENT); // Invalidate CPU
    dspqueue_write(queue,
                   0, // Flags
                   2, resp_bufs, // Buffer references
                   4, (const uint8_t*)resp_msg, // Message
                   DSPQUEUE_TIMEOUT_NONE);
~~~

Note that the response packet also includes a reference to the input
buffer to release the reference taken in the request packet. The
response also triggers cache maintenance operations for the output
buffer to ensure the results are visible to the host CPU.

At the end of the test the processing test unmaps and frees buffers.

Finally the processing test also illustrates how to use early wakeup
packets to reduce latency between the DSP and host CPU. See
[Early Wakeup](../../software/ipc/dspqueue.md#early-wakeup) for more discussion on how and
when to use early wakeup packets.


### Performance Measurements

The `dspqueue` example application performs some performance
measurements to illustrate when using the packet queue can be more
efficient than regular synchronous FastRPC calls. For each scenario
the application measures and prints three sets of numbers for each
configuration:

* Total elapsed time
* DSP processing time: The time taken on the DSP to run the processing
  algorithm
* Overhead: The difference between DSP processing time and total
  elapsed time, divided by the number of operations. This accounts for
  inter-processor communication, cache maintenance operations, and
  other overhead from the framework.

The exact values seen will vary on depending on the target device used
for testing, but generally the following results should be visible:

* For individual operations without early wakeup the packet queue is
  not more efficient than synchronous FastRPC calls
* Early wakeup can significantly reduce the latency for single-shot
  operations
* The packet queue can have significantly lower overhead than
  synchronous FastRPC calls when the application can queue a larger
  number of requests.

Additionally, the example also illustrates using persistently mapped
buffers to reduce overhead for regular synchronous FastRPC calls.
