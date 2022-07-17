/**=============================================================================
All Rights Reserved Qualcomm Proprietary
Copyright (c) 2017, 2020 Qualcomm Technologies Incorporated.
=============================================================================**/

#include "qprintf_example.h"
#include "AEEStdErr.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "dsp_capabilities_utils.h"     // available under <HEXAGON_SDK_ROOT>/utils/examples
#include "os_defines.h"

int test_main_start(bool isUnsignedPD_Enabled);

static void print_usage()
{
    printf( "Usage:\n"
            "    qprintf_example [-d domain] [-U unsigned_PD]\n\n"
            "Options:\n"
            "-d domain: Run on a specific domain. This example is supported only on CDSP.\n"
            "    3: Run the example on CDSP\n"
            "        Default Value: 3(CDSP) for targets having CDSP.\n"
            "-U unsigned_PD: Run on signed or unsigned PD.\n"
            "    0: Run on signed PD.\n"
            "    1: Run on unsigned PD.\n"
            "        Default Value: 1\n"
            );
}

int main(int argc, char* argv[])
{
    int nErr = AEE_SUCCESS;
    int option = 0;
    int domain = -1;
    int unsignedPDFlag = 1;
    bool isUnsignedPD_Enabled = false;

    while((option = getopt(argc, argv,"d:U:")) != -1)
    {
        switch (option)
        {
            case 'd' : domain = atoi(optarg);
                break;
            case 'U' : unsignedPDFlag = atoi(optarg);
                break;
            default:
                print_usage();
                return -1;
        }
    }
    if(domain == -1)
    {
        printf("\nDSP domain is not provided. Retrieving DSP information using Remote APIs.\n");
        nErr = get_dsp_support(&domain);
        if(nErr != AEE_SUCCESS) {
            printf("ERROR in get_dsp_support: 0x%x, defaulting to CDSP domain\n", nErr);
        }
    }

    if (domain != CDSP_DOMAIN_ID)
    {
        nErr = -1;
        printf("\nERROR 0x%x: Invalid domain %d\n", nErr, domain);
        print_usage();
        goto bail;
    }

    if (unsignedPDFlag < 0 || unsignedPDFlag > 1) {
        nErr = AEE_EBADPARM;
        printf("\nERROR 0x%x: Invalid unsigned PD flag %d\n", nErr, unsignedPDFlag);
        print_usage();
        goto bail;
    }
    if(unsignedPDFlag == 1) {
        isUnsignedPD_Enabled = get_unsignedpd_support();
    }

    printf("Attempting to run on %s PD on domain %d\n", isUnsignedPD_Enabled==true?"unsigned":"signed", domain);
    nErr =  test_main_start(isUnsignedPD_Enabled);
    if( nErr != AEE_SUCCESS ) {
        printf("ERROR 0x%x: Test FAILED\n", nErr);
        goto bail;
    }

bail:
    return nErr;
}

// This is a simple program to initiate a test on the DSP.
int test_main_start(bool isUnsignedPD_Enabled)
{
    int retVal = AEE_SUCCESS;
    char *qprintf_example_URI_Domain = NULL;
    remote_handle64 handle = -1;

    if(isUnsignedPD_Enabled) {
        if(remote_session_control) {
            struct remote_rpc_control_unsigned_module data;
            data.domain = CDSP_DOMAIN_ID;
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

    qprintf_example_URI_Domain = qprintf_example_URI CDSP_DOMAIN;
    retVal = qprintf_example_open(qprintf_example_URI_Domain, &handle);
    if (retVal != AEE_SUCCESS)
    {
        printf("ERROR 0x%x: Unable to create FastRPC session\n", retVal);
        printf("Exiting...\n");
        goto bail;
    }
    if (-1 == handle)
    {
        printf("Failed to open a channel\n");
        goto bail;
    }

    retVal = qprintf_example_run(handle);

    qprintf_example_close(handle);
    handle = -1;

bail:
    if (retVal)
    {
        printf("Test FAILED: 0x%x \n", retVal);
    }
    else
    {
        printf("Test PASSED\n");
    }
    printf("See Mini-DM output for more details\n");

    return retVal;
}

