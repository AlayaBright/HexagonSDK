/**=============================================================================
Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

/*
 * @file            hap_example.c
 *
 * @services        hap_example function calls DSP functions that demonstrate the usage of HAP APIs
 *
 * @description     Calls the DSP HAP API function based on the command-line argument provided
 */


#include "hap_example.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>


/*Header file for ION memory allocation APIs*/
#include <rpcmem.h>

/*Header file for Standard Error Codes returned by QuRT*/
#include "AEEStdErr.h"
#include "os_defines.h"

/*Length of the ION buffer in bytes*/
#define BUF_LEN 4096

enum example_case {
    COMPUTE_RES = 0,
    FARF_RUNTIME,
    MEM,
    PERF,
    POWER,
    SYSMON_CACHELOCK,
    VTCM
};



int hap_example(int domain, bool isUnsignedPD_Enabled, int example_selector)
{
    int retVal = AEE_SUCCESS;
    remote_handle64 handle = -1;
    char *hap_example_URI_domain = NULL;

    if(isUnsignedPD_Enabled) {
        if(remote_session_control) {
            struct remote_rpc_control_unsigned_module data;
            data.domain = domain;
            data.enable = 1;
            if (AEE_SUCCESS != (retVal = remote_session_control(DSPRPC_CONTROL_UNSIGNED_MODULE, (void*)&data, sizeof(data)))) {
                printf("ERROR 0x%x: remote_session_control failed\n", retVal);
                goto bail;
            }
        }
        else {
            retVal = AEE_EUNSUPPORTED;
            printf("ERROR 0x%x: remote_session_control interface is not supported on this device\n", retVal);
            goto bail;
        }
    }

    if(domain == ADSP_DOMAIN_ID)
        hap_example_URI_domain = hap_example_URI ADSP_DOMAIN;
    else if(domain == CDSP_DOMAIN_ID)
        hap_example_URI_domain = hap_example_URI CDSP_DOMAIN;

    retVal = hap_example_open(hap_example_URI_domain, &handle);
    if(retVal!=0)
    {
        printf("ERROR 0x%x: Unable to create FastRPC session on domain %d\n", retVal, domain);
        goto bail;
    }


    switch(example_selector)
    {
        case COMPUTE_RES:
        {
            /*
             * DSP function that demonstrates HAP_compute_res.h
             */
            if(domain != CDSP_DOMAIN_ID)
            {
                printf("ERROR: HAP_compute_res.h APIs supported only CDSP\n");
                retVal = -1;
                break;
            }
            printf("\nDemonstrating HAP_compute_res.h APIs\n");
            retVal = hap_example_compute_res(handle);

            if(retVal == (DSP_OFFSET + AEE_EUNSUPPORTED))
            {
                printf("HAP_compute_res.h APIs are not supported on this target\n");
                return AEE_SUCCESS;
            }

            break;
        }

        case FARF_RUNTIME:
        {
            /*
             * DSP function that demonstrates FARF macro used for logging
             * and APIs to control run-time logging
             */
            printf("\nDemonstrating FARF run-time logging\n");
            hap_example_farf_runtime(handle, 0);
            hap_example_farf_runtime(handle, 1);
            break;
        }

        case MEM:
        {
            rpcmem_init();

            printf("\nDemonstrating HAP_mem.h APIs\n");

            /*
             * rpcmem_alloc is used for ION memory allocation
             * Please refer to HEXAGON_SDK_ROOT/docs/doxygen/rpcmem.html for more information
             *
             * BUF_LEN is the size of the ION Memory buffer in bytes
             */
            void* ion_buffer = rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS, BUF_LEN);

            if(ion_buffer==NULL)
            {
                printf("ERROR: Failed to allocate ION memory\n");
                retVal = -1;
                goto bail;
            }

            /*
             * rpcmem_to_fd is used to obtain a file descriptor for the buffer
             */
            int fd = rpcmem_to_fd(ion_buffer);

            /* For targets that do not support file descriptor, skipping the following methods*/
            if (fd == -1)
            {
                rpcmem_free(ion_buffer);
                rpcmem_deinit();
                break;
            }

            /* There are two ways to map a buffer persistently across function calls and share to the DSP :
             * 1) Using fastrpc_mmap() and passing the fd directly to the DSP (available from Lahaina and later targets)
             * 2) As a dmahandle object using the dmahandle IDL parameter (legacy)
             *
             * Please refer to $HEXAGON_SDK_ROOT/docs/software/ipc/rpc.html#persistent-shared-buffers-using-dmahandle-objects
             * for more information
             */

            /* Using fastrpc_mmap */
            if(fastrpc_mmap)
            {
                retVal = fastrpc_mmap(CDSP_DOMAIN_ID, fd, ion_buffer, 0, BUF_LEN, FASTRPC_MAP_FD);

                if (retVal == AEE_SUCCESS)
                {
                    unsigned int buffer_length = BUF_LEN;
                    int ret;
                    /* The file descriptor is passed directly as an int parameter to the DSP */
                    retVal = hap_example_mem_fastrpc_mmap(handle, fd, buffer_length);
                    if(retVal!=0)
                    {
                        printf("Error Code 0x%x : returned from hap_example_mem_fastrpc_mmap(handle, fd, buffer_length)\n", retVal);
                    }

                    ret = fastrpc_munmap(CDSP_DOMAIN_ID, fd, NULL, 0);
                    if (ret != AEE_SUCCESS)
                    {
                        printf("Error Code 0x%x : returned from the fastrpc_munmap() API\n", ret);
                    }
                }
                else if (retVal == AEE_EUNSUPPORTED)
                {
                    printf("fastrpc_mmap APIs are not supported on this target\n");
                }
                else
                {
                    printf("Error Code 0x%x : returned from the fastrpc_mmap() API\n", retVal);
                }
            }

            else
            {
                printf("fastrpc_mmap APIs are not supported on this target\n");
            }


            /* Using dmahandle:
            * The file descriptor is passed to the DSP via the dmahandle IDL type
            */
            unsigned int offset = 0;
            unsigned int buffer_length = BUF_LEN;

            retVal = hap_example_mem_dmahandle(handle, fd, offset, buffer_length);
            if (retVal != AEE_SUCCESS)
            {
                    printf("Error Code 0x%x : returned from hap_example_mem_dmahandle(handle, fd, offset, buffer_length)\n", retVal);
            }

            rpcmem_free(ion_buffer);
            rpcmem_deinit();
            break;
        }

        case PERF:
        {
            /*
             * DSP function that demonstrates HAP_perf.h APIs
             */
            printf("\nDemonstrating HAP_perf.h APIs\n");
            retVal = hap_example_perf(handle);
            break;
        }

        case POWER:
        {
            /*
             * DSP function that demonstrates HAP_power.h APIs
             */
            printf("\nDemonstrating HAP_power.h APIs\n");
            retVal = hap_example_power(handle);
            break;
        }

        case SYSMON_CACHELOCK:
        {
            /*
             * DSP function that demonstrates sysmon_cachelock.h APIs. This is not supported on Unsigned PD.
             */
            if(domain != CDSP_DOMAIN_ID || isUnsignedPD_Enabled == 1)
            {
                printf("ERROR: sysmon_cachelock.h APIs supported only on signed CDSP\n");
                retVal = -1;
                break;
            }
            printf("\nDemonstrating sysmon_cachelock.h APIs\n");
            retVal = hap_example_sysmon_cachelock(handle);
            break;
        }

        case VTCM:
        {
            /*
             * DSP function that demonstrates HAP_vtcm_mgr.h APIs
             */
            if(domain != CDSP_DOMAIN_ID)
            {
                printf("ERROR: HAP_vtcm_mgr.h APIs supported only CDSP\n");
                retVal = -1;
                break;
            }
            printf("\nDemonstrating HAP_vtcm_mgr.h APIs\n");
            retVal = hap_example_vtcm_mgr(handle);
            break;
        }


        default:
        {
            printf("Please select a number ranging from 0 to 6\n");
            retVal = -1;
            break;
        }
    }

    hap_example_close(handle);


 bail:
    if(retVal==AEE_SUCCESS)
    {
        printf("\nhap_example function PASSED\n");
    }
    else
    {
        printf("\nhap_example function FAILED\n");
    }
    printf("Please look at the mini-dm logs or the adb logcat logs for DSP output\n");

    return retVal;
}
