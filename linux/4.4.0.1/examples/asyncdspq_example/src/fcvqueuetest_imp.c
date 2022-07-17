/*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <AEEStdErr.h>
#include <HAP_perf.h>
#include <HAP_power.h>
#include <HAP_farf.h>
#include <string.h>
#include <stdlib.h>
#include "fcvqueuetest.h"
#include "dlfcn.h"


// Placeholder structure to create a unique handle for a FastRPC session
typedef void (*fcv_filter_dilate3x3u8)(const uint8_t* , uint32_t, uint32_t, uint8_t*);
typedef void (*fcv_filter_gaussian3x3u8)(const uint8_t* , uint32_t, uint32_t, uint8_t*, int);
typedef void (*fcv_filter_median3x3u8)(const uint8_t* , uint32_t, uint32_t, uint8_t*);

typedef struct {
    int element;
    void *obj_fcvlib;
    fcv_filter_dilate3x3u8 dilate_3x3;
    fcv_filter_gaussian3x3u8 gaussian_3x3;
    fcv_filter_median3x3u8 median_3x3;
} fcvqueuetest_context_t;


AEEResult fcvqueuetest_open(const char *uri, remote_handle64 *h)
{
    fcvqueuetest_context_t *fcvqueuetest_ctx = malloc(sizeof(fcvqueuetest_context_t));

    if(fcvqueuetest_ctx==NULL)
    {
        FARF(ERROR, "Failed to allocate memory for the fcvqueuetest context");
        return AEE_ENOMEMORY;
    }
    memset(fcvqueuetest_ctx, 0, sizeof(fcvqueuetest_context_t));
    // Load the fastCV library
    // The library is part of the DSP image and will be available on the target
    fcvqueuetest_ctx->obj_fcvlib = dlopen("libfastcvadsp.so", RTLD_NOW);
    if (fcvqueuetest_ctx->obj_fcvlib == NULL) {
        FARF(ERROR,"Failed to open libfastcvadsp.so file ");
        return AEE_EUNABLETOLOAD;
    }

    fcvqueuetest_ctx->dilate_3x3 = dlsym(fcvqueuetest_ctx->obj_fcvlib, "fcvFilterDilate3x3u8");
    if(fcvqueuetest_ctx->dilate_3x3 == NULL) {
        FARF(ERROR,"Failed to load the symbol: %s from libfastcvadsp.so", "fcvFilterDilate3x3u8");
        return AEE_EUNABLETOLOAD;
    }

    fcvqueuetest_ctx->gaussian_3x3 = dlsym(fcvqueuetest_ctx->obj_fcvlib, "fcvFilterGaussian3x3u8");
    if(fcvqueuetest_ctx->gaussian_3x3 == NULL) {
        FARF(ERROR,"Failed to load the symbol: %s from libfastcvadsp.so", "fcvFilterGaussian3x3u8");
        return AEE_EUNABLETOLOAD;
    }

    fcvqueuetest_ctx->median_3x3 = dlsym(fcvqueuetest_ctx->obj_fcvlib, "fcvFilterMedian3x3u8");
    if(fcvqueuetest_ctx->median_3x3 == NULL) {
        FARF(ERROR,"Failed to load the symbol: %s from libfastcvadsp.so", "fcvFilterMedian3x3u8");
        return AEE_EUNABLETOLOAD;
    }

    *h = (remote_handle64) fcvqueuetest_ctx;

    return AEE_SUCCESS;
}


AEEResult fcvqueuetest_close(remote_handle64 h)
{
    fcvqueuetest_context_t *fcvqueuetest_ctx = (fcvqueuetest_context_t*) h;

    if(fcvqueuetest_ctx==NULL)
    {
        FARF(ERROR, "NULL handle received in fcvqueuetest_close()");
        return AEE_EBADHANDLE;
    }

    dlclose(fcvqueuetest_ctx->obj_fcvlib);

    free(fcvqueuetest_ctx);

    return AEE_SUCCESS;
}


AEEResult fcvqueuetest_fastcv_dilate3x3(remote_handle64 h, const uint8 *src, int srclen,
                                        uint8 *dest, int destlen,
                                        uint32 width, uint32 height, uint32 stride) {
    fcvqueuetest_context_t *fcvqueuetest_ctx = (fcvqueuetest_context_t*) h;
    if ( (srclen != destlen) ||
         (srclen != (stride * height)) ) {
        return AEE_EBADPARM;
    }
    fcvqueuetest_ctx->dilate_3x3(src, width, height, dest);
    return AEE_SUCCESS;
}


AEEResult fcvqueuetest_fastcv_gaussian3x3(remote_handle64 h, const uint8 *src, int srclen,
                                          uint8 *dest, int destlen,
                                          uint32 width, uint32 height, uint32 stride) {
    fcvqueuetest_context_t *fcvqueuetest_ctx = (fcvqueuetest_context_t*) h;
    if ( (srclen != destlen) ||
         (srclen != (stride * height)) ) {
        return AEE_EBADPARM;
    }
    fcvqueuetest_ctx->gaussian_3x3(src, width, height, dest, 1);
    return AEE_SUCCESS;
}


AEEResult fcvqueuetest_fastcv_median3x3(remote_handle64 h, const uint8 *src, int srclen,
                                        uint8 *dest, int destlen,
                                        uint32 width, uint32 height, uint32 stride) {
    fcvqueuetest_context_t *fcvqueuetest_ctx = (fcvqueuetest_context_t*) h;
    if ( (srclen != destlen) ||
         (srclen != (stride * height)) ) {
        return AEE_EBADPARM;
    }
    fcvqueuetest_ctx->median_3x3(src, width, height, dest);
    return AEE_SUCCESS;
}


AEEResult fcvqueuetest_set_clocks(remote_handle64 h) {

    int err;

    // Set client class
    HAP_power_request_t request;
    memset(&request, 0, sizeof(HAP_power_request_t));
    request.type = HAP_power_set_apptype;
    request.apptype = HAP_POWER_COMPUTE_CLIENT_CLASS;
    fcvqueuetest_context_t *fcvqueuetest_ctx = (fcvqueuetest_context_t*) h;
    if ( (err = HAP_power_set((void*) fcvqueuetest_ctx, &request)) != 0 ) {
        return err;
    }

    // Set to turbo and disable DCVS
    memset(&request, 0, sizeof(HAP_power_request_t));
    request.type = HAP_power_set_DCVS_v2;
    request.dcvs_v2.dcvs_enable = FALSE;
    request.dcvs_v2.set_dcvs_params = TRUE;
    request.dcvs_v2.dcvs_params.min_corner = HAP_DCVS_VCORNER_DISABLE;
    request.dcvs_v2.dcvs_params.max_corner = HAP_DCVS_VCORNER_DISABLE;
    request.dcvs_v2.dcvs_params.target_corner = HAP_DCVS_VCORNER_TURBO;
    request.dcvs_v2.set_latency = TRUE;
    request.dcvs_v2.latency = 100;
    if ( (err = HAP_power_set((void*) fcvqueuetest_ctx, &request)) != 0 ) {
        return err;
    }

    // Vote for HVX power
    memset(&request, 0, sizeof(HAP_power_request_t));
    request.type = HAP_power_set_HVX;
    request.hvx.power_up = TRUE;
    return HAP_power_set((void*) fcvqueuetest_ctx, &request);
}
