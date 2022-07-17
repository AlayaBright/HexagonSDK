/**=============================================================================
Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

/*
 * @file            hap_example_main.c
 *
 * @services        Start point of hap_example example on APPs
 *
 * @description     Main function on APPs that calls the FastRPC Capablity API function, HAP API Example and HAP API Unit test
 */

#include "hap_example.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>

/*Header file for ION memory allocation APIs*/
#include <rpcmem.h>

/*Header file for Standard Error Codes returned by QuRT*/
#include "AEEStdErr.h"
#include "dsp_capabilities_utils.h"     // available under <HEXAGON_SDK_ROOT>/utils/examples
#include "os_defines.h"

/*
 * Function Declarations
 */
int retrieve_vtcm_stats(void);
int hap_example(int domain, bool isUnsignedPD_Enabled, int example_selector);
//int hap_unit_test(void);

static void print_usage()
{
    printf( "Usage:\n"
            "    hap_example [-d domain] [-U unsigned_PD] [-f example_selector]\n\n"
            "Options:\n"
            "-d domain: Run on a specific domain.\n"
            "    0: Run the example on ADSP\n"
            "    3: Run the example on CDSP\n"
            "        Default Value: 3(CDSP) for targets having CDSP and 0(ADSP) for targets not having CDSP like Agatti.\n"
            "-U unsigned_PD: Run on signed or unsigned PD.\n"
            "    0: Run on signed PD.\n"
            "    1: Run on unsigned PD.\n"
            "        Default Value: 1\n"
            "-f example_selector: 0-6 select HAP API type\n"
            "    0: HAP_compute_res (Default)\n"
            "    1: HAP_farf\n"
            "    2: HAP_mem\n"
            "    3: HAP_perf\n"
            "    4: HAP_power\n"
            "    5: sysmon_cachelock\n"
            "    6: HAP_vtcm_mgr\n"
            );
}


int main(int argc, char* argv[])
{
    int nErr = AEE_SUCCESS;
    int option = 0;
    int domain = -1;
    int example_selector = 0;
    int unsignedPDFlag = 1;
    bool isUnsignedPD_Enabled = false;

    while((option = getopt(argc, argv,"d:f:U:")) != -1)
    {
        switch (option) {
            case 'd' : domain = atoi(optarg);
                break;
            case 'f' : example_selector = atoi(optarg);
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
            printf("Error in get_dsp_support: 0x%x, defaulting to CDSP domain\n", nErr);
        }
    }

    if ((domain < ADSP_DOMAIN_ID) || (domain > CDSP_DOMAIN_ID)) {
        nErr = AEE_EBADPARM;
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
        if (domain == CDSP_DOMAIN_ID) {
            isUnsignedPD_Enabled = get_unsignedpd_support();
        }
        else {
            printf("Overriding user request for unsigned PD. Only signed offload is allowed on domain %d.\n", domain);
            unsignedPDFlag = 0;
        }
    }
    printf("Attempting to run on %s PD on domain %d\n", isUnsignedPD_Enabled==true?"unsigned":"signed", domain);

    /*
     * Example that shows the usage of HAP APIs. Function implemented in hap_example.c
     */
    printf("\n\n-----------------------HAP API Example--------------------------------\n\n");
    nErr = hap_example(domain, isUnsignedPD_Enabled, example_selector);
    if( nErr != AEE_SUCCESS ) {
        printf("ERROR 0x%x: Test FAILED\n", nErr);
        goto bail;
    }

    if(nErr!=AEE_SUCCESS)
        goto bail;

    /*
     * Unit test that validates the HAP APIs. Function implemented in hap_unit_test.c
     */
//    printf("\n\n--------------------------HAP Unit Test-------------------------------\n\n");
//    nErr = hap_unit_test();
    if(nErr!=AEE_SUCCESS)
    {
        goto bail;
    }

 bail:
    if(nErr==AEE_SUCCESS)
    {
        printf("\nhap_example PASSED\n");
    }
    else
    {
        printf("\nhap_example FAILED with nErr = 0x%x\n", nErr);
    }

    return nErr;
}
