/**=============================================================================
Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

#include "hap_example.h"

#include "remote.h"

#include "HAP_farf.h"
#include "HAP_perf.h"

#include "AEEStdErr.h"

/*
 * HAP_perf APIs are used for profiling DSP code execution without the overhead of RPC
 */


AEEResult hap_example_perf(remote_handle64 h)
{
    int retVal = 0;

    /*
     * HAP_perf_get_time_us : Returns the current value of a 56-bit global hardware counter in micro-seconds.
     */
    FARF(ALWAYS , "\nHAP_perf_get_time_us:\n");

    unsigned long long time_us = HAP_perf_get_time_us();

    FARF(ALWAYS, "Hardware counter time_us = %llu\n", time_us);


    /*
     * HAP_perf_get_qtimer_count : Returns the current value of a 56-bit global hardware counter
     */
    FARF(ALWAYS , "\nHAP_perf_get_qtimer_count:\n");

    unsigned long long qtimer_count = HAP_perf_get_qtimer_count();

    FARF(ALWAYS, "Hardware counter qtimer_count = %llu\n", qtimer_count);


    /*
     * HAP_perf_qtimer_count_to_us : To convert a 19.2 MHz global hardware count to micro-seconds
     */
    FARF(ALWAYS , "\nHAP_perf_qtimer_count_to_us:\n");

    time_us = HAP_perf_qtimer_count_to_us(qtimer_count);

    FARF(ALWAYS, "Converted qtimer_count = %llu to time_us = %llu\n", qtimer_count, time_us);


    /*
     * HAP_perf_get_pcycles : To get current count of Hexagon processor cycle count
     */
    FARF(ALWAYS , "\nHAP_perf_get_pcycles:\n");

    unsigned long long pcycle_count = HAP_perf_get_pcycles();

    FARF(ALWAYS, "Current Hexagon processor cycle count pcycle_count = %llu\n", pcycle_count);


    /*
     * HAP_timer_sleep : To suspend the calling thread for a specified duration
     */

    FARF(ALWAYS , "\nHAP_timer_sleep:\n");

    unsigned long long sleep_duration = 1000000;

    retVal = HAP_timer_sleep(sleep_duration);

    if(retVal!=AEE_SUCCESS)
    {
        FARF(ERROR, "Failed to suspend the current thread\n");
        goto bail;
    }

    FARF(ALWAYS, "Suspended the thread for %llu micro-seconds\n", sleep_duration);

bail:
    if(retVal==AEE_SUCCESS)
    {
        FARF(ALWAYS, "\nhap_example_perf PASSED\n");
    }
    else
    {
        FARF(ALWAYS, "\nhap_example_perf FAILED\n");
    }

    return retVal;
}
