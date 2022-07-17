/*-----------------------------------------------------------------------------
 * Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------------*/

#ifndef SYSMON_VTCMMGR_INT_H_
#define SYSMON_VTCMMGR_INT_H_

/* Device name for VTCM manager QDI driver */
#define VTCMMGR_QDI_NODE_NAME           "/dev/vtcmmgr"
/* VTCM manager private APIs used for QDI calls */
#define VTCMMGR_QUERY_TOTAL             (QDI_PRIVATE+0)     //For querying total space
#define VTCMMGR_QUERY_AVAIL             (QDI_PRIVATE+1)     //For querying available space
#define VTCMMGR_ALLOC                   (QDI_PRIVATE+2)     //For allocation requests
#define VTCMMGR_FREE                    (QDI_PRIVATE+3)     //For free requests
#define VTCMMGR_ALLOC_ASYNC             (QDI_PRIVATE+4)     //For async allocation requests
#define VTCMMGR_ALLOC_ASYNC_DONE        (QDI_PRIVATE+5)     //For async allocation done intimations
#define VTCMMGR_ALLOC_ASYNC_CANCEL      (QDI_PRIVATE+6)     //For async allocation cancel intimations
#define COMPUTE_RES_MGR_REQUEST_ASYNC           (QDI_PRIVATE+7)
#define COMPUTE_RES_MGR_REQUEST_ASYNC_CANCEL    (QDI_PRIVATE+8)
#define COMPUTE_RES_MGR_REQUEST_ASYNC_DONE      (QDI_PRIVATE+9)
#define COMPUTE_RES_MGR_REQUEST_FREE            (QDI_PRIVATE+10)
#define COMPUTE_RES_MGR_REQUEST_MAILBOX         (QDI_PRIVATE+11)
#define COMPUTE_RES_MGR_REQUEST_CACHED_ASYNC    (QDI_PRIVATE+16)
#define COMPUTE_RES_MGR_REQUEST_CACHED_FREE     (QDI_PRIVATE+17)
#define COMPUTE_RES_MGR_QUERY_VTCM              (QDI_PRIVATE+18)
#define COMPUTE_RES_MGR_QUERY_RELEASE_CB_STATE  (QDI_PRIVATE+19)

/** HAP VTCM allocation error list */
#define HAP_VTCM_ALLOC_FAIL_NO_MEM                          1
#define HAP_VTCM_ALLOC_FAIL_INVALID_ARGS                    2

#define HAP_VTCM_ALLOC_ASYNC_INVALID_ARGS                   1
#define SYSMON_SRM_WAITQ_FULL                               2
#define SYSMON_SRM_FAIL_MAILBOX_FULL                        3

#define COMPUTE_RES_FAIL_INVALID_ARGS                       1
#define COMPUTE_RES_FAIL_SERIALIZATION                      2
#define COMPUTE_RES_FAIL_NO_MEM                             3
#define COMPUTE_RES_FAIL_NO_MAILBOX                         4
#define COMPUTE_RES_FAIL_RETRY_WITH_MAILBOX                 5
#define COMPUTE_RES_FAIL_NO_CONTEXT_ID                      6
#define COMPUTE_RES_FAIL_RES_UNAVAILABLE                    7
#define COMPUTE_RES_FAIL_WAITQ_FULL                         8
#define COMPUTE_RES_FAIL_CONTEXTS_FULL                      9
#define COMPUTE_RES_FAIL_ALLOCATED_ALREADY                  10

#define COMPUTE_RES_REG_PARTITION_FAILED_ALREADY            1
#define COMPUTE_RES_PARTITION_INFO_AUTH_FAILURE             2
#define COMPUTE_RES_PARTITION_INFO_INCORRECT_PARAMS         3

#define COMPUTE_RES_REG_PRIV_FAILED_ALREADY                 1
#define COMPUTE_RES_PRIV_INFO_AUTH_FAILURE                  2
#define COMPUTE_RES_PRIV_INFO_MAP_MEM_FAILURE               3
#define COMPUTE_RES_PRIV_INFO_PID_NOT_FOUND                 4
#define COMPUTE_RES_PRIV_INFO_INVALID_PARAM                 5

#define COMPUTE_RES_HMX_CONTEXT_INVALID                     1


#define COMPUTE_RES_MGR_QUERY_RELEASE_CB_NOT_NEEDED         0
#define COMPUTE_RES_MGR_QUERY_RELEASE_CB_NEEDED             1
#define COMPUTE_RES_MGR_QUERY_RELEASE_CB_INVALID            2

#define COMPUTE_RES_VER_START                           0x0100
#define COMPUTE_RES_VER_END                             0x0200
#define COMPUTE_RES_COMPILED_VER                        0x0200
#define COMPUTE_RES_MAX_LEN                             8

/** HAP_release_VTCM error list */
#define HAP_VTCM_RELEASE_FAIL_QDI                          -1
#define HAP_VTCM_RELEASE_FAIL_INVALID_ARGS                  1
#define HAP_VTCM_RELEASE_FAIL_MEMCLEAR                      2
#define HAP_VTCM_RELEASE_FAIL_MEMREGION_DEL                 3
#define HAP_VTCM_RELEASE_FAIL_REQUEST_NOT_FOUND             4

/** HAP query VTCM error list */
#define HAP_VTCM_QUERY_FAIL_QDI                            -1
#define HAP_VTCM_QUERY_FAIL_QDI_INVOKE                     -2
#define HAP_VTCM_QUERY_FAIL_INIT_NOTDONE                   -3

typedef int (*releaseCb)(unsigned int contextId, void *clientCtx);

typedef union {
    unsigned long long attributes[COMPUTE_RES_MAX_LEN];
    struct {
        unsigned short ver;
        unsigned char bSerialize;
        unsigned char bVtcmSinglePage;
        unsigned int vtcmSize;
        void *pVtcm;
        unsigned int minPageSize;
        unsigned int minVtcmSize;
        unsigned char bHMX;
        unsigned char appId;
        unsigned char bCacheable;
        void *clientCtx;
        releaseCb releaseCallback;
        unsigned int outVtcmSize;
    } attr;
} compute_res_attr_t;

/**
 * Structure containing a VTCM page size and the number of pages with that size.
 */
typedef struct {
    unsigned int page_size;  /**< Page size in bytes. */
    unsigned int num_pages;  /**< Number of pages each of size page_size. */
} compute_res_vtcm_page_def_t;

/**
 * Structure describing the VTCM memory pages.
 */
typedef struct {
    unsigned int block_size;           /**< Block size in bytes */
    unsigned int page_list_len;      /**< Number of valid elements in page_list array */
    compute_res_vtcm_page_def_t page_list[8];  /**< Array of pages. */
} compute_res_vtcm_page_t;


/**************************************************************************//**
 * @fn sysmon_vtcm_mgr_init
 * @brief Init function to be called at DSP init. Initializes VTCM
 *        Manager mem nodes and QDI interfaces.
 *****************************************************************************/
void sysmon_vtcmMgr_init(void);

typedef void (*vtcm_release_cb)(int clientHandle, unsigned int PA, unsigned int VA);

int QDI_vtcmMgr_invocation (int clientHandle,
        qurt_qdi_obj_t *obj,
        int method,
        qurt_qdi_arg_t a1,
        qurt_qdi_arg_t a2,
        qurt_qdi_arg_t a3,
        qurt_qdi_arg_t a4,
        qurt_qdi_arg_t a5,
        qurt_qdi_arg_t a6,
        qurt_qdi_arg_t a7,
        qurt_qdi_arg_t a8,
        qurt_qdi_arg_t a9);

#endif /* SYSMON_VTCMMGR_INT_H_ */
