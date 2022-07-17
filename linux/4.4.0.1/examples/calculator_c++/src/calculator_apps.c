/*==============================================================================
  Copyright (c) 2015, 2020-2021 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#include "AEEStdErr.h"
#include "calculator_plus.h"
#include "rpcmem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "os_defines.h"
#include "verify.h"
#include "dsp_capabilities_utils.h"     // available under <HEXAGON_SDK_ROOT>/utils/examples
#include "os_defines.h"

#define MAX_LINE_LEN (100)

static void print_usage()
{
    printf( "Usage:\n"
            "    calculator_plus -m method_name [-i input] [-d domain] [-U unsigned_PD]\n\n"
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
            "-m method_name: Supported methods:\n"
            "        sum: calculates sum of elements of vector of integers.\n"
            "        static_sum: calculates sum of elements of vector of integers of statically initialized object.\n"
            "        iostream_sum: calculates sum of elements of stream of data passed as file using standard input/output streams library.\n"
            "        uppercase_count: counts uppercase letters in passed string\n"
            "        test_tls: tests thread-local storage feature of C++11\n"
            "-i input: Argument based on the method:\n"
            "    For methods sum and static_sum:\n"
            "        Natural number up to which sum is calculated from 0 to (i-1).\n"
            "    For method uppercase_count:\n"
            "        String to check count of uppercase characters.\n"
            "    For method iostream_sum:\n"
            "        Absolute device path of input file containing list of numbers.\n"
            );
}

int calculator_test(int domain, bool isUnsignedPD_Enabled, char *method, char *str)
{
    int nErr = AEE_SUCCESS;
    int *test = NULL;
    int result = 0;
    int ii, len = 0;
    int num;
    int64 num64;
    FILE *fp = NULL;
    int64 result1 = 0;
    int64 result2 = 0;
    char *uri = NULL;
    remote_handle64 handle = -1;
    int retVal = 0;
    int heapid = RPCMEM_HEAP_ID_SYSTEM;
#if defined(SLPI) || defined(MDSP)
    heapid = RPCMEM_HEAP_ID_CONTIG;
#endif

    rpcmem_init();

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
        uri = calculator_plus_URI ADSP_DOMAIN;
    else if (domain == CDSP_DOMAIN_ID)
        uri = calculator_plus_URI CDSP_DOMAIN;
    else if (domain == MDSP_DOMAIN_ID)
        uri = calculator_plus_URI MDSP_DOMAIN;
    else if (domain == SDSP_DOMAIN_ID)
        uri = calculator_plus_URI SDSP_DOMAIN;

    retVal = calculator_plus_open(uri, &handle);
    if (retVal != 0) {
        printf("ERROR 0x%x: Unable to create FastRPC session on domain %d\n", retVal, domain);
        printf("Exiting...\n");
        goto bail;
    }

    if (!strcmp(method, "uppercase_count")) {
        VERIFYC(str != NULL, AEE_EBADPARM);
        printf("Run uppercase_count: %s\n", str);
        if (AEE_SUCCESS == (nErr = calculator_plus_uppercase_count(handle, str, &result))) {
            printf("%d upper case letters found in %s\n", result, str);
        }

    } else if (!strcmp(method, "test_tls")) {
        printf("Run test_tls\n");
        if (AEE_SUCCESS == (nErr = calculator_plus_test_tls(handle, &result1))) {
            if (!result) {
            printf("tls test passed\n");
            } else {
                nErr = AEE_EUNKNOWN;
            }
        }

    } else if (!strcmp(method, "sum")) {
        VERIFYC(str != NULL, AEE_EBADPARM);
        num = atoi(str);
        VERIFYC(num > 0, AEE_EBADPARM);
        num64 = num;
        printf("Compute sum on domain %d\n", domain);
        len = sizeof(*test) * num;

        printf("Allocate %d bytes from ION heap\n", len);
        if (0 == (test = (int *)rpcmem_alloc(heapid, RPCMEM_DEFAULT_FLAGS, len))) {
                nErr = AEE_ENORPCMEMORY;
                printf("ERROR 0x%x: memory alloc failed\n", nErr);
                goto bail;
        }

        printf("Creating sequence of numbers from 0 to %d\n", num - 1);
        for (ii = 0; ii < num; ++ii) {
            test[ii] = ii;
        }

        result2 = (num64 * (num64 - 1)) / 2;
        if (AEE_SUCCESS == (nErr = calculator_plus_sum(handle, test, num, &result1))) {
            if (result1 == result2) {
                printf("Sum = %lld\n", result1);
            } else {
                printf("ERROR: Calculated sum of first %d integers %lld does not match expected value %lld.\n", num, result1, result2);
                nErr = AEE_EUNKNOWN;
            }
        }

    } else if (!strcmp(method, "static_sum")) {
        VERIFYC(str != NULL, AEE_EBADPARM);
        num = atoi(str);
        VERIFYC(num > 0, AEE_EBADPARM);
        num64 = num;
        printf("Compute static_sum\n");
        len = sizeof(*test) * num;

        printf("Allocate %d bytes from ION heap\n", len);
        if (0 == (test = (int *)rpcmem_alloc(heapid, RPCMEM_DEFAULT_FLAGS, len))) {
                nErr = AEE_ENORPCMEMORY;
                printf("ERROR 0x%x: memory alloc failed\n", nErr);
                goto bail;
        }

        printf("Creating sequence of numbers from 0 to %d\n", num - 1);
        for (ii = 0; ii < num; ++ii) {
            test[ii] = ii;
        }

        printf("Compute sum on domain %d using static constructor\n", domain);
        if (AEE_SUCCESS == (nErr = calculator_plus_static_sum(handle, test, num, &result1, &result2))) {
            if (result2 == result1) {
                printf("Sum = %lld\n", result2);
            } else {
                printf("ERROR: Calculated sum of first %d integers %lld does not match expected value %lld.\n", num, result2, result1);
                nErr = AEE_EUNKNOWN;
            }
        }

    } else if (!strcmp(method, "iostream_sum")) {
        char line[MAX_LINE_LEN];
        VERIFYC(str != NULL, AEE_EBADPARM);
        printf("Compute sum on domain %d using iostream, file %s\n", domain, str);
        fp = fopen(str, "r");
        if (fp == NULL) {
            printf("Cannot open input file %s\n",str);
        }
        result2 = 0;
        while (fgets( line, MAX_LINE_LEN, fp )) {
            result2 += atoi(line);
        }

        if (AEE_SUCCESS == (nErr = calculator_plus_iostream_sum(handle, str, &result1))) {
            if (result1 == result2) {
                printf("Sum = %lld\n", result1);
            } else {
                nErr = AEE_EUNKNOWN;
            }
        }

    } else {
        printf("Unsupported method - %s\n", method);
        nErr = AEE_EBADPARM;
        print_usage();
    }

bail:
    if (handle) {
        calculator_plus_close(handle);
    }

    if (test) {
        rpcmem_free(test);
    }

    if (fp) {
        fclose(fp);
        fp = NULL;
    }

    rpcmem_deinit();

    return nErr;
}


int main(int argc, char *argv[])
{
    int nErr = AEE_SUCCESS;
    int option = 0;
    int domain = -1;
    char *method = NULL;
    char *str = NULL;
    int unsignedPDFlag = 1;
    bool isUnsignedPD_Enabled = false;

    while((option = getopt(argc, argv,"d:m:i:U:")) != -1) {
        switch (option) {
            case 'd' : domain = atoi(optarg);
                break;
            case 'm' : method = optarg;
                break;
            case 'i' : str = optarg;
                break;
            case 'U' : unsignedPDFlag = atoi(optarg);
                break;
            default:
                print_usage();
                return AEE_EUNKNOWN;
        }
    }

    if(domain == -1) {
        printf("\nDSP domain is not provided. Retrieving DSP information using Remote APIs.\n");
        nErr = get_dsp_support(&domain);
        if(nErr != AEE_SUCCESS) {
            printf("ERROR in get_dsp_support: 0x%x, defaulting to CDSP domain\n", nErr);
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

    if (method == NULL) {
        nErr = AEE_EBADPARM;
        printf("\nERROR 0x%x: Invalid method\n", nErr);
        print_usage();
        goto bail;
    }

    printf("Attempting to run on %s PD on domain %d\n", isUnsignedPD_Enabled==true?"unsigned":"signed", domain);
    nErr = calculator_test(domain, isUnsignedPD_Enabled, method, str);
    if (nErr) {
        printf("ERROR 0x%x: Calculator test failed\n\n", nErr);
    }

bail:
    if (nErr) {
        printf("ERROR 0x%x: Test FAILED\n", nErr);
    } else {
        printf("Success\n");
    }
    return nErr;
}

void HAP_debug(const char *msg, int level, const char *filename, int line)
{
    #ifdef __LA_FLAG
    __android_log_print(level, "adsprpc", "%s:%d: %s", filename, line, msg);
    #endif
}
