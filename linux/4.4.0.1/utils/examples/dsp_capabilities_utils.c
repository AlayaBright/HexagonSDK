/**=============================================================================
@file
    dsp_capabilities_utils.c

@brief
    Wrapper functions for FastRPC Capability APIs.

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include "remote.h"
#include "os_defines.h"
#include "dsp_capabilities_utils.h"

int get_dsp_support(int *domain)
{
    int nErr = AEE_SUCCESS;
    *domain = CDSP_DOMAIN_ID; // DSP domain default value is CDSP_DOMAIN_ID

    if(remote_handle_control)
    {
        struct remote_dsp_capability dsp_capability_domain = {CDSP_DOMAIN_ID, DOMAIN_SUPPORT, 0};
        nErr = remote_handle_control(DSPRPC_GET_DSP_INFO, &dsp_capability_domain, sizeof(struct remote_dsp_capability));
        if((nErr & 0xFF)==(AEE_EUNSUPPORTEDAPI & 0xFF))
        {
            printf("\nFastRPC Capability API is not supported on this device\n");
            goto bail;
        }

        if(dsp_capability_domain.capability == 0)
        {
            dsp_capability_domain.domain = ADSP_DOMAIN_ID;     // Check for ADSP support.
            dsp_capability_domain.attribute_ID = DOMAIN_SUPPORT;
            dsp_capability_domain.capability = 0;
            nErr = remote_handle_control(DSPRPC_GET_DSP_INFO, &dsp_capability_domain, sizeof(struct remote_dsp_capability));
            if(dsp_capability_domain.capability)
            {
                *domain = ADSP_DOMAIN_ID;    // For targets like Agatti (not having cDSP), domain is ADSP_DOMAIN_ID
            }
        }
    }
    else
    {
        nErr = AEE_EUNSUPPORTEDAPI;
        printf("remote_dsp_capability interface is not supported on this device\n");
    }

bail:
    return nErr;
}

int get_vtcm_info(int domain, uint32_t *capability, uint32_t attr)
{
    int nErr = AEE_SUCCESS;
    *capability = 0;

    if(attr == VTCM_PAGE || attr == VTCM_COUNT) {
    }
    else {
        nErr = AEE_EBADPARM;
        printf("Unsupported attr. Only VTCM_PAGE and VTCM_COUNT supported\n");
        goto bail;
    }
    if(remote_handle_control) {
        if(domain == ADSP_DOMAIN_ID || domain == CDSP_DOMAIN_ID) {
            /*
            * Query the DSP for VTCM information
            * Since the ADSP does not have a dedicated VTCM, we expect the output to be 0
            */
            struct remote_dsp_capability dsp_capability_vtcm_dsp;
            dsp_capability_vtcm_dsp.domain = (uint32_t)domain;
            dsp_capability_vtcm_dsp.attribute_ID = attr;
            dsp_capability_vtcm_dsp.capability = (uint32_t)0;
            nErr = remote_handle_control( DSPRPC_GET_DSP_INFO, &dsp_capability_vtcm_dsp, sizeof(struct remote_dsp_capability) );
            if((nErr & 0xFF)==(AEE_EUNSUPPORTEDAPI & 0xFF)) {
                printf("\nFastRPC Capability API is not supported on this device\n");
                goto bail;
            }
            else if (nErr == AEE_SUCCESS) {
                *capability = dsp_capability_vtcm_dsp.capability;
            }
        }
        else {
            nErr = AEE_EUNSUPPORTED;
            printf("Unsupported domain %d\n", domain);
            goto bail;
        }
    }
    else {
        nErr = AEE_EUNSUPPORTEDAPI;
        printf("remote_dsp_capability interface is not supported on this device\n");
    }

bail:
    return nErr;
}

bool get_unsignedpd_support(void)
{
    int nErr = AEE_SUCCESS;
    if(remote_handle_control)
    {
        struct remote_dsp_capability dsp_capability_domain = {CDSP_DOMAIN_ID, UNSIGNED_PD_SUPPORT, 0};
        nErr = remote_handle_control(DSPRPC_GET_DSP_INFO, &dsp_capability_domain, sizeof(struct remote_dsp_capability));
        if((nErr & 0xFF)==(AEE_EUNSUPPORTEDAPI & 0xFF))
        {
            printf("\nFastRPC Capability API is not supported on this device. Falling back to signed pd.\n");
            return false;
        }
        if(nErr)
        {
            printf("\nERROR 0x%x: FastRPC Capability API failed. Falling back to signed pd.", nErr);
            return false;
        }
        if(dsp_capability_domain.capability == 1)
        {
            return true;
        }
    }
    else
    {
        printf("remote_dsp_capability interface is not supported on this device. Falling back to signed pd.\n");
        return false;
    }
return false;
}
