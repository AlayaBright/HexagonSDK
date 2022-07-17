/*==============================================================================
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#include "profiling.h"
#include "verify.h"
#include "HAP_power.h"
#include "HAP_farf.h"
#include "HAP_perf.h"
#include "HAP_ps.h"
#include "remote.h"
#include "AEEStdErr.h"
#include "qurt.h"

#include <stdio.h>
#include <stdlib.h>

#pragma weak HAP_send_early_signal
#pragma weak fastrpc_send_early_signal

AEEResult profiling_noop(remote_handle64 handle)
{
    return AEE_SUCCESS;
}

AEEResult profiling_inbuf(remote_handle64 handle, const uint8* src, int srcLen, int mem_test, unsigned long long *run_time)
{
    int nErr = AEE_SUCCESS;
    unsigned long long func_start_time = HAP_perf_get_time_us();
    if (mem_test) {
        int ii;
        for(ii = 0; ii < srcLen; ii += 32) {
            if(src[ii] != (uint8)ii) {
                nErr = AEE_EFAILED;
            }
        }
    }
    unsigned long long func_end_time = HAP_perf_get_time_us();
    *run_time = func_end_time - func_start_time;

    return nErr;
}

AEEResult profiling_routbuf(remote_handle64 handle, uint8* src, int srcLen, int mem_test, unsigned long long *run_time)
{
    int nErr = AEE_SUCCESS;
    unsigned long long func_start_time = HAP_perf_get_time_us();
    if (mem_test) {
        int ii;
        for(ii = 0; ii < srcLen; ii += 32) {
            src[ii] = (uint8)ii;
        }
    }
    unsigned long long func_end_time = HAP_perf_get_time_us();
    *run_time = func_end_time - func_start_time;

    return nErr;
}

AEEResult profiling_sleep_latency(remote_handle64 handle, uint32 latency_us)
{
    AEEResult res = AEE_SUCCESS;
    int retval = 0;
    //Clear the structure to only update the selected fields
    HAP_power_request_t request = {0};
    request.type = HAP_power_set_DCVS_v3;
    request.dcvs_v3.set_latency = TRUE;
    request.dcvs_v3.latency = latency_us;

    void* rpcperf_ctx = (void*) handle;
    retval = HAP_power_set(rpcperf_ctx, &request);
    if (retval == HAP_POWER_ERR_UNKNOWN) {
        FARF(ERROR, "HAP_power_set FAILED, result 0x%x: Unknown\n", retval);
        res = AEE_EUNKNOWN;
    }
    else if (retval == HAP_POWER_ERR_INVALID_PARAM) {
        FARF(ERROR, "HAP_power_set FAILED, result 0x%x: Invalid Param\n", retval);
        res = AEE_EBADPARM;
    }
    else if (retval == HAP_POWER_ERR_UNSUPPORTED_API) {
        FARF(ERROR, "HAP_power_set FAILED, result 0x%x: Unsupported API\n", retval);
        res = AEE_EUNSUPPORTED;
    }
    return res;
}

AEEResult profiling_power_boost(remote_handle64 handle, uint32 on)
{
    AEEResult res = AEE_SUCCESS;
    int retval = 0;
    //Clear the structure to only update the selected fields
    HAP_power_request_t request = {0};
    void* rpcperf_ctx = (void*) handle;

    if(on) {
        request.type = HAP_power_set_DCVS_v3;
        request.dcvs_v3.set_dcvs_enable = TRUE;
        request.dcvs_v3.dcvs_enable = TRUE;
        request.dcvs_v3.dcvs_option = HAP_DCVS_V2_PERFORMANCE_MODE;
        request.dcvs_v3.set_bus_params = TRUE;
        request.dcvs_v3.bus_params.min_corner = HAP_DCVS_VCORNER_MAX;
        request.dcvs_v3.bus_params.max_corner = HAP_DCVS_VCORNER_MAX;
        request.dcvs_v3.bus_params.target_corner = HAP_DCVS_VCORNER_MAX;
        request.dcvs_v3.set_core_params = TRUE;
        request.dcvs_v3.core_params.min_corner = HAP_DCVS_VCORNER_MAX;
        request.dcvs_v3.core_params.max_corner = HAP_DCVS_VCORNER_MAX;
        request.dcvs_v3.core_params.target_corner = HAP_DCVS_VCORNER_MAX;
        request.dcvs_v3.set_sleep_disable = TRUE;
        request.dcvs_v3.sleep_disable = TRUE;
        res = HAP_power_set(rpcperf_ctx, &request);
    }
    else {
        //These commands are to reset the voting done previously
        request.type = HAP_power_set_DCVS_v3;
        request.dcvs_v3.set_core_params = TRUE;
        res = HAP_power_set(rpcperf_ctx, &request);
    }
    if (retval == HAP_POWER_ERR_UNKNOWN) {
        FARF(ERROR, "HAP_power_set FAILED, result 0x%x: Unknown\n", retval);
        res = AEE_EUNKNOWN;
    }
    else if (retval == HAP_POWER_ERR_INVALID_PARAM) {
        FARF(ERROR, "HAP_power_set FAILED, result 0x%x: Invalid Param\n", retval);
        res = AEE_EBADPARM;
    }
    else if (retval == HAP_POWER_ERR_UNSUPPORTED_API) {
        FARF(ERROR, "HAP_power_set FAILED, result 0x%x: Unsupported API\n", retval);
        res = AEE_EUNSUPPORTED;
    }
    return res;
}

AEEResult profiling_early_signal(remote_handle64 handle, uint32 early_wakeup_time_us)
{
    int retval = 0;

    uint32 qurt_thread_id = qurt_thread_get_id();

    if(HAP_send_early_signal) {
        retval = HAP_send_early_signal(qurt_thread_id, early_wakeup_time_us);
        if(retval != 0) {
            FARF(ERROR, "HAP_send_early_signal FAILED for qurt_thread_id = %u with early_wakeup_time_us = %u, result 0x%x\n", qurt_thread_id, early_wakeup_time_us, retval);
        }
    }
    else {
        FARF(HIGH, "HAP_send_early_signal is unsupported, using legacy fastrpc_send_early_signal API\n");
        if(fastrpc_send_early_signal) {
            retval = fastrpc_send_early_signal(qurt_thread_id, early_wakeup_time_us);
            if(retval != 0) {
                FARF(ERROR, "fastrpc_send_early_signal FAILED for qurt_thread_id = %u with early_wakeup_time_us = %u, result 0x%x\n", qurt_thread_id, early_wakeup_time_us, retval);
            }
        }
        else {
            FARF(HIGH, "fastrpc_send_early_signal API is unsupported\n");
            retval = AEE_EUNSUPPORTED;
        }
    }

    return retval;
}
