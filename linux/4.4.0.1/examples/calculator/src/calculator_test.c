/*==============================================================================
  Copyright (c) 2012-2014,2017,2020 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "AEEStdErr.h"
#include "calculator.h"
#include "calculator_test.h"
#include "rpcmem.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "remote.h"
#include "os_defines.h"

int local_calculator_sum(const int* vec, int vecLen, int64* res) {
  int ii = 0;
  *res = 0;
  for (ii = 0; ii < vecLen; ++ii)
    *res = *res + vec[ii];
  printf( "===============     DSP: local sum result %lld ===============\n", *res);
  return 0;
}

int local_calculator_max(const int* vec, int vecLen, int *res) {
  int ii;
  int max = 0;
  for (ii = 0; ii < vecLen; ii++)
     max = vec[ii] > max ? vec[ii] : max;
  *res = max;
  printf( "===============     DSP: local max result %d ===============\n", *res);
  return 0;
}

int calculator_test(int runLocal, int domain, int num, bool isUnsignedPD_Enabled) {
  int nErr = AEE_SUCCESS;
  int* test = NULL;
  int ii, len = 0, resultMax = 0;
  int retry = 10;
  int64 result = 0;
  remote_handle64 handleMax = -1;
  remote_handle64 handleSum = -1;
  char *uri = NULL;

  rpcmem_init();

  len = sizeof(*test) * num;
  printf("\nAllocate %d bytes from ION heap\n", len);

  int heapid = RPCMEM_HEAP_ID_SYSTEM;
#if defined(SLPI) || defined(MDSP)
  heapid = RPCMEM_HEAP_ID_CONTIG;
#endif

  if (0 == (test = (int*)rpcmem_alloc(heapid, RPCMEM_DEFAULT_FLAGS, len))) {
    nErr = AEE_ENORPCMEMORY;
    printf("ERROR 0x%x: memory alloc failed\n", nErr);
    goto bail;
  }

  printf("Creating sequence of numbers from 0 to %d\n", num - 1);
  for (ii = 0; ii < num; ++ii)
    test[ii] = ii;

  if (runLocal) {
    printf("Compute sum locally\n");
    if (0 != local_calculator_sum(test, num, &result)) {
      nErr = AEE_EFAILED;
      printf("ERROR 0x%x: local compute sum failed\n", nErr);
      goto bail;
    }
    printf("Find max locally\n");
    if (0 != local_calculator_max(test, num, &resultMax)) {
      nErr = AEE_EFAILED;
      printf("ERROR 0x%x: local find max failed\n", nErr);
      goto bail;
    }
  } else {

    printf("Compute sum on domain %d\n", domain);

    if(isUnsignedPD_Enabled) {
      if(remote_session_control) {
        struct remote_rpc_control_unsigned_module data;
        data.domain = domain;
        data.enable = 1;
        if (AEE_SUCCESS != (nErr = remote_session_control(DSPRPC_CONTROL_UNSIGNED_MODULE, (void*)&data, sizeof(data)))) {
          printf("ERROR 0x%x: remote_session_control failed\n", nErr);
          goto bail;
        }
      }
      else {
        nErr = AEE_EUNSUPPORTED;
        printf("ERROR 0x%x: remote_session_control interface is not supported on this device\n", nErr);
        goto bail;
      }
    }

    if (domain == ADSP_DOMAIN_ID)
      uri = calculator_URI ADSP_DOMAIN;
    else if (domain == CDSP_DOMAIN_ID)
      uri = calculator_URI CDSP_DOMAIN;
    else if (domain == MDSP_DOMAIN_ID)
      uri = calculator_URI MDSP_DOMAIN;
    else if (domain == SDSP_DOMAIN_ID)
      uri = calculator_URI SDSP_DOMAIN;
    else {
      nErr = AEE_EINVALIDDOMAIN;
      printf("ERROR 0x%x: unsupported domain %d\n", nErr, domain);
      goto bail;
    }

    do {
      if (AEE_SUCCESS == (nErr = calculator_open(uri, &handleSum))) {
        printf("\nCall calculator_sum on the DSP\n");
        nErr = calculator_sum(handleSum, test, num, &result);
      }

      if (!nErr) {
        printf("Sum = %lld\n", result);
        break;
      } else {
        if (nErr == AEE_ECONNRESET && errno == ECONNRESET) {
          /* In case of a Sub-system restart (SSR), AEE_ECONNRESET is returned by FastRPC
          and errno is set to ECONNRESET by the kernel.*/
          retry--;
          SLEEP(5); /* Sleep for x number of seconds */
        } else if (nErr == AEE_ENOSUCH || (nErr == (AEE_EBADSTATE + DSP_OFFSET))) {
          /* AEE_ENOSUCH is returned when Protection domain restart (PDR) happens and
          AEE_EBADSTATE is returned from DSP when PD is exiting or crashing.*/
          /* Refer to AEEStdErr.h for more info on error codes*/
          retry -= 2;
        } else {
          break;
        }
      }

      /* Close the handle and retry handle open */
      if (handleSum != -1) {
        if (AEE_SUCCESS != (nErr = calculator_close(handleSum))) {
          printf("ERROR 0x%x: Failed to close handle\n", nErr);
        }
      }
    } while(retry);

    if (nErr) {
      printf("Retry attempt unsuccessful. Timing out....\n");
      printf("ERROR 0x%x: Failed to compute sum on domain %d\n", nErr, domain);
    }

    if (AEE_SUCCESS == (nErr = calculator_open(uri, &handleMax))) {
      printf("\nCall calculator_max on the DSP\n");
      if (AEE_SUCCESS == (nErr = calculator_max(handleMax, test, num, &resultMax))) {
        printf("Max value = %d\n", resultMax);
      }
    }

    if (nErr) {
      printf("ERROR 0x%x: Failed to find max on domain %d\n", nErr, domain);
    }

    if (handleSum != -1) {
      if (AEE_SUCCESS != (nErr = calculator_close(handleSum))) {
        printf("ERROR 0x%x: Failed to close handleSum\n", nErr);
      }
    }

    if (handleMax != -1) {
      if (AEE_SUCCESS != (nErr = calculator_close(handleMax))) {
        printf("ERROR 0x%x: Failed to close handleMax\n", nErr);
      }
    }
  }
bail:
  if (test) {
    rpcmem_free(test);
  }
  rpcmem_deinit();
  return nErr;
}
