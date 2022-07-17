/*=============================================================================
 * FILE:                      sysmon_vtcm_mgr_srv.c
 *
 * DESCRIPTION:
 *    Vector TCM management and Compute Resource Manager
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
                                  Includes
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

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif
#ifndef NULL
#define NULL 0
#endif

unsigned int SYSMON_VTCM_MIN_CHUNK = (4 * 1024);

/* Number of static memory block identifiers (nodes) */
#define VTCMMGR_NUM_MEM_NODES           128
/* Maximum number of QDI objects to be defined statically */
#define VTCMMGR_QDI_MAX_OBJS            20
/* Maximum number of wait nodes to be defined statically */
#define VTCMMGR_NUM_WAIT_NODES          64
/* Maximum number of compute resource contexts */
#define COMPUTE_RES_MAX_RESOURCES       128
/* Maximum number of partitions supported */
#define VTCMMGR_NUM_PARTITION_NODES     16

#define VTCMMGR_NUM_CACHE_NODES         128
/* Maximum number of application IDs */
#define VTCMMGR_NUM_APP_IDS             32
/* Maximum number of VTCM partitions */
#define VTCMMGR_NUM_PARTITIONS          16
/* Maximum number of PID to privilege level mappings */
#define VTCMMGR_NUM_PID_PRIVILEGE_MAPS  16

/* Offset of VTCM base element in the CFGBASE table */
#define VTCMMGR_CFGBASE_VTCM_BASE_OFFSET    0x38
/* Offset of VTCM size element in the CFGBASE table */
#define VTCMMGR_CFGBASE_VTCM_SIZE_OFFSET    0x3c

/* Min supported version of compute resource acquire framework */
#define COMPUTE_RES_VER_START                           0x0100
/* Max supported version of compute resource acquire framework */
#define COMPUTE_RES_VER_END                             0x0200

#define COMPUTE_RES_ALIGN(v, n) ( (v) + (n) - 1) & ( ~( (n) - 1) )
#define COMPUTE_RES_GET_PA(V) ( ( (V) - g_vtcmMgr.VA) + g_vtcmMgr.PA)
#define COMPUTE_RES_GET_VA(P) ( ( (P) - g_vtcmMgr.PA) + g_vtcmMgr.VA)
#define FAILURE -2
#define SUCCESS 0

enum {
    VTCM_PARTITION_FLAG_PRIMARY = 1,
    VTCM_PARTITION_FLAG_SECONDARY = 2,
    VTCM_PARTITION_FLAG_PRIVILEGE = 4,
    VTCM_PARTITION_FLAG_MAX = 0x80000000
};

typedef enum {
    CRM_RES_NOT_REQUESTED = 0,
    CRM_RES_AVAILABLE = 1,
    CRM_RES_RELEASE_NEEDED = 2,
    CRM_RES_WAIT_FOR_RELEASE = 3,
    CRM_RES_UNAVAILABLE = 4,
    CRM_RES_EALREADY = 5,
    CRM_RES_INVALID_PARAMS = 6,
}crm_res_alloc_t;

typedef enum {
    VTCM_RESOURCE_DEFAULT = 0,
    VTCM_RESOURCE_UNAVAILABLE,
    VTCM_RESOURCE_SELECTED,
    VTCM_RESOURCE_ASSIGNED,
    VTCM_RESOURCE_RESERVED,
    VTCM_RESOURCE_FAILED,
}vtcm_alloc_status_t;

/* VTCM Manager QDI object definition */
typedef struct vtcmMgr_qdi_obj {
    qurt_qdi_obj_t qdiObj;          //QuRT QDI object identifier
    unsigned char b_valid;
    unsigned char b_partitionMaster;
    unsigned char b_privilegeMaster;
    int clientHandle;
    int pid;
    unsigned long long mailboxId;
    struct vtcmMgr_qdi_obj* next;  //Pointer to the next free QDI object node
} vtcmMgr_qdi_obj_t;

/* Compute resource node for computeResQ */
typedef struct computeResMgr_node{
    struct computeResMgr_node *next;   //Pointer to the next node
    struct computeResMgr_node *prev;   //Pointer to the previous node
    unsigned int contextId;             //Context id allocated to the caller
    int clientHandle;                   //client handle of the caller
    qurt_addr_t pVA;                    //VTCM virtual address returned to the caller
    unsigned int size;                  //Size of the allocation
    unsigned int pageSize;              //PageSize of the allocation
    unsigned char bSerialize;           //Serialization requested by the caller
    unsigned char bHMX;                 //HMX requested by the caller
    unsigned char bReleasable;          //If the request has a release callback registered
    unsigned char bCached;              //If request is cached
    unsigned char bAllocated;           //If a cacheable request is currently active
    unsigned char bReleaseRequested;    //If a release request has been submitted
    unsigned char applicationId;        //Application id for a VTCM request
    int priority;                       //Priority of the request (0 - 255)
    int memHandle;                      //Memory handle if allocated
} computeResMgr_node_t;

typedef struct vtcmMgr_cache_node {
    struct vtcmMgr_cache_node *next;
    struct vtcmMgr_cache_node *prev;    //Double linked list
    computeResMgr_node_t *context;        //Allocated context for this node
    unsigned char prio;                 //Priority of the allocator
} vtcmMgr_cache_node_t;

typedef struct vtcmMgr_wait_node {
    struct vtcmMgr_wait_node *next;
    struct vtcmMgr_wait_node *prev;
    int clientHandle;
    unsigned long long mailboxId;
    unsigned int size;
    unsigned int min_size;
    unsigned short min_page_size;
    unsigned char bCacheable;
    unsigned char bReleasable;
    unsigned char applicationId;
    unsigned char bSinglePage;
    unsigned char bPending;
    unsigned char bSerialize;
    unsigned char bHMX;
    unsigned char bComputeRes;
    computeResMgr_node_t *pContext;
    qurt_addr_t pVA;
    qurt_addr_t reserved_pVA;
    unsigned int reserved_size;
    int priority;
} vtcmMgr_wait_node_t;

/* VTCM manager mem node descriptor structure */
typedef struct vtcmMgr_mem_node {
    struct vtcmMgr_mem_node* next; //Pointer to the next node
    struct vtcmMgr_mem_node* prev; //Pointer to the previous node
    struct vtcmMgr_mem_node *parent; //Parent node if any
    struct vtcmMgr_mem_node* pFreeList; //Pointer to the free list absorbed in this node when it's busy
    qurt_addr_t PA;                 //Physical addr of this node
    unsigned int size;              //Allocated size
    struct{
        unsigned char reserved:6;
        unsigned char bReleasable:1; //Releasable and busy will go hand in hand
        unsigned char bFree:1;
    } desc;
    computeResMgr_node_t *context;   //Context ID if allocated as a compute resource
    vtcmMgr_wait_node_t *reserving_node; //Wait node of the reserving entity
    vtcmMgr_cache_node_t *cache_head; //Head of the cache node list
    unsigned char num_cached;         //number of cache allocations on this node
    unsigned char busy_prio;       //Priority of the holding entity if any
    qurt_addr_t VA;                //Virtual address returned to the caller
    int memHandle;                 //SRM mapping handle
    int clientHandle;              //Allocating client handle
    vtcm_release_cb release_cb;    //Release callback if registered for a busy node
    unsigned int release_PA;       //PA pointed out for the callback
} vtcmMgr_mem_node_t;


typedef struct vtcmMgr_wait_queue {
    vtcmMgr_wait_node_t *pBusy;
    vtcmMgr_wait_node_t *pFree;
    unsigned short mailboxCount;
    unsigned short mailboxNumber;
} vtcmMgr_wait_queue_t;

typedef struct computeResMgr_queue {
    computeResMgr_node_t *pBusy;
    computeResMgr_node_t *pFree;
    unsigned short busyDepth;
} computeResMgr_queue_t;

typedef struct {
    computeResMgr_node_t *context;
    vtcmMgr_wait_node_t *reserve;
} crm_single_res_t;

typedef struct {
    vtcmMgr_mem_node_t *startNode;
    vtcmMgr_mem_node_t *endNode;
    unsigned int startPA;
    unsigned int allocSize;
    unsigned int allocPoolSize;
    unsigned int pageSize;
    int priority;
    vtcm_alloc_status_t allocStatus;
    unsigned char allocPrio;
    unsigned char numBusyNodes;
    unsigned char numCachedNodes;
} vtcm_select_t;

/*
 * VTCM partition node descriptor
 */
typedef struct vtcmMgr_partition_node{
    struct vtcmMgr_partition_node *next;
    unsigned char partitionId;
    unsigned int flags; //BitMask
    qurt_addr_t PA;     //Physical start of the partition
    unsigned int size;  //Size of the partition
    vtcmMgr_mem_node_t *pMemList;
} vtcmMgr_partition_node_t;

typedef struct {
    unsigned char partition_id;
    unsigned int size_in_kb;
    unsigned int flags;
} vtcm_partition_info_t;

typedef struct {
    unsigned char app_id;
    unsigned char partition_id;
} vtcm_appid_map_t;

typedef struct {
    unsigned char b_active;
    int pid;
    unsigned int privilege_level;
} vtcm_pid_priv_map_t;

typedef struct {
    /** Partition master provided partition and app mapping tables     */
    unsigned char numPartitions;
    vtcm_partition_info_t partitions[VTCMMGR_NUM_PARTITIONS];
    unsigned char numAppIds;
    vtcm_appid_map_t appIdMaps[VTCMMGR_NUM_APP_IDS];
    /** App id to partition id mapping table (index based) derived from
     * partition master info. */
    unsigned char appId2PartitionId[VTCMMGR_NUM_APP_IDS];
    /** Flag pending partition table update */
    unsigned char bPending;
    /** QDI objects for partition and app privilege masters */
    vtcmMgr_qdi_obj_t *partMaster;
    vtcm_pid_priv_map_t privMap[VTCMMGR_NUM_PID_PRIVILEGE_MAPS];
    vtcmMgr_qdi_obj_t *privMaster;
} vtcmMgr_vtcm_info_t;

/* VTCM manager global structure definition */
typedef struct {
    unsigned char bInitDone;        //boolean for INITIALIZED
    qurt_mem_pool_t pool;           //QuRT memory pool identifier
    int srmMapHandle;
    int vtcmFuseHandle;             //Memory Handle for VTCM Fuse mapping
    unsigned int vtcmFuseVal;       //Fuse value read from the register
    qurt_addr_t vtcmFuseVA;         //Virtual address returned by Fuse mapping
    qurt_addr_t PA;                 //Physical start address for VTCM pool
    qurt_addr_t VA;                 //Virtual start address for VTCM pool in SRM
    qurt_size_t size;               //Size of VTCM pool
    qurt_size_t mapSize;            //Map size of VTCM pool
    vtcmMgr_vtcm_info_t vtcmInfo;
    vtcmMgr_partition_node_t *vtcmPartitions;
    /**< List of VTCM partitions */
    crm_single_res_t serialize;
    crm_single_res_t hmx;
    computeResMgr_node_t computeResArray[COMPUTE_RES_MAX_RESOURCES];
    computeResMgr_queue_t computeResQ; //Compute resource acquired client list
    vtcmMgr_wait_queue_t waitQ;     //WaitQ
} g_vtcmMgr_t;

/* ============================================================================
                                 Declarations
   ========================================================================= */

static void hvx_memset(void *ptr, int value, unsigned int size);
static void driver_init(void);
static void driver_mem_init(void);
static void vtcm_memnode_push(vtcmMgr_mem_node_t* pMem);
static vtcmMgr_mem_node_t* vtcm_memnode_pop(void);
static unsigned int vtcm_alloc_align(unsigned int size,
                                     unsigned char bSinglePage);
static int vtcmMgr_query_total(unsigned int* size,
                               unsigned int* numPages);
static void vtcmMgr_query_pages(vtcmMgr_partition_node_t* partition,
                                vtcmMgr_mem_node_t* pNode,
                                unsigned int* maxBlockSize,
                                unsigned int* numBlocks);
static int vtcmMgr_query_avail(unsigned int* freeBlockSize,
                               unsigned int* maxPageSize,
                               unsigned int* numPages);
int QDI_vtcmMgr_invocation( int clientHandle,
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
void QDI_vtcmMgr_release (qurt_qdi_obj_t *qdiobj);
static void driver_QDI_init(void);
static vtcmMgr_qdi_obj_t* driver_QDI_pop(void);
static void driver_QDI_push(vtcmMgr_qdi_obj_t* pQDIObj);
static void vtcmMgr_waitNode_push(vtcmMgr_wait_node_t* pMem);
static vtcmMgr_wait_node_t* vtcmMgr_waitNode_pop(void);
static void vtcmMgr_recheck_waitQ(void);
static computeResMgr_node_t* compute_context_alloc(int clientHandle);
static int compute_context_delete(computeResMgr_node_t *context,
                                  int clientHandle);
static int vtcmMgr_create_wait_node(vtcmMgr_wait_node_t **p_waitNode,
                                    int priority,
                                    compute_res_attr_t *request);
static int unreserve_wait_node(vtcmMgr_wait_node_t *waitNode);
static vtcmMgr_partition_node_t* vtcm_partitionnode_pop(void);
static void vtcm_partitionnode_push(vtcmMgr_partition_node_t* pNode);
static vtcmMgr_cache_node_t* vtcm_cachenode_pop(void);
static void vtcm_cachenode_push(vtcmMgr_cache_node_t *node);
static int invoke_release_callback(computeResMgr_node_t *context);
static int unreserve_vtcm(vtcmMgr_wait_node_t *waitNode);
int vtcm_allocate_immediate(int clientHandle,
                            vtcmMgr_partition_node_t *partition,
                            computeResMgr_node_t *context,
                            vtcm_select_t *selection,
                            vtcmMgr_wait_node_t *waitNode);
int __attribute__((weak)) qurt_system_config_get(int index, unsigned int *data);

/* ============================================================================
                                 Globals
   ========================================================================= */

/* An array of VTCM mem block identifiers/nodes */
vtcmMgr_mem_node_t vtcmMemNodeArray[VTCMMGR_NUM_MEM_NODES];
/* An array of VTCM  */
vtcmMgr_wait_node_t vtcmWaitNodeArray[VTCMMGR_NUM_WAIT_NODES];
/* Array of VTCM partition nodes */
vtcmMgr_partition_node_t vtcmPartitionNodeArray[VTCMMGR_NUM_PARTITION_NODES];

vtcmMgr_cache_node_t vtcmCacheNodeArray[VTCMMGR_NUM_CACHE_NODES];

vtcmMgr_cache_node_t* pVtcmCacheNodeList = NULL;
/* A single linked list containing available partition nodes */
vtcmMgr_partition_node_t* pVtcmPartitionNodeList = NULL;
/* A single linked list operated as LIFO for memory nodes */
vtcmMgr_mem_node_t* pVtcmMemNodeList = NULL;
/* VTCM manager QDI object array */
vtcmMgr_qdi_obj_t vtcmQDIObjArray[VTCMMGR_QDI_MAX_OBJS];
/* VTCM manager QDI object linked list storing the available QDI objects */
vtcmMgr_qdi_obj_t* pVtcmQDIObjList = NULL;

/* VTCM manager global structure */
g_vtcmMgr_t g_vtcmMgr = {0};

/* ============================================================================
                                CODE STARTS HERE
   ========================================================================= */

/******************************************************************************
 * Function: sysmon_vtcm_mgr_init
 * Description: Init function to be called at DSP init. Initializes VTCM
 *              Manager mem nodes and QDI interfaces.
 *****************************************************************************/
static void driver_init()
{
    /* Initialize memory */
    driver_mem_init();
    /* Initialize VTCM manager QDI interface */
    driver_QDI_init();
}

static int local_memcpy(void *dest, void *src, int size)
{
    memcpy(dest, src, size);
    return 0;
}

static void driver_prepare_vtcm()
{
    int i;
    vtcmMgr_mem_node_t *mem, *mem_temp;
    vtcmMgr_partition_node_t *partition;
    vtcmMgr_partition_node_t *partition_prev = NULL, *partition_temp = NULL;
    unsigned char bFree = 1;

    if (!g_vtcmMgr.VA && !g_vtcmMgr.size)
        return;

    /*
     * Check if we already have initialized VTCM. If yes,
     * 1. if there is an active VTCM transaction, keep the VTCM information in
     *    pending state.
     * 2. if there is no active VTCM transaction, disable older partition info
     *    and apply the new info.
     */
    if (g_vtcmMgr.vtcmPartitions)
    {
        partition = g_vtcmMgr.vtcmPartitions;

        /*
         * Loop around all the partitions to check if everything is free
         */
        while (partition)
        {
            mem = partition->pMemList;
            /*
             * Loop around the memory nodes in each partition to check if
             * everything is free
             */
            while (mem)
            {
                /*
                 * Return if there is any used node - either busy, reserved or
                 * cached nodes.
                 */
                if (!mem->desc.bFree ||
                    mem->reserving_node ||
                    mem->cache_head)
                {
                    bFree = 0;
                    break;
                }

                mem = mem->next;
            }

            /*
             * Break the partition loop if any memory node is found to be busy
             */
            if (!bFree)
                break;

            partition = partition->next;
        }

        /*
         * Return if any busy memory node is encountered.
         */
        if (!bFree)
            return;
        else
        {
            /*
             * Everything is free, release all the partitions and their
             * memory nodes.
             */
            partition = g_vtcmMgr.vtcmPartitions;

            while (partition)
            {
                partition_temp = partition->next;
                mem = partition->pMemList;

                while (mem)
                {
                    mem_temp = mem->next;
                    vtcm_memnode_push(mem);
                    mem = mem_temp;
                }

                vtcm_partitionnode_push(partition);
                partition = partition_temp;
            }
        }
    }

    for (i = 0; i < g_vtcmMgr.vtcmInfo.numPartitions; i++)
    {
        /* We have a valid memory spread
         * Create a default PRIMARY partition for the entire VTCM size */
        partition = vtcm_partitionnode_pop();

        /* Check if we are able to get a partition node */
        if (!partition)
            return;

        partition->partitionId = g_vtcmMgr.vtcmInfo.partitions[i].partition_id;
        partition->flags = g_vtcmMgr.vtcmInfo.partitions[i].flags;
        partition->size = ( (unsigned int)g_vtcmMgr.vtcmInfo.partitions[i].size_in_kb ) << 10;

        if (!partition_prev)
            partition->PA = g_vtcmMgr.PA;
        else
            partition->PA = partition_prev->PA + partition->size;

        /* Create a node covering entire space in free list of this
         * partition */
        mem = vtcm_memnode_pop();

        if (!mem)
        {
            /* Failed to get a mem node from list */
            vtcm_partitionnode_push(partition);
            break;
        }

        /*
         * Node:
         *      PA: VTCM start physical address
         *      size: Size of VTCM pool
         *      desc.bFree : Free (1)
         */
        mem->PA = partition->PA;
        mem->size = partition->size;
        mem->desc.bFree = TRUE;
        mem->desc.bReleasable = FALSE;
        mem->cache_head = NULL;
        mem->reserving_node = NULL;
        mem->next = NULL;
        mem->prev = NULL;
        mem->memHandle = -1;
        mem->clientHandle = -1;
        mem->release_cb = NULL;
        mem->VA = 0;
        /* Push the mem node created to the mem list */
        partition->pMemList = mem;
        partition->next = NULL;

        if (partition_prev)
            partition_prev->next = partition;
        else
        {
            /* Update the partitions list in the global structure
             * Only one node for now covering entire VTCM */
            g_vtcmMgr.vtcmPartitions = partition;
        }

        partition_prev = partition;
    }

    g_vtcmMgr.vtcmInfo.bPending = FALSE;
}

/* :: driver_mem_init ::
 * Initializes VTCM manager mem nodes and attaches to the VTCM_PHYSPOOL from
 * QuRT
 */
static void driver_mem_init()
{
    int i, result = 0;

    /* Push all the statically defined mem nodes to the node list */
    for (i = 0; i < VTCMMGR_NUM_MEM_NODES; i++)
        vtcm_memnode_push(&vtcmMemNodeArray[i]);

    /* Push all wait nodes to the free list */
    for (i = 0; i < VTCMMGR_NUM_WAIT_NODES; i++)
        vtcmMgr_waitNode_push(&vtcmWaitNodeArray[i]);

    /* Create a list for partition nodes */
    for (i = 0; i < VTCMMGR_NUM_PARTITION_NODES; i++)
        vtcm_partitionnode_push(&vtcmPartitionNodeArray[i]);

    /* Push all compute resource contexts to the free queue */
    for (i = 0; i < COMPUTE_RES_MAX_RESOURCES; i++)
    {
        g_vtcmMgr.computeResArray[i].next = g_vtcmMgr.computeResQ.pFree;
        g_vtcmMgr.computeResArray[i].contextId = (unsigned int)&g_vtcmMgr.computeResArray[i];
        g_vtcmMgr.computeResQ.pFree = &g_vtcmMgr.computeResArray[i];
    }

    for (i = 0; i < VTCMMGR_NUM_CACHE_NODES; i++)
    {
        vtcm_cachenode_push(&vtcmCacheNodeArray[i]);
    }

    /* Hardcode the VTCM start for simulator assuming --l2tcm_base 0xd800 */
    g_vtcmMgr.PA = 0xd8200000;
    g_vtcmMgr.size = 0x40000;

    /* If QuRT system config read API is defined, try reading VTCM Size from
     * CFGBASE and update the VTCM size accordingly */
    if (qurt_system_config_get)
    {
        result = qurt_system_config_get(VTCMMGR_CFGBASE_VTCM_SIZE_OFFSET,
                                       (unsigned int *)&g_vtcmMgr.size);
        if (g_vtcmMgr.size && (result == QURT_EOK) )
        {
            g_vtcmMgr.size = g_vtcmMgr.size << 10;
            FARF(ALWAYS, "Got VTCM size for this arch as 0x%x", g_vtcmMgr.size);
        }
        else
        {
            /* Fall back to 256KB VTCM */
            g_vtcmMgr.size = 0x40000;
            FARF(ERROR, "Failed to get VTCM size from QuRT, falling back to 0x%x", g_vtcmMgr.size);
        }

        /* Set minimum chunk to 256KB if size is greater than or equal to 1MB */
        if (g_vtcmMgr.size >= 0x100000)
            SYSMON_VTCM_MIN_CHUNK = (256 * 1024);

        result = qurt_system_config_get(VTCMMGR_CFGBASE_VTCM_BASE_OFFSET,
                                        (unsigned int *)&g_vtcmMgr.PA);
        if (g_vtcmMgr.PA && (result == QURT_EOK))
        {
            g_vtcmMgr.PA = g_vtcmMgr.PA << 16;
        }
        else
        {
            /* Hardcode the VTCM start for simulator where in --l2tcm_base 0xd800 */
            if (g_vtcmMgr.size < 0x200000)
            {
                g_vtcmMgr.PA = 0xd8200000;
            }
            else if (g_vtcmMgr.size <= 0x400000)
            {
                g_vtcmMgr.PA = 0xd8400000;
            }
            else
            {
                g_vtcmMgr.PA = 0xd8800000;
            }
            FARF(ALWAYS, "Unable to get VTCM PA from QuRT, falling back to 0x%x", g_vtcmMgr.PA);
        }
    }

    i = qurt_mem_map_static_query(&g_vtcmMgr.VA,
                                g_vtcmMgr.PA,
                                g_vtcmMgr.size,
                                QURT_MEM_CACHE_WRITEBACK,
                                (QURT_PERM_READ | QURT_PERM_WRITE) );
    if (0 != i)
    {
        FARF(ALWAYS, "Unable to query VTCM virtual address, result = %d, VA = 0x%x, size = 0x%x",
                i, g_vtcmMgr.VA, g_vtcmMgr.size);
        FARF(ALWAYS, "Falling back to one to one mapping VA = 0x%x = PA!", g_vtcmMgr.PA);
        g_vtcmMgr.VA = g_vtcmMgr.PA;
    }
    else
    {
        FARF(ALWAYS, "Got VTCM virtual address as 0x%x", g_vtcmMgr.VA);
    }

    if (g_vtcmMgr.VA)
    {
        /*
         * Initialize to single partition state. All application IDs would be
         * mapped to this single partition.
         */
        g_vtcmMgr.vtcmInfo.numPartitions = 1;
        g_vtcmMgr.vtcmInfo.partitions[0].partition_id = 0;
        g_vtcmMgr.vtcmInfo.partitions[0].size_in_kb = (g_vtcmMgr.size >> 10);
        g_vtcmMgr.vtcmInfo.partitions[0].flags = VTCM_PARTITION_FLAG_PRIMARY;

        for (i = 0; i < VTCMMGR_NUM_APP_IDS; i++)
            g_vtcmMgr.vtcmInfo.appId2PartitionId[i] = 0;

        g_vtcmMgr.vtcmInfo.numAppIds = 1;

        /*
         * Call into the prepare VTCM function with the updated vtcmInfo for
         * a single primary partition mapping all available VTCM.
         */
        driver_prepare_vtcm();

        /* Update init done to success in the end */
        g_vtcmMgr.bInitDone = TRUE;
    }
}

/* :: vtcm_memnode_push ::
 *    Pushes given mem node to mem node LIFO
 */
static void vtcm_memnode_push(vtcmMgr_mem_node_t* pMem)
{
    vtcmMgr_cache_node_t *cache = NULL;
    vtcmMgr_cache_node_t *cache_temp = NULL;

    if (pMem)
    {
        /*
         * Free any cache nodes mapped in the mem node
         */
        cache = pMem->cache_head;

        while (cache)
        {
            cache_temp = cache->next;
            vtcm_cachenode_push(cache);
            cache = cache_temp;
        }

        /* Push given object to the front of the list */
        pMem->next = pVtcmMemNodeList;
        pVtcmMemNodeList = pMem;
    }
}

/* :: vtcm_memnode_pop ::
 *    Pops out a mem node from the mem node LIFO
 */
static vtcmMgr_mem_node_t* vtcm_memnode_pop()
{
    vtcmMgr_mem_node_t* retVal;
    /* Pop out a mem node from the mem node list */
    retVal = pVtcmMemNodeList;

    if (pVtcmMemNodeList)
    {
        pVtcmMemNodeList = pVtcmMemNodeList->next;
    }

    /* return the node */
    return retVal;
}

/* :: vtcm_partitionnode_push ::
 *    Pushes given partition node to partition list
 */
static void vtcm_partitionnode_push(vtcmMgr_partition_node_t* pNode)
{
    if (pNode)
    {
        /* Push given object to the front of the list */
        pNode->next = pVtcmPartitionNodeList;
        pVtcmPartitionNodeList = pNode;
    }
}

/* :: vtcm_partitionnode_pop ::
 *    Pop a node from the partition list
 */
static vtcmMgr_partition_node_t* vtcm_partitionnode_pop()
{
    vtcmMgr_partition_node_t* retVal;
    /* Pop out a mem node from the mem node list */
    retVal = pVtcmPartitionNodeList;

    if (pVtcmPartitionNodeList)
    {
        pVtcmPartitionNodeList = pVtcmPartitionNodeList->next;
    }
    else
    {
        pVtcmPartitionNodeList = NULL;
    }

    /* return the node */
    return retVal;
}

/* :: vtcm_get_partition ::
 *    Get partition node for the provided partition id
 */
static vtcmMgr_partition_node_t* vtcm_get_partition(unsigned char partitionId)
{
    vtcmMgr_partition_node_t* retVal = g_vtcmMgr.vtcmPartitions;

    /* Try finding the partition in the carved out partitions */
    while (retVal)
    {
        if (retVal->partitionId == partitionId)
            break;

        retVal = retVal->next;
    }

    /* return the node */
    return retVal;
}

static vtcmMgr_qdi_obj_t *driver_get_qdi(int clientHandle)
{
    vtcmMgr_qdi_obj_t *qdiObj;

    qdiObj = vtcmQDIObjArray;

    for (int i = 0; i < VTCMMGR_QDI_MAX_OBJS; i++)
    {
        if ( qdiObj[i].b_valid &&
             (qdiObj[i].clientHandle == clientHandle) )
        {
            return &qdiObj[i];
        }
    }

    return NULL;
}

static void __attribute__((noinline))
hvx_memset(void *ptr, int value, unsigned int size)
{
    memset(ptr, value, size);
}

/* :: vtcm_alloc_align ::
 *    Aligns size provided by the user to desired VTCM mem block size based on
 *    single page requirement
 */
static unsigned int vtcm_alloc_align(unsigned int size,
                                     unsigned char bSinglePage)
{
    unsigned int tempSize = 0;

    if (bSinglePage)
    {
        /*
         * If provided size is less than the minimum chunk, override it
         */
        if (size < SYSMON_VTCM_MIN_CHUNK)
            size = SYSMON_VTCM_MIN_CHUNK;

        /* User is requesting for a single page memory allocation.
         * Align size to 4k/16k/64k/256KB/1MB/4MB/16MB multiples to get a
         * single page allocation (TLB entry) by QuRT.
         */
        tempSize = 31 - Q6_R_cl0_R(size);

        tempSize = ( (tempSize >> 1) << 1);
        tempSize = 1 << tempSize;

        if (tempSize < size)
            tempSize = tempSize * 4;

    }
    else
    {
        /* Align to MIN Chunk multiple */
        tempSize = ( (size + (SYSMON_VTCM_MIN_CHUNK - 1) ) & (-SYSMON_VTCM_MIN_CHUNK) );
    }

    return tempSize;
}

// Add node to a list
static void memlist_add_node_after (vtcmMgr_mem_node_t *curr_node, vtcmMgr_mem_node_t *node)
{
    node->prev = curr_node;
    node->next = curr_node->next;
    curr_node->next = node;

    if (node->next)
        node->next->prev = node;
}

static void memlist_add_node_before(vtcmMgr_mem_node_t **head, vtcmMgr_mem_node_t *curr_node, vtcmMgr_mem_node_t *node)
{
    node->next = curr_node;
    node->prev = curr_node->prev;

    if (!curr_node->prev)
    {
        if (head) *head = node;
    }
    else
    {
        curr_node->prev->next = node;
    }

    curr_node->prev = node;
}

static int memlist_copy_contents (vtcmMgr_mem_node_t *dest, vtcmMgr_mem_node_t* src)
{
    dest->PA = src->PA;
    dest->size = src->size;
    dest->desc = src->desc;
    dest->busy_prio = src->busy_prio;
    dest->clientHandle = src->clientHandle;
    dest->context = src->context;
    dest->reserving_node = src->reserving_node;
    dest->num_cached = src->num_cached;
    dest->VA = src->VA;
    dest->parent = src->parent;
    dest->memHandle = src->memHandle;
    dest->release_cb = src->release_cb;
    dest->release_PA = src->release_PA;
    /*
     * We don't expect this function to duplicate a busy node. Busy node will
     * have a free list that it points to. Duplicating a free list is not
     * legal given how the code is written. So initialize the freelist to
     * NULL.
     */
    dest->pFreeList = NULL;
    dest->cache_head = NULL;
    unsigned char temp = src->num_cached;
    vtcmMgr_cache_node_t *cache_node;
    vtcmMgr_cache_node_t *src_node = NULL;

    /*
     * Check if we have enough cache nodes to return
     * gracefully otherwise.
     */
    cache_node = pVtcmCacheNodeList;

    while (temp && cache_node)
    {
        cache_node = cache_node->next;
        temp--;
    }

    if (temp)
        return FAILURE;

    if (src->num_cached)
    {
        src_node = src->cache_head;
        cache_node = vtcm_cachenode_pop();
        dest->cache_head = cache_node;
        cache_node->context = src_node->context;
        cache_node->prio = src_node->prio;
        cache_node->prev = NULL;
        cache_node->next = NULL;
        src_node = src_node->next;
    }

    while (src_node)
    {
        cache_node->next = vtcm_cachenode_pop();
        cache_node->next->prev = cache_node;
        cache_node = cache_node->next;
        cache_node->context = src_node->context;
        cache_node->prio = src_node->prio;
        cache_node->next = NULL;
        src_node = src_node->next;
    }

    return SUCCESS;
}

static unsigned int get_privilege_level_from_pid(int pid)
{
    unsigned int privilege_level = 0;
    int i;

    for (i = 0; i < VTCMMGR_NUM_PID_PRIVILEGE_MAPS; i++)
    {
        if ( (g_vtcmMgr.vtcmInfo.privMap[i].pid == pid) &&
             g_vtcmMgr.vtcmInfo.privMap[i].b_active)
        {
            privilege_level = g_vtcmMgr.vtcmInfo.privMap[i].privilege_level;
            break;
        }
    }

    return privilege_level;
}

static vtcmMgr_partition_node_t *get_partition_from_app_id(int clientHandle,
                                                           unsigned char appId)
{
    unsigned char partitionId = 0;
    vtcmMgr_partition_node_t *partition = NULL;
    int i, privileged = 0, found = 0;
    vtcmMgr_qdi_obj_t *qdiObj = vtcmQDIObjArray;

    if (appId >= VTCMMGR_NUM_APP_IDS)
        return NULL;

    partitionId = g_vtcmMgr.vtcmInfo.appId2PartitionId[appId];

    if (partitionId >= VTCMMGR_NUM_PARTITIONS)
        return NULL;

    partition = g_vtcmMgr.vtcmPartitions;

    while (partition)
    {
        if (partition->partitionId == partitionId)
            break;

        partition = partition->next;
    }

    if (!partition)
        return NULL;

    if (partition->flags & VTCM_PARTITION_FLAG_PRIVILEGE)
    {

        for (i = 0; i < VTCMMGR_QDI_MAX_OBJS; i++)
        {
            if ( (qdiObj[i].clientHandle == clientHandle) && (qdiObj[i].b_valid) )
            {
                found = 1;
                privileged = get_privilege_level_from_pid(qdiObj[i].pid);

                /*
                 * If the client is not privileged, privileged partition cannot
                 * be used.
                 */
                if (!privileged)
                    partition = NULL;

                break;
            }
        }

        /*
         * Handle the case where the clientHandle is not found, as it is a
         * privileged partition, return NULL in such a case. Ideally it
         * shouldn't be the case.
         */
        if (!found)
            partition = NULL;
    }

    return partition;
}

//search algorithm for non-cached requests
static int vtcm_search_noncached(vtcmMgr_partition_node_t *partition,
                                 unsigned int size,
                                 unsigned int min_size,
                                 unsigned int page_size,
                                 unsigned int size_given,
                                 unsigned int min_size_given,
                                 unsigned char priority,
                                 vtcmMgr_wait_node_t *waitNode,
                                 vtcm_select_t *result)
{
    /*
     * Search in free list without any reservations or including lower priority
     * reservations for any size >= min_size
     */
    vtcmMgr_mem_node_t *topNode, *leafNode, *lastNode, *startNode;
    unsigned int alloc_size, total_pool_size, PA_aligned = 0, temp_size;
    unsigned char num_busy, num_cached, b_update, alloc_prio;

    if (!result)
        return FAILURE;

    if (!partition)
    {
        result->allocStatus = VTCM_RESOURCE_FAILED;
        return FAILURE;
    }

    if (!partition->pMemList)
    {
        result->allocStatus = VTCM_RESOURCE_FAILED;
        return FAILURE;
    }

    /*
     * Based on the page size given, check if we can serve the request based
     * on partition definition.
     */
    if (page_size)
    {
        PA_aligned = COMPUTE_RES_ALIGN(partition->PA, page_size);

        if ( (PA_aligned >= (partition->PA + partition->size) ) ||
             ( (PA_aligned + min_size_given) >
                            (partition->PA + partition->size) ) )
        {
            result->allocStatus = VTCM_RESOURCE_FAILED;
            return FAILURE;
        }
    }

    topNode = partition->pMemList;
    result->startPA = 0;
    result->allocSize = 0;
    result->allocPoolSize = 0;
    result->allocPrio = 0;
    result->numBusyNodes = 0;
    result->numCachedNodes = 0;
    result->startNode = NULL;
    result->endNode = NULL;
    result->allocStatus = VTCM_RESOURCE_DEFAULT;
    result->pageSize = page_size;

    if (page_size)
    {
        PA_aligned = COMPUTE_RES_ALIGN(topNode->PA, page_size);
    }

    while (topNode)
    {

        if (page_size)
        {

            if (PA_aligned < topNode->PA)
            {
                /*
                 * The top node has advanced/crossed the PA_aligned, mostly
                 * because it couldn't fit in the allocation. Advance PA to
                 * the next aligned memory from the top nodes PA.
                 */
                PA_aligned = COMPUTE_RES_ALIGN(topNode->PA, page_size);
            }

            if (PA_aligned >= (topNode->PA + topNode->size) )
            {
                /*
                 * Find the node having the first aligned PA and start with
                 * that as the top node.
                 */
                while (topNode)
                {
                    if (PA_aligned < (topNode->PA + topNode->size) )
                        break;

                    topNode = topNode->next;
                }

                /*
                 * Break if the entire memlist is already parsed and the
                 * PA_aligned has moved out of the partition region.
                 */
                if (!topNode)
                    break;
            }
        }

        alloc_size = 0;
        total_pool_size = 0;
        alloc_prio = 0;
        num_busy = 0;
        num_cached = 0;
        startNode = NULL;
        leafNode = topNode;
        lastNode = NULL;

        while (leafNode)
        {
            b_update = 0;

            if ( (!leafNode->desc.bFree &&
                  (!leafNode->desc.bReleasable ||
                         (leafNode->busy_prio <= priority) ) ) ||
                 (leafNode->reserving_node &&
                   (leafNode->reserving_node != waitNode) &&
                         (leafNode->reserving_node->priority <= priority) ) )
            {
                /*
                 * The node is either busy by a non-releaseable client or
                 * a same/higher priority client, or, is reserved by
                 * a same/higher priority client. We cannot take this node.
                 */
                b_update = 0;
            }
            else if (leafNode->desc.bFree &&
                     !leafNode->cache_head &&
                     (!leafNode->reserving_node ||
                      (leafNode->reserving_node == waitNode) ) )
            {
                /*
                 * Found a free node which isn't cached and isn't reserved
                 * or is reserved by the caller already.
                 */
                if (alloc_size < size)
                {
                    b_update = 1;

                    if (alloc_prio < 1)
                        alloc_prio = 1;
                }
            }
            else if (leafNode->desc.bFree &&
                     !leafNode->cache_head &&
                     leafNode->reserving_node)
            {
                /*
                 * Found a free node which is not cached and is
                 * reserved by a lower priority thread.
                 */
                if (alloc_size < size)
                {
                    b_update = 1;

                    if (alloc_prio < 2)
                        alloc_prio = 2;
                }
            }
            else if (leafNode->desc.bFree &&
                     leafNode->cache_head)
            {

                if (leafNode->cache_head->prio > priority)
                {
                    /*
                     * Found a cached but free node whose allocator priority is
                     * lower than the current requester.
                     * The node could have been reserved by a lower priority
                     * thread.
                     */
                    if (alloc_size < size)
                    {
                        b_update = 1;
                        num_cached += leafNode->num_cached;

                        if (alloc_prio < 3)
                            alloc_prio = 3;
                    }
                }
                else
                {
                    /*
                     * Found a cached but free node. The priority of the
                     * allocator is greater than or equal to the current one.
                     * The node could have been reserved by a lower priority
                     * thread.
                     */
                    if (alloc_size < min_size)
                    {
                        b_update = 1;
                        num_cached += leafNode->num_cached;

                        if (alloc_prio < 4)
                            alloc_prio = 4;
                    }
                }
            }
            else if (!leafNode->desc.bFree)
            {
                /*
                 * Found a busy node allocated by a lower priority thread and
                 * is releasable.
                 * The node could have been reserved by a lower priority
                 * thread.
                 */
                if (alloc_size < min_size)
                {
                    b_update = 1;
                    num_busy += 1;

                    if (alloc_prio < 5)
                        alloc_prio = 5;
                }
            }

            /*
             * Check if we have a candidate node. b_update = 1 signifies that.
             */
            if (b_update)
            {
                /*
                 * Found a possible node, update trackers
                 */
                if (startNode)
                {
                    alloc_size += leafNode->size;
                    total_pool_size += leafNode->size;
                }
                else
                {
                    /*
                     * Check if page size is provided, if yes, we should have
                     * started with a aligned PA and it should be in the leaf
                     * node. Safe to check it again here.
                     */
                    if (page_size)
                    {
                        if (PA_aligned >= (leafNode->PA + leafNode->size))
                        {
                            PA_aligned = NULL;
                            num_cached = 0;
                            num_busy = 0;
                            alloc_prio = 0;
                        }
                    }
                    else
                    {
                        PA_aligned = leafNode->PA;
                    }

                    if (PA_aligned)
                    {
                        alloc_size = leafNode->size -
                                            (PA_aligned - leafNode->PA);
                        total_pool_size = leafNode->size;
                        startNode = leafNode;
                    }
                }

                lastNode = leafNode;

                if (alloc_size >= size)
                    break;
            }
            else
            {
                /*
                 * We have hit a node which can't be allocated, see if we
                 * have accummulated enough size, otherwise, advance the outer
                 * loop to start with this node.
                 */
                if (alloc_size < min_size)
                    topNode = leafNode;

                break;
            }
            /*
             * Done with the current node, move on to the next one in the list
             */
            leafNode = leafNode->next;
        }

        if (startNode &&
              ( (page_size &&
                  (alloc_size < size) &&
                  (alloc_size >= min_size_given) &&
                  !partition->next && !lastNode->next) ||
                (alloc_size >= min_size) ) )
        {
            /*
             * If page size is provided and our last node is the last in the
             * physical VTCM section, check if the allocated size
             * accommodates the given size / given minimum size.
             */

            /*
             * - If gathered size accommodates entire size, update allocation
             * size as size.
             * - else, if minimum page size is provided, align the allocation
             * size based on the given minimum page size.
             */
            if (alloc_size >= size)
            {
                alloc_size = size;
            }
            else if (page_size && (partition->next || lastNode->next) )
            {
                temp_size = alloc_size;
                alloc_size = alloc_size & ( ~(page_size - 1) );
                /*
                 * We have cut down on the allocation size due to page alignment
                 * check if we have to update last node and pool size.
                 * Ideally alloc priority needs to be recalculated, but that's
                 * an overhead which we are trying to avoid here. //TODO
                 */
                while (lastNode)
                {
                    if (temp_size >= lastNode->size)
                        temp_size = temp_size - lastNode->size;
                    else
                        temp_size = 0;

                    if (temp_size >= alloc_size)
                    {
                        if (!lastNode->desc.bFree)
                            num_busy--;

                        else if (lastNode->cache_head)
                            num_cached--;

                        total_pool_size -= lastNode->size;
                        lastNode = lastNode->prev;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            if (result->allocSize)
            {
                if ( ( (alloc_prio < result->allocPrio) ||
                       ( (alloc_prio == result->allocPrio) &&
                         ( (num_busy < result->numBusyNodes) ||
                           ( (num_busy == result->numBusyNodes) &&
                              ( ( (page_size &&
                                  ( ( (result->allocSize > size_given) &&
                                      (alloc_size < result->allocSize) &&
                                      (alloc_size >= size_given) ) ||
                                    ( (result->allocSize < size_given) &&
                                      (alloc_size > result->allocSize) ) ) ) ||
                                  (!page_size &&
                                  ( ( (result->allocSize > size) &&
                                      (alloc_size < result->allocSize) &&
                                      (alloc_size >= size) ) ||
                                    ( (result->allocSize < size) &&
                                      (alloc_size > result->allocSize) ) ) ) ) ||
                                ( (alloc_size == result->allocSize) &&
                                  ( (num_cached < result->numCachedNodes) ||
                                    (total_pool_size < result->allocPoolSize) ) )
                             )
                           )
                         )
                       )
                     )
                   )
                {
                    result->startNode = startNode;
                    result->endNode = lastNode;
                    result->startPA = PA_aligned;
                    result->allocSize = alloc_size;
                    result->allocPoolSize = total_pool_size;
                    result->allocPrio = alloc_prio;
                    result->numBusyNodes = num_busy;
                    result->numCachedNodes = num_cached;
                }
            }
            else
            {
                result->startNode = startNode;
                result->endNode = lastNode;
                result->startPA = PA_aligned;
                result->allocSize = alloc_size;
                result->allocPoolSize = total_pool_size;
                result->allocPrio = alloc_prio;
                result->numBusyNodes = num_busy;
                result->numCachedNodes = num_cached;
            }
        }

        if (page_size)
            PA_aligned = PA_aligned + page_size;
        else
            topNode = topNode->next;
    }

    if (result->startNode && result->endNode && result->allocSize)
    {
        result->allocStatus = VTCM_RESOURCE_SELECTED;
        result->priority = priority;
        return SUCCESS;
    }
    else
    {
        result->allocStatus = VTCM_RESOURCE_UNAVAILABLE;
        return FAILURE;
    }
}

//Allocation/reservation of an immediate request
int vtcm_allocate_immediate(int clientHandle,
                            vtcmMgr_partition_node_t *partition,
                            computeResMgr_node_t *context,
                            vtcm_select_t *selection,
                            vtcmMgr_wait_node_t *waitNode)
{
    vtcmMgr_mem_node_t *pTemp, *pBusyNode = NULL, *pPreNode = NULL, *pPostNode = NULL;
    vtcmMgr_cache_node_t *pCacheNode = NULL;
    unsigned int temp_size = 0, poolSize = 0;
    int memHandle, memHandleCreated = 0;

    if (!selection)
        return FAILURE;

    if (selection->allocStatus != VTCM_RESOURCE_SELECTED)
        return FAILURE;

    if (context && context->bAllocated && context->pVA && context->size)
        return SUCCESS;

    /*
     * Check if we have a node that can be allocated
     * If release requests are to be made, submit them
     * and mark the nodes as reserved with reserve priority
     * set to the caller priority
     */
    if (selection->startNode && selection->endNode)
    {
        if (!selection->numBusyNodes)
        {
            /*
             * We don't have any busy nodes in the list,
             * so, no release requests. Hence the allocation
             * can be complete
             */
            pTemp = NULL;
            pBusyNode = vtcm_memnode_pop();

            if (!pBusyNode)
            {
                /*
                 * So many mem nodes already created! return failure.
                 */
                selection->allocStatus = VTCM_RESOURCE_FAILED;

                return FAILURE;
            }

            poolSize = selection->allocPoolSize;

            /*
             * Check if we have additional memory in the start which has to
             * be segregated from the current allocation.
             */
            if (selection->startNode->PA < selection->startPA)
            {
                pPreNode = vtcm_memnode_pop();

                if (!pPreNode)
                {
                    /*
                     * So many mem nodes already created! return failure.
                     */
                    vtcm_memnode_push(pBusyNode);
                    selection->allocStatus = VTCM_RESOURCE_FAILED;

                    return FAILURE;
                }

                /*
                 * Reduce pool size by the extra size to be allocated to this
                 * pre node.
                 */
                poolSize = poolSize - (selection->startPA -
                                       selection->startNode->PA);
            }

            /*
             * Check if we have additional memory in the selection at the end
             */
            if (poolSize > selection->allocSize)
            {
                pPostNode = vtcm_memnode_pop();

                if (!pPostNode)
                {
                    /*
                     * So many mem nodes already created! return failure.
                     */
                    vtcm_memnode_push(pBusyNode);
                    if (pPreNode) vtcm_memnode_push(pPreNode);
                    selection->allocStatus = VTCM_RESOURCE_FAILED;

                    return FAILURE;
                }

            }

            pBusyNode->PA = selection->startPA;
            /*
             * Convert PA to VA and save it in terms of VA page (4K) for
             * mapping creation.
             */
            pBusyNode->VA = COMPUTE_RES_GET_VA(selection->startPA) >> 12;
            pBusyNode->size = selection->allocSize;

            if (context &&
                    context->memHandle == selection->startNode->memHandle)
            {
                /*
                 * We have a cached memhandle with the context and it matches
                 * with the memhandle from the free node. Update the busyNodes
                 * memhandle.
                 */
                pBusyNode->memHandle = context->memHandle;
            }
            else
            {
                pBusyNode->memHandle = -1;
            }

            /*
             * Close all open memhandles in this region which don't match
             * with the busy nodes mem handle (if present).
             */
            pTemp = selection->startNode;

            if (!pTemp->desc.bFree)
            {
                /*
                 * The selection says there are no busy nodes, but we got
                 * one!
                 */
                vtcm_memnode_push(pBusyNode);
                if (pPreNode) vtcm_memnode_push(pPreNode);
                if (pPostNode) vtcm_memnode_push(pPostNode);
                selection->allocStatus = VTCM_RESOURCE_FAILED;

                return FAILURE;
            }

            /*
             * Check if there are any idle mappings that needs to be
             * removed and memory cleaned.
             * For cached allocations, if the memhandle is the one for the
             * cached context, don't remove!
             */
            memHandle = -1;

            if ( (pTemp->memHandle >= 0) &&
                 (pBusyNode->memHandle != pTemp->memHandle ) )
            {
                memHandle = pTemp->memHandle; //take backup before reset

                if (pTemp->release_cb)
                    pTemp->release_cb(pTemp->clientHandle,
                                      pTemp->release_PA,
                               COMPUTE_RES_GET_VA(pTemp->release_PA));

                /*
                 * Close the memhandle with QURT, clear the memory
                 * and reset memhandle (-1).
                 */
                //qurt_qdi_close(pTemp->memHandle);

                pTemp->memHandle = -1;
                pTemp->release_cb = NULL;

                /*
                 * Clear the memory if the allocating client handle is
                 * different. The node's VA may no longer be
                 * valid, derive VA from the node's PA.
                 */
                if (pTemp->clientHandle != clientHandle)
                    hvx_memset( (void *)( COMPUTE_RES_GET_VA(pTemp->PA)),
                                0,
                                pTemp->size);

                /*
                 * The mem handle might be from one of the cached contexts
                 * in this free node. Reset the cached context's
                 * memhandle as well.
                 */
                pCacheNode = pTemp->cache_head;

                while (pCacheNode)
                {
                    if (pCacheNode->context->memHandle == memHandle)
                    {
                        pCacheNode->context->memHandle = -1;
                        break;
                    }

                    pCacheNode = pCacheNode->next;
                }

                /*
                 * look backward in the mem list and reset the
                 * memhandles if it matches with the current handle.
                 */
                pTemp = pTemp->prev;

                while (pTemp)
                {
                    /*
                     * If we hit a busy node, abort.
                     */
                    if (!pTemp->desc.bFree)
                        break;

                    /*
                     * If we hit a memhandle which is different, abort.
                     */
                    if (pTemp->memHandle != memHandle)
                        break;

                    pTemp->memHandle = -1;

                    /*
                     * Clear the memory if the allocating client handle is
                     * different. The node's VA may no longer be
                     * valid, derive VA from the node's PA.
                     */
                    if (pTemp->clientHandle != clientHandle)
                        hvx_memset( (void *)( COMPUTE_RES_GET_VA(pTemp->PA)),
                                    0,
                                    pTemp->size);

                    pTemp = pTemp->prev;
                }

            }

            /*
             * Now look forward till the end node and clear memhandles
             * which aren't a match with the busy node's memhandle.
             */
            pTemp = selection->startNode->next;

            while (pTemp && pTemp->prev != selection->endNode)
            {
                /*
                 * When we hit a busy node, bail out.
                 */
                if (!pTemp->desc.bFree)
                    break;

                if ( (pTemp->memHandle >= 0) &&
                     (pTemp->memHandle != pBusyNode->memHandle) )
                {

                    if (memHandle != pTemp->memHandle)
                    {
                        /*
                         * Backup the mem handle, close the handle and
                         * reset the memory. Reset the current node's mem
                         * handle.
                         */
                        memHandle = pTemp->memHandle;

                        if (pTemp->release_cb)
                            pTemp->release_cb(pTemp->clientHandle,
                                       pTemp->release_PA,
                                       COMPUTE_RES_GET_VA(pTemp->release_PA));

                        //qurt_qdi_close(pTemp->memHandle);

                        /*
                         * The mem handle might be from one of the cached contexts
                         * in this free node. Reset the cached context's
                         * memhandle as well.
                         */
                        pCacheNode = pTemp->cache_head;

                        while (pCacheNode)
                        {
                            if (pCacheNode->context->memHandle == memHandle)
                            {
                                pCacheNode->context->memHandle = -1;
                                break;
                            }

                            pCacheNode = pCacheNode->next;
                        }
                    }

                    pTemp->memHandle = -1;
                    pTemp->release_cb = NULL;

                    /*
                     * Clear the memory if the allocating client handle is
                     * different. The node's VA may no longer be
                     * valid, derive VA from the node's PA.
                     */
                    if (pTemp->clientHandle != clientHandle)
                        hvx_memset( (void *)( COMPUTE_RES_GET_VA(pTemp->PA) ),
                                    0,
                                    pTemp->size);
                }

                pTemp = pTemp->next;
            }

            /*
             * If the last node has an open mem handle, search subsequent
             * nodes in the list for the same mem handle and reset them.
             */
            while ( (memHandle >= 0) && (pTemp) )
            {
                /*
                 * Abort if we either hit a busy node or a node with different
                 * memory handle. Note that this portion is out of our
                 * selection, so don't close any new handles.
                 */
                if (!pTemp->desc.bFree || pTemp->memHandle != memHandle)
                    break;

                pTemp->memHandle = -1;

                /*
                 * Clear the memory if the allocating client handle is
                 * different. The node's VA may no longer be
                 * valid, derive VA from the node's PA.
                 */
                if (pTemp->clientHandle != clientHandle)
                    hvx_memset( (void *)( COMPUTE_RES_GET_VA(pTemp->PA) ),
                                0,
                                pTemp->size);

                pTemp = pTemp->next;
            }

            /*
             * Create a mapping for the user if it was not mapped already.
             */
            if (pBusyNode->memHandle < 0)
                pBusyNode->memHandle = 1;

            if (pBusyNode->memHandle < 0)
            {
                /*
                 * The memory mapping failed! we should have already removed
                 * any open mem handle for this mapping. Do cleanup and return
                 * error.
                 */
                vtcm_memnode_push(pBusyNode);
                if (pPreNode) vtcm_memnode_push(pPreNode);
                if (pPostNode) vtcm_memnode_push(pPostNode);
                selection->allocStatus = VTCM_RESOURCE_FAILED;

                return FAILURE;
            }

            if (pPreNode)
            {
                if (memlist_copy_contents(pPreNode, selection->startNode) )
                {
                   /*
                    * So many cache nodes already created! return failure.
                    */
                    if (memHandleCreated) qurt_qdi_close(pBusyNode->memHandle);
                    vtcm_memnode_push(pBusyNode);
                    vtcm_memnode_push(pPreNode);
                    if (pPostNode) vtcm_memnode_push(pPostNode);
                    selection->allocStatus = VTCM_RESOURCE_FAILED;

                    return FAILURE;
                }

                pPreNode->size = selection->startPA - selection->startNode->PA;
                pPreNode->PA = selection->startNode->PA;
            }

            if (pPostNode)
            {
                if (memlist_copy_contents(pPostNode, selection->endNode) )
                {
                    /*
                     * So many mem nodes already created! return failure.
                     */
                    if (memHandleCreated) qurt_qdi_close(pBusyNode->memHandle);
                    vtcm_memnode_push(pBusyNode);
                    if (pPreNode) vtcm_memnode_push(pPreNode);
                    vtcm_memnode_push(pPostNode);
                    selection->allocStatus = VTCM_RESOURCE_FAILED;

                    return FAILURE;
                }

                pPostNode->size = poolSize - selection->allocSize;
                pPostNode->PA = selection->endNode->PA +
                                selection->endNode->size -
                                pPostNode->size;
            }

            pBusyNode->VA = pBusyNode->VA << 12; //Convert page to address
            pBusyNode->clientHandle = clientHandle;
            pBusyNode->next = selection->endNode->next;
            pBusyNode->prev = selection->startNode->prev;
            pBusyNode->pFreeList = selection->startNode;

            if (!pBusyNode->prev)
                partition->pMemList = pBusyNode;
            else
                pBusyNode->prev->next = pBusyNode;

            if (pBusyNode->next)
                pBusyNode->next->prev = pBusyNode;

            *(unsigned int *)&(pBusyNode->desc) = 0;
            pBusyNode->busy_prio = selection->priority;
            pBusyNode->cache_head = NULL;
            pBusyNode->num_cached = 0;
            pBusyNode->reserving_node = NULL;
            pBusyNode->parent = NULL;

            if (context)
                pBusyNode->desc.bReleasable = context->bReleasable;

            pBusyNode->context = context;
            pBusyNode->release_cb = NULL;
            pBusyNode->release_PA = 0;

            if (pPreNode)
            {
                /*
                 * We need to partition the starting node
                 * due to PA alignment
                 */
                selection->startNode->PA = selection->startPA;
                selection->startNode->size = selection->startNode->size -
                                             pPreNode->size;
                memlist_add_node_before(&partition->pMemList,
                                        pBusyNode,
                                        pPreNode);
            }

            selection->startNode->prev = NULL;

            if (pPostNode)
            {
                /*
                 * We need to partition the end node
                 * due to extra size
                 */
                selection->endNode->size = selection->endNode->size -
                                               pPostNode->size;
                memlist_add_node_after(pBusyNode, pPostNode);
            }

            selection->endNode->next = NULL;

            /*
             * Now mark the busy node as parent in all the child
             * nodes under the free list. By now, we have delinked the free
             * list from the partitions memlist.
             */
            pTemp = pBusyNode->pFreeList;

            while (pTemp)
            {
                pTemp->parent = pBusyNode;

                /*
                 * Check if we have any reserving wait node, unreserve as
                 * the memory is already allocated. Unreserve all resources
                 * as VTCM is one of the resources which is being unreserved,
                 * having other still reserved doesn't make sense.
                 */
                if (pTemp->reserving_node)
                {
                    unreserve_wait_node(pTemp->reserving_node);
                    pTemp->reserving_node = NULL;
                }

                pTemp = pTemp->next;
            }

            if (context)
            {
                context->clientHandle = clientHandle;
                context->pVA = pBusyNode->VA;
                context->size = pBusyNode->size;
                context->pageSize = selection->pageSize;
                context->priority = selection->priority;
                context->bAllocated = TRUE;
                context->memHandle = pBusyNode->memHandle;
            }

            selection->allocStatus = VTCM_RESOURCE_ASSIGNED;
        }
        else if (waitNode)
        {
            /*
             * We have busy nodes in the selection, but are releasable.
             * User has provided a waitNode to queuing into the wait list.
             * Send release requests and mark them reserved. Update reserving
             * priority as current requester priority. Mark Free nodes in
             * the selection as reserved as well.
             */

            waitNode->reserved_pVA = (selection->startPA - g_vtcmMgr.PA) +
                                     g_vtcmMgr.VA;
            waitNode->reserved_size = selection->allocSize;
            pTemp = selection->startNode;
            temp_size = 0;

            while ( (temp_size < selection->allocSize) && pTemp)
            {
                if (pTemp->desc.bFree && !pTemp->parent)
                {
                    /*
                     * Found a free node. Can be reserved.
                     * Overwrite reserving node and priority
                     * and mark the node as reserved
                     */
                    pTemp->reserving_node = waitNode;
                }
                else if (!pTemp->parent)
                {
                    /*
                     * Found a busy node. Should be releasable.
                     * Check if release request is already made
                     * otherwise post a release request and mark
                     * the node as reserved
                     */
                    if (!pTemp->context->bReleaseRequested)
                    {
                        invoke_release_callback(pTemp->context);
                    }

                    pTemp->reserving_node = waitNode;
                }
                else
                {
                    /*
                     * This is a free node, but there is a parent node that
                     * is busy. Should be releasable, check if release request
                     * is already made, otherwise post a release request and
                     * reserve the node
                     */
                    if (!pTemp->parent->context->bReleaseRequested)
                    {
                        invoke_release_callback(pTemp->parent->context);
                    }

                    pTemp->parent->reserving_node = waitNode;
                }

                if (temp_size == 0)
                {
                    /*
                     * The first node might not be completely used.
                     * Account for alignment holes in the accumulated size.
                     */
                    temp_size += pTemp->size -
                               (selection->startPA - selection->startNode->PA);
                }
                else
                {
                    temp_size += pTemp->size;
                }

                /*
                 * We might be traversing the free list within a busy node,
                 * merge back to the main list if that is the case
                 */
                if (!pTemp->next && pTemp->parent)
                {
                    pTemp = pTemp->parent;
                }
                else
                {
                    pTemp = pTemp->next;
                }
            }

            selection->allocStatus = VTCM_RESOURCE_RESERVED;
        }
        else
        {
            /*
             * Wait node is not provided! return failure.
             */
            selection->allocStatus = VTCM_RESOURCE_FAILED;

            return FAILURE;
        }

    }
    else
    {
        selection->allocStatus = VTCM_RESOURCE_FAILED;

        return FAILURE;
    }

    return SUCCESS;
}

//Search algorithm for cached requests
static int vtcm_search_cached(vtcmMgr_partition_node_t *partition,
                              unsigned int size,
                              unsigned int min_size,
                              unsigned int page_size,
                              unsigned int size_given,
                              unsigned int min_size_given,
                              unsigned char priority,
                              vtcm_select_t *result)
{
    vtcmMgr_mem_node_t *topNode;
    vtcmMgr_mem_node_t *leafNode, *lastNode, *startNode, *backupNode;
    unsigned int alloc_size, total_pool_size, PA_aligned = 0, temp_size;
    unsigned char num_busy, num_cached, b_update, alloc_prio;

    if (!result)
        return FAILURE;

    if (!partition)
    {
        result->allocStatus = VTCM_RESOURCE_FAILED;
        return FAILURE;
    }

    if (!partition->pMemList)
    {
        result->allocStatus = VTCM_RESOURCE_FAILED;
        return FAILURE;
    }

    /*
     * With the given page size, check if we can serve the request based
     * on partition definition.
     */
    if (page_size)
    {
        PA_aligned = COMPUTE_RES_ALIGN(partition->PA, page_size);

        if ( (PA_aligned >= (partition->PA + partition->size) ) ||
             ( (PA_aligned + min_size_given) >
                            (partition->PA + partition->size) ) )
        {
            result->allocStatus = VTCM_RESOURCE_FAILED;
            return FAILURE;
        }
    }

    topNode = partition->pMemList;
    result->startPA = NULL;
    result->allocSize = 0;
    result->allocPoolSize = 0;
    result->allocPrio = 0;
    result->numBusyNodes = 0;
    result->numCachedNodes = 0;
    result->startNode = NULL;
    result->endNode = NULL;
    result->allocStatus = VTCM_RESOURCE_DEFAULT;
    result->pageSize = page_size;

    if (page_size)
    {
        PA_aligned = COMPUTE_RES_ALIGN(topNode->PA, page_size);
    }

    while (topNode)
    {

        if (page_size)
        {

            if (PA_aligned < topNode->PA)
            {
                /*
                 * The top node has advanced/crossed the PA_aligned, mostly
                 * because it couldn't fit in the allocation. Advance PA to
                 * the next aligned memory from the top nodes PA.
                 */
                PA_aligned = COMPUTE_RES_ALIGN(topNode->PA, page_size);
            }

            if (PA_aligned >= (topNode->PA + topNode->size) )
            {
                /*
                 * Find the node having the first aligned PA and start with
                 * that as the top node.
                 */
                while (topNode)
                {
                    if (PA_aligned < (topNode->PA + topNode->size) )
                        break;

                    topNode = topNode->next;
                }

                /*
                 * Break if the entire memlist is already parsed and the
                 * PA_aligned has moved out of the partition region.
                 */
                if (!topNode)
                    break;
            }
        }

        alloc_size = 0;
        total_pool_size = 0;
        alloc_prio = 0;
        num_busy = 0;
        num_cached = 0;
        startNode = NULL;
        leafNode = topNode;
        backupNode =  NULL;
        lastNode = NULL;

        while (leafNode)
        {
            b_update = 0;

            if (backupNode)
            {
                /*
                 * This is one of the shadow nodes overlapping a busy node.
                 */
                if (alloc_size < min_size)
                {
                    b_update = 1;

                    if (alloc_prio < 4)
                        alloc_prio = 4;
                }
            }
            else if (leafNode->desc.bFree &&
                     !leafNode->cache_head &&
                     !leafNode->reserving_node)
            {
                /*
                 * Found a free node which isn't cached and reserved
                 */
                if (alloc_size < size)
                {
                    b_update = 1;

                    if (alloc_prio < 1)
                        alloc_prio = 1;
                }
            }
            else if (leafNode->desc.bFree &&
                       (!leafNode->cache_head ||
                         (leafNode->cache_head &&
                           (leafNode->cache_head->prio >= priority) ) ) &&
                       (!leafNode->reserving_node ||
                         (leafNode->reserving_node &&
                          (leafNode->reserving_node->priority > priority) ) ) )
            {
                /*
                 * Found a cached/reserved but free node with allocator priority
                 * lower (or equal to in cache case) than the current requester
                 */
                if (alloc_size < size)
                {
                    b_update = 1;
                    num_cached += leafNode->num_cached;

                    if (alloc_prio < 2)
                        alloc_prio = 2;
                }
            }
            else if ( (leafNode->desc.bFree &&
                      (leafNode->cache_head ||
                         leafNode->reserving_node) ) )
            {
                /*
                 * Found a cached/reserved but free node with allocator priority
                 * higher (or equal to in case of reserved) than the current
                 * requester
                 */
                if (alloc_size < min_size)
                {
                    b_update = 1;
                    num_cached += leafNode->num_cached;

                    if (alloc_prio < 3)
                        alloc_prio = 3;
                }
            }
            else if (!leafNode->desc.bFree)
            {
                /*
                 * Found a busy node, traverse through the free list instead
                 */
                if (alloc_size < min_size)
                {
                    num_busy++;

                    if (leafNode->pFreeList)
                    {
                        backupNode = leafNode->next;
                        leafNode = leafNode->pFreeList;
                        continue;
                    }
                }
            }

            /*
             * Check if we have a candidate node. b_update = 1 signifies that.
             */
            if (b_update)
            {
                /*
                 * Found a possible node, update trackers
                 */
                if (startNode)
                {
                    alloc_size += leafNode->size;
                    total_pool_size += leafNode->size;
                }
                else
                {
                    /*
                     * Check if page size is provided, if yes, we should have
                     * started with a aligned PA and it should be in the leaf
                     * node. Safe to check it again here.
                     */
                    if (page_size)
                    {
                        if (PA_aligned >= (leafNode->PA + leafNode->size))
                        {
                            PA_aligned = NULL;
                            num_cached = 0;
                            num_busy = 0;
                            alloc_prio = 0;
                        }
                    }
                    else
                    {
                        PA_aligned = leafNode->PA;
                    }

                    if (PA_aligned)
                    {
                        alloc_size = leafNode->size -
                                            (PA_aligned - leafNode->PA);
                        total_pool_size = leafNode->size;
                        startNode = leafNode;
                    }
                }

                lastNode = leafNode;

                if (alloc_size >= size)
                    break;
            }
            else
            {
                /*
                 * We have hit a node which can't be allocated, see if we
                 * have accummulated enough size, otherwise, advance the outer
                 * loop to start with this node.
                 */
                if (alloc_size < min_size)
                {
                    if (backupNode)
                        topNode = backupNode;
                    else
                        topNode = leafNode;
                }

                break;
            }
            /*
             * Done with the current node, move on to the next one in the list
             */
            leafNode = leafNode->next;

            /*
             * Check if we were parsing free list within a busy node and reached
             * end of the free list. Then the loop should continue from where it
             * left i.e. the next node of the busy node. Check if there is a backupNode
             * node created (which should point to the next node after the busy node)
             * and update the current node.
             */
            if (!leafNode && backupNode)
            {
                leafNode = backupNode;
                backupNode = NULL;
            }
        }

        if (startNode &&
                ( (page_size &&
                    (alloc_size < size) &&
                    (alloc_size >= min_size_given) &&
                    !partition->next && !lastNode->next) ||
                  (alloc_size >= min_size) ) )
        {
            /*
             * If page size is provided and our last node is the last in the
             * physical VTCM section, check if the allocated size
             * accommodates the given size / given minimum size.
             */

            /*
             * - If gathered size accommodates entire size, update allocation
             * size as size.
             * - else, if minimum page size is provided, align the allocation
             * size based on the given minimum page size.
             */
            if (alloc_size >= size)
            {
                alloc_size = size;
            }
            else if (page_size && (partition->next || lastNode->next) )
            {
                temp_size = alloc_size;
                alloc_size = alloc_size & ( ~(page_size - 1) );
                backupNode = NULL;
                /*
                 * We may have to cut down on the allocation size due to
                 * page size requirement.
                 * check if we have to update last node and pool size.
                 * Ideally alloc priority needs to be recalculated, but that's
                 * an overhead which we are trying to avoid here.
                 */
                while (lastNode)
                {
                    /*
                     * NOTE: As this is a cached search, the start and last
                     * nodes will always be free nodes.
                     * So, even after any modification in this loop, the same
                     * constraint will be applicable.
                     */
                    if (temp_size >= lastNode->size)
                        temp_size = temp_size - lastNode->size;
                    else
                        temp_size = 0;

                    if (temp_size >= alloc_size)
                    {
                        /*
                         * We are over allocated, adust the last node.
                         */
                        if (lastNode->cache_head)
                        {
                            num_cached--;
                        }

                        total_pool_size -= lastNode->size;

                        if (!lastNode->prev && lastNode->parent)
                        {
                            lastNode = lastNode->parent->prev;
                            num_busy--;
                        }
                        else
                        {
                            lastNode = lastNode->prev;
                        }

                        if (lastNode && !lastNode->desc.bFree)
                        {
                            /*
                             * We have a hit a busy node. As cached search
                             * works only on free list, get to the last node
                             * with in the free list and use that as last node
                             * Once the free list is done, hop back to the
                             * main one by using parent node.
                             */
                            backupNode = lastNode->pFreeList;

                            while (backupNode->next)
                            {
                                backupNode = backupNode->next;
                            }

                            lastNode = backupNode;
                        }
                    }
                    else
                    {
                        break;
                    }

                }
            }

            if (result->allocSize)
            {
                if ( (alloc_prio < result->allocPrio) ||
                     ( (alloc_prio == result->allocPrio) &&
                       ( (num_busy < result->numBusyNodes) ||
                         ( (num_busy == result->numBusyNodes) &&
                           ( (num_cached < result->numCachedNodes) ||
                             ( (num_cached == result->numCachedNodes) &&
                               ( ( (page_size &&
                                      ( ( (result->allocSize > size_given) &&
                                          (alloc_size < result->allocSize) &&
                                          (alloc_size >= size_given) ) ||
                                        ( (result->allocSize < size_given) &&
                                          (alloc_size > result->allocSize) ) ) ) ||
                                    (!page_size &&
                                       ( ( (result->allocSize > size) &&
                                           (alloc_size < result->allocSize) &&
                                           (alloc_size >= size) ) ||
                                         ( (result->allocSize < size) &&
                                           (alloc_size > result->allocSize) ) ) ) ) ||
                                  ( (alloc_size == result->allocSize) &&
                                    ( (num_cached < result->numCachedNodes) ||
                                      (total_pool_size < result->allocPoolSize) ) )
                               )
                             )
                           )
                         )
                       )
                     )
                   )
                {
                    result->startNode = startNode;
                    result->endNode = lastNode;
                    result->startPA = PA_aligned;
                    result->allocSize = alloc_size;
                    result->allocPoolSize = total_pool_size;
                    result->allocPrio = alloc_prio;
                    result->numBusyNodes = num_busy;
                    result->numCachedNodes = num_cached;
                }
            }
            else
            {
                result->startNode = startNode;
                result->endNode = lastNode;
                result->startPA = PA_aligned;
                result->allocSize = alloc_size;
                result->allocPoolSize = total_pool_size;
                result->allocPrio = alloc_prio;
                result->numBusyNodes = num_busy;
                result->numCachedNodes = num_cached;
            }
        }

        if (page_size)
            PA_aligned = PA_aligned + page_size;
        else
            topNode = topNode->next;
    }

    if (result->startNode && result->endNode && result->allocSize)
    {
        result->allocStatus = VTCM_RESOURCE_SELECTED;
        result->priority = priority;
        return SUCCESS;
    }
    else
    {
        result->allocStatus = VTCM_RESOURCE_UNAVAILABLE;
        return FAILURE;
    }
}

//Assign a cached request
int vtcm_assign_cached(int clientHandle,
                       vtcmMgr_partition_node_t *partition,
                       computeResMgr_node_t *context,
                       vtcm_select_t *selection)
{
    vtcmMgr_mem_node_t *pTemp, *pPreNode, *pPostNode;
    vtcmMgr_cache_node_t *temp_cache, *new_cache;
    vtcmMgr_mem_node_t *pBackup = NULL;

    unsigned int temp_size = 0, poolSize = 0;

    if (!selection)
        return FAILURE;

    if (!partition)
    {
        selection->allocStatus = VTCM_RESOURCE_FAILED;
        return FAILURE;
    }

    if (!partition->pMemList)
    {
        selection->allocStatus = VTCM_RESOURCE_FAILED;
        return FAILURE;
    }

    /*
     * Check if we have a node that can be marked as
     * cached. Allocate a context and mark all the nodes
     * appropriately
     */
    if (selection->allocStatus == VTCM_RESOURCE_SELECTED &&
        selection->startNode &&
        selection->endNode)
    {
        pPreNode = NULL;
        pPostNode = NULL;
        poolSize = selection->allocPoolSize;

        if (selection->startNode->PA < selection->startPA)
        {
            pPreNode = vtcm_memnode_pop();

            if (!pPreNode)
            {
                /*
                 * No more mem nodes! return error.
                 */
                selection->allocStatus = VTCM_RESOURCE_FAILED;

                return FAILURE;
            }

            if (memlist_copy_contents(pPreNode, selection->startNode) )
            {
                /*
                 * No more cache nodes! return error.
                 */
                selection->allocStatus = VTCM_RESOURCE_FAILED;

                return FAILURE;
            }

            pPreNode->size = (selection->startPA - selection->startNode->PA);
            pPreNode->PA = selection->startNode->PA;
            poolSize = poolSize - pPreNode->size;
        }

        if (poolSize > selection->allocSize)
        {
            pPostNode = vtcm_memnode_pop();

            if (!pPostNode)
            {
                /*
                 * No more mem nodes! return error.
                 */
                if (pPreNode) vtcm_memnode_push(pPreNode);
                selection->allocStatus = VTCM_RESOURCE_FAILED;

                return FAILURE;
            }

            if (memlist_copy_contents(pPostNode, selection->endNode) )
            {
                /*
                 * No more cache nodes! return error.
                 */
                if (pPreNode) vtcm_memnode_push(pPreNode);
                vtcm_memnode_push(pPostNode);
                selection->allocStatus = VTCM_RESOURCE_FAILED;

                return FAILURE;
            }

            pPostNode->size = poolSize - selection->allocSize;
            pPostNode->PA = selection->endNode->PA +
                            selection->endNode->size -
                            pPostNode->size;
        }

        /*
         * Check if we have enough cache nodes
         */
        pTemp = selection->startNode;
        new_cache = pVtcmCacheNodeList;

        while (pTemp && (temp_size < selection->allocPoolSize) )
        {
            if (pTemp->desc.bFree)
            {
                /*
                 * A free node in the selection, create a cache
                 * node and append it to the cache list. Update
                 * number of cache requests and cache priority.
                 * cache priority = lowest of all cached requests.
                 */
                if (!new_cache)
                {
                    if (pPreNode) vtcm_memnode_push(pPreNode);
                    if (pPostNode) vtcm_memnode_push(pPostNode);
                    selection->allocStatus = VTCM_RESOURCE_FAILED;

                    return FAILURE;
                }

                temp_size += pTemp->size;
                new_cache = new_cache->next;
            }
            else
            {
                /*
                 * Found a busy node in the selection. Traverse
                 * the free list and update all free node's cache
                 * list with a new cache node. Update cache priority
                 * accordingly for all the nodes.
                 */
                pBackup = pTemp->next;
                pTemp = pTemp->pFreeList;
                continue;
            }

            if (!pTemp->next && pBackup)
            {
                /*
                 * We are at the end of a FreeList in a busy node and have
                 * a backup point to merge to the main list.
                 */
                pTemp = pBackup;
                pBackup = NULL;
            }
            else if (!pTemp->next && pTemp->parent)
            {
                /*
                 * We are at the end of a free list, merge to the parent one.
                 */
                pTemp = pTemp->parent->next;
            }
            else
            {
                /*
                 * We are not at the end of a free list, continue to the next
                 * node.
                 */
                pTemp = pTemp->next;
            }
        }

        context->bAllocated = FALSE;
        context->priority = selection->priority;
        context->pVA = COMPUTE_RES_GET_VA(selection->startPA);
        context->size = selection->allocSize;
        context->pageSize = selection->pageSize;
        context->clientHandle = clientHandle;

        if (pPreNode)
        {
            /*
             * We need to partition the starting node
             * due to PA alignment
             */
            selection->startNode->PA = selection->startPA;
            selection->startNode->size = selection->startNode->size - pPreNode->size;

            if (selection->startNode->parent)
                memlist_add_node_before(&selection->startNode->parent->pFreeList,
                                        selection->startNode,
                                        pPreNode);
            else
                memlist_add_node_before(&partition->pMemList,
                                        selection->startNode,
                                        pPreNode);
        }

        if (pPostNode)
        {
            /*
             * We need to partition the end node
             * due to extra size
             */

            selection->endNode->size = selection->endNode->size -
                                       pPostNode->size;
            memlist_add_node_after(selection->endNode, pPostNode);
        }

        pTemp = selection->startNode;
        temp_size = 0;

        while (pTemp && (temp_size < selection->allocSize) )
        {
            if (pTemp->desc.bFree)
            {
                /*
                 * A free node in the selection, create a cache
                 * node and append it to the cache list. Update
                 * number of cache requests and cache priority.
                 * cache priority = lowest of all cached requests.
                 */
                temp_size += pTemp->size;
                new_cache = vtcm_cachenode_pop();
                new_cache->context = context;
                new_cache->prio = selection->priority;
                temp_cache = pTemp->cache_head;

                while (temp_cache)
                {
                    if (temp_cache->prio > new_cache->prio)
                        break;
                    else if (temp_cache->next == NULL)
                        break;

                    temp_cache = temp_cache->next;
                }

                if (temp_cache)
                {
                    if (temp_cache->prio > new_cache->prio)
                    {
                        /*
                         * Append new cache node before the current node
                         */
                        new_cache->next = temp_cache;
                        new_cache->prev = temp_cache->prev;
                        temp_cache->prev = new_cache;

                        if (new_cache->prev)
                            new_cache->prev->next = new_cache;
                        else
                            pTemp->cache_head = new_cache;
                    }
                    else
                    {
                        /*
                         * Append new cache node after the current node
                         */
                        new_cache->next = temp_cache->next;
                        new_cache->prev = temp_cache;
                        temp_cache->next = new_cache;

                        if (new_cache->next)
                            new_cache->next->prev = new_cache;
                    }
                }
                else
                {
                    pTemp->cache_head = new_cache;
                    new_cache->next = NULL;
                    new_cache->prev = NULL;
                }

                pTemp->num_cached++;
            }
            else
            {
                /*
                 * Found a busy node in the selection. Traverse
                 * the free list and update all free node's cache
                 * list with a new cache node. Update cache priority
                 * accordingly for all the nodes.
                 */
                pBackup = pTemp->next;
                pTemp = pTemp->pFreeList;
                continue;
            }

            if (!pTemp->next && pBackup)
            {
                /*
                 * We are at the end of a FreeList in a busy node and have
                 * a backup point to merge to the main list.
                 */
                pTemp = pBackup;
                pBackup = NULL;
            }
            else if (!pTemp->next && pTemp->parent)
            {
                /*
                 * We are at the end of a free list, merge to the parent one.
                 */
                pTemp = pTemp->parent->next;
            }
            else
            {
                /*
                 * We are not at the end of a free list, continue to the next
                 * node.
                 */
                pTemp = pTemp->next;
            }
        }

        selection->allocStatus = VTCM_RESOURCE_ASSIGNED;

        return SUCCESS;
    }

    selection->allocStatus = VTCM_RESOURCE_FAILED;

    return FAILURE;
}

static int vtcm_allocate_cached(computeResMgr_node_t *context,
                                vtcm_select_t *selection,
                                int priority)
{
    vtcmMgr_mem_node_t *mem, *backup = NULL;
    vtcmMgr_partition_node_t *partition;
    unsigned int PA;
    int found = 0, foundBusy = 0;

    if (!selection || !context)
        return FAILURE;

    if (!context->bCached || context->bAllocated)
    {
        /*
         * Not a cached request or
         * the cached request is already allocated!
         * return failure
         */
        selection->allocStatus = VTCM_RESOURCE_FAILED;
        return FAILURE;
    }

    if (context->pVA && context->size)
    {
        /*
         * We have got a virtual address and size pair assigned for this
         * context, get the partition first.
         */
        partition = vtcm_get_partition(context->applicationId);

        if (!partition)
        {
            /*
             * Didn't get any partition for the application id,
             * return failure.
             */
            selection->allocStatus = VTCM_RESOURCE_FAILED;
            return FAILURE;
        }
        else if (!partition->pMemList)
        {
            /*
             * Couldnt' find any memnode in the partition, return failure.
             */
            selection->allocStatus = VTCM_RESOURCE_FAILED;
            return FAILURE;
        }

        /*
         * Get the PA for this context
         */
        PA = COMPUTE_RES_GET_PA(context->pVA);

        selection->startNode = NULL;
        selection->startPA = NULL;
        selection->endNode = NULL;
        selection->numBusyNodes = 0;
        selection->numCachedNodes = 0;
        selection->allocSize = 0;
        selection->allocPoolSize = 0;
        selection->allocPrio = 0;
        selection->priority = priority;
        selection->allocStatus = VTCM_RESOURCE_DEFAULT;
        selection->pageSize = context->pageSize;

        mem = partition->pMemList;

        while (mem)
        {
            /*
             * Try to find the first node with the given PA.
             */
            if (!mem->desc.bFree)
            {
                if (found &&
                    ( !mem->context ||
                      (mem->context &&
                          ( (mem->context->priority <= priority) ||
                            (!mem->context->bReleasable) ) ) )
                   )
                {
                    /*
                     * We have got a busy node which is either held by
                     * a higher priority client or is not releasable.
                     * Break the loop and mark the request to wait.
                     */
                    foundBusy = 1;

                    break;
                }
                else
                {
                    /*
                     * Hit a busy node, traverse the free list.
                     * Backup the next node to get back to the search.
                     */
                    backup = mem->next;
                    mem = mem->pFreeList;

                    if (found)
                        selection->numBusyNodes++;

                    continue;
                }
            }

            if (mem->PA == PA)
            {
                /*
                 * Set num busy nodes to one on the first find if we are in
                 * a free list under a busy node.
                 */
                if (mem->parent)
                {
                    if (!mem->parent->context ||
                            (mem->parent->context &&
                               ( (mem->parent->context->priority <= priority) ||
                                  (!mem->parent->context->bReleasable) ) ) )
                    {
                        /* We have hit a busy node which cannot be allocated.
                         * Break the loop.
                         */
                        foundBusy = 1;

                        break;
                    }

                    selection->numBusyNodes = 1;
                }

                found = 1;
                selection->startNode = mem;
                selection->startPA = PA;
            }

            if (found)
            {
                selection->allocSize += mem->size;
                selection->allocPoolSize += mem->size;

                /*
                 * See if we have hit the last node. As this is cached, we
                 * expect the node to end exactly at the context->size from PA.
                 * Break the search loop once the end node is found.
                 */
                if ( (mem->PA + mem->size) == (PA + context->size) )
                {
                    selection->endNode = mem;
                    break;
                }
            }

            if (backup && !mem->next)
            {
                /*
                 * We have reached end of a free list, jump back to the backup
                 */
                mem = backup;
                backup = NULL;
            }
            else
                mem = mem->next;
        }

        if (selection->startNode && selection->endNode)
        {
            selection->allocStatus = VTCM_RESOURCE_SELECTED;

            return SUCCESS;
        }
        else if (foundBusy)
        {
            selection->allocStatus = VTCM_RESOURCE_UNAVAILABLE;

            return FAILURE;
        }
        else
        {
            selection->allocStatus = VTCM_RESOURCE_FAILED;

            return FAILURE;
        }
    }
    else
    {
        selection->allocStatus = VTCM_RESOURCE_FAILED;

        return FAILURE;
    }
}

static void copy_request_into_waitnode(vtcmMgr_wait_node_t *waitNode,
                                      compute_res_attr_t *request)
{
    /*
     * Copy the compute resource manager requests from provided
     * request structure to the provided wait node.
     * waitNode and request structures should be non-NULL.
     */
    waitNode->bHMX = request->attr.bHMX;
    waitNode->bSerialize = request->attr.bSerialize;
    waitNode->bCacheable = request->attr.bCacheable;
    waitNode->bReleasable = request->attr.releaseCallback ? TRUE : FALSE;
    waitNode->size = request->attr.vtcmSize;
    waitNode->min_size = request->attr.minVtcmSize;
    waitNode->min_page_size = request->attr.minPageSize;
    waitNode->bSinglePage = request->attr.bVtcmSinglePage;
    waitNode->applicationId = request->attr.appId;
}

static int unreserve_vtcm(vtcmMgr_wait_node_t *waitNode)
{
    /*
     * Unreserve VTCM nodes reserved by the given wait node
     */
    vtcmMgr_mem_node_t *node;
    vtcmMgr_partition_node_t *partition =
            get_partition_from_app_id(waitNode->clientHandle,
                                      waitNode->applicationId);
    int found = 0;

    if (!partition)
        return FAILURE;

    node = partition->pMemList;

    while (node)
    {

        if (node->reserving_node == waitNode)
        {
            found = 1;
            node->reserving_node = NULL;
        }

        node = node->next;
    }

    if (found)
        return SUCCESS;
    else
        return FAILURE;
}

static int unreserve_wait_node(vtcmMgr_wait_node_t *waitNode)
{
    int result = SUCCESS;

    if (!waitNode)
        return FAILURE;

    /*
     * Unreserve resources pointed out by the wait node.
     * resources to be unreserved:
     * 1. Serialization
     * 2. HMX
     * 3. VTCM
     */
    if (g_vtcmMgr.serialize.reserve == waitNode)
    {
        /*
         * The current waiter has reserved the serialization resource.
         */
        g_vtcmMgr.serialize.reserve = NULL;
    }

    if (g_vtcmMgr.hmx.reserve == waitNode)
    {
        /*
         * The current waiter has reserved HMX resource. Release the same.
         */
        g_vtcmMgr.hmx.reserve = NULL;
    }

    if (waitNode->reserved_pVA && waitNode->reserved_size)
    {
        /*
         * The current waiter has reserved VTCM section, release the same.
         */
        result = unreserve_vtcm(waitNode);
    }

    return result;
}

static int invoke_release_callback(computeResMgr_node_t *context)
{
    vtcmMgr_qdi_obj_t *qdiObj;
    unsigned long long message;

    if (!context)
        return FAILURE;

    if (!context->clientHandle || !context->bReleasable)
        return FAILURE;

    if (context->bReleaseRequested)
        return SUCCESS;
    /*
     * Look up release request mailbox for the client handle and send
     * the context release request via the mailbox.
     */
    qdiObj = driver_get_qdi(context->clientHandle);

    if (!qdiObj)
        return FAILURE;

    if (!qdiObj->mailboxId)
        return FAILURE;

    /*
     * Message will point to the context to be released.
     */
    message = (unsigned int)context;
    qurt_mailbox_send(qdiObj->mailboxId,
                      QURT_MAILBOX_SEND_OVERWRITE,
                      message);
    context->bReleaseRequested = TRUE;

    return SUCCESS;
}

int try_allocate_from_waitq(vtcmMgr_wait_node_t *waitNode)
{
    int result = SUCCESS;
    unsigned char contextCreated = FALSE;
    vtcm_select_t selection;
    crm_res_alloc_t serialize = CRM_RES_NOT_REQUESTED;
    crm_res_alloc_t hmx = CRM_RES_NOT_REQUESTED;
    crm_res_alloc_t vtcm = CRM_RES_NOT_REQUESTED;
    vtcmMgr_partition_node_t *partition;
    unsigned int size = 0, min_size = 0, page_size = 0;

    if (waitNode->bSerialize)
    {
        if (g_vtcmMgr.serialize.context && !g_vtcmMgr.serialize.reserve)
        {
            /*
             * Serialization is taken, check the priority and
             * releasable property
             */
            if ( (g_vtcmMgr.serialize.context->priority > waitNode->priority) &&
                 g_vtcmMgr.serialize.context->bReleasable)
            {
                serialize = CRM_RES_RELEASE_NEEDED;
            }
            else
            {
                serialize = CRM_RES_UNAVAILABLE;
            }
        }
        else if (g_vtcmMgr.serialize.reserve &&
                (g_vtcmMgr.serialize.reserve != waitNode) )
        {
            /*
             * An entity is reserving the resource. Check priority
             * of the reserving entity
             */
            if (g_vtcmMgr.serialize.reserve->priority > waitNode->priority)
            {
                /*
                 * The requester is of higher priority than the reserving
                 * entity:
                 * - If the resource is already taken and a release request
                 * is already placed, set wait for release.
                 * - If the resource is already taken and a release request
                 * is pending, set release needed.
                 * - If the resource is free, set available.
                 *
                 */
                if (g_vtcmMgr.serialize.context &&
                    g_vtcmMgr.serialize.context->bReleaseRequested)
                    serialize = CRM_RES_WAIT_FOR_RELEASE;
                else if (g_vtcmMgr.serialize.context)
                    serialize = CRM_RES_RELEASE_NEEDED;
                else
                    serialize = CRM_RES_AVAILABLE;
            }
            else
            {
                serialize = CRM_RES_UNAVAILABLE;
            }
        }
        else
        {
            /*
             * Resource is available, no allocating or reserving entity or
             * the wait node is the reserving entity
             */
            serialize = CRM_RES_AVAILABLE;
        }

        if (serialize >= CRM_RES_UNAVAILABLE)
            result = FAILURE;
    }

    if ( (result == SUCCESS) && waitNode->bHMX)
    {
        if (g_vtcmMgr.hmx.context && !g_vtcmMgr.hmx.reserve)
        {
            /*
             * HMX is taken, check the priority of the allocator and
             * releasable property
             */
            if ( (g_vtcmMgr.hmx.context->priority > waitNode->priority) &&
                 (g_vtcmMgr.hmx.context->bReleasable) )
            {
                hmx = CRM_RES_RELEASE_NEEDED;
            }
            else
            {
                hmx = CRM_RES_UNAVAILABLE;
            }
        }
        else if (g_vtcmMgr.hmx.reserve && (g_vtcmMgr.hmx.reserve != waitNode) )
        {
            /*
             * An entity is reserving the resource. Check priority
             * of the reserving entity
             */
            if (g_vtcmMgr.hmx.reserve->priority > waitNode->priority)
            {
                /*
                 * The requester is of higher priority than the reserving
                 * entity:
                 * - If the resource is already taken and a release request
                 * is already placed, set wait for release.
                 * - If the resource is already taken and a release request
                 * is pending, set release needed.
                 * - If the resource is free, set available.
                 *
                 */
                if (g_vtcmMgr.hmx.context &&
                    g_vtcmMgr.hmx.context->bReleaseRequested)
                    hmx = CRM_RES_WAIT_FOR_RELEASE;
                else if (g_vtcmMgr.hmx.context)
                    hmx = CRM_RES_RELEASE_NEEDED;
                else
                    hmx = CRM_RES_AVAILABLE;
            }
            else
            {
                hmx = CRM_RES_UNAVAILABLE;
            }
        }
        else
        {
            /*
             * Resource is available, either no reserving entity or the
             * reserving entity is the wait node itself.
             */
            hmx = CRM_RES_AVAILABLE;
        }

        if (hmx >= CRM_RES_UNAVAILABLE)
            result = FAILURE;
    }

    if ( (result == SUCCESS) &&
         ( (waitNode->size) || (waitNode->bCacheable && waitNode->pContext) ) )
    {
        /*
         * The wait node has a pending VTCM request.
         * 1. It can be a cached request
         * 2. It can be a non-cached request
         * 3. It can be a reserved request (while being cached or non-cached)
         */
        if (waitNode->bCacheable && waitNode->pContext)
        {
            /*
             * This is a cacheable request.
             * Could have been reserved.
             * Try to get the allocation status and populate the selection.
             */
            result = vtcm_allocate_cached(waitNode->pContext,
                                          &selection,
                                          waitNode->priority);
            /*
             * Load partition for the allocation call.
             */
            partition = get_partition_from_app_id(waitNode->clientHandle,
                                                  waitNode->applicationId);

            if (!partition)
            {
                selection.allocStatus = VTCM_RESOURCE_FAILED;
                result = FAILURE;
            }
        }
        else
        {
            partition = get_partition_from_app_id(waitNode->clientHandle,
                                                  waitNode->applicationId);

            if (partition)
            {
                if (waitNode->bSinglePage)
                {
                    size = vtcm_alloc_align(waitNode->size, 1);
                    min_size = 0;
                    page_size = size;
                }
                else if (waitNode->min_page_size)
                {
                    page_size = vtcm_alloc_align(waitNode->min_page_size, 1);
                    size = COMPUTE_RES_ALIGN(waitNode->size, page_size);
                    min_size = COMPUTE_RES_ALIGN(waitNode->min_size, page_size);
                }
                else
                {
                    size = vtcm_alloc_align(waitNode->size, 0);
                    min_size = vtcm_alloc_align(waitNode->min_size, 0);
                    page_size = 0;
                }

                if (!min_size)
                    min_size = size;
                /*
                 * Not a cached request, but can be a reserved one.
                 * Even if we have a PA and size, search again to see if we
                 * can get a free one, otherwise, stick with the reserved one.
                 */
                result = vtcm_search_noncached(partition,
                                               size,
                                               min_size,
                                               page_size,
                                               waitNode->size,
                                               waitNode->min_size,
                                               waitNode->priority,
                                               waitNode,
                                               &selection);

                if (waitNode->reserved_pVA && waitNode->reserved_size)
                {

                    if (!result &&
                            selection.allocStatus == VTCM_RESOURCE_SELECTED)
                    {
                        /*
                         * Wait node already has a reserved section, we launched
                         * a new search which yielded a success. Irrespective
                         * of what was reserved, we are going to go ahead
                         * with the new search result and reserve yet again if
                         * required. So unreserve the previous section to keep
                         * it clean. //TODO
                         */

                        unreserve_vtcm(waitNode);
                    }
                }
            }
            else
            {
                selection.allocStatus = VTCM_RESOURCE_FAILED;
                result = FAILURE;
            }
        }

        if (!result && selection.allocStatus == VTCM_RESOURCE_SELECTED)
        {
            if (selection.numBusyNodes)
                vtcm = CRM_RES_RELEASE_NEEDED;
            else
                vtcm = CRM_RES_AVAILABLE;
        }
        else if (selection.allocStatus == VTCM_RESOURCE_UNAVAILABLE)
        {
            vtcm = CRM_RES_UNAVAILABLE;
        }
        else
        {
            vtcm = CRM_RES_INVALID_PARAMS;
        }

    }

    if ( (serialize == CRM_RES_NOT_REQUESTED) &&
         (hmx == CRM_RES_NOT_REQUESTED) &&
         (vtcm == CRM_RES_NOT_REQUESTED) )
    {
        result = COMPUTE_RES_FAIL_INVALID_ARGS;
    }
    else if ( (serialize <= CRM_RES_AVAILABLE) &&
              (hmx <= CRM_RES_AVAILABLE) &&
              (vtcm <= CRM_RES_AVAILABLE)
    )
    {
        /*
         * All requested resources are available. Go ahead and assign them
         */
        result = SUCCESS;

        if (!waitNode->pContext && waitNode->bComputeRes)
        {
            waitNode->pContext = compute_context_alloc(waitNode->clientHandle);

            if (!waitNode->pContext)
            {
                /*
                 * Context allocation has failed - all contexts used?!
                 * handle failure,
                 */
                result = FAILURE;
            }
            else
            {
                contextCreated = TRUE;
                waitNode->pContext->bCached = waitNode->bCacheable;
                waitNode->pContext->bHMX = waitNode->bHMX;
                waitNode->pContext->bSerialize = waitNode->bSerialize;
                waitNode->pContext->bReleasable = waitNode->bReleasable;
                waitNode->pContext->priority = waitNode->priority;
                waitNode->pContext->applicationId = waitNode->applicationId;
            }
        }

        if (!result && (vtcm == CRM_RES_AVAILABLE) )
        {
            /*
             * Immediate request for VTCM and it is available. Use the
             * selection that we got from the search and allocate.
             */
            result = vtcm_allocate_immediate(waitNode->clientHandle,
                                             partition,
                                             waitNode->pContext,
                                             &selection,
                                             NULL);

            if (result && contextCreated)
            {
                /*
                 * Handle failure. Context deallocation
                 */
                compute_context_delete(waitNode->pContext,
                                       waitNode->clientHandle);
            }
            else if (!result &&
                    (selection.allocStatus == VTCM_RESOURCE_ASSIGNED) )
            {
                waitNode->pVA = COMPUTE_RES_GET_VA(selection.startPA);
                waitNode->size = selection.allocSize;
            }
        }

        if (!result && (serialize == CRM_RES_AVAILABLE) )
        {
            g_vtcmMgr.serialize.context = waitNode->pContext;

            if (g_vtcmMgr.serialize.reserve)
            {
                /*
                 * The resource is taken, reserving entity needs to relinquish
                 * this resource.
                 */
                unreserve_wait_node(g_vtcmMgr.serialize.reserve);
            }
        }

        if (!result && (hmx == CRM_RES_AVAILABLE) )
        {
            g_vtcmMgr.hmx.context = waitNode->pContext;

            if (g_vtcmMgr.hmx.reserve)
            {
                /*
                 * The resource is taken, reserving entity needs to relinquish
                 * this resource.
                 */
                unreserve_wait_node(g_vtcmMgr.hmx.reserve);
            }
        }

    }
    else if ( (serialize < CRM_RES_UNAVAILABLE) &&
              (hmx < CRM_RES_UNAVAILABLE) &&
              (vtcm < CRM_RES_UNAVAILABLE) )
    {

        if (vtcm >= CRM_RES_AVAILABLE)
        {
            if (vtcm == CRM_RES_AVAILABLE)
            {
                /*
                 * VTCM is available, but other resources are not. Instead
                 * of allocating, only reserve. Doing this by saying that we
                 * have hit busy nodes in search.
                 */
                if (!selection.numBusyNodes)
                    selection.numBusyNodes = 1;
            }
            /*
             * Selected VTCM has busy nodes. Either release request for
             * these nodes has already been sent or to be sent.
             *
             * We should reach here only for immediate requests.
             */
            result = vtcm_allocate_immediate(waitNode->clientHandle,
                                             partition,
                                             waitNode->pContext,
                                             &selection,
                                             waitNode);
        }

        if (!result && (serialize >= CRM_RES_AVAILABLE) )
        {
            /*
             * The resource is available (unavailability is already checked)
             * 2 possibilities:
             * - Releasable request but release request has to be made
             * - Releasable request and release request has already made.
             *
             * If there is a reserving entity, release request would have
             * been made. Go ahead and unreserve the waiter.
             *
             * Issue a release request if need be.
             *
             * Reserve the resource in the end.
             */
            if (g_vtcmMgr.serialize.reserve &&
                    (g_vtcmMgr.serialize.reserve != waitNode) )
            {
                /*
                 * There is already a reserving entity, probably of lower
                 * priority, release the reserve
                 */
                unreserve_wait_node(g_vtcmMgr.serialize.reserve);
            }

            if (serialize == CRM_RES_RELEASE_NEEDED)
            {
                invoke_release_callback(g_vtcmMgr.serialize.context);
            }

            g_vtcmMgr.serialize.reserve = waitNode;
        }

        if (!result && (hmx >= CRM_RES_AVAILABLE) )
        {
            /*
             * The resource is available (unavailability is already checked)
             * 2 possibilities:
             * - Releasable request but release request has to be made
             * - Releasable request and release request has already made.
             *
             * If there is a reserving entity, release request would have
             * been made. Go ahead and unreserve the waiter.
             *
             * Issue a release request if need be.
             *
             * Reserve the resource in the end.
             */
            if (g_vtcmMgr.hmx.reserve &&
                    (g_vtcmMgr.hmx.reserve != waitNode) )
            {
                /*
                 * There is already a reserving entity, probably of lower
                 * priority, release the reserve
                 */
                unreserve_wait_node(g_vtcmMgr.hmx.reserve);
            }

            if (hmx == CRM_RES_RELEASE_NEEDED)
            {
                invoke_release_callback(g_vtcmMgr.hmx.context);
            }

            g_vtcmMgr.hmx.reserve = waitNode;
        }

        result = FAILURE;
    }
    else
    {
        result = FAILURE;
    }

    return result;
}

// compute resource manager request
int compute_resource_manager_request(int clientHandle,
                                     computeResMgr_node_t *context,
                                     compute_res_attr_t *request,
                                     int priority,
                                     vtcmMgr_wait_node_t **waitNode,
                                     computeResMgr_node_t **contextOut)
{
    /*
     * We have 3 resources managed. 1. Serialization 2. HMX 3. VTCM
     * Prioritization is done for all the resource requests. A single
     * wait queue to manage pending allocations. A request is successful
     * if all resources requested can be assigned. A cached request only
     * tries VTCM allocation and doesn't go into a wait queue.
     * If a resource isn't available and a release request is created,
     * other resources even available won't be allocated but reserved
     * and the request will be pushed into wait queue.
     */
    int result = SUCCESS;
    unsigned char contextCreated = FALSE;
    crm_res_alloc_t serialize = CRM_RES_NOT_REQUESTED;
    crm_res_alloc_t hmx = CRM_RES_NOT_REQUESTED;
    crm_res_alloc_t vtcm = CRM_RES_NOT_REQUESTED;
    vtcm_select_t selection;
    vtcmMgr_partition_node_t *partition = NULL;
    unsigned int size = 0, min_size = 0, page_size = 0;

    if (waitNode)
        *waitNode = NULL;

    if (!request && !context)
    {
        return COMPUTE_RES_FAIL_INVALID_ARGS;
    }

    /*
     * Check if the request is for serialization.
     * For cacheable request, as the request is deferred to a cacheable acquire
     * no need to check or assign the serialization resource.
     * If request is a cacheable acquire, check if serialization is in the
     * provided context.
     */
    if ( (request &&
             (!request->attr.bCacheable) && (request->attr.bSerialize) ) ||
         (context && context->bSerialize) )
    {
        if (g_vtcmMgr.serialize.context &&
                (context == g_vtcmMgr.serialize.context) )
        {
            /*
             * It is a cached request and the context is already assigned!
             */
            serialize = CRM_RES_EALREADY;
        }
        else if (g_vtcmMgr.serialize.context && !g_vtcmMgr.serialize.reserve)
        {
            /*
             * Serialization is taken, check the priority and
             * releasable property
             */
            if ( (g_vtcmMgr.serialize.context->priority > priority) &&
                 g_vtcmMgr.serialize.context->bReleasable)
            {
                serialize = CRM_RES_RELEASE_NEEDED;
            }
            else
            {
                serialize = CRM_RES_UNAVAILABLE;
            }
        }
        else if (g_vtcmMgr.serialize.reserve)
        {
            /*
             * An entity is reserving the resource. Check priority
             * of the reserving entity
             */
            if (g_vtcmMgr.serialize.reserve->priority > priority)
            {
                /*
                 * The requester is of higher priority than the reserving
                 * entity:
                 * - If the resource is already taken and a release request
                 * is already placed, set wait for release.
                 * - If the resource is already taken and a release request
                 * is pending, set release needed.
                 * - If the resource is free, set available.
                 *
                 */
                if (g_vtcmMgr.serialize.context &&
                        g_vtcmMgr.serialize.context->bReleaseRequested)
                    serialize = CRM_RES_WAIT_FOR_RELEASE;
                else if (g_vtcmMgr.serialize.context)
                    serialize = CRM_RES_RELEASE_NEEDED;
                else
                    serialize = CRM_RES_AVAILABLE;
            }
            else
            {
                serialize = CRM_RES_UNAVAILABLE;
            }
        }
        else
        {
            /*
             * Resource is available, no allocating or reserving entity
             */
            serialize = CRM_RES_AVAILABLE;
        }
    }

    /*
     * Check if the request is for HMX.
     * For cacheable request, as the request is deferred to a cacheable acquire
     * no need to check or assign the HMX resource.
     * If the request is for a cacheable acquire, check if context has HMX
     * requested.
     */
    if ( (request &&
            (!request->attr.bCacheable) && (request->attr.bHMX) ) ||
           (context && context->bHMX) )
    {
        if (g_vtcmMgr.hmx.context && (context == g_vtcmMgr.hmx.context) )
        {
            /*
             * It is a cached request and the context is already assigned!
             */
            hmx = CRM_RES_EALREADY;
        }
        else if (g_vtcmMgr.hmx.context && !g_vtcmMgr.hmx.reserve)
        {
            /*
             * HMX is taken, check the priority of the allocator and
             * releasable property
             */
            if ( (g_vtcmMgr.hmx.context->priority > priority) &&
                 (g_vtcmMgr.hmx.context->bReleasable) )
            {
                hmx = CRM_RES_RELEASE_NEEDED;
            }
            else
            {
                hmx = CRM_RES_UNAVAILABLE;
            }
        }
        else if (g_vtcmMgr.hmx.reserve)
        {
            /*
             * An entity is reserving the resource. Check priority
             * of the reserving entity
             */
            if (g_vtcmMgr.hmx.reserve->priority > priority)
            {
                /*
                 * The requester is of higher priority than the reserving
                 * entity:
                 * - If the resource is already taken and a release request
                 * is already placed, set wait for release.
                 * - If the resource is already taken and a release request
                 * is pending, set release needed.
                 * - If the resource is free, set available.
                 *
                 */
                if (g_vtcmMgr.hmx.context &&
                    g_vtcmMgr.hmx.context->bReleaseRequested)
                    hmx = CRM_RES_WAIT_FOR_RELEASE;
                else if (g_vtcmMgr.hmx.context)
                    hmx = CRM_RES_RELEASE_NEEDED;
                else
                    hmx = CRM_RES_AVAILABLE;
            }
            else
            {
                hmx = CRM_RES_UNAVAILABLE;
            }
        }
        else
        {
            /*
             * Resource is available, no allocating or reserving entity
             */
            hmx = CRM_RES_AVAILABLE;
        }
    }

    if ( (serialize < CRM_RES_UNAVAILABLE) &&
         (hmx < CRM_RES_UNAVAILABLE) )
    {
        if (request && request->attr.vtcmSize)
        {

            if (request->attr.bVtcmSinglePage)
            {
                size = vtcm_alloc_align(request->attr.vtcmSize, 1);
                min_size = 0;
                page_size = size;
            }
            else if (request->attr.minPageSize)
            {
                page_size = vtcm_alloc_align(request->attr.minPageSize, 1);
                size = COMPUTE_RES_ALIGN(request->attr.vtcmSize, page_size);
                min_size = COMPUTE_RES_ALIGN(request->attr.minVtcmSize, page_size);
            }
            else
            {
                size = vtcm_alloc_align(request->attr.vtcmSize, 0);
                min_size = vtcm_alloc_align(request->attr.minVtcmSize, 0);
                page_size = 0;
            }

            partition = get_partition_from_app_id(clientHandle,
                                                  request->attr.appId);

            if (partition)
            {

                /*
                 * If minimum size is provided as 0, it says size is the absolute
                 * requirement, so set minimum size = size.
                 */
                if (!min_size)
                    min_size = size;

                if ( (request->attr.vtcmSize <= partition->size) &&
                     (min_size <= size))
                {
                    if (!request->attr.bCacheable)
                    {
                        result = vtcm_search_noncached(partition,
                                                       size,
                                                       min_size,
                                                       page_size,
                                                       request->attr.vtcmSize,
                                                       request->attr.minVtcmSize,
                                                       priority,
                                                       NULL,
                                                       &selection);

                        if (result &&
                              (selection.allocStatus ==
                                      VTCM_RESOURCE_UNAVAILABLE) )
                        {
                            vtcm = CRM_RES_UNAVAILABLE;
                        }
                        else if (selection.allocStatus ==
                                      VTCM_RESOURCE_SELECTED)
                        {
                            if (selection.numBusyNodes)
                                vtcm = CRM_RES_RELEASE_NEEDED;
                            else
                                vtcm = CRM_RES_AVAILABLE;
                        }
                        else
                        {
                            vtcm = CRM_RES_INVALID_PARAMS;
                        }
                    }
                    else
                    {
                        result = vtcm_search_cached(partition,
                                                    size,
                                                    min_size,
                                                    page_size,
                                                    request->attr.vtcmSize,
                                                    request->attr.minVtcmSize,
                                                    priority,
                                                    &selection);


                        if (result &&
                              (selection.allocStatus ==
                                      VTCM_RESOURCE_UNAVAILABLE) )
                        {
                            vtcm = CRM_RES_UNAVAILABLE;
                        }
                        else if (selection.allocStatus ==
                                      VTCM_RESOURCE_SELECTED)
                        {
                            vtcm = CRM_RES_AVAILABLE;
                        }
                        else
                        {
                            vtcm = CRM_RES_INVALID_PARAMS;
                        }
                    }
                }
                else
                {
                    vtcm = CRM_RES_INVALID_PARAMS;
                }
            }
            else
            {
                vtcm = CRM_RES_INVALID_PARAMS;
            }
        }
        else if (context)
        {
            if (context->bAllocated)
            {
                vtcm = CRM_RES_EALREADY;
            }
            else
            {
                /*
                 * Allocation is for an assigned cache request.
                 * Check if we have VTCM assigned to this context, if yes,
                 * populate selection and update vtcm state accordingly.
                 * Get partition for subsequent allocate request.
                 */
                partition = get_partition_from_app_id(clientHandle,
                                                      context->applicationId);

                if (partition)
                {
                    result = vtcm_allocate_cached(context, &selection, priority);

                    if (result &&
                          (selection.allocStatus == VTCM_RESOURCE_UNAVAILABLE) )
                    {
                        vtcm = CRM_RES_UNAVAILABLE;
                    }
                    else if (selection.allocStatus == VTCM_RESOURCE_SELECTED)
                    {
                        if (selection.numBusyNodes)
                            vtcm = CRM_RES_RELEASE_NEEDED;
                        else
                            vtcm = CRM_RES_AVAILABLE;
                    }
                    else
                    {
                        vtcm = CRM_RES_INVALID_PARAMS;
                    }
                }
                else
                {
                    vtcm = CRM_RES_INVALID_PARAMS;
                }
            }
        }
    }

    if ( (serialize == CRM_RES_NOT_REQUESTED) &&
         (hmx == CRM_RES_NOT_REQUESTED) &&
         (vtcm == CRM_RES_NOT_REQUESTED) )
    {
        result = COMPUTE_RES_FAIL_INVALID_ARGS;
    }
    else if ( (serialize == CRM_RES_INVALID_PARAMS) ||
              (hmx == CRM_RES_INVALID_PARAMS) ||
              (vtcm == CRM_RES_INVALID_PARAMS) )
    {
        result =  COMPUTE_RES_FAIL_INVALID_ARGS;
    }
    else if ( (serialize == CRM_RES_EALREADY) ||
              (hmx == CRM_RES_EALREADY) ||
              (vtcm == CRM_RES_EALREADY) )
    {
        result = COMPUTE_RES_FAIL_ALLOCATED_ALREADY;
    }
    else if ( (serialize <= CRM_RES_AVAILABLE) &&
              (hmx <= CRM_RES_AVAILABLE) &&
              (vtcm <= CRM_RES_AVAILABLE)
    )
    {
        /*
         * All requested resources are available. Go ahead and assign them
         */
        if (!context)
        {
            context = compute_context_alloc(clientHandle);

            if (context)
            {
                contextCreated = TRUE;
                context->bCached = request->attr.bCacheable;
                context->bHMX = request->attr.bHMX;
                context->bSerialize = request->attr.bSerialize;
                context->bReleasable = (request->attr.releaseCallback != NULL);
                context->priority = priority;
                context->applicationId = request->attr.appId;
                result = SUCCESS;
            }
            else
            {
                /*
                 * No available context, return failure
                 */
                result = COMPUTE_RES_FAIL_CONTEXTS_FULL;
            }
        }
        else
        {
            result = SUCCESS;
        }

        if (!result && (vtcm == CRM_RES_AVAILABLE) )
        {
            if (!request || (request && !request->attr.bCacheable) )
            {
                /*
                 * Immediate request for VTCM and it is available. Use the
                 * selection that we got from the search and allocate.
                 */
                result = vtcm_allocate_immediate(clientHandle,
                                                 partition,
                                                 context,
                                                 &selection,
                                                 NULL); //We are not expecting to wait
            }
            else
            {
                /*
                 * Cached request for VTCM and is available. Use the
                 * selection that we got from the search and assign.
                 */
                result = vtcm_assign_cached(clientHandle,
                                            partition,
                                            context,
                                            &selection);
            }

            if (result)
            {

                if (contextCreated)
                {
                    /*
                     * Handle failure. Context deallocation
                     */
                    compute_context_delete(context,
                                           clientHandle);
                    context = NULL;
                }

                result = COMPUTE_RES_FAIL_RES_UNAVAILABLE;
            }

        }

        if (!result && (serialize == CRM_RES_AVAILABLE) )
        {
            g_vtcmMgr.serialize.context = context;

            if (g_vtcmMgr.serialize.reserve)
            {
                /*
                 * The resource is taken, reserving entity needs to relinquish
                 * this resource.
                 */
                unreserve_wait_node(g_vtcmMgr.serialize.reserve);
            }
        }

        if (!result && (hmx == CRM_RES_AVAILABLE) )
        {
            g_vtcmMgr.hmx.context = context;

            if (g_vtcmMgr.hmx.reserve)
            {
                /*
                 * The resource is taken, reserving entity needs to relinquish
                 * this resource.
                 */
                unreserve_wait_node(g_vtcmMgr.hmx.reserve);
            }
        }
    }
    else if ( waitNode &&
              ( (serialize < CRM_RES_UNAVAILABLE) &&
                (hmx < CRM_RES_UNAVAILABLE) &&
                (vtcm < CRM_RES_UNAVAILABLE) ) )
    {
        /*
         * Need to wait for one or more resources. Either release request is
         * needed or release already requested, push the current one to the
         * waitQ, raise release requests if needed, reserve resources.
         */
        if (context)
        {
            /*
             * It's a cacheable allocation, so no request structure to pass
             * for wait node creation
             */
            result = vtcmMgr_create_wait_node(waitNode,
                                              priority,
                                              NULL);

            if (!result)
            {
                (*waitNode)->bComputeRes = TRUE;
                (*waitNode)->clientHandle = clientHandle;
                (*waitNode)->bCacheable = TRUE;
                (*waitNode)->pContext = context;
                (*waitNode)->size = context->size;
                (*waitNode)->min_page_size = context->pageSize;
                (*waitNode)->bHMX = context->bHMX;
                (*waitNode)->bSerialize = context->bSerialize;
                (*waitNode)->bReleasable = context->bReleasable;
            }
        }
        else
        {
            result = vtcmMgr_create_wait_node(waitNode,
                                              priority,
                                              request);

            if (!result)
                (*waitNode)->clientHandle = clientHandle;

        }

        if (result)
        {
            result = COMPUTE_RES_FAIL_WAITQ_FULL;
        }
        else
        {
            (*waitNode)->clientHandle = clientHandle;

            if (vtcm >= CRM_RES_AVAILABLE)
            {
                if (vtcm == CRM_RES_AVAILABLE)
                {
                    /*
                     * VTCM is available, one or more of other resources are
                     * to be released (or release requested). We need to
                     * reserve the VTCM chunk to avoid allocating to a lower
                     * priority request. But as VTCM is available, we simulate
                     * a reserve by saying that the selection has busy nodes.
                     * TODO
                     */
                    if (!selection.numBusyNodes)
                        selection.numBusyNodes = 1;
                }
                /*
                 * Selected VTCM has busy nodes. Either release request for
                 * these nodes has already been sent or to be sent.
                 *
                 * We should reach here only for immediate requests.
                 */
                result = vtcm_allocate_immediate(clientHandle,
                                                 partition,
                                                 context,
                                                 &selection,
                                                 *waitNode);

                if (result)
                    result = COMPUTE_RES_FAIL_NO_MEM;
            }

            if (!result && (serialize >= CRM_RES_AVAILABLE) )
            {
                /*
                 * The resource is available (unavailability is already checked)
                 * 3 possibilities:
                 * - Request is available but other resources may not be
                 * - Releasable request but release request has to be made
                 * - Releasable request and release request has already made.
                 *
                 * If there is a reserving entity, release request would have
                 * been made. Go ahead and unreserve the waiter.
                 *
                 * Issue a release request if need be.
                 *
                 * Reserve the resource in the end.
                 */
                if (g_vtcmMgr.serialize.reserve &&
                        (g_vtcmMgr.serialize.reserve != *waitNode) )
                {
                    /*
                     * There is already a reserving entity, probably of lower
                     * priority, release the reserve
                     */
                    unreserve_wait_node(g_vtcmMgr.serialize.reserve);
                }

                if (serialize == CRM_RES_RELEASE_NEEDED)
                {
                    invoke_release_callback(g_vtcmMgr.serialize.context);
                }

                g_vtcmMgr.serialize.reserve = *waitNode;
            }

            if (!result && (hmx >= CRM_RES_AVAILABLE) )
            {
                /*
                 * The resource is available (unavailability is already checked)
                 * 3 possibilities:
                 * - Resource is available, but other resources may not be
                 * - Releasable request but release request has to be made
                 * - Releasable request and release request has already made.
                 *
                 * If there is a reserving entity, release request would have
                 * been made. Go ahead and unreserve the waiter.
                 *
                 * Issue a release request if need be.
                 *
                 * Reserve the resource in the end.
                 */
                if (g_vtcmMgr.hmx.reserve &&
                        (g_vtcmMgr.hmx.reserve != *waitNode) )
                {
                    /*
                     * There is already a reserving entity, probably of lower
                     * priority, release the reserve
                     */
                    unreserve_wait_node(g_vtcmMgr.hmx.reserve);
                }

                if (hmx == CRM_RES_RELEASE_NEEDED)
                {
                    invoke_release_callback(g_vtcmMgr.hmx.context);
                }

                g_vtcmMgr.hmx.reserve = *waitNode;
            }

        }
    }
    else if ( waitNode &&
              ( (serialize == CRM_RES_UNAVAILABLE) ||
                (hmx == CRM_RES_UNAVAILABLE) ||
                (vtcm == CRM_RES_UNAVAILABLE) ) )
    {
        /*
         * One or more resources is unavailable, push the current
         * one into the waitQ
         */
        if (context)
        {
            /*
             * It's a cacheable allocation, so no request structure to pass
             * for wait node creation
             */
            result = vtcmMgr_create_wait_node(waitNode,
                                              priority,
                                              NULL);

            if (!result)
            {
                (*waitNode)->bComputeRes = TRUE;
                (*waitNode)->clientHandle = clientHandle;
                (*waitNode)->bCacheable = TRUE;
                (*waitNode)->pContext = context;
                (*waitNode)->size = context->size;
                (*waitNode)->min_page_size = context->pageSize;
                (*waitNode)->bHMX = context->bHMX;
                (*waitNode)->bSerialize = context->bSerialize;
                (*waitNode)->bReleasable = context->bReleasable;
            }
            else
                result = COMPUTE_RES_FAIL_WAITQ_FULL;

        }
        else
        {
            /*
             * A cacheable request without a context shouldn't ideally go
             * into the waitQ, return failure if true.
             */
            if (request && request->attr.bCacheable)
            {
                result = COMPUTE_RES_FAIL_RES_UNAVAILABLE;
            }
            else
            {
                result = vtcmMgr_create_wait_node(waitNode,
                                                  priority,
                                                  request);
                if (!result)
                    (*waitNode)->clientHandle = clientHandle;
                else
                    result = COMPUTE_RES_FAIL_WAITQ_FULL;
            }
        }
    }
    else if (!waitNode)
    {
        /*
         * Wait node was not requested and the request couldn't be served.
         * return resource unavailable.
         */
        result = COMPUTE_RES_FAIL_RES_UNAVAILABLE;
    }

    /*
     * If context has to be returned
     */
    if (contextOut)
        *contextOut = context;

    return result;
}

static int vtcm_release(int clientHandle,
                        unsigned int pVA,
                        computeResMgr_node_t *context,
                        vtcmMgr_partition_node_t *partition,
                        unsigned char bCached)
{
    int result = SUCCESS, found = 0;
    vtcmMgr_mem_node_t *mem, *temp;
    vtcmMgr_mem_node_t *postNode = NULL;
    vtcmMgr_mem_node_t *preNode = NULL;
    qurt_addr_t PA;
    unsigned int size;
    vtcmMgr_cache_node_t *cache, *temp_cache;

    if (!partition && context)
        partition = get_partition_from_app_id(context->clientHandle,
                                              context->applicationId);

    if (!partition)
        return FAILURE;

    if (!pVA)
        return FAILURE;

    if (context && bCached && context->bCached && !context->bAllocated)
    {
        /*
         * Context is a cached one. Not in allocated state.
         * The request being made is a cached release.
         * As the context is not allocated yet, return failure.
         */
        return FAILURE;
    }

    if ( (!context) ||
         (context && (!context->bCached || context->bAllocated) ) )
    {

        mem = partition->pMemList;
        /*
         * The context is either cached or non-cached, but allocated.
         * Search for the busy node with exact VA and size match.
         */
        while (mem)
        {

            if (mem->VA > pVA)
            {
                /*
                 * We have somehow passed the section, couldn't find the node
                 */
                break;
            }

            if ( (!mem->desc.bFree) &&
                  (mem->VA == pVA) &&
                  (mem->clientHandle == clientHandle) )
            {
                /*
                 * Found the busy node. Release the busy node and pull out the
                 * free list from busy node. If context is cached and request
                 * is not a cached release, release cache contexts from free
                 * nodes.
                 * Copy any reserving entities to the free list.
                 */
                found = 1;

                if ( (mem->memHandle >= 0) &&
                    (!bCached ||
                     (context && !context->bCached) ||
                     (mem->release_cb)) )
                {
                    /*
                     * Either its an non-cached allocation, or a non cached
                     * release of a cached allocation, or a release callback
                     * is registered on this node, unmap for now
                     */

                    if (mem->release_cb)
                        mem->release_cb(mem->clientHandle,
                                        mem->release_PA,
                                   COMPUTE_RES_GET_VA(mem->release_PA));

                    //qurt_qdi_close(mem->memHandle);

                    mem->memHandle = -1;
                    mem->release_cb = NULL;

                    if (context)
                        context->memHandle = -1;

                    hvx_memset( (void *)mem->VA, 0, mem->size);
                }

                /*
                 * Get the freelist to the front end
                 */
                temp = mem->pFreeList;
                temp->prev = mem->prev;

                if (temp->prev == NULL)
                    partition->pMemList = temp;
                else
                    temp->prev->next = temp;

                while (temp)
                {

                    /*
                     * Check if there is a reserving wait node. If yes,
                     * mark the free nodes corresponding to the reserved
                     * size as reserved.
                     */
                    if (mem->reserving_node)
                    {
                        if (mem->reserving_node->pContext)
                        {
                            /*
                             * Wait node has a context, it should be a cached
                             * allocation. free nodes would have been carved
                             * appropriately for this allocation already.
                             */
                            PA = COMPUTE_RES_GET_PA(mem->reserving_node->pContext->pVA);
                            size = mem->reserving_node->pContext->size;

                            if ( (temp->PA >= PA) &&
                                 ( (temp->PA + temp->size) <= (PA + size) ) )
                            {
                                temp->reserving_node = mem->reserving_node;
                            }
                        }
                        else if (mem->reserving_node->reserved_pVA)
                        {
                            /*
                             * No context assigned to the waiter. Should be a
                             * non-cached allocation.
                             */
                            PA = COMPUTE_RES_GET_PA(mem->reserving_node->reserved_pVA);
                            size = mem->reserving_node->reserved_size;

                            if ( (temp->PA < PA) &&
                                 ( (temp->PA + temp->size) > PA ) )
                            {
                                /*
                                 * Carve a pre-node
                                 */
                                preNode = vtcm_memnode_pop();

                                if (preNode)
                                {
                                    if (SUCCESS ==
                                          memlist_copy_contents(preNode, temp))
                                    {
                                        preNode->size = PA - temp->PA;
                                        temp->PA = PA;
                                        temp->size = temp->size - preNode->size;
                                        memlist_add_node_before(&partition->pMemList, temp, preNode);
                                    }
                                }
                            }

                            if ( (temp->PA < (PA + size) ) &&
                                 ( (temp->PA + temp->size) > (PA + size) ))
                            {
                                postNode = vtcm_memnode_pop();

                                if (postNode)
                                {
                                    if (SUCCESS ==
                                         memlist_copy_contents(postNode, temp))
                                    {
                                        postNode->size = (temp->PA + temp->size) - (PA + size);
                                        postNode->PA = temp->PA + temp->size - postNode->size;
                                        temp->size = temp->size - postNode->size;
                                        memlist_add_node_after(temp, postNode);
                                    }
                                }
                            }

                            if ( (temp->PA >= PA) &&
                                 ( (temp->PA + temp->size) <= (PA + size) ) )
                            {
                                temp->reserving_node = mem->reserving_node;
                            }
                        }
                    }

                    /*
                     * Copy the mem handle from the busy node into the free
                     * list nodes and mark the parent as NULL as there is no
                     * busy node on top.
                     */
                    temp->memHandle = mem->memHandle;
                    temp->parent = NULL;

                    if (temp->next == NULL)
                    {
                        temp->next = mem->next;

                        if (mem->next)
                            mem->next->prev = temp;

                        break;
                    }

                    temp = temp->next;
                }

                /*
                 * Release the busy node, we have already stitched back the
                 * free nodes to the mem list
                 */
                vtcm_memnode_push(mem);

                break;
            }

            mem = mem->next;
        }

        if (!found)
            result = HAP_VTCM_RELEASE_FAIL_REQUEST_NOT_FOUND;
        else if (context)
            context->bAllocated = FALSE;
    }

    if (!result && !bCached && context && context->bCached)
    {
        /*
         * The context is a cached allocation, any allocation from this
         * context would have been freed in the above loop. Now, this is
         * a release of the context call, remove this context from all
         * free node's cached list.
         */
        mem = partition->pMemList;
        found = 0;
        temp = NULL;
        /*
         * VA would have been lost as the busy node would have been
         * freed in the above loop (either now or previous cached release
         * call). Comparisons will now be in free node where VA will be 0.
         * Hence, use PA instead.
         */
        PA = COMPUTE_RES_GET_PA(context->pVA);

        while (mem)
        {
            if (!mem->desc.bFree)
            {
                /*
                 * We have hit a busy node, as cached nodes are maintained only
                 * in the free list, traverse the free list of this busy node
                 * and in the end merge back to the next one.
                 */
                temp = mem->next;
                mem = mem->pFreeList;
                continue;
            }

            if (mem->PA == PA)
            {
                found = 1;

                cache = mem->cache_head;

                while (cache)
                {

                    if (cache->context == context)
                    {
                        if (cache->next)
                            cache->next->prev = cache->prev;

                        if (cache->prev)
                            cache->prev->next = cache->next;
                        else
                            mem->cache_head = cache->next;

                        vtcm_cachenode_push(cache);
                        mem->num_cached--;

                        if ( (context->memHandle >= 0) &&
                                (mem->memHandle == context->memHandle) )
                        {
                            /*
                             * The context has an active mapping request.
                             * Close the same and reset memory handles and
                             * underlying memory as well.
                             */
                            //qurt_qdi_close(context->memHandle);

                            context->memHandle = -1;
                            mem->memHandle = -1;

                            hvx_memset( (void *)context->pVA,
                                  0,
                                  context->size);
                        }

                        break;
                    }

                    cache = cache->next;
                }

            }
            else if (found)
            {
                /*
                 * We have already found the starting node, see if subsequent
                 * nodes fall under the cached section.
                 */
                if ( (mem->PA + mem->size) <= (PA + context->size) )
                {
                    cache = mem->cache_head;

                    while (cache)
                    {
                        if (cache->context == context)
                        {
                            if (cache->next)
                                cache->next->prev = cache->prev;

                            if (cache->prev)
                                cache->prev->next = cache->next;
                            else
                                mem->cache_head = cache->next;

                            vtcm_cachenode_push(cache);
                            mem->num_cached--;

                            break;
                        }

                        cache = cache->next;
                    }

                }
                else
                {
                    /* We are already past the cached allocation size,
                     * break
                     */
                    break;
                }
            }

            mem = mem->next;

            /*
             * Handle the merging condition if we are in the free list, merge
             * back to the main list once traversing of this free list is done.
             */
            if (temp && !mem)
            {
                mem = temp;
                temp = NULL;
            }
        }

        if (!found)
            result = HAP_VTCM_RELEASE_FAIL_REQUEST_NOT_FOUND;
    }

    /*
     * Search for an opportunity to coalesce free nodes.
     */
    mem = partition->pMemList;

    while (mem)
    {
        found = 0;

        if (mem->desc.bFree)
        {
            /*
             * Found a free node, see if it can be merged with the next one
             */
            temp = mem->next;

            if (temp && temp->desc.bFree)
            {
                if ( (mem->reserving_node == temp->reserving_node) &&
                     (mem->memHandle == temp->memHandle) &&
                     (mem->num_cached == temp->num_cached) )
                {
                    temp_cache = temp->cache_head;
                    cache = mem->cache_head;

                    while (cache && temp_cache)
                    {
                        if (cache->context != temp_cache->context)
                            break;

                        cache = cache->next;
                        temp_cache = temp_cache->next;
                    }

                    if (!cache)
                    {
                        /*
                         * Cache nodes, reserving node and mem handle are same
                         * basically we can merge these nodes.
                         */
                        temp_cache = temp->cache_head;

                        while (temp_cache)
                        {
                            cache = temp_cache->next;
                            vtcm_cachenode_push(temp_cache);
                            temp_cache = cache;
                        }

                        temp->cache_head = NULL;

                        /*
                         * Merge the nodes and release one.
                         */
                        mem->size = mem->size + temp->size;
                        mem->next = temp->next;

                        if (temp->next)
                            temp->next->prev = mem;

                        vtcm_memnode_push(temp);
                        found = 1;
                    }
                }
            }
        }

        /*
         * If we have merged the current node, there is still a chance that
         * it can be merged with the next one, hence if found, start with the
         * same node again.
         */
        if (!found)
            mem = mem->next;
    }

    return result;
}

int compute_resource_manager_release(int clientHandle,
                                     computeResMgr_node_t *context,
                                     unsigned char bCached)
{
    int result = SUCCESS, bReleased = 0;

    if (!context || (context->clientHandle != clientHandle))
        return COMPUTE_RES_FAIL_INVALID_ARGS;

    if (context->pVA && context->size)
    {
        /*
         * VTCM is acquired by the context.
         * Check if it's a cached release for cached requests.
         */
        result = vtcm_release(clientHandle,
                              context->pVA,
                              context,
                              NULL,    //Partition will be derived from context
                              bCached);

        if (result == SUCCESS)
            bReleased = 1;
    }

    if ( (SUCCESS == result) &&
          context->bSerialize && g_vtcmMgr.serialize.context == context)
    {
        /*
         * Context had serialization resource, release the same.
         */
        g_vtcmMgr.serialize.context = NULL;
        bReleased = 1;
    }

    if ( (SUCCESS == result) &&
         context->bHMX && g_vtcmMgr.hmx.context == context)
    {
        /*
         * Context had HMX resource, release the same.
         */
        g_vtcmMgr.hmx.context = NULL;
        bReleased = 1;
    }

    if (!context->bCached || !bCached)
    {
        /*
         * Either it's a non-cached request or it is a context release of a
         * cached request, both the cases, context has to be freed.
         */
        compute_context_delete(context,
                               context->clientHandle);
    }

    /*
     * If we have successfully released the resources pointed out by the
     * context, go ahead and check the waitQ
     */
    if (bReleased && (result == SUCCESS) )
    {
        vtcmMgr_recheck_waitQ();
    }

    if (result == SUCCESS)
    {
        /*
         * Reset the release requested flag for this context for subsequent
         * release requests to work.
         */
        context->bReleaseRequested = FALSE;
    }

    return result;
}

/* :: vtcmMgr_query_total ::
 *    Returns total available page size and number of such pages in VTCM
 */
static int vtcmMgr_query_total(unsigned int* size, unsigned int* numPages)
{
    unsigned int temp = 0, sizeAligned, size_local, numPages_local;
    int result = 0;
    vtcmMgr_partition_node_t *partition;

    if (!g_vtcmMgr.bInitDone)
    {
        /* VTCM manager not initialized yet, return 0 for size and page count */
        result = HAP_VTCM_QUERY_FAIL_INIT_NOTDONE;

        if (size)
            *size = 0;

        if (numPages)
            *numPages = 0;

        return result;
    }

    /*
     * Get partition for the default application ID i.e. 0.
     * Client handle doesn't matter for a non-privileged partition.
     * If there is no partition assigned for 0 application Id or if the
     * partition assigned is privileged, a NULL partition will be returned.
     */
    partition = get_partition_from_app_id(NULL, 0);

    if (partition)
    {
        sizeAligned = vtcm_alloc_align(partition->size, 1);

        if ( (!partition->next) &&
             (sizeAligned > partition->size) &&
             (!(partition->PA & (sizeAligned - 1) ) ) )
        {
            size_local = partition->size;
            numPages_local = 1;
        }
        else
        {
            temp = 31 - Q6_R_cl0_R(partition->size);
            temp = ( (temp >> 1) << 1);
            numPages_local = partition->size >> temp;
            size_local = (1 << temp);
        }

        if (numPages)
            *numPages = numPages_local;

        if (size)
            *size = size_local;
    }
    else
    {
        if (size)
            *size = 0;

        if (numPages)
            *numPages = 0;
    }

    return result;
}

/* :: vtcmMgr_query_pages ::
 *    Queries max page that can be created in the given node
 *    and number of such pages possible
 */
static void vtcmMgr_query_pages(vtcmMgr_partition_node_t* partition,
                                vtcmMgr_mem_node_t* pNode,
                                unsigned int* maxBlockSize,
                                unsigned int* numBlocks)
{
    unsigned int blockSize = 0, blocks = 0, size = 0, size_local = 0;
    unsigned int blockSizeBitPos = 0, alignedPA = 0, alignedSize = 0;
    unsigned int alignedHole = 0, maxBlockSize_local = 0, numBlocks_local = 0;

    if (pNode && pNode->desc.bFree && pNode->size && partition)
    {
        size = pNode->size;
        size_local = size;
        alignedSize = vtcm_alloc_align(size, 1);

        /*
         * Check if the node is the last one in the partition
         * and this is the last partition.
         * There is a possibility of over-mapping the region
         * than the defined VTCM to allow higher page mappings.
         * Set size to the aligned for checking maximum page.
         */
        if (!partition->next &&
            ( (pNode->PA + pNode->size) ==
                (partition->PA + partition->size) ) &&
            (alignedSize > size) )
        {
            size = alignedSize;
        }

        blockSizeBitPos = 31 - Q6_R_cl0_R(size);
        blockSizeBitPos = ( (blockSizeBitPos >> 1) << 1);
        blockSize = 1 << blockSizeBitPos;

        do {
            alignedPA = COMPUTE_RES_ALIGN(pNode->PA, blockSize);
            alignedHole = alignedPA - pNode->PA;

            if (size_local > alignedHole)
            {
                alignedSize = size_local - alignedHole;
                blocks = alignedSize >> blockSizeBitPos;
                if (!blocks)
                    blocks = 1;

                if (alignedSize > maxBlockSize_local)
                {
                    maxBlockSize_local = alignedSize / blocks;
                    numBlocks_local = blocks;
                }
            }

            blockSizeBitPos = blockSizeBitPos - 2;
            blockSize = 1 << blockSizeBitPos;
        } while ((maxBlockSize_local < blockSize) && (blockSizeBitPos >= 12) );
    }

    if (maxBlockSize)
        *maxBlockSize = maxBlockSize_local;

    if (numBlocks)
        *numBlocks = numBlocks_local;
}

/* :: vtcmMgr_query_avail ::
 *    Returns max contiguous VTCM block size avaiable,
 *    maximum possible single page allocation
 *    and number of such pages
 */
static int vtcmMgr_query_avail(unsigned int* freeBlockSize,
                               unsigned int* maxPageSize,
                               unsigned int* numPages)
{
    unsigned int maxBlock = 0, maxPage = 0, maxNumPages = 0,
                 tempMaxPage = 0, tempMaxNumPages = 0;
    int result = 0;
    vtcmMgr_partition_node_t *partition;
    vtcmMgr_mem_node_t* pNode;

    if (!g_vtcmMgr.bInitDone)
    {
        /* VTCM manager not initialized yet, return failure */
        result = HAP_VTCM_QUERY_FAIL_INIT_NOTDONE;
        if (freeBlockSize)
            *freeBlockSize = 0;

        if (maxPageSize)
            *maxPageSize = 0;

        if (numPages)
            *numPages = 0;
    }

    /*
     * Get partition for the default application ID i.e. 0.
     * Client handle doesn't matter for a non-privileged partition.
     * If there is no partition assigned for 0 application Id or if the
     * partition assigned is privileged, a NULL partition will be returned.
     */
    partition = get_partition_from_app_id(NULL, 0);

    if (partition)
    {
        pNode = partition->pMemList;

        while (pNode)
        {
            if (pNode->desc.bFree)
            {
                maxBlock = (maxBlock < pNode->size) ? pNode->size : maxBlock;
                vtcmMgr_query_pages(partition, pNode, &tempMaxPage, &tempMaxNumPages);

                if ( (tempMaxPage > maxPage) ||
                     ( (tempMaxPage == maxPage) && (tempMaxNumPages > maxNumPages) ) )
                {
                    maxPage = tempMaxPage;
                    maxNumPages = tempMaxNumPages;
                }
            }

            pNode = pNode->next;
        }
    }

    if (freeBlockSize)
        *freeBlockSize = maxBlock;

    if (maxPageSize)
        *maxPageSize = maxPage;

    if (numPages)
        *numPages = maxNumPages;

    return result;
}

/* :: vtcmMgr_recheck_waitQ ::
 *    If free list is not empty, traverse the wait queue
 *    and check for possible allocations
 */
static void vtcmMgr_recheck_waitQ()
{
    vtcmMgr_wait_node_t *waitNode;
    unsigned long long info = 0x0;
    int result = 0;

    /*
     * Check if there is any pending VTCM partitioning information
     */
    if (g_vtcmMgr.vtcmInfo.bPending)
        driver_prepare_vtcm();

    /* Check if there is a waiting thread */
    if (g_vtcmMgr.waitQ.pBusy)
    {
        waitNode = g_vtcmMgr.waitQ.pBusy;
        /*
         * Get to the last node
         */
        while (waitNode->next != NULL)
            waitNode = waitNode->next;

        while (waitNode)
        {

            if (!waitNode->bPending ||
                !waitNode->mailboxId ||
                !waitNode->clientHandle)
            {
                waitNode = waitNode->prev;
                continue;
            }

            result = try_allocate_from_waitq(waitNode);

            if (result == SUCCESS)
            {
                if (waitNode->bComputeRes &&
                    waitNode->pContext)
                {
                    info = (unsigned long long) waitNode->pVA;
                    info = info | ((unsigned long long) waitNode->pContext << 32);
                    qurt_mailbox_send(waitNode->mailboxId,
                                      QURT_MAILBOX_SEND_OVERWRITE,
                                      info);
                    waitNode->bPending = FALSE;
                }
                else if (!waitNode->bComputeRes &&
                         waitNode->pVA &&
                         waitNode->size)
                {
                    info = (unsigned long long) waitNode->pVA;
                    qurt_mailbox_send(waitNode->mailboxId,
                                      QURT_MAILBOX_SEND_OVERWRITE,
                                      info);
                    waitNode->bPending = FALSE;
                }
            }

            waitNode = waitNode->prev;
        }
    }
}

static int vtcmMgr_delete_mailbox(unsigned long long *mailboxId)
{
    int result = FAILURE;

    if (mailboxId)
    {
        if (*mailboxId)
        {
            if (QURT_EOK  == qurt_mailbox_delete(*mailboxId))
            {
                *mailboxId = 0;
                g_vtcmMgr.waitQ.mailboxCount--;
                result = SUCCESS;
            }
        }
    }

    return result;
}

vtcmMgr_wait_node_t* vtcmMgr_delete_wait_node(vtcmMgr_wait_node_t *waitNode)
{
    vtcmMgr_wait_node_t* next;
    /* Delete any mailbox created for this wait node */
    vtcmMgr_delete_mailbox (&waitNode->mailboxId);
    /* Unreserve resources */
    unreserve_wait_node(waitNode);

    /* Pop the node from busy list */
    if (waitNode->prev)
    {
        waitNode->prev->next = waitNode->next;
    }
    else
    {
        g_vtcmMgr.waitQ.pBusy = waitNode->next;
    }
    if (waitNode->next)
    {
        waitNode->next->prev = waitNode->prev;
    }
    next = waitNode->next;
    /* Push it to the free list */
    vtcmMgr_waitNode_push(waitNode);

    return next;
}

static int vtcmMgr_create_mailbox(unsigned long long *mailbox)
{
    int result = FAILURE;
    char mailboxName[9] = "vtcm";
    unsigned short mailboxCount = g_vtcmMgr.waitQ.mailboxCount + 1;

    /*
     * Some mailboxes can be open for a long time, if the mailboxnumber
     * overflows, there is a chance that we hit an already allocated mailbox
     * number, instead of having a look up table, retry if we hit such scenario
     * mailboxCount+1 number of times.
     */
    while (mailboxCount && result)
    {
        if (!(g_vtcmMgr.waitQ.mailboxNumber & 0xFF))
            g_vtcmMgr.waitQ.mailboxNumber++;

        mailboxName[4] = g_vtcmMgr.waitQ.mailboxNumber & 0xFF;
        mailboxName[5] = (g_vtcmMgr.waitQ.mailboxNumber >> 8) & 0xFF;
        mailboxName[6] = 0;
        /* Create a mailbox with receiver as userPD. This
         * works when receiver is GuestOS, userPD, unsigned
         * and CPZ processes as well
         */
        *mailbox = qurt_mailbox_create(mailboxName,
                                  QURT_MAILBOX_AT_USERPD);

        if (*mailbox == QURT_MAILBOX_ID_NULL)
        {
            mailboxCount--;
            g_vtcmMgr.waitQ.mailboxNumber++;
        }
        else
            result = SUCCESS;
    }

    if (result)
        return result;

    /* Increment mailbox count */
    g_vtcmMgr.waitQ.mailboxCount++;
    /* Increment mailbox number. This is an always incrementing value.
     * To create unique mailbox Id each time for a wait node. Currently
     * it's a short, but that would give us 2^16 values, should be
     * good enough.
     */
    g_vtcmMgr.waitQ.mailboxNumber++;

    return SUCCESS;
}

static int vtcmMgr_create_wait_node(vtcmMgr_wait_node_t **p_waitNode,
                                    int priority,
                                    compute_res_attr_t *request)
{
    int result = 0;
    vtcmMgr_wait_node_t *waitNode;
    vtcmMgr_wait_node_t *tempNode = NULL;
    unsigned long long mailboxId;

    waitNode = vtcmMgr_waitNode_pop();

    if (waitNode)
    {
        /*
         * Create a mailbox
         */
        result = vtcmMgr_create_mailbox(&mailboxId);

        if (result == FAILURE)
        {
            /* Failed to create a mailbox ID for the waiting
             * thread, return error. Push the wait node
             * back to the list
             */
            vtcmMgr_waitNode_push(waitNode);
            result = SYSMON_SRM_FAIL_MAILBOX_FULL;
        }
        else
        {
            *p_waitNode = waitNode;
            waitNode->mailboxId = mailboxId;

            if (request)
            {
                copy_request_into_waitnode(waitNode, request);
                waitNode->bComputeRes = TRUE;
            }
            else
            {
                waitNode->applicationId = 0;
                waitNode->bComputeRes = FALSE;
                waitNode->bSerialize = FALSE;
                waitNode->bHMX = FALSE;
                waitNode->size = 0;
                waitNode->bSinglePage = FALSE;
                waitNode->min_size = 0;
                waitNode->min_page_size = 0;
                waitNode->bCacheable = 0;
                waitNode->bReleasable = 0;
                waitNode->bSinglePage = 0;
                waitNode->applicationId = 0;
            }

            waitNode->pVA = 0;
            waitNode->pContext = NULL;
            waitNode->bPending = TRUE;
            waitNode->priority = priority;
            waitNode->reserved_pVA = 0;
            waitNode->reserved_size = 0;
            waitNode->clientHandle = 0;

            /* Push it to its place in the queue dependent on priority */
            if (NULL == g_vtcmMgr.waitQ.pBusy ||
                waitNode->priority >= g_vtcmMgr.waitQ.pBusy->priority)
            {
                /* Push it to the end of the queue */
                waitNode->next = g_vtcmMgr.waitQ.pBusy;
                waitNode->prev = NULL;

                if (g_vtcmMgr.waitQ.pBusy)
                    g_vtcmMgr.waitQ.pBusy->prev = waitNode;

                g_vtcmMgr.waitQ.pBusy = waitNode;
            }
            else
            {
                /* Push the node into the queue directly before the node
                 * with the next lowest priority, or at the start of the queue
                 * if no such node exists
                 */
                tempNode = g_vtcmMgr.waitQ.pBusy;

                while(tempNode->next &&
                        (tempNode->priority > waitNode->priority) )
                    tempNode = tempNode->next;

                if(tempNode->priority <= waitNode->priority)
                {
                    waitNode->next = tempNode;
                    waitNode->prev = tempNode->prev;
                    tempNode->prev->next = waitNode;
                    tempNode->prev = waitNode;
                }
                else
                {
                    waitNode->next = NULL;
                    waitNode->prev = tempNode;
                    tempNode->next = waitNode;
                }
            }
        }
    }
    else
    {
        result = SYSMON_SRM_WAITQ_FULL;
    }

    return result;
}

static computeResMgr_node_t* compute_context_alloc(int clientHandle)
{
    computeResMgr_node_t *result = NULL;

    if (g_vtcmMgr.computeResQ.pFree)
    {
        result = g_vtcmMgr.computeResQ.pFree;
        g_vtcmMgr.computeResQ.pFree = result->next;
        result->clientHandle = clientHandle;
        result->next = g_vtcmMgr.computeResQ.pBusy;
        result->prev = NULL;

        if (g_vtcmMgr.computeResQ.pBusy)
            g_vtcmMgr.computeResQ.pBusy->prev = result;

        g_vtcmMgr.computeResQ.pBusy = result;
        g_vtcmMgr.computeResQ.busyDepth++;

        /*
         * Initialize some vars
         */
        result->memHandle = -1;
        result->bAllocated = FALSE;
        result->bReleaseRequested = FALSE;
        result->pVA = 0;
        result->size = 0;
        result->pageSize = 0;
    }

    return result;
}

static computeResMgr_node_t* computeRes_context_search(unsigned int contextId,
                                     int clientHandle)
{
    computeResMgr_node_t* pNode = g_vtcmMgr.computeResQ.pBusy;

    while (pNode)
    {
        if ( (pNode->contextId == contextId) &&
             (pNode->clientHandle == clientHandle) )
        {
            return pNode;
        }

        pNode = pNode->next;
    }

    return NULL;
}

static int compute_context_delete(computeResMgr_node_t *context,
                                     int clientHandle)
{
    if (!context)
        return COMPUTE_RES_FAIL_NO_CONTEXT_ID;

    if (context->clientHandle == clientHandle )
    {
        if (context->next)
            context->next->prev = context->prev;

        if (context->prev)
            context->prev->next = context->next;
        else
            g_vtcmMgr.computeResQ.pBusy = context->next;

        g_vtcmMgr.computeResQ.busyDepth--;
        context->next = g_vtcmMgr.computeResQ.pFree;
        g_vtcmMgr.computeResQ.pFree = context;
    }

    return SUCCESS;
}

/* :: vtcmMgr_waitNode_push ::
 *    Pushes given wait node to waitQ free LIFO
 */
static void vtcmMgr_waitNode_push(vtcmMgr_wait_node_t* pMem)
{
    if (pMem)
    {
        /* Push given object to the front of the list */
        pMem->next = g_vtcmMgr.waitQ.pFree;
        g_vtcmMgr.waitQ.pFree = pMem;
    }
}

/* :: vtcmMgr_waitNode_pop ::
 *    Pops out a wait node from the waitQ free LIFO
 */
static vtcmMgr_wait_node_t* vtcmMgr_waitNode_pop()
{
    vtcmMgr_wait_node_t* retVal;
    /* Pop out a mem node from the mem node list */
    retVal = g_vtcmMgr.waitQ.pFree;
    if (g_vtcmMgr.waitQ.pFree)
    {
        g_vtcmMgr.waitQ.pFree = g_vtcmMgr.waitQ.pFree->next;
    }
    /* return the node */
    return retVal;
}

static void vtcm_cachenode_push(vtcmMgr_cache_node_t *node)
{
    if (node)
    {
        node->next = pVtcmCacheNodeList;
        pVtcmCacheNodeList = node;
    }
}

static vtcmMgr_cache_node_t* vtcm_cachenode_pop()
{
    vtcmMgr_cache_node_t* retVal;
    retVal = pVtcmCacheNodeList;

    if (pVtcmCacheNodeList)
    {
        pVtcmCacheNodeList = pVtcmCacheNodeList->next;
    }

    return retVal;
}

/*
 *       ***** QDI driver interface *****
 */

/* :: driver_QDI_init ::
 *    Initializes QDI objects
 */
static void driver_QDI_init()
{
    /* Push all QDI objects into the list */
    for (int i = 0; i < VTCMMGR_QDI_MAX_OBJS; i++)
    {
        driver_QDI_push(&vtcmQDIObjArray[i]);
    }
}

/* :: driver_QDI_pop ::
 *    Pops a QDI object from the QDI object LIFO
 */
static vtcmMgr_qdi_obj_t* driver_QDI_pop()
{
    vtcmMgr_qdi_obj_t* retVal;
    retVal = pVtcmQDIObjList;

    if (pVtcmQDIObjList)
    {
        pVtcmQDIObjList = pVtcmQDIObjList->next;
    }

    if (retVal)
    {
        retVal->b_partitionMaster = FALSE;
        retVal->b_privilegeMaster = FALSE;
        retVal->mailboxId = 0;
        retVal->pid = -1;
        retVal->b_valid = TRUE;
    }

    /* return a QDI object to the caller */
    return retVal;
}

/* :: driver_QDI_push ::
 *    Pushes the given QDI object into the QDI object LIFO
 */
static void driver_QDI_push(vtcmMgr_qdi_obj_t* pQDIObj)
{
    if (pQDIObj)
    {
        pQDIObj->b_valid = FALSE;
        /* Push given object to the front of the list */
        pQDIObj->next = pVtcmQDIObjList;
        pVtcmQDIObjList = pQDIObj;
    }
}

/******************************************************************************
 * Function: QDI_VtcmMgr_Release
 * Description:
 *           QDI release function called when all reference count associated
 *           with the calling client becomes 0 i.e, no active QDI object from
 *           the given client.
 *           Deallocates memory requests from the given client handle and
 *           pushes the QDI object back to the QDI object LIFO.
 *****************************************************************************/
void QDI_vtcmMgr_release (qurt_qdi_obj_t *qdiObj)
{
    vtcmMgr_qdi_obj_t *vtcmQdiObj = (vtcmMgr_qdi_obj_t *)qdiObj;
    vtcmMgr_wait_node_t *waitNode = g_vtcmMgr.waitQ.pBusy;
    vtcmMgr_wait_node_t *tempWaitNode;
    vtcmMgr_mem_node_t *memNode;
    vtcmMgr_partition_node_t *partition = g_vtcmMgr.vtcmPartitions;
    int clientHandle = vtcmQdiObj->clientHandle;
    computeResMgr_node_t *context = g_vtcmMgr.computeResQ.pBusy;
    computeResMgr_node_t *temp;
    qurt_addr_t pVA;

    /* Free all compute resource contexts allocated to this process.
     * Bypass recheck here as we will anyway do a recheck post all this within
     * this function itself. Set free all contexts with this client handle flag
     * Context Id can be 0 as we ignore if bAllContexts is set.
     */
    while (context)
    {

        if (context->clientHandle == clientHandle)
        {
            temp = context;
            context = context->next;
            compute_resource_manager_release(clientHandle,
                                             temp,
                                             0);
        }
        else
        {
            context = context->next;
        }
    }

    /*
     * We have deleted all the contexts, see if legacy VTCM allocations are
     * present by going through each partition for the client handle.
     */
    while (partition)
    {
        memNode = partition->pMemList;

        /* Free all the VTCM allocations done by the process */
        while (memNode)
        {

            if (!memNode->desc.bFree &&
                    (memNode->clientHandle == clientHandle) )
            {
                /* Found a mem node allocated by the exiting process.
                 * Call free on this node.
                 */
                pVA = memNode->VA;
                vtcm_release(clientHandle,
                             pVA,
                             NULL,
                             partition,
                             0);
                memNode = partition->pMemList;
            }
            else
            {
                memNode = memNode->next;
            }
        }

        partition = partition->next;
    }

    waitNode = g_vtcmMgr.waitQ.pBusy;
    /*
     * Delete all the requests from this client handle pending
     * in the wait queue
     * */
    while (waitNode)
    {

        if (waitNode->clientHandle == clientHandle)
        {
            /* Delete the node */
            tempWaitNode = vtcmMgr_delete_wait_node(waitNode);
            /* Update waitNode to point to the next one */
            waitNode = tempWaitNode;
        }
        else
        {
            waitNode = waitNode->next;
        }

    }

    /* Check if we can allocate for any of the other waiting nodes */
    vtcmMgr_recheck_waitQ();

    /* Check if we have a mailbox attached to the QDI object and delete */
    if (vtcmQdiObj->mailboxId)
        vtcmMgr_delete_mailbox(&vtcmQdiObj->mailboxId);

    /* Push the QDI object back to the free list */
    driver_QDI_push(vtcmQdiObj);
}

/******************************************************************************
 * Function: QDI_vtcmMgr_invocation
 * Description:
 *           Implements VTCM manager Alloc, release, query APIs along with
 *           QDI specific Open and close functionalities.
 *****************************************************************************/
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
        qurt_qdi_arg_t a9)
{
    int result = 0;
    vtcmMgr_qdi_obj_t* qdiObj = (vtcmMgr_qdi_obj_t *)obj;

    switch (method)
    {

        case QDI_OPEN:
        {
            if (TRUE != g_vtcmMgr.bInitDone)
            {
                driver_init();
            }

            vtcmMgr_qdi_obj_t* localQdiObj = driver_QDI_pop();

            if (localQdiObj)
            {
                localQdiObj->qdiObj.invoke = QDI_vtcmMgr_invocation;
                localQdiObj->qdiObj.release = QDI_vtcmMgr_release;
                localQdiObj->qdiObj.refcnt = QDI_REFCNT_INIT;
                localQdiObj->clientHandle = clientHandle;
                local_memcpy(a1.ptr, localQdiObj, sizeof(qurt_qdi_obj_t *));
            }

            break;
        }

        case COMPUTE_RES_MGR_REQUEST_MAILBOX:
        {
            /*
             * a1.ptr = Return mailbox Id created for listening to release
             *          requests
             */
            if (a1.ptr)
            {
                /*
                 * Create a mailbox if not already created for this client
                 */
                if (!qdiObj->mailboxId)
                    vtcmMgr_create_mailbox(&qdiObj->mailboxId);

                if (qdiObj->mailboxId)
                    result = local_memcpy(a1.ptr,
                                                   (void *)&qdiObj->mailboxId,
                                                   sizeof(qdiObj->mailboxId));
                else
                    result = SYSMON_SRM_FAIL_MAILBOX_FULL;
            }

            break;
        }

        case COMPUTE_RES_MGR_REQUEST_ASYNC_CANCEL:
        case VTCMMGR_ALLOC_ASYNC_CANCEL:
        {
            /*
             * a1.ptr = Mailbox Id that the user is cancelling wait on
             * a2.ptr = Return VTCM VA if allocated
             * a3.ptr = Return context Id if assigned
             * a4.ptr = Return VTCM size if allocated
             */
            int found = 0;
            unsigned long long mailboxId = 0;
            vtcmMgr_wait_node_t *waitNode = g_vtcmMgr.waitQ.pBusy;

            if (a1.ptr)
            {
                local_memcpy(&mailboxId,
                                        (void *)a1.ptr,
                                        sizeof(mailboxId) );

                if (!mailboxId)
                {
                    result = HAP_VTCM_ALLOC_ASYNC_INVALID_ARGS;
                    break;
                }

                while (waitNode)
                {
                    /* Search the node in wait queue */
                    if ( (waitNode->clientHandle == clientHandle) &&
                         (waitNode->mailboxId == mailboxId ) &&
                         ( (waitNode->bPending == TRUE) ||
                            a2.ptr || a3.ptr) )
                    {
                        found = 1;

                        if (waitNode->bPending == FALSE)
                        {
                            /* Driver already allocated the requested chunk
                             * to the caller. Return user the allocated pointer
                             */
                            if (waitNode->bComputeRes)
                            {
                                if (a3.ptr)
                                {
                                    local_memcpy((void *)a3.ptr,
                                                  (void *)&waitNode->pContext,
                                                  sizeof(waitNode->pContext) );

                                    if (a2.ptr)
                                    {
                                        local_memcpy((void *)a2.ptr,
                                                      (void *)&waitNode->pVA,
                                                      sizeof(waitNode->pVA) );
                                    }

                                    if (a4.ptr)
                                    {
                                        local_memcpy((void *)a4.ptr,
                                                      (void *)&waitNode->size,
                                                      sizeof(waitNode->size) );
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }
                            else if (a2.ptr)
                            {
                                local_memcpy((void *)a2.ptr,
                                                      (void *)&waitNode->pVA,
                                                      sizeof(waitNode->pVA) );
                            }
                            else
                            {
                                /* We found the node, but can't delete it
                                 * yet
                                 */
                                break;
                            }
                        }

                        /* Found a wait node in pending state matching the
                         * client information. Delete the node.
                         */
                        vtcmMgr_delete_wait_node(waitNode);

                        break;
                    }

                    waitNode = waitNode->next;
                }

                if (!found)
                {
                    /*
                     * Couldn't find a wait node with given mailboxId, return
                     * failure.
                     */
                    result = HAP_VTCM_ALLOC_ASYNC_INVALID_ARGS;
                }
            }
            else
            {
                /*
                 * a1.ptr is NULL, return error
                 */
                result = HAP_VTCM_ALLOC_ASYNC_INVALID_ARGS;
            }

            break;
        }

        case COMPUTE_RES_MGR_REQUEST_ASYNC_DONE:
        case VTCMMGR_ALLOC_ASYNC_DONE:
        {
            /*
             * a1.num = mailbox Id LSB
             * a2.num = mailbox Id MSB
             * a3.ptr = Return VTCM size if allocated
             */
            unsigned long long mailboxId;
            vtcmMgr_wait_node_t *waitNode = g_vtcmMgr.waitQ.pBusy;

            mailboxId = (a1.num | ((unsigned long long)a2.num << 32) );

            if (!mailboxId)
            {
                result = HAP_VTCM_ALLOC_ASYNC_INVALID_ARGS;

                break;
            }

            while (waitNode)
            {
                if ( (waitNode->clientHandle == clientHandle) &&
                     (waitNode->mailboxId == mailboxId ) )
                {
                    if (a3.ptr)
                    {
                        /*
                         * Check if we have to return size of allocation
                         */
                        local_memcpy((void *)a3.ptr,
                                             (void *)&waitNode->size,
                                             sizeof(waitNode->size) );
                    }

                    vtcmMgr_delete_wait_node(waitNode);

                    break;
                }

                waitNode = waitNode->next;
            }

            break;
        }

        case COMPUTE_RES_MGR_REQUEST_CACHED_ASYNC:
        {
            /*
             * a1.num = context Id to be acquired
             * a2.num = priority of the caller
             * a3.ptr = Return mailbox id if the request is put in wait Q
             */
            vtcmMgr_wait_node_t *waitNode = NULL;
            unsigned int contextId = (unsigned int)a1.num;
            int priority = (int)a2.num;
            computeResMgr_node_t *context = NULL;

            context = computeRes_context_search(contextId, clientHandle);

            priority = ( (priority < 0 || priority > 255) ? 255 : priority);

            if (context)
                result = compute_resource_manager_request(clientHandle,
                                                      context,
                                                      NULL,
                                                      priority,
                                                      (a3.ptr ? &waitNode : NULL),
                                                      &context);
            else
                result = COMPUTE_RES_FAIL_NO_CONTEXT_ID;

            if (0 == result)
            {
                if (waitNode && a3.ptr)
                {
                    if (0 != local_memcpy((void *)a3.ptr,
                                                  (void *)&waitNode->mailboxId,
                                                  sizeof(waitNode->mailboxId) ) )
                    {
                        vtcmMgr_delete_wait_node(waitNode);
                        result = COMPUTE_RES_FAIL_INVALID_ARGS;
                    }
                    else
                    {
                        result = COMPUTE_RES_FAIL_RETRY_WITH_MAILBOX;
                    }
                }
            }

            break;
        }

        case COMPUTE_RES_MGR_REQUEST_CACHED_FREE:
        {
            /*
             * a1.num = context Id to be freed
             */
            unsigned int contextId;
            computeResMgr_node_t *context = NULL;

            contextId = (unsigned int)a1.num;

            if (NULL == contextId)
            {
                /*
                 * Couldn't find the provided contextId
                 */
                result = COMPUTE_RES_FAIL_NO_CONTEXT_ID;
                break;
            }
            /*
             * Search for the context with the given context Id and the
             * client's handle
             */
            context = computeRes_context_search(contextId, clientHandle);

            /*
             * If we are able to get a context, call a release which would
             * delete the context if all the resources pointed out by the
             * context are released and also invoke a recheck on wait queue.
             * This is a cached release, set bCached = 1.
             */
            if (context)
                result = compute_resource_manager_release(clientHandle,
                                                          context,
                                                          1);
            else
                result = COMPUTE_RES_FAIL_NO_CONTEXT_ID;

            break;
        }

        case COMPUTE_RES_MGR_QUERY_RELEASE_CB_STATE:
        {
            /*
             * a1.num = context ID
             *
             * Query API to check if the release callback initiated is still
             * valid.
             */
            unsigned int contextId;
            computeResMgr_node_t *context = NULL;
            vtcmMgr_wait_node_t *waitNode = NULL;
            vtcmMgr_partition_node_t *partition = NULL;
            vtcmMgr_mem_node_t *mem = NULL;

            contextId = (unsigned int)a1.num;

            if (NULL == contextId)
            {
                /*
                 * Couldn't find the provided contextId
                 */
                result = COMPUTE_RES_MGR_QUERY_RELEASE_CB_INVALID;
                break;
            }

            /*
             * Search for the context with the given context Id and the
             * client's handle
             */
            context = computeRes_context_search(contextId, clientHandle);

            if (!context || !context->bReleaseRequested)
            {
                result = COMPUTE_RES_MGR_QUERY_RELEASE_CB_INVALID;
                break;
            }

            /*
             * See if the context has any VTCM allocation, if yes, see if there
             * is a reserving wait node for such allocation. If there is a
             * wait node which invoked the release callback for this context
             * then it would hold reserve.
             *
             * If NOT NEEDED is the result, check if HMX is assigned to the
             * context and if there is a reserving / waiting entity.
             *
             * If NOT NEEDED Is the result, check for serialization resource.
             *
             * Ultimately, return release NEEDED or NOT_NEEDED hint to
             * the caller.
             *
             * Start with a result of NOT NEEDED.
             */

            result = COMPUTE_RES_MGR_QUERY_RELEASE_CB_NOT_NEEDED;

            if (context->bAllocated && context->pVA)
            {
                partition = get_partition_from_app_id(context->clientHandle,
                                                      context->applicationId);

                if (!partition)
                {
                    result = COMPUTE_RES_MGR_QUERY_RELEASE_CB_INVALID;
                    break;
                }

                mem = partition->pMemList;

                while (mem)
                {
                    if (!mem->desc.bFree && mem->context == context)
                    {
                        if (mem->reserving_node && mem->reserving_node->bPending)
                        {
                            waitNode = mem->reserving_node;
                            break;
                        }
                    }

                    else if (mem->PA >=
                            (COMPUTE_RES_GET_PA(context->pVA) + context->size) )
                    {
                        break;
                    }

                    mem = mem->next;
                }

                if (waitNode)
                    result = COMPUTE_RES_MGR_QUERY_RELEASE_CB_NEEDED;
                else
                    result = COMPUTE_RES_MGR_QUERY_RELEASE_CB_NOT_NEEDED;
            }

            if ( (result == COMPUTE_RES_MGR_QUERY_RELEASE_CB_NOT_NEEDED) &&
                    (context->bHMX) )
            {
                if ( (g_vtcmMgr.hmx.context == context) &&
                        (g_vtcmMgr.hmx.reserve &&
                                g_vtcmMgr.hmx.reserve->bPending) )
                {
                    result = COMPUTE_RES_MGR_QUERY_RELEASE_CB_NEEDED;
                }
                else
                {
                    result = COMPUTE_RES_MGR_QUERY_RELEASE_CB_NOT_NEEDED;
                }
            }

            if ( (result == COMPUTE_RES_MGR_QUERY_RELEASE_CB_NOT_NEEDED) &&
                     (context->bSerialize) )
            {
                if ( (g_vtcmMgr.serialize.context == context) &&
                        (g_vtcmMgr.serialize.reserve &&
                                g_vtcmMgr.serialize.reserve->bPending) )
                {
                    result = COMPUTE_RES_MGR_QUERY_RELEASE_CB_NEEDED;
                }
                else
                {
                    result = COMPUTE_RES_MGR_QUERY_RELEASE_CB_NOT_NEEDED;
                }
            }

            /*
             * If we are saying that a release is not needed, update the
             * release requested flag to FALSE, so that in future if the
             * context needs to be released, a release callback notification
             * could be sent.
             */
            if (result == COMPUTE_RES_MGR_QUERY_RELEASE_CB_NOT_NEEDED)
                context->bReleaseRequested = FALSE;

            break;
        }

        case COMPUTE_RES_MGR_REQUEST_ASYNC:
        {
            /*
             * a1.num = Version of the interface
             * a2.ptr = compute resource request structure (attributes)
             * a3.ptr = Return virtual address of VTCM (if requested)
             * a4.ptr = Return Mailbox Id in case of request being put in wait Q
             * a5.ptr = Return context Id
             * a6.num = priority of the caller
             * a7.ptr = Return of the size of vtcm allocation (if requested)
             */
            vtcmMgr_wait_node_t *waitNode = NULL;
            unsigned int version = (unsigned int)a1.num;
            compute_res_attr_t request;
            computeResMgr_node_t *context = NULL;
            int ret;
            int priority = 0;

            if ( (COMPUTE_RES_VER_START > version) ||
                 (COMPUTE_RES_VER_END < version) ||
                 (!a5.ptr) || (!a2.ptr) )
            {
                result = COMPUTE_RES_FAIL_INVALID_ARGS;
                break;
            }

            if (a2.ptr)
            {
                /* Copy user provided request structure */
                ret = local_memcpy(&request,
                                              (void *)a2.ptr,
                                              sizeof(compute_res_attr_t) );
                if (ret)
                {
                    /* QuRT copy failed, return invalid args */
                    result = COMPUTE_RES_FAIL_INVALID_ARGS;
                    break;
                }
            }

            if ( (request.attr.vtcmSize > g_vtcmMgr.size) ||
                 (request.attr.vtcmSize && !a3.ptr) )
            {
                result = COMPUTE_RES_FAIL_INVALID_ARGS;
                break;
            }

            priority = ((a6.num < 0 || a6.num > 255) ? 255 : a6.num);

            result = compute_resource_manager_request(clientHandle,
                                                      NULL,
                                                      &request,
                                                      priority,
                                                      (a4.ptr ? &waitNode : NULL),
                                                      &context);

            if (0 == result)
            {

                if (waitNode && a4.ptr)
                {
                    result = local_memcpy((void *)a4.ptr,
                                                  (void *)&waitNode->mailboxId,
                                                   sizeof(waitNode->mailboxId) );

                    if (result)
                    {
                        vtcmMgr_delete_wait_node(waitNode);
                        result = COMPUTE_RES_FAIL_INVALID_ARGS;
                    }
                    else
                        result = COMPUTE_RES_FAIL_RETRY_WITH_MAILBOX;
                }
                else if (context)
                {

                    if (context->pVA && a3.ptr)
                    {
                        result = local_memcpy((void *)a3.ptr,
                                                        (void *)&context->pVA,
                                                        sizeof(context->pVA) );

                        if (result)
                        {
                            /*
                             * Failed to send allocated VTCM pointer to the caller
                             * Free the memory chunk forcefully and return failure
                             */
                            compute_resource_manager_release(clientHandle,
                                                             context,
                                                             0);
                            result = COMPUTE_RES_FAIL_INVALID_ARGS;
                        }

                        if (!result && a7.ptr)
                        {
                            result = local_memcpy((void *)a7.ptr,
                                                        (void *)&context->size,
                                                        sizeof(context->size));

                            if (result)
                            {
                                /*
                                 * Failed to send allocated VTCM pointer to
                                 * the caller
                                 * Free the memory chunk forcefully and return
                                 * failure
                                 */
                                compute_resource_manager_release(clientHandle,
                                                                 context,
                                                                 0);
                                result = COMPUTE_RES_FAIL_INVALID_ARGS;
                            }
                        }
                    }
                    else if (request.attr.vtcmSize)
                    {
                        /*
                         * There was a VTCM request but no allocation.
                         * Return failure. Ideally we should have put in a
                         * wait node and returned mailbox for this.
                         */
                        compute_resource_manager_release(clientHandle,
                                                         context,
                                                         0);
                        result = COMPUTE_RES_FAIL_NO_MEM;
                    }

                    if (!result)
                    {
                        result = local_memcpy((void *)a5.ptr,
                                                   (void *)&context->contextId,
                                                   sizeof(context->contextId) );

                        if (result)
                        {
                            /*
                             * Failed to copy context Id back to the user.
                             * DO clean up
                             */
                            compute_resource_manager_release(clientHandle,
                                                             context,
                                                             0);
                            result = COMPUTE_RES_FAIL_INVALID_ARGS;
                        }
                    }
                }
                else
                {
                    result = COMPUTE_RES_FAIL_NO_CONTEXT_ID;
                }
            }

            break;
        }

        case COMPUTE_RES_MGR_REQUEST_FREE:
        {
            /*
             * a1.num = context ID
             */
            unsigned int contextId;
            computeResMgr_node_t *context = NULL;

            contextId = (unsigned int)a1.num;

            if (NULL == contextId)
            {
                /*
                 * Couldn't find the provided contextId
                 */
                result = COMPUTE_RES_FAIL_NO_CONTEXT_ID;
                break;
            }
            /*
             * Search for the context with the given context Id and the
             * client's handle
             */
            context = computeRes_context_search(contextId, clientHandle);

            /*
             * If we are able to get a context, call a release which would
             * delete the context if all the resources pointed out by the
             * context are released and also invoke a recheck on wait queue.
             */
            if (context)
                result = compute_resource_manager_release(clientHandle,
                                                          context,
                                                          0);
            else
                result = COMPUTE_RES_FAIL_NO_CONTEXT_ID;

            break;
        }

        case COMPUTE_RES_MGR_QUERY_VTCM:
        {
            /*
             * a1.num application Id
             * a2.ptr Total VTCM size
             * a3.ptr VTCM start address
             * a4.ptr Available VTCM chunk size
             * a5.ptr Start address of this available VTCM chunk
             */
            vtcmMgr_partition_node_t *partition;
            vtcmMgr_mem_node_t *pNode;
            unsigned int total_size = 0, total_VA = 0, avail_size = 0;
            unsigned int avail_VA = 0, max_avail_size = 0, max_avail_VA = 0;

            if (!g_vtcmMgr.bInitDone)
            {
                result = HAP_VTCM_QUERY_FAIL_INIT_NOTDONE;
                break;
            }

            if (a1.num >= VTCMMGR_NUM_APP_IDS)
            {
                result = COMPUTE_RES_FAIL_INVALID_ARGS;
                break;
            }

            partition = get_partition_from_app_id(clientHandle,
                                                  (unsigned char)a1.num);

            if (partition)
            {
                total_size = partition->size;
                total_VA = COMPUTE_RES_GET_VA(partition->PA);

                pNode = partition->pMemList;

                while (pNode)
                {
                    if (pNode->desc.bFree)
                    {
                        if (!avail_size)
                        {
                            avail_size = pNode->size;
                            avail_VA = COMPUTE_RES_GET_VA(pNode->PA);
                        }
                        else
                            avail_size += pNode->size;

                        if (pNode->next && pNode->next->desc.bFree)
                        {
                            pNode = pNode->next;
                            continue;
                        }
                        else
                        {
                            if (max_avail_size < avail_size)
                            {
                                max_avail_size = avail_size;
                                max_avail_VA = avail_VA;
                            }

                            avail_size = 0;
                            avail_VA = 0;
                        }
                    }

                    pNode = pNode->next;
                }

                if (a2.ptr)
                    local_memcpy(a2.ptr,
                                          &total_size,
                                          sizeof(total_size));

                if (a3.ptr)
                    local_memcpy(a3.ptr,
                                          &total_VA,
                                          sizeof(total_VA));

                if (a4.ptr)
                    local_memcpy(a4.ptr,
                                          &max_avail_size,
                                          sizeof(max_avail_size));

                if (a5.ptr)
                    local_memcpy(a5.ptr,
                                          &max_avail_VA,
                                          sizeof(max_avail_VA));
            }
            else
            {
                result = COMPUTE_RES_FAIL_NO_MEM;
            }

            break;
        }

        case VTCMMGR_QUERY_TOTAL:
        {
            /*
             * a1.ptr = Return page size
             * a2.ptr = Return page count
             */
            unsigned int pageSize;
            unsigned int pageCount;

            result = vtcmMgr_query_total(&pageSize, &pageCount);

            if (a1.ptr)
            {
                local_memcpy(a1.ptr,
                                      &pageSize,
                                      sizeof(pageSize));
            }

            if (a2.ptr)
            {
                local_memcpy(a2.ptr,
                                      &pageCount,
                                      sizeof(pageCount));
            }

            break;
        }

        case VTCMMGR_QUERY_AVAIL:
        {
            /*
             * a1.ptr = Retrun max free block size
             * a2.ptr = Return max page size
             * a3.ptr = Return number of max pages
             */
            unsigned int maxFreeBlock;
            unsigned int maxPageSize;
            unsigned int pageCount;

            result = vtcmMgr_query_avail(&maxFreeBlock,
                                         &maxPageSize,
                                         &pageCount);

            if (a1.ptr)
            {
                local_memcpy((void *)a1.ptr,
                                      (void *)&maxFreeBlock,
                                      sizeof(maxFreeBlock) );
            }

            if (a2.ptr)
            {
                local_memcpy((void *)a2.ptr,
                                      (void *)&maxPageSize,
                                      sizeof(maxPageSize) );
            }

            if (a3.ptr)
            {
                local_memcpy((void *)a3.ptr,
                                      (void *)&pageCount,
                                      sizeof(pageCount) );
            }

            break;
        }

        case VTCMMGR_ALLOC:
        {
            /*
             * a1.num = size of the request
             * a2.num = single page flag
             * a3.ptr = Return VTCM VA on success
             * a4.num = priority of the request
             */
            unsigned int pVA, size, pageSize, sizeAligned;
            vtcmMgr_partition_node_t *partition = NULL;
            vtcm_select_t selection;
            int priority = 255;

            /*
             * Get partition for the default application id i.e. 0
             */
            partition = get_partition_from_app_id(clientHandle, 0);
            size = (unsigned int)a1.num;
            priority = ( (a4.num < 0 || a4.num > 255) ? 255 : a4.num);

            if (!partition)
            {
                /*
                 * We don't have a partition carved for the default
                 * applications, return no memory
                 */
                result = HAP_VTCM_ALLOC_FAIL_NO_MEM;
            }
            else if ( (unsigned int)a1.num > partition->size)
            {
                /*
                 * Requested size can't be accommodated, return no memory
                 */
                result = HAP_VTCM_ALLOC_FAIL_NO_MEM;
            }
            else if ( (0 == (unsigned int)a1.num) || (!a3.ptr) )
            {
                /*
                 * Invalid size argument, return error.
                 */
                result = HAP_VTCM_ALLOC_FAIL_INVALID_ARGS;
            }
            else
            {
                sizeAligned = vtcm_alloc_align(size, (unsigned char)a2.num);

                if ( (unsigned char)a2.num)
                    pageSize = sizeAligned;
                else
                    pageSize = 0;


                result = vtcm_search_noncached(partition,
                                               sizeAligned,
                                               sizeAligned,
                                               pageSize,
                                               size,
                                               size,
                                               priority,
                                               NULL,
                                               &selection);

                if (result)
                {
                    /*
                     * Unsuccessful in finding vtcm for the request, return
                     * no mem
                     */
                    result = HAP_VTCM_ALLOC_FAIL_NO_MEM;
                }
                else if (selection.numBusyNodes)
                {
                    /*
                     * Successful, but found busy nodes in the search. As
                     * this is a synchronous call, can't wait. return no mem.
                     * Do the same even if there are free but cached nodes
                     * as well - TODO - currently removed cached nodes
                     * condition, revisit.
                     */
                    result = HAP_VTCM_ALLOC_FAIL_NO_MEM;
                }
                else if (selection.allocStatus == VTCM_RESOURCE_SELECTED)
                {
                    result = vtcm_allocate_immediate(clientHandle,
                                                     partition,
                                                     NULL,
                                                     &selection,
                                                     NULL);
                }

                if (!result &&
                    (selection.allocStatus == VTCM_RESOURCE_ASSIGNED) )
                {
                    pVA = COMPUTE_RES_GET_VA(selection.startPA);

                    if (0 != local_memcpy((void *)a3.ptr,
                                                   (void *)&pVA,
                                                   sizeof(pVA) ) )
                    {
                        /*
                         * Failed to pass back the VA, free memory
                         */
                        vtcm_release(clientHandle,
                                     pVA,
                                     NULL, //No context to be passed
                                     partition,
                                     0); //Non cached request
                        result = HAP_VTCM_ALLOC_FAIL_INVALID_ARGS;
                    }
                }
                else
                {
                    /*
                     * Failed to allocate!
                     */
                    result = HAP_VTCM_ALLOC_FAIL_NO_MEM;
                }
            }

            break;
        }

        case VTCMMGR_ALLOC_ASYNC:
        {
            /*
             * a1.num = Request VTCM size
             * a2.num = Single page flag
             * a3.ptr = Return virtual address if successful
             * a4.ptr = Mailbox id if the request is put in waitQ
             * a5.num = priority of the request
             */
            unsigned int pVA, size, pageSize, sizeAligned;
            vtcmMgr_partition_node_t *partition = NULL;
            vtcm_select_t selection;
            int priority = 255, search_result;
            vtcmMgr_wait_node_t *waitNode = NULL;

            /*
             * Get partition for the default application id i.e. 0
             */
            partition = get_partition_from_app_id(clientHandle, 0);
            size = (unsigned int)a1.num;

            if (!partition)
            {
                /*
                 * We don't have a partition carved for the default
                 * applications, return no memory
                 */
                result = HAP_VTCM_ALLOC_FAIL_NO_MEM;
            }
            else if (size > partition->size)
            {
                /*
                 * Requested size can't be accommodated, return no memory
                 */
                result = HAP_VTCM_ALLOC_FAIL_NO_MEM;
            }
            else if ( (0 == size) || (!a3.ptr) )
            {
                /*
                 * Invalid size argument, return error.
                 */
                result = HAP_VTCM_ALLOC_FAIL_INVALID_ARGS;
            }
            else
            {
                sizeAligned = vtcm_alloc_align(size, (unsigned char)a2.num);

                if ( (unsigned char)a2.num)
                    pageSize = sizeAligned;
                else
                    pageSize = 0;

                priority = ( (a5.num < 0 || a5.num > 255) ? 255 : a5.num);

                search_result = vtcm_search_noncached(partition,
                                                      sizeAligned,
                                                      sizeAligned,
                                                      pageSize,
                                                      size,
                                                      size,
                                                      priority,
                                                      NULL,
                                                      &selection);

                if ((selection.allocStatus == VTCM_RESOURCE_UNAVAILABLE) ||
                    ( (selection.allocStatus == VTCM_RESOURCE_SELECTED) &&
                      selection.numBusyNodes) )
                {
                    /*
                     * Either we couldn't succeed in allocating the request
                     * or the request has some releasable nodes, both the cases
                     * we need to wait, create a wait node.
                     */
                    if (a4.ptr)
                    {
                        result = vtcmMgr_create_wait_node(&waitNode, priority, NULL);

                        if (!result)
                        {
                            waitNode->bSinglePage = (unsigned char)a2.num;
                            waitNode->size = size;
                            waitNode->clientHandle = clientHandle;
                        }
                    }
                    else
                    {
                        /*
                         * Caller didn't provide a return pointer for mailbox
                         * return failure as we don't have requested memory
                         * readily available.
                         */
                        result = HAP_VTCM_ALLOC_FAIL_NO_MEM;
                    }
                }

                if (!result &&
                    !search_result &&
                    (selection.allocStatus == VTCM_RESOURCE_SELECTED) )
                {
                    /*
                     * TODO: We may need to get timeout duration to see
                     * if the caller is gonna wait for the release
                     * requests to return!
                     */
                    /*
                     * Search was successful (search_result), if there were
                     * busy releasable nodes, wait node would have been
                     * created successfully (result)
                     */
                    result = vtcm_allocate_immediate(clientHandle,
                                                     partition,
                                                     NULL,
                                                     &selection,
                                                     waitNode);

                    if (!result &&
                        (selection.allocStatus == VTCM_RESOURCE_ASSIGNED) )
                    {
                        pVA = COMPUTE_RES_GET_VA(selection.startPA);

                        if (0 != local_memcpy((void *)a3.ptr,
                                                       (void *)&pVA,
                                                       sizeof(pVA) ) )
                        {
                            /*
                             * Failed to pass back the VA, free memory
                             */
                            vtcm_release(clientHandle,
                                         pVA,
                                         NULL, //No context to be passed
                                         partition,
                                         0); //Non cached request
                            result = HAP_VTCM_ALLOC_FAIL_INVALID_ARGS;
                        }
                    }
                    else if (result)
                    {
                        /*
                         * Unsuccessful in finding vtcm for the request, return
                         * no mem
                         */
                        result = HAP_VTCM_ALLOC_FAIL_NO_MEM;
                    }
                }

                if (waitNode &&
                    ( (selection.allocStatus == VTCM_RESOURCE_UNAVAILABLE) ||
                      (selection.allocStatus == VTCM_RESOURCE_RESERVED) ) )
                {

                    if (0 != local_memcpy((void *)a4.ptr,
                                                   (void *)&waitNode->mailboxId,
                                                   sizeof(waitNode->mailboxId) ) )
                    {
                        /*
                         * Failed to pass the mailbox id, free wait node
                         */
                        vtcmMgr_delete_wait_node(waitNode);
                        result = HAP_VTCM_ALLOC_FAIL_INVALID_ARGS;
                    }
                }
                else if (waitNode)
                {
                    /*
                     * VTCM was neither unavailable nor reserved, so it
                     * failed, no need of a wait node! return failure
                     */
                    vtcmMgr_delete_wait_node(waitNode);
                    result = HAP_VTCM_ALLOC_FAIL_NO_MEM;
                }
            }

            break;
        }

        case VTCMMGR_FREE:
        {
            /*
             * a1.num = VTCM address to free
             */
            unsigned int pVA = (unsigned int)a1.num;
            vtcmMgr_partition_node_t *partition = NULL;

            partition = get_partition_from_app_id(clientHandle, 0);

            if (!partition)
            {
                result = HAP_VTCM_RELEASE_FAIL_REQUEST_NOT_FOUND;

                break;
            }

            result = vtcm_release(clientHandle,
                                  pVA,
                                  NULL, //No context to be passed
                                  partition,
                                  0); //Non cached request

            /* Check if we can allocate for any of the waiting threads */
            vtcmMgr_recheck_waitQ();

            break;
        }

        default:
            /*
             * Default method handler.
             */
            result = -1;

            break;
    }

    return result;
}
