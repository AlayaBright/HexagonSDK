# Memory Management

The RPCMEM APIs provide a set of APIs to allocate memory from the Android side with the intent of sharing it with the DSP via FastRPC calls. Different types of memory and caching options are available depending on the needs of the application.

The advantage of using the RPCMEM APIs over the Android ION memory allocator is that it both allocates the memory and registers the buffers to the FastRPC framework.

Applications compiled for targets prior to Lahaina (SM8150 and SM8250) can use rpcmem_alloc and rpcmem_free APIs from libadsprpc.so, libcdsprpc.so or libsdsprpc.so, which are present in the vendor and system partitions of the Android image. However, if they are also calling rpcmem_init or rpcmem_deinit APIs, they need to link to the rpcmem.a static library.
