/**=============================================================================
Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

#include "hap_example.h"

#include "remote.h"

#include "HAP_farf.h"
#include "HAP_vtcm_mgr.h"

#include "AEEStdErr.h"


/*
 * HAP_vtcm_mgr APIs are used to query the availability, request, or release the VTCM memory on the CDSP
 */

AEEResult hap_example_vtcm_mgr(remote_handle64 h)
{
    int retVal = 0;

    /*
     * HAP_query_total_VTCM : To query the total VTCM attributes as defined by the architecture
     */
    FARF(HIGH,"\nHAP_query_total_VTCM:\n");

    unsigned int arch_page_size, arch_page_count;

    retVal = HAP_query_total_VTCM(&arch_page_size, &arch_page_count);

    if(retVal!=AEE_SUCCESS) {
        FARF(ERROR,"Error Code 0x%x returned from function : HAP_query_total_VTCM\n", retVal);
        goto bail;
    }

    FARF(HIGH,"Maximum page size allocation possible is arch_page_size = %u bytes\n"
              "Number of arch_page_size blocks available is arch_page_count = %u\n",
               arch_page_size, arch_page_count);


    /*
     * HAP_query_avail_VTCM : To query the available VTCM attributes
     */
    FARF(HIGH,"\nHAP_query_avail_VTCM:\n");

    unsigned int avail_block_size, max_page_size, num_pages;

    retVal = HAP_query_avail_VTCM(&avail_block_size, &max_page_size, &num_pages);

    if(retVal!=AEE_SUCCESS) {
        FARF(ERROR,"Error Code 0x%x returned from function : HAP_query_avail_VTCM\n", retVal);
        goto bail;
    }

    FARF(HIGH,"Maximum contiguous memory available in VTCM is avail_block_size = %u bytes\n"
              "Maximum possible page size allocation in the available VTCM region is max_page_size = %u bytes\n"
              "Number of max_page_size blocks available is %u\n",
               avail_block_size, max_page_size, num_pages);


    /*
     * HAP_request_VTCM : To request VTCM memory of required size and with single-page requirement
     */
    FARF(HIGH,"\nHAP_request_VTCM:\n");

    unsigned int vtcm_size = arch_page_size, single_page_flag = 1;
    void* vtcm_start_address;

    vtcm_start_address = HAP_request_VTCM(vtcm_size, single_page_flag);

    if(vtcm_start_address==NULL) {
        FARF(ERROR,"Failed to request VTCM memory, NULL pointer returned from HAP_request_VTCM\n");
        retVal = -1;
        goto bail;
    }


    FARF(HIGH,"Requested VTCM memory of vtcm_size : %u\n"
              "with single_page_flag : %u\n"
              "vtcm_start_address : %p\n",
               vtcm_size, single_page_flag, vtcm_start_address);


    /*
     * HAP_release_VTCM : To release VTCM memory
     */
    FARF(HIGH,"\nHAP_release_VTCM:\n");

    retVal = HAP_release_VTCM(vtcm_start_address);

    if(retVal!=AEE_SUCCESS) {
        FARF(ERROR,"Error Code 0x%x returned from function : HAP_release_VTCM\n", retVal);
        goto bail;
    }

    FARF(HIGH,"Released the VTCM memory from vtcm_start_address: %p\n", vtcm_start_address);


    /*
     * HAP_request_async_VTCM : To request VTCM memory of required size, with single page requirement
     *                          and a timeout in micro-seconds
     */
    FARF(HIGH,"\nHAP_request_async_VTCM:\n");

    unsigned int vtcm_timeout_us = 10000;


    if(HAP_request_async_VTCM)
    {
        vtcm_start_address = HAP_request_async_VTCM(vtcm_size, single_page_flag, vtcm_timeout_us);
    }
    else
    {

        FARF(ALWAYS, "HAP_request_async_VTCM not supported on target\n");
        goto bail;
    }

    if(vtcm_start_address==NULL) {
        FARF(ERROR,"Failed to request VTCM memory, NULL pointer returned from HAP_request_async_VTCM\n");
        retVal = -1;
        goto bail;
    }


    FARF(HIGH,"Requested VTCM memory of vtcm_size : %u\n"
              "with single_page_flag : %u\n"
              "vtcm_timeout_us = %u micro-seconds\n"
              "vtcm_start_address : %p\n",
               vtcm_size, single_page_flag, vtcm_timeout_us, vtcm_start_address);


    /*
     * Release the VTCM memory using HAP_release_VTCM
     */
    FARF(HIGH,"\nHAP_release_VTCM:\n");

    retVal = HAP_release_VTCM(vtcm_start_address);

    if(retVal!=AEE_SUCCESS) {
        FARF(ERROR,"Error Code 0x%x returned from function : HAP_release_VTCM\n", retVal);
        goto bail;
    }

    FARF(HIGH,"Released the VTCM memory from vtcm_start_address: %p\n", vtcm_start_address);

bail:

    if(retVal==AEE_SUCCESS)
    {
        FARF(HIGH,"\nhap_example_vtcm_mgr PASSED\n");
    }
    else
    {
        FARF(HIGH,"\nhap_example_vtcm_mgr FAILED\n");
    }


    return retVal;
}
