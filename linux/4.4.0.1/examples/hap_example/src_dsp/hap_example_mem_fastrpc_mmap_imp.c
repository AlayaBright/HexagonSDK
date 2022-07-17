/**=============================================================================
Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

#include "hap_example.h"

#include "remote.h"

#include "HAP_farf.h"
#include "HAP_mem.h"

#include "AEEStdErr.h"

/* Weak reference needed as this API is suported only on Lahaina and Cedros */
uint64 HAP_mem_available_stack();
#pragma weak HAP_mem_available_stack

/*
 * HAP_mem.h APIs allow the user to manage the user PD heap memory and control the mapping of buffers into this memory
 */

AEEResult hap_example_mem_fastrpc_mmap(remote_handle64 h, int buffer_fd, uint32 buffer_length)
{

    int retval = 0;
    void *mmap_address;


    /*
     * HAP_mmap_get : To get the address associated with the buffer and increment its reference count
     */
    FARF(ALWAYS,"\nHAP_mmap_get:\n");

    uint64 phys_address;

    retval = HAP_mmap_get(buffer_fd, &mmap_address, &phys_address);

    if(retval!=AEE_SUCCESS)
    {
        FARF(ERROR,"Error Code 0x%x returned from function : HAP_mmap_get\n", retval);
        goto bail;
    }

    FARF(ALWAYS,"Incremented the reference count of buffer with buffer_fd = %d"
                "virtual address = %p and physical address = %llu\n", buffer_fd, mmap_address, phys_address);


    /* NOTE:
     *  If the buffer is used to share data with other cores, cache maintenance operations must be performed manually.
     *  This is done on the DSP using qurt_mem_cache_clean() API to invalidate or flush the cache.
     *  e.g.:
     *      1)Flush the caches for the buffer :
     *          retval = qurt_mem_cache_clean((qurt_addr_t)mmap_address, (qurt_size_t)buffer_len, QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE);
     *
     *      2)Invalidate the caches for the buffer :
     *          retval = qurt_mem_cache_clean((qurt_addr_t)mmap_address, (qurt_size_t)buffer_len, QURT_MEM_CACHE_INVALIDATE, QURT_MEM_DCACHE);
     */


    /*
     * HAP_mmap_put : Decrement the reference count of the file descriptor
     */
    FARF(ALWAYS,"\nHAP_mmap_put:\n");

    retval = HAP_mmap_put(buffer_fd);

    if(retval!=AEE_SUCCESS)
    {
        FARF(ERROR,"Error Code 0x%x returned from function : HAP_mmap_put\n", retval);
        goto bail;
    }

    FARF(ALWAYS,"Decremented the reference count of the buffer with buffer_fd = %d\n", buffer_fd);


bail:

    if(retval==AEE_SUCCESS)
    {
        FARF(ALWAYS,"\nhap_example_mem_fastrpc_mmap PASSED\n");
    }
    else
    {
        FARF(ALWAYS,"\nhap_example_mem_fastrpc_mmap FAILED\n");
    }

    return retval;
}
