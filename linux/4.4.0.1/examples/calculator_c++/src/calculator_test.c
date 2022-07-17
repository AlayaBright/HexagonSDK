/*==============================================================================
  Copyright (c) 2015, 2020 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#define VERIFY_PRINT_ERROR 1
#define FARF_ERROR 1
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "calculator_plus.h"
#include "rpcmem.h"
#include "verify.h"
#include "HAP_farf.h"
#include "remote.h"
#include "AEEStdErr.h"


#define MAX_LINE_LEN 100

int calculator_test(int domain)
{
    int nErr = 0;
    int *test = 0;
    int len = 0, ii, num = 10000;
    int64 result1 = 0, result2 = 0;
    int result3 = 0;
    char line[MAX_LINE_LEN];
	char *uri = NULL;
    remote_handle64 handle = -1;

    FILE *fp = fopen("calculator.input", "r");

    rpcmem_init();

    len = sizeof(*test) * num;

    int heapid = RPCMEM_HEAP_ID_SYSTEM;
#if defined(SLPI) || defined(MDSP)
    heapid = RPCMEM_HEAP_ID_CONTIG;
#endif
    VERIFY(test = (int *)rpcmem_alloc(heapid, RPCMEM_DEFAULT_FLAGS, len));

    for (ii = 0; ii < num; ++ii) {
        test[ii] = ii;
    }

    if (domain == ADSP_DOMAIN_ID)
      uri = calculator_plus_URI ADSP_DOMAIN;
    else if (domain == CDSP_DOMAIN_ID)
      uri = calculator_plus_URI CDSP_DOMAIN;
    else if (domain == MDSP_DOMAIN_ID)
      uri = calculator_plus_URI MDSP_DOMAIN;
    else if (domain == SDSP_DOMAIN_ID)
      uri = calculator_plus_URI SDSP_DOMAIN;
    else {
      nErr = AEE_EINVALIDDOMAIN;
      printf("ERROR 0x%x: unsupported domain %d\n", nErr, domain);
      goto bail;
    }

    nErr = calculator_plus_open(uri, &handle);
    if (nErr) {
        printf("Error Code: 0x%x - Unable to create FastRPC session on domain %d\n", nErr, domain);
        printf("Exiting...\n");
        goto bail;
    }

    VERIFY(!calculator_plus_sum(handle, test, num, &result1));
    VERIFY(result1 == (num * (num - 1)) / 2);

    VERIFY(!calculator_plus_static_sum(handle, test, num, &result1, &result2));
    VERIFY(result1 == result2);
    VERIFY(result1 == (num * (num - 1)) / 2);

    VERIFY (fp);
    VERIFY(!calculator_plus_iostream_sum(handle, "calculator.input", &result1));

    result2 = 0;
    while (fgets(line, MAX_LINE_LEN, fp)) {
        result2 += atoi(line);
    }
    VERIFY(result1 == result2);

    VERIFY(!calculator_plus_uppercase_count(handle, "Hello World C++14!", &result3));
    VERIFY(result3 == 3);

bail:
    if (handle)
        calculator_plus_close(handle);

    if (test)
        rpcmem_free(test);

    if (fp) {
        fclose(fp);
        fp = NULL;
    }

    rpcmem_deinit();
    return nErr;
}

