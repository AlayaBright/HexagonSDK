/**=============================================================================
Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

#include "hap_example.h"

#include "remote.h"

#include "HAP_farf.h"
#include "HAP_compute_res.h"

#include <hexagon_types.h>

#include "AEEStdErr.h"

#define BLOCK_SIZE       (8*1024/128)


/*
 * HAP_compute_res.h exposes a set of APIs to request and release compute resources and enable serialization of these requests
 */




typedef struct {
    volatile unsigned char release_flag;
} release_cb_context_t;

/*
 *  Release callback entry function which will be called when a higher-priority client requires some or all of the resources reserved.
 *  Here we set a flag and check in a loop to see if a resource is waiting on this and release accordingly.
 */
int release_cb(unsigned int context_id, void* client_context)
{
    release_cb_context_t *data = (release_cb_context_t*) client_context;
    data->release_flag = 1;
    return 0;
}



AEEResult hap_example_compute_res(remote_handle64 h)
{
    int retVal = 0;

    /*
     * HAP_compute_res_attr_init : To initialize compute resource attribute structure - compute_res_attr_t res_info
     */
    FARF(ALWAYS, "\nHAP_compute_res_attr_init:\n");

    compute_res_attr_t res_info;


    /*
     * Check if compute resource manager APIs are supported on the target
     */
    if(!(compute_resource_attr_init))
    {
        FARF(ALWAYS, "Compute resource manager APIs are not supported\n");
        return AEE_EUNSUPPORTED;
    }


    retVal = HAP_compute_res_attr_init(&res_info);

    if(retVal!=AEE_SUCCESS)
    {
        FARF(ERROR, "Failed to initialize the compute resource structure\n");
        goto bail;
    }

    FARF(ALWAYS, "Initialized the compute resource structure:\n");
    for(int i=0; i<8; i++)
    {
        FARF(ALWAYS, "res_info.attributes[%d] = %llu\n", i, res_info.attributes[i]);
    }


    /*
     * HAP_compute_res_attr_set_serialize : To set/reset the serialize option in the compute resource attribute structure
     */
    FARF(ALWAYS, "\nHAP_compute_res_attr_set_serialize:\n");

    unsigned char b_serialize = 1;
    retVal = HAP_compute_res_attr_set_serialize(&res_info, b_serialize);

    if(retVal!=AEE_SUCCESS)
    {
        FARF(ERROR, "Failed to set serialize option in the compute resource structure\n");
        goto bail;
    }

    FARF(ALWAYS, "Set the serialize option in the compute resource structure:\n");
    for(int i=0; i<8; i++)
    {
        FARF(ALWAYS, "res_info.attributes[%d] = %llu\n", i, res_info.attributes[i]);
    }


    /*
     * HAP_compute_res_attr_set_vtcm_param : To set the vtcm size and single-page requirement for the compute resource attribute structure
     */
    FARF(ALWAYS, "\nHAP_compute_res_attr_set_vtcm_param:\n");

    unsigned int vtcm_size = 65536;
    unsigned char b_vtcm_single_page = 1;
    retVal = HAP_compute_res_attr_set_vtcm_param(&res_info, vtcm_size, b_vtcm_single_page);

    if(retVal!=AEE_SUCCESS)
    {
        FARF(ERROR, "Failed to request VTCM memory with the given attributes\n");
        goto bail;
    }

    FARF(ALWAYS, "Requested VTCM memory of size: %u and with single-page flag b_vtcm_single_page : %u\n", vtcm_size, b_vtcm_single_page);
    for(int i=0; i<8; i++)
    {
        FARF(ALWAYS, "res_info.attributes[%d] = %llu\n", i, res_info.attributes[i]);
    }


    /*
     * HAP_compute_res_acquire : To request a context with the given resource attribute structure parameters
     */
    FARF(ALWAYS, "\nHAP_compute_res_acquire:\n");

    unsigned int timeout_us = 10000;
    unsigned int context_id;
    context_id = HAP_compute_res_acquire(&res_info, timeout_us);

    if((context_id)==0)
    {
        FARF(ERROR, "Failed to acquire the requested resource in the given timeout duration\n");
        retVal =  AEE_ERESOURCENOTFOUND;
        goto bail;
    }

    FARF(ALWAYS, "Acquired the compute resource with context_id : %u\n", context_id);


    /*
     * HAP_compute_res_attr_get_vtcm_ptr : To request the vtcm start address from the structure
     */
    FARF(ALWAYS, "\nHAP_compute_res_attr_get_vtcm_ptr:\n");

    void *vtcm_start_address = HAP_compute_res_attr_get_vtcm_ptr(&res_info);

    if(vtcm_start_address==NULL)
    {
        FARF(ERROR, "Failed to get the VTCM pointer\n");
        retVal = -1;
        goto bail;
    }

    FARF(ALWAYS, "\nVTCM memory starting address is at vtcm_start_address : %p\n", vtcm_start_address);


    /*
     * HAP_compute_res_release : To release the resources with the given context ID
     */
    FARF(ALWAYS, "\nHAP_compute_res_release:\n");

    retVal = HAP_compute_res_release(context_id);

    if(retVal!=0)
    {
        FARF(ERROR, "Failed to Release compute resource with the context_id : %u\n", context_id);
        goto bail;
    }

    FARF(ALWAYS, "Released the compute resources linked to context_id : %u\n", context_id);

#if !defined(__hexagon__)
    /*
     * The HAP_compute_res_attr_set_release_callback API is supported only on Lahaina and later targets
     */

    if(compute_resource_attr_set_release_callback)
    {

        release_cb_context_t data;
        data.release_flag = 0;

        HAP_compute_res_attr_set_serialize(&res_info, 1);

        HAP_compute_res_attr_set_vtcm_param(&res_info, vtcm_size, 1);

        /*
         *  Cached requests maintain the same VTCM pointer and size across calls to HAP_compute_res_acquire_cached()
         *  and HAP_compute_res_release_cached()
         */
        HAP_compute_res_attr_set_cache_mode(&res_info, 1);

        /*
         *  Specify a release callback function for our request. The compute resource manager calls the release_callback function
         *  when any of the resources reserved by the specified context are required by a higher priority client.
         */
        HAP_compute_res_attr_set_release_callback(&res_info, release_cb, (void *) &data);

        context_id = HAP_compute_res_acquire(&res_info, timeout_us);

        if((context_id)==0)
        {
            FARF(ERROR, "Failed to acquire the requested resource in the given timeout duration\n");
            retVal =  AEE_ERESOURCENOTFOUND;
            goto bail;
        }

        FARF(ALWAYS, "Acquired the compute resource with context_id : %u\n", context_id);

        void *vtcm_start_address = HAP_compute_res_attr_get_vtcm_ptr(&res_info);

        if(vtcm_start_address==NULL)
        {
            FARF(ERROR, "Failed to get the VTCM pointer\n");
            retVal = -1;
            goto bail;
        }

        int *input = (int*) vtcm_start_address;
        int __attribute__((aligned(128))) tmp_buf[32];
        int minimum = -1;

        HVX_Vector sout = Q6_V_vsplat_R(*(volatile int32_t *)input);
        HVX_Vector first = Q6_V_vsplat_R(*(volatile int32_t *)input);
        HVX_Vector *iptr = (HVX_Vector *) input;
        HVX_Vector sline1, sline1c, sline1p;

        int vectors_in_rounddown = vtcm_size / 32;

        int have_resource = 0;

        for (int32_t i = vectors_in_rounddown; i > 0; i -= BLOCK_SIZE)
        {

            if(!have_resource)
            {
                /* Cached call to acquire the resource linked to our handle 'context_id'. Here we are again specifying a
                   timeout of 10000us after which we will abandon the request. Once this has returned successfully, we can
                   use our resource until we call HAP_compute_res_release_cached */
                if(0 != HAP_compute_res_acquire_cached(context_id, timeout_us))
                {
                    FARF(ERROR, "Unable to acquire requested resource in the given timeout duration");
                    return AEE_ERESOURCENOTFOUND;
                }

                have_resource = 1;
            }

            int block = Q6_R_min_RR(i, BLOCK_SIZE);

            for (int32_t j = 0; j < block; ++j)
            {
                sline1c = *iptr++;
                sline1 = Q6_V_valign_VVR(sline1c, sline1p, (size_t) input);

                sout = Q6_Vw_vmin_VwVw(sout, sline1);

                sline1p = sline1c;
            }


            /* Check to see if our release flag has been set by the release callback function.
               If so, stop processing after the current block, release the VTCM resource,
               and return to the beginning of the loop where we can rerequest the VTCM resource.
               Once the higher priority client release VTCM or enough VTCM becomes available,
               we will be able to acquire the VTCM resource and will reload its contents so that we can
               resume processing the same input data. */
            if (data.release_flag)
            {
                have_resource = 0;
                FARF(ALWAYS, "Release Callback requested, abandoning loop %d", i);

                /* Release our cached resource. As this is only a cached release, the manager will maintain a mapping
                   for our request, and once we call HAP_compute_res_acquire_cached, we can reacquire the resource
                   with the same VTCM pointer and size as before. */
                HAP_compute_res_release_cached(context_id);
            }
        }

        if(have_resource)
        {
            HAP_compute_res_release_cached(context_id);
        }



        for (int32_t i = 64; i >= 4; i >>= 1)
        {
            sout = Q6_Vw_vmin_VwVw(sout, Q6_V_vlalign_VVR(sout, first, i));
        }

        *(HVX_Vector *) tmp_buf = sout;
        minimum = tmp_buf[31];

        if(minimum==0)
        {
            FARF(ALWAYS, "HAP_compute_res_attr_set_release_callback passed\n");
        }
        else
        {
            FARF(ERROR, "HAP_compute_res_attr_set_release_callback failed\n");
        }

        HAP_compute_res_release(context_id);
    }
    else
    {
        FARF(ALWAYS, "The HAP release callback API is not supported on this target\n");
    }

#endif

bail:

    if(retVal==AEE_SUCCESS)
    {
        FARF(ALWAYS, "\nhap_example_compute_res PASSED\n");
    }
    else
    {
        FARF(ALWAYS, "\nhap_example_compute_res FAILED\n");
    }

    return retVal;
}
