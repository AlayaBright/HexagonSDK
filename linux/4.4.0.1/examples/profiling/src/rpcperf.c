/*==============================================================================
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rpcmem.h"
#include "profiling.h"
#include "verify.h"
#include "rpcperf.h"
#include "time_utils.h"
#include "AEEStdErr.h"


static int rpcperf_alloc(int size, int ion, int uncached, void **ppo)
{
    int nErr = 0;

    if(!ion) {
        VERIFYC(0 != (*ppo = calloc(size, 1)), AEE_ENOMEMORY);
    }
    else {
        const char* eheap = getenv("RPCPERF_HEAP_ID");
        int heapid = eheap == 0 ? -1 : atoi(eheap);
        const char* eflags = getenv("RPCPERF_HEAP_FLAGS");
        unsigned int flags = eflags == 0 ? RPCMEM_DEFAULT_FLAGS : atoi(eflags);
        if(uncached) {
            flags |= RPCMEM_HEAP_UNCACHED;
        }
        VERIFYC(0 != (*ppo = rpcmem_alloc(heapid, flags, size)), AEE_ENOMEMORY);
    }
bail:
    return nErr;
}

static void rpcperf_free(void *po, int ion)
{
    if(!po)
       return;

    if(!ion) {
        free(po);
    }
    else {
        rpcmem_free(po);
    }
}

int rpcperf_noop(remote_handle64 handle, int cnt)
{
    int nErr = 0;
    int ii = 0;
    unsigned long long start, end;
    VERIFY(0 == (nErr = profiling_noop(handle)));     //Open the session before to ignore the time taken to open a new fastrpc session for first time
    unsigned long long overhead = 0;
    unsigned long long total_overhead = 0;
    unsigned long long avg_overhead = 0;
    for(ii = 0; ii < cnt; ii++) {
        start = GET_TIME();
        VERIFY(0 == (nErr = profiling_noop(handle)));
        end = GET_TIME();
        overhead = end - start;
        total_overhead += overhead;
    }
    avg_overhead = total_overhead/cnt;
    printf("%lluus\n", avg_overhead);
bail:
    return nErr;
}

int rpcperf_inbuf(remote_handle64 handle, int uncached, int size, int ion, int mem_test, int cnt)
{
    int nErr = 0;
    int ii = 0;
    unsigned long long start, end;
    unsigned long long cpu_processing_time = 0;
    unsigned long long overhead = 0;
    unsigned long long total_overhead = 0;
    unsigned long long avg_overhead = 0;
    unsigned long long dsp_processing_time = 0;
    VERIFY(0 == (nErr = profiling_noop(handle)));     //Open the session before to ignore the time taken to open a new fastrpc session for first time
    uint8 *x;
    VERIFY(0 == (nErr = rpcperf_alloc(size, ion, uncached, (void **)&x)));
    if(mem_test == 1) {
        memset(x, 0, size);
        for(ii = 0; ii < size; ii += 32) {
            x[ii] = (uint8)ii;
        }
    }
    for(ii = 0; ii < cnt; ii++) {
        start = GET_TIME();
        nErr |= profiling_inbuf(handle, x, size, mem_test, &dsp_processing_time);
        end = GET_TIME();

        cpu_processing_time = end - start;
        overhead = cpu_processing_time - dsp_processing_time;
        total_overhead += overhead;
    }
    avg_overhead = total_overhead/cnt;
    printf("%lluus\n", avg_overhead);
    rpcperf_free(x, ion);

bail:
    return nErr;
}

int rpcperf_routbuf(remote_handle64 handle, int uncached, int size, int ion, int mem_test, int cnt)
{
    int nErr = 0;
    int ii = 0;
    unsigned long long start, end;
    unsigned long long cpu_processing_time = 0;
    unsigned long long overhead = 0;
    unsigned long long dsp_processing_time = 0;
    unsigned long long total_overhead = 0;
    unsigned long long avg_overhead = 0;
    VERIFY(0 == (nErr = profiling_noop(handle)));     //Open the session before to ignore the time taken to open a new fastrpc session for first time
    uint8 *x;
    VERIFY(0 == (nErr = rpcperf_alloc(size, ion, uncached, (void **)&x)));
    if(mem_test == 1) {
        memset(x, 0, size);
    }
    for(ii = 0; ii < cnt; ii++) {
        start = GET_TIME();
        VERIFY(0 == (nErr = profiling_routbuf(handle, x, size, mem_test, &dsp_processing_time)));
        end = GET_TIME();

        if(mem_test == 1) {
            int jj;
            for(jj = 0; jj < size; jj += 32) {
                VERIFY(x[jj] == (uint8)jj);
            }
        }
        cpu_processing_time = end - start;
        overhead = cpu_processing_time - dsp_processing_time;
        total_overhead += overhead;
    }
    avg_overhead = total_overhead/cnt;
    printf("%lluus\n", avg_overhead);
    rpcperf_free(x, ion);

bail:
    return nErr;
}

int rpcperf_default(remote_handle64 handle, int ion)
{
    int nErr = 0;
    unsigned ii;
    //Run all the tests by default
    printf("\n[Test_Name Buffer_Size]  Avg FastRPC overhead per iteration(usecs)\n\n");
    printf("[noop        0K]          ");
    VERIFY(0 == (nErr = rpcperf_noop(handle, 1000)));
    const unsigned long long test[]={32 * 1024, 64 * 1024, 128 * 1024, 1024 * 1024, 4 * 1024 * 1024, 8 * 1024 * 1024, 16 * 1024 * 1024};
    const char test_readable[7][5]={" 32K", " 64K", "128K", "  1M", "  4M", "  8M", " 16M"};
    for(ii = 0; ii < (sizeof(test)/sizeof(unsigned long long)); ii++) {
        printf("[inbuf     %s]          ", &test_readable[ii][0]);
        //Defaults: cached, ion set by user, variable size, check_memory off
        VERIFY(0 == (nErr = rpcperf_inbuf(handle, 0, test[ii], ion, 0, 1000)));
        printf("[routbuf   %s]          ", &test_readable[ii][0]);
        VERIFY(0 == (nErr = rpcperf_routbuf(handle, 0, test[ii], ion, 0, 1000)));
    }
bail:
    return nErr;
}
