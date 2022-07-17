/**=============================================================================
Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

#include "hap_example.h"

#include "remote.h"

#include "HAP_farf.h"
#include "sysmon_cachelock.h"

#include "AEEStdErr.h"

/*
 * sysmon_cachelock APIs allow user to lock/unlock memory in cache
 */


AEEResult hap_example_sysmon_cachelock(remote_handle64 h)
{
    int retVal = 0;

    /*
     * HAP_cache_lock : Allocate and lock memory of given size.
     *                  Return virtual address and optionally return physical address
     */
    FARF(ALWAYS , "\nHAP_cache_lock:\n");

    int locked_cache_size = 4096;
    unsigned long long physical_address;

    void *allocated_virtual_address = HAP_cache_lock(locked_cache_size, &physical_address);

    if(allocated_virtual_address == NULL)
    {
        FARF(ERROR, "Failed to allocate and lock cache memory\n");
        retVal = -1;
        goto bail;
    }

    FARF(ALWAYS , "HAP_cache_lock allocated cache memory size locked_cache_size = %d,"
                "\nwith virtual address allocated_virtual_address = %p\n"
                "and physical address physical_address = %llu\n" , locked_cache_size, allocated_virtual_address, physical_address);


    /*
     * HAP_query_total_cachelock : To query the total locked cache size
     */
    FARF(ALWAYS , "\nHAP_query_total_cachelock:\n");

    int total_locked_cache_size;

    total_locked_cache_size = HAP_query_total_cachelock();

    if(total_locked_cache_size < 0)
    {
        FARF(ERROR, "Failed to get the total locaked cache size\n");
        retVal = -1;
        goto bail;
    }

    FARF(ALWAYS, "Total locked cache size is total_locked_cache_size = %u bytes\n", total_locked_cache_size);


    /*
     * HAP_query_avail_cachelock : To get the size of max contiguous memory block available for cache locking
     */
    int available_cache_size ;

    FARF(ALWAYS, "\nHAP_query_avail_cachelock:\n");

    available_cache_size = HAP_query_avail_cachelock();

    if(available_cache_size < 0)
    {
        FARF(ERROR, "Failed to get the available Size in cache\n");
        retVal = -1;
        goto bail;
    }

    FARF(ALWAYS, "Size of max contiguous memory block available for cache locking is available_cache_size =  %u bytes\n", available_cache_size);


    /*
     * HAP_cache_unlock : De-allocate memory and unlock cache at the given virtual address
     */
    FARF(ALWAYS , "\nHAP_cache_unlock:\n");

    retVal = HAP_cache_unlock(allocated_virtual_address);
    if(retVal!=AEE_SUCCESS)
    {
        FARF(ERROR, "Failed to unlock the cache buffer\n");
        goto bail;
    }

    FARF(ALWAYS, "Unlocked the cache memory address at allocated_virtual_address = %u\n", allocated_virtual_address);

bail:

    if(retVal==AEE_SUCCESS)
    {
        FARF(ALWAYS, "\nhap_example_sysmon_cachelock PASSED\n");
    }
    else
    {
        FARF(ALWAYS, "\nhap_example_sysmon_cachelock FAILED\n");
    }

    return retVal;
}
