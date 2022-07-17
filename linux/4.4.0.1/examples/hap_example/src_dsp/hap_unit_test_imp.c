/**=============================================================================
Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

#include "hap_example.h"

#include "remote.h"

#include "HAP_farf.h"
#include "HAP_compute_res.h"
#include "HAP_mem.h"
#include "HAP_vtcm_mgr.h"

#include "AEEStdErr.h"

#define ARRAY_LEN 1024

AEEResult hap_example_unit_test(remote_handle64 h, int buffer_fd, uint32 buffer_offset, uint32 buffer_len)
{
    int retVal = 0;

    unsigned int arch_page_size, arch_page_count;

    unsigned int vtcm_size;
    unsigned int single_page_flag;
    unsigned char b_vtcm_single_page;

    unsigned int context_id;

    unsigned int avail_block_size, max_page_size, num_pages;

    void* vtcm_start_address = NULL;

    unsigned int timeout_us;

    unsigned int *mmap_address;


    int prot, flags;
    int i;


    retVal = HAP_query_total_VTCM(&arch_page_size, &arch_page_count);

    if(retVal!=AEE_SUCCESS) {
        FARF(ERROR, "Error Code 0x%x returned from function : HAP_query_total_VTCM\n", retVal);
        goto bail;
    }

    /*
     * Check if compute resource manager APIs are supported
     */
    if(compute_resource_attr_init)
    {
        /*
         * HAP_compute_res_attr_set_vtcm_param
         */
        compute_res_attr_t res_info;

        retVal = HAP_compute_res_attr_init(&res_info);

        if(retVal!=AEE_SUCCESS)
        {
            FARF(ERROR, "Error Code 0x%x returned from function : HAP_compute_res_attr_init\n", retVal);
            goto bail;
        }

        vtcm_size = arch_page_size;
        b_vtcm_single_page = 1;
        retVal = HAP_compute_res_attr_set_vtcm_param(&res_info, vtcm_size, b_vtcm_single_page);

        if(retVal!=AEE_SUCCESS)
        {
            FARF(ERROR, "Error Code 0x%x returned from function : HAP_compute_res_attr_set_vtcm_param\n", retVal);
            goto bail;
        }

        /*
         * HAP_compute_res_acquire
         */
        timeout_us = 10000;
        context_id = HAP_compute_res_acquire(&res_info, timeout_us);

        if((context_id)==0)
        {
            FARF(ERROR, "Failed to acquire requested resource in given timeout duration\n");
            retVal = AEE_ERESOURCENOTFOUND;
            goto bail;
        }
        /*
         * Validation : HAP_compute_res_acquire can be validated using HAP_query_avail_VTCM
         *
         *              Since we have requested bytes with b_vtcm_single_page=1, we should be assigned "arch_page_size" byte page size.
         *                  the available vtcm memory available must be 0 bytes,
         *                  the maximum possible page size allocation is 0 bytes and
         *                  the number of page size blocks is 0
         */
        retVal = HAP_query_avail_VTCM(&avail_block_size, &max_page_size, &num_pages);

        if(retVal!=AEE_SUCCESS) {
            FARF(ERROR, "Error Code 0x%x returned from function : HAP_query_avail_VTCM\n", retVal);
            goto bail;
        }

        if( (avail_block_size==0) && (max_page_size==0) && (num_pages==0) )
        {
            FARF(ALWAYS, "HAP_compute_res_attr_set_vtcm_param : TEST PASSED\n");
            FARF(ALWAYS, "HAP_compute_res_acquire             : TEST PASSED\n");
        }
        else
        {
            FARF(ALWAYS, "HAP_compute_res_attr_set_vtcm_param : TEST FAILED\n");
            FARF(ALWAYS, "HAP_compute_res_acquire             : TEST FAILED\n");
        }


        /*
         * HAP_compute_res_release
         */
        retVal = HAP_compute_res_release(context_id);

        if(retVal!=AEE_SUCCESS)
        {
            FARF(ERROR, "Error Code 0x%x returned from function : HAP_compute_res_release\n", retVal);
            goto bail;
        }


        /*
         * Validation : HAP_compute_res_release can be validated using HAP_query_avail_VTCM
         *
         *              Since we have requested to release the VTCM memory:
         *                  the available vtcm memory available must be "arch_page_size" bytes and
         *                  the maximum possible page size allocation is "arch_page_size" and
         *                  the number of 262144 byte size blocks is arch_page_count
         */
        retVal = HAP_query_avail_VTCM(&avail_block_size, &max_page_size, &num_pages);

        if(retVal!=AEE_SUCCESS)
        {
            FARF(ERROR, "Error Code 0x%x returned from function : HAP_query_avail_VTCM\n", retVal);
            goto bail;
        }

        if( (avail_block_size==arch_page_size) && (max_page_size==arch_page_size) && (num_pages==arch_page_count) )
        {
            FARF(ALWAYS, "HAP_compute_res_release             : TEST PASSED\n");
        }
        else
        {
            FARF(ALWAYS, "HAP_compute_res_release             : TEST FAILED\n");
        }

    }
    else
    {
        FARF(ALWAYS, "Compute resource manager APIs are not supported\n");
    }


    /*
     * HAP_mmap
     */

    if (buffer_fd != -1) {

        prot = HAP_PROT_READ | HAP_PROT_WRITE;
        flags = 0;
        mmap_address = (unsigned int*) HAP_mmap(NULL, buffer_len, prot, flags, buffer_fd, buffer_offset);


        if(mmap_address==(unsigned int*)0xFFFFFFFF)
        {
            FARF(ERROR,"Failed to allocate memory and map the buffer using : HAP_mmap\n");
            retVal = -1;
            goto bail;
        }

        /*
         * Validation: HAP_mmap can be validated by verifying the contents of the ION memory buffer
         *             ION buffer is initialized from APPS processor with a size of 4096 bytes;
         *             ion_buffer[i] = i and ion_buffer element is of type unsigned int
         */

        for(i=0; i<ARRAY_LEN; ++i)
        {
            if(mmap_address[i]!=i)
            {
                break;
            }
        }

        if(i==ARRAY_LEN)
        {
            FARF(ALWAYS, "HAP_mmap                            : TEST PASSED\n");
        }
        else
        {
            FARF(ALWAYS, "HAP_mmap                            : TEST FAILED\n");
        }
    }

    else {
        FARF(ALWAYS, "Skipped HAP_mmap Test!\n");
    }


    /*
     * HAP_request_VTCM
     */
    vtcm_size = arch_page_size, single_page_flag = 1;

    vtcm_start_address = HAP_request_VTCM(vtcm_size, single_page_flag);

    if(vtcm_start_address==NULL) {
        FARF(ERROR,"Failed to request VTCM memory, NULL pointer returned from HAP_request_VTCM\n");
        goto bail;
    }

    /*
     * Validation : HAP_request_VTCM can be validated using HAP_query_avail_VTCM
     *
     *              Since we have requested bytes with single_page_flag=1, we should be assigned "arch_page_size" byte page size.
     *                  the available vtcm memory available must be 0 bytes,
     *                  the maximum possible page size allocation must be 0 bytes and
     *                  the number of page size blocks must be 0
     */
    retVal = HAP_query_avail_VTCM(&avail_block_size, &max_page_size, &num_pages);

    if(retVal!=AEE_SUCCESS) {
        FARF(ERROR, "Error Code 0x%x returned from function : HAP_query_avail_VTCM\n", retVal);
        goto bail;
    }

    if( (avail_block_size==0) && (max_page_size==0) && (num_pages==0) )
    {
        FARF(ALWAYS, "HAP_query_request_VTCM              : TEST PASSED\n");
    }
    else
    {
        FARF(ALWAYS, "HAP_query_request_VTCM              : TEST FAILED\n");
    }


    /*
     * HAP_release_VTCM
     */
    retVal = HAP_release_VTCM(vtcm_start_address);

    if(retVal!=AEE_SUCCESS) {
        FARF(ERROR, "Error Code 0x%x returned from function : HAP_release_VTCM\n", retVal);
        goto bail;
    }

    /*
     * Validation : HAP_release_VTCM can be validated using HAP_query_avail_VTCM
     *
     *              Since we have requested to release the VTCM memory:
     *                  the available vtcm memory available must be "arch_page_size" bytes and
     *                  the maximum possible page size allocation must be "arch_page_size" and
     *                  the number of "arch_page_size" byte size blocks must be "arch_page_count"
     */
    retVal = HAP_query_avail_VTCM(&avail_block_size, &max_page_size, &num_pages);

    if(retVal!=AEE_SUCCESS) {
        FARF(ERROR, "Error Code 0x%x returned from function : HAP_query_avail_VTCM\n", retVal);
        goto bail;
    }

    if( (avail_block_size==arch_page_size) && (max_page_size==arch_page_size) && (num_pages==arch_page_count) )
    {
        FARF(ALWAYS, "HAP_query_release_VTCM              : TEST PASSED\n");
    }
    else
    {
        FARF(ALWAYS, "HAP_query_release_VTCM              : TEST FAILED\n");
    }


    /*
     * HAP_request_async_VTCM
     */
    vtcm_size = arch_page_size, single_page_flag = 1;
    timeout_us = 1000;

    vtcm_start_address = HAP_request_async_VTCM(vtcm_size, single_page_flag, timeout_us);

    if(vtcm_start_address==NULL) {
        FARF(ERROR,"Failed to request VTCM memory, NULL pointer returned from HAP_request_async_VTCM\n");
        goto bail;
    }

    /*
     * Validation : HAP_request_async_VTCM can be validated using HAP_query_avail_VTCM
     *
     *              Since we have requested bytes with single_page_flag=1, we should be assigned "arch_page_size" byte page size.
     *                  the available vtcm memory available must be 0 bytes,
     *                  the maximum possible page size allocation must be 0 bytes and
     *                  the number of page size blocks must be 0
     */
    retVal = HAP_query_avail_VTCM(&avail_block_size, &max_page_size, &num_pages);

    if(retVal!=AEE_SUCCESS) {
        FARF(ERROR, "Error Code 0x%x returned from function : HAP_query_avail_VTCM\n", retVal);
        goto bail;
    }

    if( (avail_block_size==0) && (max_page_size==0) && (num_pages==0) )
    {
        FARF(ALWAYS, "HAP_query_request_async_VTCM        : TEST PASSED\n");
    }
    else
    {
        FARF(ALWAYS, "HAP_query_request _async_VTCM       : TEST FAILED\n");
    }

    /*
     * Release the VTCM memory requested using HAP_request_async_VTCM
     */
    retVal = HAP_release_VTCM(vtcm_start_address);

    if(retVal!=AEE_SUCCESS) {
        FARF(ERROR, "Error Code 0x%x returned from function : HAP_release_VTCM\n", retVal);
        goto bail;
    }


bail:

    if(retVal==AEE_SUCCESS)
    {
        FARF(ALWAYS, "\nhap_example_unit_test PASSED\n");
    }
    else
    {
        FARF(ALWAYS, "\nhap_example_unit_test FAILED\n");
    }

    return retVal;
}
