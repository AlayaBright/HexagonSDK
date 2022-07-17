/*==============================================================================
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#include "HAP_farf.h"
#include "profiling.h"
#include "HAP_debug.h"
#include "AEEStdErr.h"
#include "HAP_perf.h"
#include <stdlib.h>
#include <string.h>
#include "profiling_asm.h"
#include "hexagon_types.h"
#include "hexagon_protos.h"
#include "qprintf.h"
#include <limits.h>
#include "remote.h"
#include "qurt.h"
#include "verify.h"


typedef struct {
    int element;
} profiling_context_t;


AEEResult profiling_open(const char *uri, remote_handle64 *handle)
{
    profiling_context_t *profiling_ctx = malloc(sizeof(profiling_context_t));

    if(profiling_ctx==NULL)
    {
        FARF(ERROR, "Failed to allocate memory for the profiling context");
        return AEE_ENOMEMORY;
    }

    *handle = (remote_handle64) profiling_ctx;

    return AEE_SUCCESS;
}

AEEResult profiling_close(remote_handle64 handle)
{
    profiling_context_t *profiling_ctx = (profiling_context_t*) handle;

    if(profiling_ctx==NULL)
    {
        FARF(ERROR, "NULL handle received in profiling_close()");
        return AEE_EBADHANDLE;
    }

    free(profiling_ctx);

    return AEE_SUCCESS;
}

AEEResult profiling_sum(remote_handle64 handle, const int* vec, int vecLen, int64* res)
{
    int ii = 0;
    *res = 0;
    for (ii = 0; ii < vecLen; ++ii) {
        *res = *res + vec[ii];
    }
    return AEE_SUCCESS;
}

/* Memcpy test */
AEEResult profiling_test_memcpy(remote_handle64 handle, int *dest_pt, int *src_pt, int len)
{
    FARF(RUNTIME_HIGH,"%s running\n", __func__);
    memcpy(dest_pt, src_pt, len);
    return AEE_SUCCESS;
}

AEEResult profiling_memcpy_time_pcycles(remote_handle64 handle, unsigned long long len, unsigned long long *run_time)
{
    int ii = 0;
    int retVal;
    int nErr = 0;
    int *src_ptr = NULL;
    int *dest_ptr = NULL;
    int64 src_result = 0;
    int64 dest_result = 0;
    unsigned long long diff_cycles = 0;

    unsigned long long func_start_time = HAP_perf_get_time_us();

    if(len <= 0 || len > INT_MAX)
        return AEE_EBADPARM;
    int64 num_bytes = len * sizeof(int);

    src_ptr = (int *) malloc(num_bytes);
    if(!src_ptr)
        return AEE_ENOMEMORY;
    dest_ptr = (int *) malloc(num_bytes);
    if(!dest_ptr) {
        free(src_ptr);
        return AEE_ENOMEMORY;
    }

    FARF(RUNTIME_HIGH,"---Creating sequence of numbers from 0 to %d\n", len - 1);
    for (ii = 0; ii < len; ++ii) {
        src_ptr[ii] = ii;
    }

    unsigned long long start_cycles = HAP_perf_get_pcycles();
    VERIFYC(0 != (retVal = profiling_test_memcpy(handle, dest_ptr, src_ptr, num_bytes)),AEE_SUCCESS);
    unsigned long long end_cycles = HAP_perf_get_pcycles();

    src_result = (len * (len - 1))/2 ;
    if (0 != profiling_sum(handle, dest_ptr, len, &dest_result)) {
      FARF(RUNTIME_HIGH,"Error: compute dst on DSP failed\n");
    }
    if(src_result != dest_result) {
        FARF(RUNTIME_HIGH,"Error in copying. Src and dest sums don't match");
        retVal = AEE_EFAILED;
        goto bail;
    }
    FARF(RUNTIME_HIGH,"src: %lld dst: %lld", src_result, dest_result);
    diff_cycles = end_cycles - start_cycles;
    FARF(RUNTIME_HIGH,"%s cycles: %llu PCycles", __func__, diff_cycles);

    unsigned long long func_end_time = HAP_perf_get_time_us();
    *run_time = func_end_time - func_start_time;
    FARF(RUNTIME_HIGH,"%s run time on dsp: %lluus", __func__, *run_time);
    retVal = AEE_SUCCESS;

bail:
    free(dest_ptr);
    free(src_ptr);
    return retVal;
}

AEEResult profiling_asm_iterations_time_us(remote_handle64 handle, unsigned int num)
{
    unsigned long long diff_time = 0;
    unsigned long long start_time = HAP_perf_get_time_us();
    unsigned int value = profiling_loop_time(num);
    unsigned long long end_time = HAP_perf_get_time_us();

    diff_time = end_time - start_time;
    FARF(RUNTIME_HIGH,"profiling_loop_time over %u iterations: %lluus", value, diff_time);
    if(value != num)
        return AEE_EFAILED;

    return AEE_SUCCESS;
}

AEEResult profiling_asm_iterations_pcycles(remote_handle64 handle, unsigned int num)
{
    unsigned long long diff_cycles = 0;
    unsigned long long start_cycles = HAP_perf_get_pcycles();
    unsigned int value = profiling_loop_pcycles(num);
    unsigned long long end_cycles = HAP_perf_get_pcycles();
    diff_cycles = end_cycles - start_cycles;
    FARF(RUNTIME_HIGH,"profiling_loop_pcycles over %u iterations: %llu pCycles", value, diff_cycles);
    if(value != num)
        return AEE_EFAILED;

    return AEE_SUCCESS;
}

AEEResult profiling_asm_iterations_qtimer(remote_handle64 handle, unsigned int num)
{
    unsigned long long diff_time = 0;
    unsigned long long diff_cycles = 0;
    unsigned long long start_cycles = HAP_perf_get_qtimer_count();
    unsigned int value = profiling_loop_time(num);
    unsigned long long end_cycles = HAP_perf_get_qtimer_count();

    unsigned long long start_time = HAP_perf_qtimer_count_to_us(start_cycles);
    unsigned long long end_time = HAP_perf_qtimer_count_to_us(end_cycles);

    diff_time = end_time - start_time;
    diff_cycles = end_cycles - start_cycles;
    FARF(RUNTIME_HIGH, "profiling_loop_time qtimer over %u iterations: %llu hw_ticks, %lluus. Observed clock rate %lluMHz",
            value, diff_cycles, diff_time, (diff_cycles / diff_time));
    if(value != num)
        return AEE_EFAILED;

    return AEE_SUCCESS;
}

AEEResult profiling_vector_assignment(remote_handle64 handle, unsigned int num)
{
    unsigned long long diff_cycles = 0;
    unsigned int num_hvx128_contexts = (qurt_hvx_get_units() >> 8) & 0xFF;
    if(!num_hvx128_contexts) {
        FARF(RUNTIME_HIGH, "HVX not supported");
        return AEE_EUNSUPPORTED;
    }
    else {
        FARF(RUNTIME_HIGH, "HVX supported");
    }
    unsigned long long start_cycles = HAP_perf_get_pcycles();
    HVX_Vector value = profiling_hvx_function(num);
    unsigned long long end_cycles = HAP_perf_get_pcycles();
    //Checking if value written matches for each of the vector registers
    for (int i=127;i>=0;i-=1) {
        unsigned int a= Q6_R_vextract_VR(value,i);
        if(a != num) {
            FARF(RUNTIME_HIGH, "Error in running VSPLAT %d: 0x%x", i, a);
            return AEE_EFAILED;
        }
    }
    diff_cycles = end_cycles - start_cycles;
    FARF(RUNTIME_HIGH,"profiling_hvx_function: %llu pCycles", diff_cycles);
    return AEE_SUCCESS;
}
