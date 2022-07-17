/**=============================================================================
Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

#include "hap_example.h"

#include "remote.h"

#include "HAP_farf.h"
#include "HAP_power.h"

#include "AEEStdErr.h"


/*
 * HAP_power APIs are used to control the DSP core and bus clocks based on the power and performance needs
 */


AEEResult hap_example_power(remote_handle64 h)
{
    int retVal = 0;

    unsigned int max_mips;
    unsigned int max_bus_bw;
    int client_class;
    unsigned int clk_freq_hz;
    boolean dcvs_enabled;
    void *context_ptr = NULL;

    HAP_power_response_t response;

    FARF(ALWAYS, "\nHAP_power_get:\n");
    context_ptr = HAP_utils_create_context();

    /*
     * HAP_power_get_max_mips : Returns the maximum MIPS supported
     * output : max_mips
     */
    memset(&response, 0, sizeof(HAP_power_response_t));
    response.type = HAP_power_get_max_mips;

    retVal = HAP_power_get(context_ptr, &response);
    if(retVal!=AEE_SUCCESS)
    {
        FARF(ERROR, "Unable to get the maximum MIPS supported\n");
        return AEE_EFAILED;
    }

    max_mips = response.max_mips;


    /*
     * HAP_power_get_max_bus_bw : Returns the maximum bus bandwidth supported
     * output : max_bus_bw
     */
    memset(&response, 0, sizeof(HAP_power_response_t));
    response.type = HAP_power_get_max_bus_bw;
    retVal = HAP_power_get(context_ptr, &response);
    if(retVal!=AEE_SUCCESS)
    {
        FARF(ERROR, "Unable to get the maximum bus bandwidth supported\n");
        return AEE_EFAILED;
    }

    max_bus_bw = response.max_bus_bw;


    /*
     * HAP_power_get_client_class : Returns the client class:
     *     0x00 - Unknown Client Class
     *     0x01 - Audio Client Class
     *     0x02 - Voice Client Class
     *     0x04 - Compute Client Class
     *     0x08 - Camera Streaming with 1 HVX Client Class
     *     0x10 - Camera Streaming with 2 HVX Client Class
     *
     * output : client_class
     */
    memset(&response, 0, sizeof(HAP_power_response_t));
    response.type = HAP_power_get_client_class;
    retVal = HAP_power_get(context_ptr, &response);
    if(retVal!=AEE_SUCCESS)
    {
        FARF(ERROR, "Unable to get the client class\n");
        return AEE_EFAILED;
    }

    client_class = response.client_class;


    /*
     * HAP_power_get_clk_Freq : Returns the Core Clock Frequency
     * output : clk_freq_hz
     */
    memset(&response, 0, sizeof(HAP_power_response_t));
    response.type = HAP_power_get_clk_Freq;
    retVal = HAP_power_get(context_ptr, &response);
    if(retVal!=AEE_SUCCESS)
    {
        FARF(ERROR, "Unable to get the DSP core clock frequency\n");
        return AEE_EFAILED;
    }

    clk_freq_hz = response.clkFreqHz;


    /*
     * HAP_power_get_dcvsEnabled : Returns the DCVS status : 0 - disabled; 1 - enabled
     * output : dcvs_enabled
     */
    memset(&response, 0, sizeof(HAP_power_response_t));
    response.type = HAP_power_get_dcvsEnabled;
    retVal = HAP_power_get(context_ptr, &response);
    if(retVal!=AEE_SUCCESS)
    {
        FARF(ERROR, "Unable to get the DCVS status\n");
        return AEE_EFAILED;
    }

    dcvs_enabled = response.dcvsEnabled;

    if(context_ptr)
    {
        HAP_utils_destroy_context(context_ptr);
        context_ptr = NULL;
    }

    FARF(ALWAYS, "\nMaximum MIPS of DSP = %u\nMaximum Bus Bandwidth supported = %u bytes/second"
                 "\nClient Class is %x\nCore clock frequency of the DSP is %u\nDCVS status is %d\n",
                  max_mips, max_bus_bw, client_class, clk_freq_hz, dcvs_enabled);


    /*
     * DCVS_V3 Interface : Demonstrate usage of the DCVS_V3 interface for voting DCVS and Latency parameters
     *                     Set the request.type to HAP_power_set_DCVS_v3
     */
    HAP_power_request_t request;
    memset(&request, 0, sizeof(HAP_power_request_t));
    request.type = HAP_power_set_DCVS_v3;

    /*
     * dcvs_option : Enables the user to request for a DCVS mode from the following options:
     *                  HAP_DCVS_V2_ADJUST_UP_DOWN
     *                  HAP_DCVS_V2_ADJUST_ONLY_UP
     *                  HAP_DCVS_V2_POWER_SAVER_MODE
     *                  HAP_DCVS_V2_POWER_SAVER_AGGRESSIVE_MODE
     *                  HAP_DCVS_V2_PERFORMANCE_MODE
     *                  HAP_DCVS_V2_DUTY_CYCLE_MODE
     */

    /*
     * HAP_DCVS_V2_PERFORMANCE_MODE :
     *                  DCVS can both increase and decrease the core/bus clock speeds
     *                  min_corner and max_corner votes are used as lower and upper limit guidelines
     *                  DCVS selects a set of aggressive thresholds in terms of performance
     *                  DCVS can quickly bump up the clocks in this mode assisting higher performance at the cost of power.
     */

    request.dcvs_v3.dcvs_enable = TRUE;
    request.dcvs_v3.dcvs_option = HAP_DCVS_V2_PERFORMANCE_MODE;


    /*
     * sleep_latency : To request for sleep latency in micro-seconds.
     *                 Sleep latency is the minimum time before which the DSP sleeps
     *                 Set latency to 65535 to reset it to the default value
     */
    request.dcvs_v3.set_latency = TRUE;
    request.dcvs_v3.latency = 1000;


    /* DCVS params: To update DCVS thresholds and target corner vote.
     *              target_corner, min_corner and max_corner can be set to one of the following options:
     *                  HAP_DCVS_VCORNER_DISABLE
     *                  HAP_DCVS_VCORNER_SVS2
     *                  HAP_DCVS_VCORNER_SVS
     *                  HAP_DCVS_VCORNER_SVS_PLUS
     *                  HAP_DCVS_VCORNER_NOM
     *                  HAP_DCVS_VCORNER_NOM_PLUS
     *                  HAP_DCVS_VCORNER_TURBO
     *                  HAP_DCVS_VCORNER_TURBO_PLUS
     *                  HAP_DCVS_VCORNER_MAX = 255
     */

    /*
     * We set the min_corner to SVS corner, max_corner to TURBO corner and the target_corner to NOMINAL corner.
     */
    request.dcvs_v3.set_bus_params = TRUE;
    request.dcvs_v3.bus_params.min_corner = HAP_DCVS_VCORNER_SVS;
    request.dcvs_v3.bus_params.max_corner = HAP_DCVS_VCORNER_TURBO;
    request.dcvs_v3.bus_params.target_corner = HAP_DCVS_VCORNER_NOM;


    /*
     * HAP_power_set : Sets the values based on the HAP_power_request_t structure provided
     * Input Parameters :
     *     context - A unique identifier to differentiate the clients used for voting
     *     request - The power request that is passed using the HAP_power_request_t structure
     *
     * returns:  0 on success, AEE_EMMPMREGISTER on MMPM client register request failure, -1 on unknown error
     */

    /*
     * The FastRPC session handle is passed as the context argument for power voting
     *
     * This handle is created by calling the hap_example_open() function present in src_dsp/hap_example_imp.c and is assigned
     * the start address of a heap memory allocated for a placeholder structure using malloc().
     * The handle remains unique for each session as long as the memory corresponding to this heap address is not freed.
     * At the end of the session, the hap_example_close() function present in src_dsp/hap_example_imp.c is called and this memory is freed.
     *
     */
    void* hap_example_power_ctx = (void*) h;
    retVal = HAP_power_set(hap_example_power_ctx, &request);
    if (retVal == HAP_POWER_ERR_UNSUPPORTED_API) {
        FARF(ALWAYS, "HAP_power_set not supported on target\n");
        retVal = AEE_SUCCESS;
        goto bail;
    }

    if(retVal!=AEE_SUCCESS)
    {
        FARF(ERROR, "Error Code 0x%x returned from function : HAP_power_set\n", retVal);
        goto bail;
    }

    FARF(ALWAYS, "Using DCVS_V3 interface for voting DCVS, latency and voltage corners:\n");
    FARF(ALWAYS, "dcvs_option set to %d - HAP_DCVS_V2_PERFORMANCE_MODE\n", request.dcvs_v3.dcvs_option);
    FARF(ALWAYS, "sleep latency set to %d", request.dcvs_v3.latency);
    FARF(ALWAYS, "dcvs params set to min_corner : %d - HAP_DCVS_VCORNER_SVS, max_corner : %d - HAP_DCVS_VCORNER_TURBO and target_corner : %d - HAP_DCVS_VCORNER_NOM\n",
                  request.dcvs_v3.bus_params.min_corner, request.dcvs_v3.bus_params.max_corner, request.dcvs_v3.bus_params.target_corner);


    /*
     * To remove all Votes and only set the DCVS option to the default DCVS mode: HAP_DCVS_V2_POWER_SAVER_MODE
     */
    FARF(ALWAYS, "Removing all votes and set the dcvs_option to %d - HAP_DCVS_V2_POWER_SAVER_MODE", request.dcvs_v3.dcvs_option);

    memset(&request, 0, sizeof(HAP_power_request_t));
    request.type = HAP_power_set_DCVS_v3;
    request.dcvs_v3.dcvs_enable = TRUE;
    request.dcvs_v3.dcvs_option = HAP_DCVS_V2_POWER_SAVER_MODE;
    retVal = HAP_power_set((void*) hap_example_power_ctx, &request);

    if(retVal!=AEE_SUCCESS)
    {
        FARF(ERROR, "Error Code 0x%x returned from function : HAP_power_set\n", retVal);
        goto bail;
    }


    /*
     * HAP_power_set_sleep_mode: Uses the DCVS_V3 interface to disable all low power modes
     */
    FARF(ALWAYS, "\nHAP_power_set_sleep_mode:\n");

    boolean sleep_disable = TRUE;
    retVal = HAP_power_set_sleep_mode((void*)hap_example_power_ctx, sleep_disable);

    if (retVal == HAP_POWER_ERR_UNSUPPORTED_API)
    {
        FARF(ALWAYS, "HAP_power_set_sleep_mode not supported on target\n");
        // Setting retVal to AEE_SUCCESS because this API is not supported on some targets
        retVal = AEE_SUCCESS;
        goto bail;
    }

    if(retVal!=AEE_SUCCESS)
    {
        FARF(ERROR, "Error Code 0x%x returned from function : HAP_power_set_sleep_mode\n", retVal);
        goto bail;
    }

    FARF(ALWAYS,"Disabled all low power modes");


bail:

    if(retVal==AEE_SUCCESS)
    {
        FARF(ALWAYS, "\nhap_example_power PASSED\n");
    }
    else
    {
        FARF(ALWAYS, "\nhap_example_power FAILED\n");
    }

    return retVal;
}
