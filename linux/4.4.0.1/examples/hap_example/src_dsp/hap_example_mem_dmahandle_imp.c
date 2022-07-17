/**=============================================================================
Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

#include "hap_example.h"

#include "remote.h"

#include "HAP_farf.h"
#include "HAP_mem.h"

#include "AEEStdErr.h"

/* Weak reference needed as this API is suported only on Lahaina and later targets */
uint64 HAP_mem_available_stack();
#pragma weak HAP_mem_available_stack

/*
 * HAP_mem.h APIs allow the user to manage the user PD heap memory and control the mapping of buffers into this memory
 */

AEEResult hap_example_mem_dmahandle(remote_handle64 h, int buffer_fd, uint32 buffer_offset, uint32 buffer_len)
{

    int retval = 0;

    /*
     * HAP_mmap : To allocate memory for and map the buffer with the given file descriptor to Memory and initialize the reference count to 1
     *            Increment the reference count if already mapped
     */
    FARF(ALWAYS,"\nHAP_mmap:\n");

    void *mmap_address;
    int prot = HAP_PROT_READ | HAP_PROT_WRITE;
    int flags = 0, avail_stack = 0;

    mmap_address = HAP_mmap(NULL, buffer_len, prot, flags, buffer_fd, buffer_offset);


    if(mmap_address==(void*)0xFFFFFFFF)
    {
        FARF(ERROR,"Failed to allocate memory and map the buffer using : HAP_mmap\n");
        retval = -1;
        goto bail;
    }

    FARF(ALWAYS,"buffer_len = %lu\nprot = %d\nflags = %d\nbuffer_fd=%d\nbuffer_offset=%lu\nmmap_address = %p\n",
                 buffer_len, prot, flags, buffer_fd, buffer_offset, mmap_address);


    /*
     * HAP_mmap_get : To get the address associated with the buffer and increment its reference count
     */
    FARF(ALWAYS,"\nHAP_mmap_get:\n");

    uint64 phys_address;

    retval = HAP_mmap_get(buffer_fd, NULL, &phys_address);

    if(retval!=AEE_SUCCESS)
    {
        FARF(ERROR,"Error Code 0x%x returned from function : HAP_mmap_get\n", retval);
        goto bail;
    }

    FARF(ALWAYS,"Incremented the reference count of buffer with buffer_fd = %d and physical address = %llu\n", buffer_fd, phys_address);


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
     * HAP_mem_available_stack : Get the stack size (in bytes) available for current thread
     */
    if (HAP_mem_available_stack == NULL )
    {
        FARF(ERROR,"%s: HAP_mem_available_stack() not supported \n", __func__);
    }
    else
    {
        avail_stack = HAP_mem_available_stack();
        if ((avail_stack == AEE_EINVALIDTHREAD) || (avail_stack == AEE_ERESOURCENOTFOUND))
        {
            FARF(ERROR,"Error Code 0x%x returned from function : HAP_mem_available_stack\n", avail_stack);
            goto bail;
        }
        else
        {
            FARF(ALWAYS,"Available stack size is %d bytes \n", avail_stack);
        }
    }

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

    /*
     * HAP_munmap : To decrease the reference count and unmap the buffer from memory at the given address if the reference count goes to 0
     */
    FARF(ALWAYS,"\nHAP_munmap:\n");

    retval = HAP_munmap(mmap_address, buffer_len);

    if(retval!=AEE_SUCCESS)
    {
        FARF(ERROR,"Error Code 0x%x returned from function : HAP_munmap\n", retval);
        goto bail;
    }

    FARF(ALWAYS,"Unmapped the buffer starting at mmap_address = %p with length buffer_len = %lu\n", mmap_address, buffer_len);

bail:

    if(retval==AEE_SUCCESS)
    {
        FARF(ALWAYS,"\nhap_example_mem_dmahandle PASSED\n");
    }
    else
    {
        FARF(ALWAYS,"\nhap_example_mem_dmahandle FAILED\n");
    }

    return retval;
}
