/*==============================================================================
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "profiling.h"
#include "AEEStdErr.h"
#include "rpcmem.h"
#include <string.h>
#include <limits.h>
#include "rpcperf.h"
#include "dsp_capabilities_utils.h"     // available under <HEXAGON_SDK_ROOT>/utils/examples
#include "time_utils.h"
#include "os_defines.h"
#include "verify.h"

#define MIN_SLEEP_LATENCY (10)
#define MAX_SLEEP_LATENCY (65535)

int profiling_cpu(remote_handle64 handle)
{
    int retVal = AEE_SUCCESS;
    unsigned long long start, end;
    unsigned long long cpu_processing_time = 0;
    unsigned long long overhead = 0;
    unsigned long long dsp_processing_time = 0;
    profiling_noop(handle);     //Open the session before to ignore the time taken to open a new fastrpc session for first time
    start = GET_TIME();
    retVal = profiling_memcpy_time_pcycles(handle, 50000, &dsp_processing_time);
    if (retVal != 0) {
        printf("ERROR 0x%x: Failed to profile CPU.",retVal);
    }
    end = GET_TIME();
    cpu_processing_time = end - start;

    overhead = cpu_processing_time - dsp_processing_time;
    printf("CPU processing time : %lluus\n", cpu_processing_time);
    printf("DSP Processing time : %lluus\n", dsp_processing_time);
    printf("Overhead time       : %lluus\n", overhead);

    return retVal;
}

int profiling_dsp(remote_handle64 handle)
{
    int retVal = AEE_SUCCESS;
    printf("Starting Assembly tests on DSP. View results in logcat.\n");
    profiling_asm_iterations_time_us(handle, 50000);
    profiling_asm_iterations_pcycles(handle, 50000);
    profiling_asm_iterations_qtimer(handle, 50000);
    retVal = profiling_vector_assignment(handle, 50000);
    if(retVal == AEE_EUNSUPPORTED + DSP_OFFSET) {
        printf("HVX not supported on this domain\n");
        //The example will not error out if DSP domain does not support HVX
        retVal = AEE_SUCCESS;
    }
    else if(retVal == AEE_EFAILED + DSP_OFFSET) {
        printf("VSPLAT on HVX failed. Check logcat for details.\n");
    }
    else if(retVal != AEE_SUCCESS){
        printf("ERROR 0x%x: Failed to profile DSP.",retVal);
    }

    return retVal;
}

int profiling_with_timers(remote_handle64 handle)
{
    int nErr = AEE_SUCCESS;
    VERIFY(AEE_SUCCESS == (nErr = profiling_cpu(handle)));
    VERIFY(AEE_SUCCESS == (nErr = profiling_dsp(handle)));
bail:
    return nErr;
}

static int open_session(remote_handle64 *handle_ptr, int domain, bool isUnsignedPD_Enabled)
{
    int nErr = AEE_SUCCESS;
    char *profiling_URI_domain = NULL;

    if (domain == CDSP_DOMAIN_ID) {
        profiling_URI_domain = profiling_URI CDSP_DOMAIN;
        printf("Opening CDSP\n");
    }
    else if (domain == ADSP_DOMAIN_ID) {
        profiling_URI_domain = profiling_URI ADSP_DOMAIN;
        printf("Opening ADSP\n");
    }
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
    nErr = profiling_open(profiling_URI_domain, handle_ptr);
    if (nErr != AEE_SUCCESS) {
        printf("ERROR 0x%x: Unable to create FastRPC session on domain %d. Exiting.\n", nErr, domain);
        nErr = -1;
        goto bail;
    }

bail:
    return nErr;
}

static void print_usage()
{
    printf( "Usage:\n"
            "   profiling -f function_name [-d domain] [-U unsigned_PD] [-p power_boost] [-n iterations]\n"
            "       [-s size] [-i ion_disable] [-m check_memory] [-u uncached] [-q fastrpc_qos] [-e early_signal]\n"
            "options:\n"
            "-f function_name: Which function to run (set to default if not specified)\n"
            "       timers: Run the timer examples\n"
            "       default: Run the fastrpc performance tests with defaults\n"
            "           defaults are: cached, ion set by user, variable size, check_memory off"
            "       noop: Run the rpc performance test with no operation\n"
            "       inbuf: Run the rpc performance test with inbuffer\n"
            "       routbuf: Run the rpc performance test with routbuffer\n"
            "-d domain: Run on a specific domain (Default Value: 3(CDSP) for targets having CDSP and 0(ADSP) for targets not having CDSP like Agatti.)\n"
            "       0: Run the example on ADSP\n"
            "       3: Run the example on CDSP\n"
            "-p power_boost: Run power boost Run power boost using the request type HAP_power_set_DCVS_v3 with HAP_DCVS_V2_PERFORMANCE_MODE\n"
            "-n iterations: iterations to run rpc performance tests\n"
            "-s size: Size of data buffer for rpc performance tests. If 0 is given the noop function will run\n"
            "-i ion_disable: Disable ION memory for rpc performance tests. Fastrpc uses ION memory by default\n"
            "-m memory_check: Verify memory operations for rpc performance tests by passing a fixed pattern to the DSP function which copies it to the output.\n"
            "       The caller then confirms the correct pattern is received.\n"
            "-u uncached: Use Uncached buffers on HLOS for rpc performance tests\n"
            "-q fastrpc_qos: Set the fastrpc QoS latency in usec. This can improve performance at some power cost.\n"
            "       QoS is turned ON by default. 0 will turn QoS OFF. Positive numbers set the QoS latency in usecs (300 is default)\n"
            "-e early_signal: Send signal to CPU for early wake up with approximate time to complete the job in usecs\n"
            "-y sleep_latency: Set sleep latency in usec. The higher the value, the deeper level of sleep is allowed when idle.\n"
            "       0 is default and means DCVS V3 vote will not be made. Minimum value is 10. Maximum value is 65535.\n"
            "-U unsigned_PD: Run on signed or unsigned PD.\n"
            "    0: Run on signed PD.\n"
            "    1: Run on unsigned PD.\n"
            "        Default Value: 1\n"
            );
}

int main(int argc, char* argv[])
{
    int retVal = AEE_SUCCESS;
    remote_handle64 handle = -1;

    int nErr = 0;
    char *function = "default";
    int domain = CDSP_DOMAIN_ID;
    int power_boost = 0;
    int uncached = 0;
    int iters = 1000;
    int ion = 1;
    int size = 0;
    int memory_check = 0;
    int qos = 300;
    int early_signal = 0;
    int latency = 0;
    int option = 0;
    uint32_t early_wakeup_time_us = 300;
    int unsigned_PD_flag = 1;
    bool isUnsignedPD_Enabled = false;

    while ((option = getopt(argc, argv,"d:f:pn:s:imuq:e:y:U:")) != -1) {
        switch (option) {
            case 'd' : domain = atoi(optarg);
                break;
            case 'f' : function = optarg;
                break;
            case 'p' : power_boost = 1;
                break;
            case 'n' : iters = atoi(optarg);
                break;
            case 's' : size = atoi(optarg);
                break;
            case 'i' : ion = 0;
                break;
            case 'm' : memory_check = 1;
                break;
            case 'u' : uncached = 1;
                break;
            case 'q' : qos = atoi(optarg);
                break;
            case 'e' : early_signal = 1; early_wakeup_time_us = atoi(optarg);
                break;
            case 'y' : latency = atoi(optarg);
                break;
            case 'U' : unsigned_PD_flag = atoi(optarg);
                break;
            default:
                print_usage();
                return -1;
        }
    }

    if (size < 0) {
        printf("ERROR 0x%x: Negative value: Size %d\n", retVal, size);
        print_usage();
        return -1;
    }

    if (iters <= 0) {
        printf("ERROR 0x%x: Invalid value: Iterations %d\n", retVal, iters);
        print_usage();
        return -1;
    }

    if (qos < 0) {
        printf("ERROR 0x%x: Negative value: QoS %d\n", retVal, qos);
        print_usage();
        return -1;
    }

    if (latency != 0 && (latency < MIN_SLEEP_LATENCY || latency > MAX_SLEEP_LATENCY)) {
        printf("ERROR 0x%x: Invalid value: latency %d\n", retVal, latency);
        print_usage();
        return -1;
    }
    rpcmem_init();

    if(domain == -1) {
        printf("\nDSP domain is not provided. Retrieving DSP information using Remote APIs.\n");
        retVal = get_dsp_support(&domain);
        if(retVal != AEE_SUCCESS) {
            printf("ERROR in get_dsp_support: 0x%x, defaulting to CDSP domain\n", retVal);
        }
    }

    if ((domain < ADSP_DOMAIN_ID) || (domain > CDSP_DOMAIN_ID)) {
        retVal = AEE_EBADPARM;
        printf("\nERROR 0x%x: Invalid domain %d\n", retVal, domain);
        goto bail;
    }

    if (domain == CDSP_DOMAIN_ID) {
        printf("Opening CDSP\n");
    }
    else if (domain == ADSP_DOMAIN_ID) {
        printf("Opening ADSP\n");
    }
    else {
        printf("ERROR: DSP not supported. Exiting.\n");
        print_usage();
        retVal = -1;
        goto err;
    }
    if(unsigned_PD_flag < 0 || unsigned_PD_flag > 1){
        retVal = AEE_EBADPARM;
        printf("\nERROR 0x%x: Invalid unsigned PD flag %d\n", retVal, unsigned_PD_flag);
        print_usage();
        goto bail;
    }
    if(unsigned_PD_flag == 1) {
        if (domain == CDSP_DOMAIN_ID) {
            isUnsignedPD_Enabled = get_unsignedpd_support();
        }
        else {
            printf("Overriding user request for unsigned PD. Only signed offload is allowed on domain %d.\n", domain);
            unsigned_PD_flag = 0;
        }
    }

    printf("Attempting to run on %s PD on domain %d\n", isUnsignedPD_Enabled==true?"unsigned":"signed", domain);
    retVal = open_session(&handle, domain, isUnsignedPD_Enabled);
    if( retVal != AEE_SUCCESS ) {
        printf("ERROR 0x%x: Open Session FAILED\n", retVal);
        goto bail;
    }

    if (qos > 0) {
        //Setting FastRPC QoS
        struct remote_rpc_control_latency data;
        data.enable = 1;
        data.latency = qos;
        if (remote_handle64_control)
            retVal = remote_handle64_control(handle, DSPRPC_CONTROL_LATENCY, (void*)&data, sizeof(data));
        else
            printf("remote_handle64_control not available on this target\n");
        if(retVal != 0)
        {
            printf("ERROR 0x%x: Failed to set QoS\n", retVal);
            printf("Exiting...\n");
            retVal = -1;
            goto bail;
        }
    }

    if (latency) {
        retVal = profiling_sleep_latency(handle, latency);
        if (retVal == AEE_EUNSUPPORTED + DSP_OFFSET) {
            printf("Failed to set sleep latency. This is not supported on some devices. Please remove the -y option.\n");
            printf("Exiting...\n");
            goto bail;
        }
        else if (retVal < 0){
            printf("Failed to set sleep latency. Check adb logcat for details.\n");
            printf("Exiting...\n");
            goto bail;
        }
    }

    if (power_boost) {
        retVal = profiling_power_boost(handle, 1);
        if (retVal == AEE_EUNSUPPORTED + DSP_OFFSET) {
            printf("Failed to set power boost. This is not supported on some devices. Please remove the -p option.\n");
            printf("Exiting...\n");
            goto bail;
        }
        else if (retVal < 0){
            printf("Failed to set power boost. Check adb logcat for details.\n");
            printf("Exiting...\n");
            goto bail;
        }
    }

    if(early_signal) {
        //Signal CPU for early wake up
        retVal = profiling_early_signal(handle, early_wakeup_time_us);
        if(retVal != 0)
        {
            printf("Error Code: 0x%x Failed to send early wakeup signal\n", retVal);
            printf("Exiting...\n");
            goto bail;
        }
    }

    printf("Operation: %s\n", function);
    printf("Iterations: %d\tBuffer size: %d\n", iters, size);
    printf("Power boost: %s\tION Memory: %s\n", power_boost?"On":"Off", ion?"Enabled":"Disabled");
    printf("Cache: %s\t\tCheck Memory: %s\n", uncached?"Uncached":"Cached", memory_check?"Yes":"No");
    printf("FastRPC QoS: %s \n", qos?"On":"Off");
    printf("Early CPU Wakeup Signal: %s \n", early_signal?"On":"Off");
    printf("Sleep Latency: %s \n", latency?"On":"Off");

    if (!strncmp(function, "timers", 10)) {
        printf("\n\n----Running the timer demonstrations----\n");
        VERIFY(0 == (retVal = profiling_with_timers(handle)));
        printf("----Completed the timer demonstrations----\n\n\n");
    }
    else if (!strncmp(function, "default", 10)) {
        VERIFY(0 == (retVal = rpcperf_default(handle, ion)));
    }
    else if (!strncmp(function, "noop", 10) || size == 0) {
        VERIFY(0 == (retVal = rpcperf_noop(handle, iters)));
    }
    else if (!strncmp(function, "inbuf", 10)) {
        VERIFY(0 == (retVal = rpcperf_inbuf(handle, uncached, size, ion, memory_check, iters)));
    }
    else if (!strncmp(function, "routbuf", 10)) {
        VERIFY(0 == (retVal = rpcperf_routbuf(handle, uncached, size, ion, memory_check, iters)));
    }
    else {
        printf ("Wrong function name.\n");
        print_usage();
        retVal = -1;
    }

    //Turning off the power boost if on, setting back to defaults
    if (power_boost) {
        retVal = profiling_power_boost(handle, 0);
        if(retVal != 0)
        {
            printf("ERROR 0x%x: Failed to reset Power boost.\n", retVal);
            printf("Exiting...\n");
            goto bail;
        }
    }
    //Resetting sleep latency
    if (latency) {
        retVal = profiling_sleep_latency(handle, MAX_SLEEP_LATENCY);
        if(retVal != 0)
        {
            printf("ERROR 0x%x: Failed to reset sleep latency.\n", retVal);
            printf("Exiting...\n");
            goto bail;
        }
    }

bail:
    if (retVal) {
        printf("Profiling example failed with ERROR = 0x%x.\n\n", retVal);
    }
    else {
        printf("Profiling example is successful.\n\n");
    }
    if(handle)
        profiling_close(handle);
err:
    rpcmem_deinit();
    return retVal;
}
