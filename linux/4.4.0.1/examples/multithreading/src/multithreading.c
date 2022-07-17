#include "multithreading.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include "AEEStdErr.h"
#include "dsp_capabilities_utils.h"     // available under <HEXAGON_SDK_ROOT>/utils/examples
#include "os_defines.h"

/*
 * Function Declarations
 */

int multithreading_test(int domain, bool isUnsignedPD_Enabled);

static void print_usage()
{
    printf( "Usage:\n"
            "    multithreading [-d domain] [-U unsigned_PD]\n\n"
            "Options:\n"
            "-d domain: Run on a specific domain.\n"
            "    0: Run the example on ADSP\n"
            "    3: Run the example on CDSP\n"
            "    2: Run the example on SDSP\n"
            "        Default Value: 3(CDSP) for targets having CDSP and 0(ADSP) for targets not having CDSP like Agatti.\n"
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

    while((option = getopt(argc, argv,"d:U:")) != -1) {
        switch (option) {
            case 'd' : domain = atoi(optarg);
                break;
            case 'U' : unsignedPDFlag = atoi(optarg);
                break;
            default:
                print_usage();
                return -1;
        }
    }
    if(domain == -1) {
        printf("\nDSP domain is not provided. Retrieving DSP information using Remote APIs.\n");
        nErr = get_dsp_support(&domain);
        if(nErr != AEE_SUCCESS) {
            printf("ERROR in get_dsp_support: 0x%x, defaulting to CDSP domain\n", nErr);
        }
    }

    if((domain < ADSP_DOMAIN_ID) || (domain > CDSP_DOMAIN_ID)) {
        nErr = AEE_EBADPARM;
        printf("\nERROR 0x%x: Invalid domain %d\n", nErr, domain);
        print_usage();
        goto bail;
    }

    if(unsignedPDFlag < 0 || unsignedPDFlag > 1){
        nErr = AEE_EBADPARM;
        printf("\nERROR 0x%x: Invalid unsigned PD flag %d\n", nErr, unsignedPDFlag);
        print_usage();
        goto bail;
    }
    if(unsignedPDFlag == 1) {
        if (domain == CDSP_DOMAIN_ID) {
            isUnsignedPD_Enabled = get_unsignedpd_support();
        }
        else {
            printf("Overriding user request for unsigned PD. Only signed offload is allowed on domain %d.\n", domain);
            unsignedPDFlag = 0;
        }
    }

    printf("Attempting to run on %s PD on domain %d\n", isUnsignedPD_Enabled==true?"unsigned":"signed", domain);
    nErr = multithreading_test(domain, isUnsignedPD_Enabled);
    if( nErr != AEE_SUCCESS ) {
        printf("ERROR 0x%x: Test FAILED\n", nErr);
        goto bail;
    }

bail:
    return nErr;
}

int multithreading_test(int domain, bool isUnsignedPD_Enabled)
{
    int retVal = AEE_SUCCESS;
    remote_handle64 handle = -1;
    char *multithreading_URI_domain = NULL;

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

    if (domain == ADSP_DOMAIN_ID)
      multithreading_URI_domain = multithreading_URI ADSP_DOMAIN;
    else if (domain == CDSP_DOMAIN_ID)
      multithreading_URI_domain = multithreading_URI CDSP_DOMAIN;
    else if (domain == MDSP_DOMAIN_ID)
      multithreading_URI_domain = multithreading_URI MDSP_DOMAIN;
    else if (domain == SDSP_DOMAIN_ID)
      multithreading_URI_domain = multithreading_URI SDSP_DOMAIN;

    retVal = multithreading_open(multithreading_URI_domain, &handle);

    if(retVal!=AEE_SUCCESS)
    {
        printf("ERROR 0x%x: Unable to create FastRPC session on domain %d\n", retVal, domain);
        printf("Exiting...\n");
        goto bail;
    }

    retVal = multithreading_parallel_sum(handle);
    if (retVal!=AEE_SUCCESS) {
        printf("ERROR 0x%x: returned from function : multithreading_parallel_sum \n", retVal);
        goto bail;
    }

    retVal = multithreading_barriers(handle);
    if(retVal!=AEE_SUCCESS) {
        printf("ERROR 0x%x: returned from function : multithreading_barriers \n", retVal);
        goto bail;
    }

    retVal = multithreading_mutexes(handle);
    if(retVal!=AEE_SUCCESS) {
        printf("ERROR 0x%x: returned from function : multithreading_mutexes \n", retVal);
        goto bail;
    }

    multithreading_close(handle);

bail:

    if(retVal==AEE_SUCCESS)
    {
        printf("Test PASSED\n");
    }
    else
    {
        printf("Test FAILED\n");
    }
    printf("Please look at the mini-dm logs or the adb logcat logs for DSP output\n");

    return retVal;
}
