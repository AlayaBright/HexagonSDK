/*=============================================================================
 * FILE:                      sysmon_vtcm_mgr_client.c
 *
 * DESCRIPTION:
 *    Vector TCM management client
 *
 * Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 * when         who          what, where, why
 * ----------   ------     ------------------------------------------------
 ============================================================================*/

/* ============================================================================
               *********** Includes ***********
   ========================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include "q6protos.h"
#include "qurt.h"
#define FARF_ERROR 1
#include "HAP_farf.h"
#include "sysmon_vtcmmgr_int.h"

/* ============================================================================
                            Variables and Defines
   ========================================================================= */
/* Stack size, should be multiple of 8 bytes */
#define SYSMON_VTCM_CLIENT_THREAD_STACK_SIZE            4096
#define SYSMON_VTCM_CLIENT_TIMEOUT_SIG                  2
#define SYSMON_VTCM_CLIENT_EXIT_SIG                     1
#define HAP_VTCM_ASYNC_REQUEST_MIN_TIMEOUT              100
#define SYSMON_VTCM_CALLBACK_TABLE_MAX                  8
#define SYSMON_VTCM_HMX_ENABLE_MAX                      8

typedef struct {
    unsigned long long mailboxId;
    qurt_anysignal_t sig;
    unsigned long long timeStamp;
    unsigned int timeout_us;
    unsigned int returnVal;
    unsigned int bComputeResource;
} sysmon_vtcm_th_arg_t;

typedef struct {
    int contextId;
    void* clientCtx;
    releaseCb callbackFunc;
} sysmon_vtcm_callback_entry_t;

typedef struct {
    int contextId;
    char bAcquireEnabled;
} sysmon_vtcm_hmx_enable_entry_t;

/* ============================================================================
                                 Declarations
   ========================================================================= */


/* ============================================================================
                                 Globals
   ========================================================================= */
static qurt_qdi_obj_t* vtcmMgrQdi = NULL;
unsigned int sysmon_vtcm_spinlock = 0;
static unsigned char callbackListenerActiveFlag = 0;
static unsigned long long callback_mailboxId = 0;
static sysmon_vtcm_callback_entry_t callback_table[SYSMON_VTCM_CALLBACK_TABLE_MAX];
static sysmon_vtcm_hmx_enable_entry_t hmx_enable_table[SYSMON_VTCM_HMX_ENABLE_MAX];
static char stack_callback[SYSMON_VTCM_CLIENT_THREAD_STACK_SIZE] __attribute__ ((aligned (16)));

/* ============================================================================
                                CODE STARTS HERE
   ========================================================================= */
int vtcmMgr_qdi_init(void)
{
    int result = 0;
    unsigned int *ptr = &sysmon_vtcm_spinlock;
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;

    if (!vtcmMgrQdi)
    {

        /* Acquire spin lock */
        __asm__  __volatile__ (
        "1: r0 = memw_locked(%0) \n"
        "    p0 = cmp.eq(r0, #0) \n"
        "    if (!p0) jump 1b \n"
        "    memw_locked(%0, p0) = %0 \n"
        "    if !p0 jump 1b \n"
        : "+r" (ptr) : : "p0","r0");

        if (!vtcmMgrQdi)
        {
            a1.ptr = (void *)&vtcmMgrQdi;
            a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
            QDI_vtcmMgr_invocation( 0, 0, QDI_OPEN,
                                   a1, a2, a3, a4, a5, a6, a7, a8, a9);
            if (NULL == vtcmMgrQdi)
            {
                result = -1;
            }
        }

        /* Release spin lock */
        sysmon_vtcm_spinlock = 0;
    }
    return result;
}

void* HAP_request_VTCM(unsigned int size, unsigned int single_page_flag)
{
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;
    void* pVA = 0;
    int result = 0;
    if (0 == vtcmMgr_qdi_init())
    {
        a1.num = size;
        a2.num = single_page_flag;
        a3.ptr = &pVA;
        a4.num = qurt_thread_get_priority(qurt_thread_get_id() );
        a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
        result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                      VTCMMGR_ALLOC,
                                      a1, a2, a3, a4, a5, a6, a7, a8, a9);
        if (0 != result)
        {
            pVA = 0;
        }
    }
    else
    {
        FARF(ERROR, "HAP_request_VTCM: driver open failed");
    }
    return pVA;
}

#ifndef SYSMON_VTCM_AYSNC_WRAP
static void vtcm_timeout_entry(void *arg)
{
    sysmon_vtcm_th_arg_t *inp_data = (sysmon_vtcm_th_arg_t *)arg;
    qurt_timer_t timer;
    qurt_timer_attr_t attr;
    unsigned long long currTimeStamp;
    unsigned int timeout_us;
    unsigned int sigMask;
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;

    qurt_timer_attr_init(&attr);
    currTimeStamp = qurt_sysclock_get_hw_ticks();

    if (currTimeStamp > inp_data->timeStamp)
    {
        timeout_us = ( ( (unsigned int)(currTimeStamp - inp_data->timeStamp) )
                       * 1000 ) / 19200;
    }
    else
    {
        timeout_us = (unsigned long)(UINT64_MAX
                            - inp_data->timeStamp + currTimeStamp);
        timeout_us = (timeout_us * 1000) / 19200;
    }

    if (inp_data->timeout_us > timeout_us)
    {
        timeout_us = inp_data->timeout_us - timeout_us;
        qurt_timer_attr_set_duration(&attr, timeout_us);

        if (QURT_EOK == qurt_timer_create(&timer, &attr, &inp_data->sig,
                    SYSMON_VTCM_CLIENT_TIMEOUT_SIG) )
        {

            sigMask = qurt_anysignal_wait(&inp_data->sig,
                                           (SYSMON_VTCM_CLIENT_TIMEOUT_SIG |
                                            SYSMON_VTCM_CLIENT_EXIT_SIG) );

            if (sigMask & SYSMON_VTCM_CLIENT_TIMEOUT_SIG)
            {
                if (inp_data->bComputeResource)
                {
                    FARF(ALWAYS, "Compute resource acquire: timeout triggered");
                    a1.ptr = &inp_data->mailboxId;
                    a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
                    QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                           COMPUTE_RES_MGR_REQUEST_ASYNC_CANCEL,
                                           a1, a2, a3, a4, a5, a6, a7, a8, a9);
                }
                else
                {
                    FARF(ALWAYS, "HAP_request_async_VTCM: timeout triggered");
                    a1.ptr = &inp_data->mailboxId;
                    a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
                    QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                           VTCMMGR_ALLOC_ASYNC_CANCEL,
                                           a1, a2, a3, a4, a5, a6, a7, a8, a9);
                }

                inp_data->returnVal = SYSMON_VTCM_CLIENT_TIMEOUT_SIG;
            }
            else
            {
                inp_data->returnVal = SYSMON_VTCM_CLIENT_EXIT_SIG;
            }

            qurt_timer_delete(timer);
        }
        else
        {
            if (inp_data->bComputeResource)
            {
                FARF(ERROR, "Compute resource acquire: failed to create a timer "
                        "instance");
                /* Unable to create a timeout instance, inject a timeout
                 * immediately */
                a1.ptr = &inp_data->mailboxId;
                a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
                QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                       COMPUTE_RES_MGR_REQUEST_ASYNC_CANCEL,
                                       a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }
            else
            {
                FARF(ERROR, "HAP_request_async_VTCM: failed to create a timer "
                        "instance");
                /* Unable to create a timeout instance, inject a timeout
                 * immediately */
                a1.ptr = &inp_data->mailboxId;
                a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
                QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                       VTCMMGR_ALLOC_ASYNC_CANCEL,
                                       a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }

            inp_data->returnVal = SYSMON_VTCM_CLIENT_TIMEOUT_SIG;
        }
    }
    else
    {
        if (inp_data->bComputeResource)
        {
            /* Given timeout elapsed, set a signal already */
            a1.ptr = &inp_data->mailboxId;
            a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
            QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                   COMPUTE_RES_MGR_REQUEST_ASYNC_CANCEL,
                                   a1, a2, a3, a4, a5, a6, a7, a8, a9);
        }
        else
        {
            /* Given timeout elapsed, set a signal already */
            a1.ptr = &inp_data->mailboxId;
            a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
            QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                   VTCMMGR_ALLOC_ASYNC_CANCEL,
                                   a1, a2, a3, a4, a5, a6, a7, a8, a9);
        }

        inp_data->returnVal = SYSMON_VTCM_CLIENT_TIMEOUT_SIG;
    }

    qurt_thread_exit(0);
}

static void compute_res_callback_entry(void* arg)
{
    unsigned long long data = 0;
    releaseCb callback = NULL;
    void* client_ctx = NULL;
    int contextId = 0;
    int result = 0;

    unsigned long long mailboxId = *((unsigned long long *)arg);

    while (1)
    {
        if (QURT_EOK == (result = qurt_mailbox_receive(mailboxId,
                                        QURT_MAILBOX_RECV_WAITING, &data)))
        {
            contextId = (data & 0xFFFFFFFF);
            for (int i = 0; i < SYSMON_VTCM_CALLBACK_TABLE_MAX; i++)
            {
                if (callback_table[i].contextId == contextId)
                {
                    callback = callback_table[i].callbackFunc;
                    client_ctx = callback_table[i].clientCtx;
                    callback(contextId,client_ctx);
                    break;
                }
            }
        }
        else
        {
            FARF(HIGH, "wait on mailbox returned failure %d, returning "
                    "error to user", result);
            break;
        }
    }
}

void* HAP_request_async_VTCM(unsigned int size,
                             unsigned int single_page_flag,
                             unsigned int timeout_us)
{
    void* pVA = 0;
    int result = 0;
    unsigned long long mailboxId = 0;
    unsigned long long data = 0;
    unsigned long long timeStamp;
    void *stack = NULL;
    sysmon_vtcm_th_arg_t *inp_data = NULL;
    qurt_thread_attr_t attr;
    qurt_thread_t tid;
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;

    if (0 == vtcmMgr_qdi_init())
    {
        if (timeout_us < HAP_VTCM_ASYNC_REQUEST_MIN_TIMEOUT)
        {
            a1.num = size;
            a2.num = single_page_flag;
            a3.ptr = &pVA;
            a4.num = qurt_thread_get_priority(qurt_thread_get_id() );
            a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
            result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                            VTCMMGR_ALLOC,
                                            a1, a2, a3, a4, a5, a6, a7, a8, a9);
        }
        else
        {
            timeStamp = qurt_sysclock_get_hw_ticks();
            a1.num = size;
            a2.num = single_page_flag;
            a3.ptr = &pVA;
            a4.ptr = &mailboxId;
            a5.num = qurt_thread_get_priority(qurt_thread_get_id() );
            a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
            result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                            VTCMMGR_ALLOC_ASYNC,
                                            a1, a2, a3, a4, a5, a6, a7, a8, a9);
            if (0 == pVA)
            {
                FARF(HIGH, "Async request failed, return mailboxId: %llu",
                     mailboxId);

                if (mailboxId)
                {
                    stack = malloc(SYSMON_VTCM_CLIENT_THREAD_STACK_SIZE
                            + sizeof(sysmon_vtcm_th_arg_t) );

                    if (!stack)
                    {
                        FARF(ERROR, "HAP_request_async_VTCM: Failed to "
                                "allocate from heap for timeout");
                        a1.ptr = &mailboxId;
                        a2.ptr = &pVA;
                        a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
                        QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                               VTCMMGR_ALLOC_ASYNC_CANCEL,
                                               a1, a2, a3, a4, a5, a6, a7, a8, a9);
                        goto bail;
                    }

                    /* Update thread input data structure */
                    inp_data = (sysmon_vtcm_th_arg_t *)( (unsigned int)stack
                            + SYSMON_VTCM_CLIENT_THREAD_STACK_SIZE);
                    inp_data->mailboxId = mailboxId;
                    inp_data->timeout_us = timeout_us;
                    inp_data->timeStamp = timeStamp;
                    inp_data->bComputeResource = FALSE;
                    qurt_anysignal_init(&inp_data->sig);

                    /* Start a thread */
                    qurt_thread_attr_init(&attr);
                    qurt_thread_attr_set_name(&attr, "VTCM_TIMEOUT");
                    qurt_thread_attr_set_priority(&attr,
                              qurt_thread_get_priority(qurt_thread_get_id() ) );
                    qurt_thread_attr_set_stack_addr(&attr, stack);
                    qurt_thread_attr_set_stack_size(&attr,
                                         SYSMON_VTCM_CLIENT_THREAD_STACK_SIZE);
                    qurt_thread_attr_set_detachstate(&attr,
                                             QURT_THREAD_ATTR_CREATE_JOINABLE);
                    if (QURT_EOK != qurt_thread_create(&tid,
                                                       &attr,
                                                       vtcm_timeout_entry,
                                                       (void *)inp_data))
                    {
                        FARF(ERROR, "HAP_request_async_VTCM: failed to create "
                                "timer thread");
                        a1.ptr = &mailboxId;
                        a2.ptr = &pVA;
                        a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
                        QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                               VTCMMGR_ALLOC_ASYNC_CANCEL,
                                               a1, a2, a3, a4, a5, a6, a7, a8, a9);
                        goto bail;
                    }
                    /* We have set up the timeout now, wait for ack from VTCM
                     * driver
                     */
                    if (QURT_EOK == (result =
                                qurt_mailbox_receive(mailboxId,
                                        QURT_MAILBOX_RECV_WAITING, &data) ) )
                    {
                        pVA = (void *) (data & 0xFFFFFFFF);
                        a1.num = (mailboxId & 0xFFFFFFFF);
                        a2.num = (mailboxId >> 32);
                        a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
                        QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                               VTCMMGR_ALLOC_ASYNC_DONE,
                                               a1, a2, a3, a4, a5, a6, a7, a8, a9);
                        qurt_anysignal_set(&inp_data->sig,
                                                   SYSMON_VTCM_CLIENT_EXIT_SIG);
                        FARF(HIGH, "Received allocation success from VTCM manager,"
                             "return data: 0x%x", pVA);
                    }
                    else
                    {
                        FARF(HIGH, "wait on mailbox returned failure %d, returning "
                                "error to user", result);
                    }
                    /* Wait for the timeout thread to join */
                    qurt_thread_join(tid, NULL);
                }
                else
                {
                    FARF(ERROR, "HAP_request_async_VTCM: SRM driver didn't return "
                            "a mailboxId, return = %d", result);
                }
            }
        }
    }
    else
    {
        FARF(ERROR, "HAP_request_async_VTCM: driver open failed");
    }
bail:
    if (stack)
        free(stack);
    return pVA;
}
#else  //SYSMON_VTCM_AYSNC_WRAP
void* HAP_request_async_VTCM(unsigned int size,
                             unsigned int single_page_flag,
                             unsigned int timeout_us)
{
    return HAP_request_VTCM(size,
                            single_page_flag);
}
#endif

int HAP_release_VTCM(void* pVA)
{
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;

    if (0 != vtcmMgr_qdi_init())
    {
        FARF(ERROR, "HAP_release_VTCM: driver open failed");
        return HAP_VTCM_RELEASE_FAIL_QDI;
    }
    else
    {
        a1.ptr = pVA;
        a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
        return QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                      VTCMMGR_FREE,
                                      a1, a2, a3, a4, a5, a6, a7, a8, a9);
    }
}

int HAP_query_total_VTCM(unsigned int* page_size, unsigned int* page_count)
{
    int result = 0;
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;

    if (0 != vtcmMgr_qdi_init())
    {
        FARF(ERROR, "HAP_query_total_VTCM: driver open failed");
        if (page_size)
        {
            *page_size = 0;
        }
        if (page_count)
        {
            *page_count = 0;
        }
        result = HAP_VTCM_QUERY_FAIL_QDI;
    }
    else
    {
        a1.ptr = page_size;
        a2.ptr = page_count;
        a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
        result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                        VTCMMGR_QUERY_TOTAL,
                                        a1, a2, a3, a4, a5, a6, a7, a8, a9);
        if (0 != result)
        {
            FARF(ERROR, "HAP_query_total_VTCM: query failed %d", result);
            if (page_size)
            {
                *page_size = 0;
            }
            if (page_count)
            {
                *page_count = 0;
            }
            result = HAP_VTCM_QUERY_FAIL_QDI_INVOKE;
        }
    }
    return result;
}

int HAP_query_avail_VTCM(unsigned int* avail_block_size,
                          unsigned int* max_page_size,
                          unsigned int* num_pages)
{
    int result = 0;
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;

    if (0 != vtcmMgr_qdi_init())
    {
        FARF(ERROR, "HAP_query_avail_VTCM: driver open failed");
        if (avail_block_size)
        {
            *avail_block_size = 0;
        }
        if (max_page_size)
        {
            *max_page_size = 0;
        }
        if (num_pages)
        {
            *num_pages = 0;
        }
        result = HAP_VTCM_QUERY_FAIL_QDI;
    }
    else
    {
        a1.ptr = avail_block_size;
        a2.ptr = max_page_size;
        a3.ptr = num_pages;
        a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
        result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                        VTCMMGR_QUERY_AVAIL,
                                        a1, a2, a3, a4, a5, a6, a7, a8, a9);
        if (0 != result)
        {
            FARF(ERROR, "HAP_query_avail_VTCM: query failed %d", result);
            if (avail_block_size)
            {
                *avail_block_size = 0;
            }
            if (max_page_size)
            {
                *max_page_size = 0;
            }
            if (num_pages)
            {
                *num_pages = 0;
            }
            result = HAP_VTCM_QUERY_FAIL_QDI_INVOKE;
        }
    }
    return result;
}

int compute_resource_attr_init(compute_res_attr_t* res_info)
{
    if (!res_info)
        return -1;

    memset(res_info, 0, sizeof(compute_res_attr_t));
    res_info->attr.ver = COMPUTE_RES_COMPILED_VER;
    res_info->attr.bSerialize = 0;
    res_info->attr.bVtcmSinglePage = 0;
    res_info->attr.vtcmSize = 0;
    res_info->attr.pVtcm = NULL;
    res_info->attr.bHMX = 0;
    res_info->attr.minPageSize = 0;
    res_info->attr.minVtcmSize = 0;
    res_info->attr.appId = 0;
    res_info->attr.bCacheable = 0;
    res_info->attr.clientCtx = NULL;
    res_info->attr.releaseCallback = NULL;
    res_info->attr.outVtcmSize = 0;

    return 0;
}

int compute_resource_attr_set_serialize(compute_res_attr_t* res_info,
                                        unsigned char b_serialize)
{
    if (!res_info)
        return -1;

    res_info->attr.bSerialize = b_serialize;

    return 0;
}

int compute_resource_attr_set_hmx_param(compute_res_attr_t* res_info,
                                        unsigned char b_hmx_enable)
{
    if (!res_info)
        return -1;

    res_info->attr.bHMX = b_hmx_enable;

    return 0;
}

int compute_resource_attr_set_vtcm_param(compute_res_attr_t* res_info,
                                         unsigned int vtcm_size,
                                         unsigned char b_vtcmSinglePage)
{
    if (!res_info)
        return -1;

    res_info->attr.vtcmSize = vtcm_size;
    res_info->attr.bVtcmSinglePage = b_vtcmSinglePage;

    return 0;
}

int compute_resource_attr_set_vtcm_param_v2(compute_res_attr_t* res_info,
                                         unsigned int vtcm_size,
                                         unsigned int min_page_size,
                                         unsigned int min_vtcm_size)
{
    if (!res_info)
        return -1;

    res_info->attr.vtcmSize = vtcm_size;
    res_info->attr.minPageSize = min_page_size;
    res_info->attr.minVtcmSize = min_vtcm_size;

    return 0;
}

int compute_resource_attr_set_app_type(compute_res_attr_t* res_info,
                                        unsigned int application_id)
{
    if (!res_info)
        return -1;

    res_info->attr.appId = application_id;

    return 0;
}

int compute_resource_attr_set_cache_mode(compute_res_attr_t* res_info,
                                        unsigned char b_cache_enable)
{
    if (!res_info)
        return -1;

    res_info->attr.bCacheable = b_cache_enable;

    return 0;
}

static int compute_resource_open_cb_mailbox()
{
    int result = 0;
    unsigned int *ptr = &sysmon_vtcm_spinlock;
    void *stack = NULL;
    qurt_thread_attr_t attr_callback;
    qurt_thread_t tid_callback;
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;

    if (0 != (result = vtcmMgr_qdi_init()))
        return result;

    /* Acquire spin lock */
    __asm__  __volatile__ (
    "1: r0 = memw_locked(%0) \n"
    "    p0 = cmp.eq(r0, #0) \n"
    "    if (!p0) jump 1b \n"
    "    memw_locked(%0, p0) = %0 \n"
    "    if !p0 jump 1b \n"
    : "+r" (ptr) : : "p0","r0");

    if (!callbackListenerActiveFlag)
    {
        callbackListenerActiveFlag = 1;
        sysmon_vtcm_spinlock = 0;
    }
    else
    {
        sysmon_vtcm_spinlock = 0;
        return 0;
    }

    a1.ptr = &callback_mailboxId;
    a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
    /* Create a mailbox for the callback thread to wait on */
    result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                    COMPUTE_RES_MGR_REQUEST_MAILBOX,
                                    a1, a2, a3, a4, a5, a6, a7, a8, a9);

    if (result || !callback_mailboxId)
    {
        callbackListenerActiveFlag = 0;

        return -1;
    }

    //TODO : Figure out stack size for the thread
    stack = (void *)stack_callback;

    /* Start the thread */
    qurt_thread_attr_init(&attr_callback);
    qurt_thread_attr_set_name(&attr_callback, "COMPUTE_RES_CALLBACK_LISTENER");
    qurt_thread_attr_set_priority(&attr_callback,
                    qurt_thread_get_priority(qurt_thread_get_id() ) );
    qurt_thread_attr_set_stack_addr(&attr_callback, stack);
    qurt_thread_attr_set_stack_size(&attr_callback,
                                SYSMON_VTCM_CLIENT_THREAD_STACK_SIZE);
    if (QURT_EOK != qurt_thread_create( &tid_callback,
                                        &attr_callback,
                                        compute_res_callback_entry,
                                        (void *)&callback_mailboxId))
    {
        FARF(ERROR, "Failed to create callback listener thread");
        callbackListenerActiveFlag = 0;

        return -1;
    }

    return result;
}

int compute_resource_attr_set_release_callback(compute_res_attr_t* res_info,
                                               int (*release_callback)(
                                                   unsigned int context_id,
                                                   void* client_context),
                                               void* client_context)
{
    int result = 0;

    if (!res_info)
        return -1;

    if (!callbackListenerActiveFlag)
    {
        result = compute_resource_open_cb_mailbox();
    }

    if (!result)
    {
        res_info->attr.releaseCallback = (releaseCb)release_callback;
        res_info->attr.clientCtx = client_context;
    }

    return result;
}

void* compute_resource_attr_get_vtcm_ptr(compute_res_attr_t* res_info)
{
    if (!res_info)
        return NULL;

    return res_info->attr.pVtcm;
}

int compute_resource_attr_get_vtcm_ptr_v2(compute_res_attr_t* res_info,
                                        void** vtcm_ptr,
                                        unsigned int* vtcm_size)
{
    if (!res_info)
        return -1;

    *vtcm_ptr = res_info->attr.pVtcm;
    *vtcm_size = res_info->attr.outVtcmSize;

    return 0;
}

int compute_resource_validate_attr(compute_res_attr_t* res_info)
{
    if (!res_info)
        return -1;
    if ( (res_info->attr.ver < COMPUTE_RES_VER_START) ||
         (res_info->attr.ver > COMPUTE_RES_VER_END) )
        return -1;
    if (res_info->attr.bSerialize > 1)
        return -1;
    if (res_info->attr.bVtcmSinglePage > 1)
        return -1;
    if (res_info->attr.bHMX > 1)
        return -1;
    if (res_info->attr.bCacheable > 1)
        return -1;
    return 0;
}

static inline unsigned int least_page(unsigned int value)
{
    unsigned int zeros = Q6_R_ct0_R(value);
    zeros = (zeros >> 1) << 1;
    if (zeros > 24) zeros = 24;
    return (1 << zeros);
}

static inline unsigned int next_page(unsigned int value)
{
    unsigned int zeros = Q6_R_ct0_R(value);
    if (!value) return 0;
    zeros = ( (zeros + 1) >> 1) << 1;
    if (zeros > 24) zeros = 24;
    return (1 << zeros);
}

static inline unsigned int max_page(unsigned int value)
{
    unsigned int zeros = Q6_R_cl0_R(value);
    zeros = ( (31 - zeros) >> 1) << 1;
    if (zeros > 24) zeros = 24;
    return (1 << zeros);
}

static inline unsigned char page_to_index(unsigned int page)
{
    unsigned int zeros = Q6_R_ct0_R(page);
    zeros = (zeros - 12) >> 1;
    return (unsigned char)zeros;
}

static inline unsigned int index_to_page(unsigned char index)
{
    return (1 << ( (index * 2) + 12) );
}

int compute_resource_query_VTCM(unsigned int application_id,
                                unsigned int* total_block_size,
                                compute_res_vtcm_page_t* total_block_layout,
                                unsigned int* avail_block_size,
                                compute_res_vtcm_page_t* avail_block_layout)
{
    int result = 0, i;
    unsigned int total_block_VA = 0, avail_block_VA = 0;
    unsigned int total_size = 0;
    unsigned int size, addr, addr_page, size_page, final_page, list_length;
    unsigned int overmapping = 0, residual_page = 0;
    unsigned int array[7];
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;

    if (0 == (result = vtcmMgr_qdi_init()))
    {
        a1.num = application_id;
        a2.ptr = &total_size;
        a3.ptr = &total_block_VA;
        a4.ptr = avail_block_size;
        a5.ptr = avail_block_layout? &avail_block_VA: NULL;
        a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
        result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                        COMPUTE_RES_MGR_QUERY_VTCM,
                                        a1, a2, a3, a4, a5, a6, a7, a8, a9);

        if (0 == result)
        {

            if (total_block_size)
                *total_block_size = total_size;

            if (total_block_layout && total_block_VA)
            {
                size = total_size;
                addr = total_block_VA;

                for (i = 6; i >= 0 ; i--)
                    array[i] = 0;

                while (size)
                {
                    addr_page = least_page(addr);
                    size_page = max_page(size);

                    if (addr_page > size_page)
                        size_page = next_page(size);

                    final_page = (addr_page > size_page) ? size_page : addr_page;

                    addr += final_page;

                    if (size >= final_page)
                    {
                        array[page_to_index(final_page)] += 1;
                        size -= final_page;
                    }
                    else
                    {
                        residual_page = size;
                        size = 0;
                    }
                }

                list_length = 0;

                for (i = 6; i >= 0 ; i--)
                {
                    if (array[i])
                    {
                        total_block_layout->page_list[list_length].page_size = index_to_page(i);
                        total_block_layout->page_list[list_length].num_pages = array[i];
                        list_length++;
                    }
                }

                /*
                 * Check if there is a residual page (overmapped page).
                 * If there is one and we have available page list entry,
                 * set the page size to the true size and set num pages to 1
                 * as we can only have 1 overmapped entry.
                 */
                if ( (list_length < 6) && (residual_page) )
                {
                    total_block_layout->page_list[list_length].page_size = residual_page;
                    total_block_layout->page_list[list_length].num_pages = 1;
                    list_length++;
                }

                total_block_layout->page_list_len = list_length;
                total_block_layout->block_size = total_size;
            }

            if(avail_block_layout && avail_block_VA)
            {
                size = *avail_block_size;
                addr = avail_block_VA;

                if ( (addr + size) == (total_block_VA + total_size) )
                    overmapping = 1;

                for (i = 6; i >= 0 ; i--)
                    array[i] = 0;

                residual_page = 0;

                while (size)
                {
                    addr_page = least_page(addr);
                    size_page = max_page(size);

                    if (overmapping && (addr_page > size_page) )
                        size_page = next_page(size);

                    final_page = (addr_page > size_page) ? size_page : addr_page;

                    addr += final_page;

                    if (size >= final_page)
                    {
                        array[page_to_index(final_page)] += 1;
                        size -= final_page;
                    }
                    else
                    {
                        residual_page = size;
                        size = 0;
                    }
                }

                list_length = 0;

                for (i = 6; i >= 0 ; i--)
                {
                    if (array[i])
                    {
                        avail_block_layout->page_list[list_length].page_size = index_to_page(i);
                        avail_block_layout->page_list[list_length].num_pages = array[i];
                        list_length++;
                    }
                }

                /*
                 * Check if there is a residual page (overmapped page).
                 * If there is one and we have available page list entry,
                 * set the page size to the true size and set num pages to 1
                 * as we can only have 1 overmapped entry.
                 */
                if ( (list_length < 6) && (residual_page) )
                {
                    avail_block_layout->page_list[list_length].page_size = residual_page;
                    avail_block_layout->page_list[list_length].num_pages = 1;
                    list_length++;
                }

                avail_block_layout->page_list_len = list_length;
                avail_block_layout->block_size = *avail_block_size;
            }
        }
    }

    return result;
}

int compute_resource_check_release_request(unsigned int context_id)
{
    int result = 0;
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;

    if (0 == (result = vtcmMgr_qdi_init()))
    {
        a1.num = context_id;
        a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
        result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                        COMPUTE_RES_MGR_QUERY_RELEASE_CB_STATE,
                                        a1, a2, a3, a4, a5, a6, a7, a8, a9);
    }

    return result;
}

int compute_resource_release(unsigned int context_id)
{
    int result = 0, i;
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;

    if (0 == (result = vtcmMgr_qdi_init()))
    {
        a1.num = context_id;
        a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
        result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                        COMPUTE_RES_MGR_REQUEST_FREE,
                                        a1, a2, a3, a4, a5, a6, a7, a8, a9);
    }

    for (i = 0; i < SYSMON_VTCM_CALLBACK_TABLE_MAX; i++)
    {
        if (callback_table[i].contextId == context_id)
        {
            callback_table[i].contextId = 0;
            callback_table[i].clientCtx = NULL;
            callback_table[i].callbackFunc = NULL;
            break;
        }
    }

    for (i = 0; i < SYSMON_VTCM_HMX_ENABLE_MAX; i++)
    {
        if (hmx_enable_table[i].contextId == context_id)
        {
            hmx_enable_table[i].contextId = 0;
            hmx_enable_table[i].bAcquireEnabled = 0;
            break;
        }
    }

    return result;
}

unsigned int compute_resource_acquire(compute_res_attr_t* res_info,
                                      unsigned int timeout_us)
{
    void* pVA = 0;
    int result = 0;
    unsigned int contextId = 0;
    unsigned int size = 0;
    unsigned long long mailboxId = 0;
    unsigned long long data = 0;
    unsigned long long timeStamp;
    sysmon_vtcm_th_arg_t *inp_data_timeout = NULL;
    qurt_thread_attr_t attr_timeout;
    qurt_thread_t tid_timeout;
    void *stack_timeout = NULL;
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;

    if (0 != compute_resource_validate_attr(res_info) )
        return 0;

    if (0 == vtcmMgr_qdi_init())
    {
        if (timeout_us < HAP_VTCM_ASYNC_REQUEST_MIN_TIMEOUT)
        {
            a1.num = res_info->attr.ver;
            a2.ptr = res_info;
            a3.ptr = &pVA;
            a4.num = 0;
            a5.ptr = &contextId;
            a6.num = 0;
            a7.ptr = &size;
            a8.ptr = a9.ptr = NULL;
            result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                            COMPUTE_RES_MGR_REQUEST_ASYNC,
                                            a1, a2, a3, a4, a5, a6, a7, a8, a9);
        }
        else
        {
            timeStamp = qurt_sysclock_get_hw_ticks();
            a1.num = res_info->attr.ver;
            a2.ptr = res_info;
            a3.ptr = &pVA;
            a4.ptr = &mailboxId;
            a5.ptr = &contextId;
            a6.num = qurt_thread_get_priority(qurt_thread_get_id());
            a7.ptr = &size;
            a8.ptr = a9.ptr = NULL;
            result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                            COMPUTE_RES_MGR_REQUEST_ASYNC,
                                            a1, a2, a3, a4, a5, a6, a7, a8, a9);

            if (COMPUTE_RES_FAIL_RETRY_WITH_MAILBOX == result)
            {

                if (!contextId && mailboxId)
                {
                    stack_timeout = malloc(SYSMON_VTCM_CLIENT_THREAD_STACK_SIZE
                            + sizeof(sysmon_vtcm_th_arg_t) );

                    if (!stack_timeout)
                    {
                        a1.ptr = &mailboxId;
                        a2.ptr = &pVA;
                        a3.ptr = &contextId;
                        a4.ptr = &size;
                        a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
                        QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                               COMPUTE_RES_MGR_REQUEST_ASYNC_CANCEL,
                                               a1, a2, a3, a4, a5, a6, a7, a8, a9);
                        goto bail;
                    }

                    /* Update thread input data structure */
                    inp_data_timeout = (sysmon_vtcm_th_arg_t *)( (unsigned int)stack_timeout
                            + SYSMON_VTCM_CLIENT_THREAD_STACK_SIZE);
                    inp_data_timeout->mailboxId = mailboxId;
                    inp_data_timeout->timeout_us = timeout_us;
                    inp_data_timeout->timeStamp = timeStamp;
                    inp_data_timeout->bComputeResource = TRUE;
                    qurt_anysignal_init(&inp_data_timeout->sig);

                    /* Start a thread */
                    qurt_thread_attr_init(&attr_timeout);
                    qurt_thread_attr_set_name(&attr_timeout, "VTCM_TIMEOUT");
                    qurt_thread_attr_set_priority(&attr_timeout,
                                 qurt_thread_get_priority(qurt_thread_get_id() ) );
                    qurt_thread_attr_set_stack_addr(&attr_timeout, stack_timeout);
                    qurt_thread_attr_set_stack_size(&attr_timeout,
                                             SYSMON_VTCM_CLIENT_THREAD_STACK_SIZE);
                    if (QURT_EOK != qurt_thread_create(&tid_timeout,
                                                       &attr_timeout,
                                                       vtcm_timeout_entry,
                                                       (void *)inp_data_timeout))
                    {
                        FARF(ERROR, "HAP_request_async_VTCM: failed to create "
                                "timer thread");
                        a1.ptr = &mailboxId;
                        a2.ptr = &pVA;
                        a3.ptr = &contextId;
                        a4.ptr = &size;
                        a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
                        QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                               COMPUTE_RES_MGR_REQUEST_ASYNC_CANCEL,
                                               a1, a2, a3, a4, a5, a6, a7, a8, a9);
                        goto bail;
                    }
                    /* We have set up the timeout now, wait for ack from VTCM
                     * driver
                     */
                    if (QURT_EOK == (result =
                                qurt_mailbox_receive(mailboxId,
                                        QURT_MAILBOX_RECV_WAITING, &data) ) )
                    {
                        pVA = (void *) (data & 0xFFFFFFFF);
                        contextId = (data >> 32); //Extract MSB portion for context ID
                        a1.num = (mailboxId & 0xFFFFFFFF);
                        a2.num = (mailboxId >> 32);
                        a3.ptr = &size;
                        a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
                        QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                               COMPUTE_RES_MGR_REQUEST_ASYNC_DONE,
                                               a1, a2, a3, a4, a5, a6, a7, a8, a9);
                        qurt_anysignal_set(&inp_data_timeout->sig,
                                                   SYSMON_VTCM_CLIENT_EXIT_SIG);
                    }
                    else
                    {
                        FARF(HIGH, "wait on mailbox returned failure %d, returning "
                                "error to user", result);
                    }
                    /* Wait for the timeout thread to join */
                    qurt_thread_join(tid_timeout, NULL);
                }
                else if (!contextId)
                {
                    FARF(ERROR, "HAP_compute_res_acquire failed: no context id");
                }
            }
            else if (result)
            {
                FARF(ERROR, "HAP_compute_res_acquire failed: %d", result);
            }
        }
        if (contextId && res_info->attr.releaseCallback)
        {
            for (int i=0; i < SYSMON_VTCM_CALLBACK_TABLE_MAX; i++)
            {
                if (!callback_table[i].contextId)
                {
                    callback_table[i].contextId = contextId;
                    callback_table[i].clientCtx = res_info->attr.clientCtx;
                    callback_table[i].callbackFunc = res_info->attr.releaseCallback;
                    break;
                }
            }
        }
        if (contextId && res_info->attr.bHMX)
        {
            for (int i=0; i < SYSMON_VTCM_HMX_ENABLE_MAX; i++)
            {
                if (!hmx_enable_table[i].contextId)
                {
                    hmx_enable_table[i].contextId = contextId;

                    if (res_info->attr.bCacheable)
                        hmx_enable_table[i].bAcquireEnabled = 0;
                    else
                        hmx_enable_table[i].bAcquireEnabled = 1;

                    break;
                }
            }
        }
    }
    else
    {
        FARF(ERROR, "HAP_compute_res_acquire: driver open failed");
    }
bail:
    FARF(HIGH, "HAP_compute_res_acquire: context 0x%x, pVA 0x%x, size 0x%x", contextId, pVA, size);
    if (stack_timeout)
        free(stack_timeout);

    res_info->attr.pVtcm = pVA;
    res_info->attr.outVtcmSize = size;

    return contextId;
}

int compute_resource_acquire_cached(unsigned int contextId,
                                    unsigned int timeout_us)
{
    void* pVA = 0;
    int result = 0;
    unsigned long long mailboxId = 0;
    unsigned long long data = 0;
    unsigned long long timeStamp;
    void *stack = NULL;
    sysmon_vtcm_th_arg_t *inp_data = NULL;
    qurt_thread_attr_t attr;
    qurt_thread_t tid;
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;

    if (0 == contextId)
        return -1;

    if (0 == vtcmMgr_qdi_init())
    {
        if (timeout_us < HAP_VTCM_ASYNC_REQUEST_MIN_TIMEOUT)
        {
            a1.num = contextId;
            a2.num = qurt_thread_get_priority(qurt_thread_get_id() );
            a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
            result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                            COMPUTE_RES_MGR_REQUEST_CACHED_ASYNC,
                                            a1, a2, a3, a4, a5, a6, a7, a8, a9);
        }
        else
        {
            timeStamp = qurt_sysclock_get_hw_ticks();
            a1.num = contextId;
            a2.num = qurt_thread_get_priority(qurt_thread_get_id() );
            a3.ptr = &mailboxId;
            a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
            result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                            COMPUTE_RES_MGR_REQUEST_CACHED_ASYNC,
                                            a1, a2, a3, a4, a5, a6, a7, a8, a9);

            if (COMPUTE_RES_FAIL_RETRY_WITH_MAILBOX == result)
            {

                if (mailboxId)
                {
                    stack = malloc(SYSMON_VTCM_CLIENT_THREAD_STACK_SIZE
                            + sizeof(sysmon_vtcm_th_arg_t) );

                    if (!stack)
                    {
                        a1.ptr = &mailboxId;
                        a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
                        QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                               COMPUTE_RES_MGR_REQUEST_ASYNC_CANCEL,
                                               a1, a2, a3, a4, a5, a6, a7, a8, a9);
                        goto bail;
                    }

                    /* Update thread input data structure */
                    inp_data = (sysmon_vtcm_th_arg_t *)( (unsigned int)stack
                            + SYSMON_VTCM_CLIENT_THREAD_STACK_SIZE);
                    inp_data->mailboxId = mailboxId;
                    inp_data->timeout_us = timeout_us;
                    inp_data->timeStamp = timeStamp;
                    inp_data->bComputeResource = TRUE;
                    qurt_anysignal_init(&inp_data->sig);

                    /* Start a thread */
                    qurt_thread_attr_init(&attr);
                    qurt_thread_attr_set_name(&attr, "VTCM_TIMEOUT");
                    qurt_thread_attr_set_priority(&attr,
                                 qurt_thread_get_priority(qurt_thread_get_id() ) );
                    qurt_thread_attr_set_stack_addr(&attr, stack);
                    qurt_thread_attr_set_stack_size(&attr,
                                             SYSMON_VTCM_CLIENT_THREAD_STACK_SIZE);
                    if (QURT_EOK != qurt_thread_create(&tid,
                                                       &attr,
                                                       vtcm_timeout_entry,
                                                       (void *)inp_data))
                    {
                        FARF(ERROR, "HAP_request_async_VTCM: failed to create "
                                "timer thread");
                        a1.ptr = &mailboxId;
                        a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
                        QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                               COMPUTE_RES_MGR_REQUEST_ASYNC_CANCEL,
                                               a1, a2, a3, a4, a5, a6, a7, a8, a9);
                        goto bail;
                    }
                    /* We have set up the timeout now, wait for ack from VTCM
                     * driver
                     */
                    if (QURT_EOK == (result =
                                qurt_mailbox_receive(mailboxId,
                                        QURT_MAILBOX_RECV_WAITING, &data) ) )
                    {
                        pVA = (void *) (data & 0xFFFFFFFF);
                        contextId = (data >> 32); //Extract MSB portion for context ID
                        a1.num = (mailboxId & 0xFFFFFFFF);
                        a2.num = (mailboxId >> 32);
                        QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                               COMPUTE_RES_MGR_REQUEST_ASYNC_DONE,
                                               a1, a2, a3, a4, a5, a6, a7, a8, a9);
                        qurt_anysignal_set(&inp_data->sig,
                                                   SYSMON_VTCM_CLIENT_EXIT_SIG);
                    }
                    else
                    {
                        FARF(HIGH, "wait on mailbox returned failure %d, returning "
                                "error to user", result);
                    }
                    /* Wait for the timeout thread to join */
                    qurt_thread_join(tid, NULL);
                }
                else if (!contextId)
                {
                    FARF(ERROR, "HAP_compute_res_acquire_cached failed: no context id");
                }
            }
            else if (result)
            {
                FARF(ERROR, "HAP_compute_res_acquire_cached failed: %d", result);
            }
        }

        for (int i=0; i < SYSMON_VTCM_HMX_ENABLE_MAX; i++)
        {
            if (hmx_enable_table[i].contextId == contextId)
            {
                hmx_enable_table[i].bAcquireEnabled = 1;
                break;
            }
        }
    }
    else
    {
        FARF(ERROR, "HAP_compute_res_acquire_cached: driver open failed");
        result = -2;
    }
bail:
    FARF(HIGH, "HAP_compute_res_acquire_cached: context 0x%x, result %d", contextId, result);
    if (stack)
        free(stack);

    return result;
}


int compute_resource_release_cached(unsigned int context_id)
{
    int result = 0;
    qurt_qdi_arg_t a1, a2, a3, a4, a5, a6, a7, a8, a9;

    if (0 == vtcmMgr_qdi_init())
    {
        a1.num = context_id;
        a2.ptr = a3.ptr = a4.ptr = a5.ptr = a6.ptr = a7.ptr = a8.ptr = a9.ptr = NULL;
        result = QDI_vtcmMgr_invocation( 0, vtcmMgrQdi,
                                        COMPUTE_RES_MGR_REQUEST_CACHED_FREE,
                                        a1, a2, a3, a4, a5, a6, a7, a8, a9);
        if (!result)
        {
            for (int i=0; i < SYSMON_VTCM_HMX_ENABLE_MAX; i++)
            {
                if (hmx_enable_table[i].contextId == context_id)
                {
                    hmx_enable_table[i].bAcquireEnabled = 0;
                    break;
                }
            }
        }

    }

    FARF(HIGH, "HAP_compute_res_release_cached: context 0x%x, result %d", context_id, result);

    return result;
}

int compute_resource_hmx_lock(unsigned int context_id)
{
    int result = 0;

    for (int i = 0; i < SYSMON_VTCM_HMX_ENABLE_MAX; i++)
    {
        if (hmx_enable_table[i].contextId == context_id &&
            hmx_enable_table[i].bAcquireEnabled)
        {
            result = qurt_hmx_lock();
            break;
        }
        else
        {
            result = COMPUTE_RES_HMX_CONTEXT_INVALID;
        }
    }

    return result;
}

int compute_resource_hmx_unlock(unsigned int context_id)
{
    return qurt_hmx_unlock();
}
