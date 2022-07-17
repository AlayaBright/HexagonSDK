/**=============================================================================

@file
   qprintf_example_imp.cpp

@brief
   implementation file for testing qprintf.

All Rights Reserved Qualcomm Proprietary
Copyright (c) 2017, 2020 Qualcomm Technologies Incorporated.
=============================================================================**/

//==============================================================================
// Include Files
//==============================================================================

// enable message outputs for profiling by defining _DEBUG and including HAP_farf.h
#ifndef _DEBUG
#define _DEBUG
#endif

#include "HAP_farf.h"
#undef FARF_HIGH
#define FARF_HIGH 1

#include "HAP_perf.h"
#include "HAP_power.h"

#include "qprintf_example.h"
#include "worker_pool.h"

#include "AEEStdErr.h"

// includes
#include "qprintf.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "hexagon_protos.h"
#include "hexagon_types.h"

extern void qprintf_example_asm();


/*===========================================================================
    DEFINITIONS
===========================================================================*/

typedef struct
{
    worker_synctoken_t  *token;            // worker pool token
    int32_t                 retval;            // return value
} run_callback_t;


// Placeholder structure to create a unique handle for a FastRPC session
typedef struct {
    int element;
} qprintf_example_context_t;



/*===========================================================================
    DECLARATIONS
===========================================================================*/

static int setClocks(remote_handle64 h)
{
    HAP_power_request_t request;
    memset(&request, 0, sizeof(HAP_power_request_t)); //Important to clear the structure if only selected fields are updated.
    request.type = HAP_power_set_apptype;
    request.apptype = HAP_POWER_COMPUTE_CLIENT_CLASS;
    qprintf_example_context_t *qprintf_example_ctx = (qprintf_example_context_t*) h;
    int retval = HAP_power_set((void*) qprintf_example_ctx, &request);
    if (retval) return AEE_EFAILED;

    // Configure clocks & DCVS mode
    memset(&request, 0, sizeof(HAP_power_request_t)); //Important to clear the structure if only selected fields are updated.
    request.type = HAP_power_set_DCVS_v2;
    request.dcvs_v2.dcvs_enable = 0;   // enable dcvs if desired, else it locks to target corner
    request.dcvs_v2.set_dcvs_params = TRUE;
    request.dcvs_v2.dcvs_params.target_corner = HAP_DCVS_VCORNER_NOM; // nominal voltage corner.
    request.dcvs_v2.set_latency = TRUE;
    request.dcvs_v2.latency = 100;
    retval = HAP_power_set((void*) qprintf_example_ctx, &request);
    if (retval) return AEE_EFAILED;

    return 0;
}

static void run_callback(void* data) {
    run_callback_t    *dptr = (run_callback_t*)data;

    // 1. Demonstrating how to use qprintf functions from C

    qprintf_set_mask(QPRINTF_MASK_ODD_32,QPRINTF_MASK_EVEN_32);

    // 2. Demonstrating how to use qprintf functions from assembly
    qprintf_example_asm();

    HVX_Vector x = Q6_V_vsplat_R(-1);
    HVX_VectorPred pred = Q6_Q_vand_VR(x,-1);
    qprintf_V("x = %d\n",x);
    qprintf_V("%d is x\n",x);
    qprintf_Q("%x is pred\n",pred);
    qprintf_Q("pred = %dddd\n",pred);

    printf("Printing all V registers\n");
    qprintf_V_all();
    printf("Printing all R registers\n");
    qprintf_R_all();

    FARF(HIGH, "SUCCESS");

    dptr->retval = 0;

    worker_pool_synctoken_jobdone(dptr->token);
}


AEEResult qprintf_example_open(const char *uri, remote_handle64 *h)
{
    qprintf_example_context_t *qprintf_example_ctx = malloc(sizeof(qprintf_example_context_t));

    if(qprintf_example_ctx==NULL)
    {
        FARF(ERROR, "Failed to allocate memory for the qprintf_example context");
        return AEE_ENOMEMORY;
    }

    *h = (remote_handle64) qprintf_example_ctx;

    return 0;
}

AEEResult qprintf_example_close(remote_handle64 h)
{
    qprintf_example_context_t *qprintf_example_ctx = (qprintf_example_context_t*) h;

    if(qprintf_example_ctx==NULL)
    {
        FARF(ERROR, "NULL handle received in qprintf_example_close()");
        return AEE_EBADHANDLE;
    }

    free(qprintf_example_ctx);

    return 0;
}

AEEResult qprintf_example_run(remote_handle64 h)
{
#if (__HEXAGON_ARCH__ < 65)
    FARF(ERROR, "qprintf example supports hexagon architectures >= 65 \n");
    return AEE_EVERSIONNOTSUPPORT;
#endif
    // setClocks will boost clocks
    if (setClocks(h)) return AEE_EFAILED;

    // execute test case from worker thread pool (because these threads have
    // larger stacks than the native thread from the RPC call).
    worker_pool_job_t   job;
    worker_synctoken_t  token;
    worker_pool_context_t context;
    run_callback_t dptr;
    dptr.retval = 0;

    int numWorkers = 1;
    dptr.token = &token;
    dptr.retval = -1;
    job.dptr = (void *)&dptr;

    //Init worker pool with custom stack size of each worker thread and get context
    int thread_stack_size = 65536;
    (void)worker_pool_init_with_stack_size(&context, thread_stack_size);
    worker_pool_synctoken_init(&token, numWorkers);
    job.fptr = run_callback;
    for (int i = 0; i < numWorkers; i++) (void) worker_pool_submit(context, job);
    worker_pool_synctoken_wait(&token);
    if (dptr.retval)
    {
        FARF(HIGH, "qprintf example test FAILURE 0x%x", dptr.retval);
        goto bail;
    }
    //Deinit worker pool and release resources
    worker_pool_deinit(&context);

    FARF(HIGH, "qprintf example test SUCCESS");
bail:
    return dptr.retval;

}
