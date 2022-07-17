# HAP_mem APIs

## Overview
The HAP_mem APIs provide functionality available from the DSP to

* allocate and free memory - HAP_malloc() and HAP_free()
* map and unmap ION buffers allocated on the application processor and passed to the DSP using file descriptors - HAP_mmap() and HAP_munmap()
* get heap statistics and set properties - HAP_mem_get_stats(), HAP_mem_set_grow_size(), HAP_mmap_get(), HAP_mem_set_heap_thresholds() and HAP_mmap_put()

## Memory mapping

A common usage scenario for using the mapping functionality consists of the application processor allocating ION memory and passing the file descriptor to the DSP.
The DSP will then use the HAP_mem APIs to map the buffer onto the DSP and obtain a memory pointer. The mapping will remain valid until the buffer is being unmapped.
This approach allows to maintain a mapping across multiple FastRPC calls.

## Memory allocation

HAP_malloc and HAP_free are simple wrappers around the DSP malloc and free functions.
If a user memory allocation request cannot be fulfilled with the existing DSP heap, the FastRPC
runtime will attempt to grow the DSP heap by reserving additional memory from the HLOS.

The HAP_set_grow_size API can be called to configure the minimum and maximum size that should be added to the DSP heap when one of these growth events occurs.
If many growth events are anticipated, it may be appropriate to set a larger growth rate to reduce the number of growth events.  However, increasing
the heap more than necessary will impact HLOS performance.  Therefore, care must be taken in finding the appropriate growth rate for a given application.

Here is how the min and max values set by the HAP_set_grow_size control the growth of the heap:

    min_grow_bytes = MIN(max,MAX(min,min_grow_bytes));

    // The value will be aligned to the next 1MB boundary.

    actual_grow_bytes = min_grow_bytes + request_size
    actual_grow_bytes = ALIGN(actual_grow_bytes,0x100000)

## Memory statistics

HAP_mem_get_stats is useful when called at the beginning and end of an application to check for any memory leaks.
