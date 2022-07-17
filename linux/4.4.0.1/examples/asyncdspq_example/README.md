# Asyncdspq example

The asyncdspq (asynchronous message queue) is a utility library for asynchronous communication between the application processor and the DSP. This example contains the following three test cases to demonstrate the usage of asyncdspq. All these examples run on target and not on simulator.

* [Basic messaging (queuetest)](#basic-messaging)
* [Image processing queue (fcvqueue)](#image-processing-queue)
* [Queue performance (queueperf)](#queue-performance)

***Note:*** This example and the asyncdspq library itself will be deprecated when its replacement, the ***asynchronous packet queue***, will be available in the next SDK release.

## Basic messaging

The ***queuetest*** example application contains a number of basic message queue tests and examples. In all cases the application processor sends two integers to the DSP, the DSP adds them together, and delivers a result back.

The following tests are located in the function `test_asyncdspq()`.

### Simple Messages

The first two tests send a single message from the application processor to the DSP and vice versa. The basic sequence is as follows:

* The application processor creates a queue with `asyncdspq_create()`.
* For read tests the CPU writes a message to the queue with `asyncdspq_write()`.
* The CPU passes the queue attach handle from `asyncdspq_create()` to the DSP over a regular FastRPC call.
* The DSP attaches to the queue with `asyncdspq_attach()`.
* The DSP either reads the message from the queue or writes one.
* The DSP detaches from the queue with `asyncdspq_detach()`.
* The application processor verifies the result.
* The application processor destroys the queue with `asyncdspq_destroy()`.

Note that both tests use a single queue and use other mechanisms to verify the response. This is a valid configuration and can be useful for clients that mostly only send messages in one direction.

The third test simply sends multiple messages from the DSP to the application processor.

### Tests with multiple queues

The following tests in `test_asyncdspq()` send messages between the CPU and the DSP using multiple queues, both using blocking read/write operations and using callbacks.

### Canceling operations

The tests in `test_asyncdspq_cancel()` test canceling synchronous read/write operations under different circumstances. Many clients may not need to use `asyncdspq_cancel()` if they have another mechanism to ensure queues are empty before destroying them; typically this is done by sending an explicit “close” message and waiting for a response.

### Threading

`test_asyncdspq_threads()` tests different threading scenarios. The message queue library is designed to be thread-safe for read and write operations, but queues should be created and destroyed from the same thread. Most clients will use a single thread for reading and writing the queue, either from their own thread or callbacks from the queue library.

## Image processing queue

The ***fcvqueue*** example implements a queue of image processing operations using FastCV, which is a available in the compute add-on. It illustrates a more complex message queue use case, and also shows how to handle buffer allocation, memory mapping, and cache management. The threading and callback model is typical for most message queue clients: fcvqueue uses blocking writes and non-blocking reads with message callbacks.

The fcvqueue example is split between three main files:

* `fcvqueue.c`: CPU-side implementation. The rest of this section refers primarily to functions in this file.
* `fcvqueue_dsp_imp.c`: DSP-side implementation for the example. Contains FastRPC methods called from fcvqueue.c and the message queue client code.
* `fcvqueuetest.c`: Test application. Start here to follow the calling sequences.

This section briefly describes the key parts of the example. For more details refer to the source code, starting with the test application.

Note that fcvqueue is not thread-safe, and while the code checks for errors codes from all the functions it calls it does not attempt to recover from errors. The queue library generally does not return recoverable errors except where separately noted; errors result from either programming errors or running out of system resources such as memory.

### Initialization and uninitialization

`fcvqueue_create()` allocates internal structures and creates two queues--one for requests and one for responses--and calls the DSP-side implementation to attach to them.

`fcvqueue_destroy()` ensures the queues are empty, cancels any operations on the DSP side, and detaches and destroys queues.

### Memory allocation and de-allocation

`fcvqueue_alloc_buffer()` allocates shareable ION buffers using the rpcmem library and sends them to the DSP to be mapped. FastRPC handles mapping the buffer to the DSP's SMMU and internal MMU; as long as the DSP code keeps a mapping open (with `HAP_mmap()`) the buffers won't be unmapped.

Subsequent operations on the buffer refer to it by its file descriptor (FD). The DSP-side code maintains a mapping of FDs to buffer addresses. This avoids having to map/unmap buffers for each operation, but also requires explicit cache maintenance operations.

`fcvqueue_free_buffer()` is the reverse of allocation: It unmaps buffers from the DSP, releasing FastRPC MMU/SMMU mappings, and deallocates the memory.

### Synchronization

Most fcvqueue operations apart from initialization and memory allocation are asynchronous. The client queues requests which are processed by the DSP asynchronously without immediate feedback. To determine that processing has reached a specific point the client queue a synchronization request.

`fcvqueue_enqueue_sync()` sends a synchronization request to the DSP. The DSP simply responds to the request with the same token; when the application processor receives the response it calls a callback function. This informs the client that all operations through the synchronization request have been completed.

`fcvqueue_sync()` is a synchronous variant of the same request. It simply calls `fcvqueue_enqueue_sync()` and waits for a callback before returning. Once `fcvqueue_sync()` returns both the request and response queues are empty.

### Cache maintenance

In addition to memory mapping, FastRPC also handles cache maintenance on its clients' behalf. Message queue clients must handle this explicitly, which can be more complex but also gives the client an opportunity to determine when to manage the cache.

The fcvqueue example exposes cache maintenance through two operations: `fcvqueue_enqueue_buffer_in()` and `fcvqueue_enqueue_buffer_out()`, corresponding to cache invalidate and flush. After a buffer is allocated it is considered to be in application processor ownership, and the CPU can place input data in the buffer.

Before the DSP can use the buffer it must be transferred to DSP ownership with `fcvqueue_enqueue_buffer_in()`. This invalidates the buffer on the DSP side to ensure there is no stale data in DSP caches. While the buffer is in DSP ownership the application processor must not access it.

To make a buffer and its contents available to the application processor again the client must call `fcvqueue_enqueue_buffer_out()`. This flushes DSP caches to ensure all data is written back to memory and is available to the CPU. Since this is an asynchronous operation the client must synchronize with the queue before accessing the buffer.

Note that the fcvqueue implementation does not perform cache maintenance operations on the application processor side, but rather assumes buffers are cache coherent. SDM845 supports one-way cache coherency with the Compute DSP and buffers are allocated as coherent by default; to use non-coherent buffers the client must perform cache maintenance on the application processor too.

### Image processing operations

fcvqueue exposes four simple image processing operations: Copy, 3x3 Dilate, 3x3 Gaussian Blur, and 3x3 Median Filter. The copy operation is implemented as a simple `memcpy()`, while the other operations use the FastCV library.

`fcvqueue_enqueue_op()` enqueues an image processing operation. Both input and output buffers must be in DSP ownership, i.e. the client must first request cache invalidation with `fcvqueue_enqueue_buffer_in()`. Since fcvqueue does not automatically perform cache maintenance the output will not be available to the application processor before the client calls `fcvqueue_enqueue_buffer_out()` and waits for its completion.

There is no need to perform cache maintenance operations on temporary buffers - if the output of one operation is just used as the input of another one on the DSP the buffers can stay local to the DSP. This is a common pattern for many use cases including chains of image processing filters or per-layer neural network processing.

### Basic Usage Sequence

A basic fcvqueue sequence consists of the following steps:

1. Initialize: `fcvqueue_create()`. See [Initialization and Uninitialization](#initialization-and-uninitialization)
2. Allocate and map buffers: `fcvqueue_alloc_buffer()`, `fcvqueue_buffer_ptr()`. See [Memory Allocation and Deallocation](#memory-allocation-and-de-allocation)
3. Place input data in input buffers
4. Cache maintenance; move buffers to DSP ownership: `fcvqueue_enqueue_buffer_in()`. See [Cache Maintenance](#cache-maintenance)
5. Use the buffers for image processing operations: `fcvqueue_enqueue_op()`. See [Image Processing Operations](#image-processing-operations)
6. Cache maintenance to get output data back to the CPU: `fcvqueue_enqueue_buffer_out()`. See [Cache Maintenance](#cache-maintenance)
7. Use output data
8. De-initialization

`fcvqueuetest.c` illustrates this approach.

### Performance

`fcvqueuetest.c` contains a couple of simple performance benchmarks. They help illustrate when using the asynchronous message queue is beneficial over simple FastRPC calls: A single synchronous operation (dilate) is as fast as a FastRPC call, but the queue shows benefit when the client can enqueue multiple operations before synchronizing.

## Queue performance

The ***queueperf*** example illustrates how message queue performance is affected by different queue depths and message sizes. Communication overhead grows when the queue can hold fewer messages; at the worst case when the queue only fits one message communication becomes synchronous and there is no benefit over using regular FastRPC calls.