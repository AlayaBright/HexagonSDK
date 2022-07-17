#ifndef QURT_THREAD_H
#define QURT_THREAD_H
/**
  @file qurt_thread.h 
  Prototypes of Thread API  

  EXTERNAL FUNCTIONS
   None.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2021  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/



#include <string.h>
#include "qurt_pmu.h"


/*=============================================================================
												CONSTANTS AND MACROS
=============================================================================*/


/*
  Bitmask configuration is for selecting DSP hardware threads. 
  To select all the hardware threads, using QURT_THREAD_CFG_BITMASK_ALL 
  and the following: \n
  - For QDSP6 V2/V3, all 6 hardware threads are selected \n
  - For QDSP6 V3L, all 4 hardware threads are selected \n
  - For QDSP6 V4, all 3 hardware threads are selected
 */  

#define QURT_THREAD_CFG_BITMASK_HT0      0x00000001   /**< HTO. */
#define QURT_THREAD_CFG_BITMASK_HT1      0x00000002   /**< HT1. */
#define QURT_THREAD_CFG_BITMASK_HT2      0x00000004   /**< HT2. */ 
#define QURT_THREAD_CFG_BITMASK_HT3      0x00000008   /**< HT3. */
#define QURT_THREAD_CFG_BITMASK_HT4      0x00000010   /**< HT4. */
#define QURT_THREAD_CFG_BITMASK_HT5      0x00000020   /**< HT5. */
/** @cond rest_reg_dist */
/** @addtogroup thread_macros
@{ */
/**   @xreflabel{sec:qurt_thread_cfg} */  

#define QURT_THREAD_CFG_BITMASK_ALL      0x000000ff   /**< Select all the hardware threads. */
/** @} */ /* end_addtogroup thread_macros */
/** @endcond */

#define QURT_THREAD_CFG_USE_RAM          0x00000000   /**< Use RAM. */
#define QURT_THREAD_CFG_USE_TCM          0x00000100   /**< Use TCM. */
/** @cond rest_reg_dist */
/** @addtogroup thread_macros
@{ */
#define QURT_THREAD_BUS_PRIO_DISABLED    0   /**< Thread internal bus priority disabled */
#define QURT_THREAD_BUS_PRIO_ENABLED     1   /**< Thread internal bus priority enabled  */
/** @} */ /* end_addtogroup thread_macros */
/** @endcond */

#define QURT_THREAD_AUTOSTACK_DISABLED    0   /**< Thread has autostack v2 feature disabled */
#define QURT_THREAD_AUTOSTACK_ENABLED     1   /**< Thread has autostack v2 feature enabled  */

/*
   Macros for QuRT thread attributes.   
 */

#define QURT_HTHREAD_L1I_PREFETCH      0x1     /**< Enables hardware L1 instruction cache prefetching. */
#define QURT_HTHREAD_L1D_PREFETCH      0x2     /**< Enables hardware L1 data cache prefetching. */
#define QURT_HTHREAD_L2I_PREFETCH      0x4     /**< Enables hardware L2 instruction cache prefetching. */
#define QURT_HTHREAD_L2D_PREFETCH      0x8     /**< Enables hardware L2 data cache prefetching. */
#define QURT_HTHREAD_DCFETCH           0x10    /**< Enables DC fetch to the provided virtual address. 
                                     DC fetch instructs the hardware that a data memory access is likely. 
                                     Instructions are dropped in the case of high bus utilization. */
/** @addtogroup thread_macros
@{ */
/** @xreflabel{hdr:partition_tcm} */
/*
   Below value will be used to create legacy QuRT threads by default.
   IF a thread has this as the detach_state, the thread can be joined
   on till it exits. Once we are able to change default behavior of all
   QuRT threads to JOINABLE (posix default), we can remove this legacy
   behavior.
*/
#define QURT_THREAD_ATTR_CREATE_LEGACY               0
#define QURT_THREAD_ATTR_CREATE_JOINABLE             1
#define QURT_THREAD_ATTR_CREATE_DETACHED             2
/** @} */ /* end_addtogroup thread_macros */


#define QURT_THREAD_ATTR_NAME_MAXLEN            16  /**< Maximum name length. */
#define QURT_THREAD_ATTR_TCB_PARTITION_RAM      0  /**< Creates threads in RAM/DDR. */
#define QURT_THREAD_ATTR_TCB_PARTITION_TCM      1  /**< Creates threads in TCM. */
/** @cond rest_reg_dist */
/** @addtogroup thread_macros
@{ */
#define QURT_THREAD_ATTR_TCB_PARTITION_DEFAULT  QURT_THREAD_ATTR_TCB_PARTITION_RAM  /**< Backward compatibility. */
#define QURT_THREAD_ATTR_PRIORITY_DEFAULT       254   /**< Priority.*/
#define QURT_THREAD_ATTR_ASID_DEFAULT           0    /**< ASID. */
#define QURT_THREAD_ATTR_AFFINITY_DEFAULT      (-1)  /**< Affinity. */
#define QURT_THREAD_ATTR_BUS_PRIO_DEFAULT       255  /**< Bus priority. */
#define QURT_THREAD_ATTR_AUTOSTACK_DEFAULT      0    /**< Default autostack v2 disabled thread*/
#define QURT_THREAD_ATTR_TIMETEST_ID_DEFAULT   (-2)  /**< Timetest ID. */
#define QURT_THREAD_ATTR_STID_DEFAULT 0              /**< STID. */
/** @} */ /* end_addtogroup thread_macros */
/** @endcond*/
/*=============================================================================
												TYPEDEFS
=============================================================================*/
/** @addtogroup thread_types
@{ */
/** @cond rest_reg_dist  */
typedef enum {
    CCCC_PARTITION = 0,     /**< Use the CCCC page attribute bits to determine the main or auxiliary partition. */
    MAIN_PARTITION = 1,     /**< Use the main partition. */
    AUX_PARTITION = 2,      /**< Use the auxiliary partition. */
    MINIMUM_PARTITION = 3   /**< Use the minimum. Allocates the least amount of cache (no-allocate policy possible) for this thread. */
} qurt_cache_partition_t;

/** @endcond */
/** Thread ID type */
typedef unsigned int qurt_thread_t;

/** @cond rest_reg_dist  */
/** Thread attributes */
typedef struct _qurt_thread_attr {
    
    char name[QURT_THREAD_ATTR_NAME_MAXLEN]; /**< Thread name. */
    unsigned char tcb_partition;  /**< Indicates whether the thread TCB resides in RAM or
                                       on chip memory (in other words, TCM). */
    unsigned char  stid;          /**< Software thread ID used to configure the stid register
                                       for profiling pusposes. */
    unsigned short priority;      /**< Thread priority. */
    unsigned char  autostack:1;    /**< Autostack v2 enabled thread. */
    unsigned char  reserved:7;     /**< Reserved bits. */
    unsigned char  bus_priority;  /**< Internal bus priority. */
    unsigned short timetest_id;   /**< Timetest ID. */
    unsigned int   stack_size;    /**< Thread stack size. */
    void *stack_addr;             /**< Pointer to the stack address base, the range of the stack is
                                       (stack_addr, stack_addr+stack_size-1). */
    unsigned short detach_state;  /**< Detach state of the thread */

} qurt_thread_attr_t;
/** @endcond */

/** @cond rest_reg_dist */
/** Dynamic TLS attributes */
typedef struct qurt_tls_info {
  unsigned int module_id;        /**< Module ID for the loaded dynamic linked library. */
  unsigned int tls_start;        /**< Start address of the TLS data. */
  unsigned int tls_data_end;     /**< End address of the TLS RW data. */
  unsigned int tls_end;          /**< End address of the TLS data. */
}qurt_tls_info;
/** @endcond */
/** @} */ /* end_addtogroup thread_types */

/*=============================================================================
												FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_thread_attr_init
  Initializes the structure used to set the thread attributes when a thread is created.
  After an attribute structure is initialized, Explicity set the individual attributes in the structure 
  using the thread attribute operations.

  The initialize operation sets the following default attribute values: \n
  - Name -- NULL string \n
  - TCB partition -- QURT_THREAD_ATTR_TCB_PARTITION_DEFAULT
  - Affinity -- QURT_THREAD_ATTR_AFFINITY_DEFAULT \n
  - Priority -- QURT_THREAD_ATTR_PRIORITY_DEFAULT \n
  - ASID -- QURT_THREAD_ATTR_ASID_DEFAULT \n   
  - Bus priority -- QURT_THREAD_ATTR_BUS_PRIO_DEFAULT \n
  - Timetest ID -- QURT_THREAD_ATTR_TIMETEST_ID_DEFAULT \n
  - stack_size -- 0 \n
  - stack_addr -- 0 \n
  - detach state -- #QURT_THREAD_ATTR_CREATE_DETACHED \n
  - STID -- #QURT_THREAD_ATTR_STID_DEFAULT

  @datatypes
  #qurt_thread_attr_t
  
  @param[in,out] attr Pointer to the thread attribute structure.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_init (qurt_thread_attr_t *attr)
{

    attr->name[0] = 0;
    attr->tcb_partition = QURT_THREAD_ATTR_TCB_PARTITION_DEFAULT;
    attr->priority = QURT_THREAD_ATTR_PRIORITY_DEFAULT;
    attr->autostack = QURT_THREAD_ATTR_AUTOSTACK_DEFAULT; /* Default attribute for autostack v2*/
    attr->bus_priority = QURT_THREAD_ATTR_BUS_PRIO_DEFAULT;
    attr->timetest_id = QURT_THREAD_ATTR_TIMETEST_ID_DEFAULT;
    attr->stack_size = 0;
    attr->stack_addr = 0;
    attr->detach_state = QURT_THREAD_ATTR_CREATE_LEGACY;
    attr->stid = QURT_THREAD_ATTR_STID_DEFAULT;
}

/**@ingroup func_qurt_thread_attr_set_name
  Sets the thread name attribute.\n
  This function specifies the name to use by a thread.
  Thread names identify a thread during debugging or profiling. \n
  @note1hang Thread names differ from the kernel-generated thread identifiers used to
  specify threads in the API thread operations.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Pointer to the thread attribute structure.
  @param[in] name     Pointer to the character string containing the thread name.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_name (qurt_thread_attr_t *attr, char *name)
{
    strlcpy (attr->name, name, QURT_THREAD_ATTR_NAME_MAXLEN);
    attr->name[QURT_THREAD_ATTR_NAME_MAXLEN - 1] = 0;
}


/**@ingroup func_qurt_thread_attr_set_tcb_partition
  Sets the thread TCB partition attribute.
  Specifies the memory type where a thread control block (TCB) of a thread is allocated.
  Allocate TCBs in RAM or TCM/LPM.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr  Pointer to the thread attribute structure.
  @param[in] tcb_partition TCB partition. Values:\n
                     - 0 -- TCB resides in RAM \n
                     - 1 -- TCB resides in TCM/LCM @tablebulletend

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_tcb_partition (qurt_thread_attr_t *attr, unsigned char tcb_partition)
{
    attr->tcb_partition = tcb_partition;
}

/**@ingroup func_qurt_thread_attr_set_priority
  Sets the thread priority to assign to a thread.
  Thread priorities are specified as numeric values in the range 1 to 255, with 1 representing
  the highest priority.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Pointer to the thread attribute structure.
  @param[in] priority Thread priority.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_priority (qurt_thread_attr_t *attr, unsigned short priority)
{
    attr->priority = priority;
}

/**@ingroup func_qurt_thread_attr_set_detachstate
  Sets the thread detach state with which thread is created.
  Thread detach state is either joinable or detached; specified by the following values:
  - #QURT_THREAD_ATTR_CREATE_JOINABLE  \n           
  - #QURT_THREAD_ATTR_CREATE_DETACHED  \n   

  When a detached thread is created (QURT_THREAD_ATTR_CREATE_DETACHED), its thread
  ID and other resources are reclaimed as soon as the thread exits. When a joinable thread 
  is created (QURT_THREAD_ATTR_CREATE_JOINABLE), it is assumed that some
  thread will be waiting to join on it using a qurt_thread_join() call. 
  By default, all qurt threads are created detached.

  @note1hang For a joinable thread (QURT_THREAD_ATTR_CREATE_JOINABLE), it is very
             important that some thread joins on it after it terminates, otherwise
			 the resources of that thread are not reclaimed, causing memory leaks.      

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Pointer to the thread attribute structure.
  @param[in] detachstate Thread detach state.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_detachstate (qurt_thread_attr_t *attr, unsigned short detachstate)
{	
    if(detachstate == QURT_THREAD_ATTR_CREATE_JOINABLE  || detachstate == QURT_THREAD_ATTR_CREATE_DETACHED){
		attr->detach_state = detachstate;
	}
}


/**@ingroup func_qurt_thread_attr_set_timetest_id
  Sets the thread timetest attribute.\n
  Specifies the timetest identifier to use by a thread.

  Timetest identifiers are used to identify a thread during debugging or profiling. \n
  @note1hang Timetest identifiers differ from the kernel-generated thread identifiers used to
             specify threads in the API thread operations.

  @datatypes
  #qurt_thread_attr_t
  
  @param[in,out] attr   Pointer to the thread attribute structure.
  @param[in] timetest_id Timetest identifier value.

  @return
  None.

  @dependencies
  None.
  */
static inline void qurt_thread_attr_set_timetest_id (qurt_thread_attr_t *attr, unsigned short timetest_id)
{
    attr->timetest_id = timetest_id;
}

/**@ingroup func_qurt_thread_attr_set_stack_size
  @xreflabel{sec:set_stack_size}
  Sets the thread stack size attribute.\n
  Specifies the size of the memory area to use for a call stack of a thread.

  The thread stack address (Section @xref{sec:set_stack_addr}) and stack size specify the memory area used as a
  call stack for the thread. The user is responsible for allocating the memory area used for
  the stack.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Pointer to the thread attribute structure.
  @param[in] stack_size Size (in bytes) of the thread stack.

  @return
  None.

  @dependencies
  None.
*/

static inline void qurt_thread_attr_set_stack_size (qurt_thread_attr_t *attr, unsigned int stack_size)
{
    attr->stack_size = stack_size;
}

/**@ingroup func_qurt_thread_attr_set_stack_size2
  @xreflabel{sec:set_stack_size}
  Sets the thread stack size attribute for Island threads requiring higher guest OS stack size than the stack size
  defined in config xml.\n
  Specifies the size of the memory area to use for a call stack of an Island thread in User and Guest mode.

  The thread stack address (Section @xref{sec:set_stack_addr}) and stack size specify the memory area used as a
  call stack for the thread. The user is responsible for allocating the memory area used for
  the stack.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Pointer to the thread attribute structure.
  @param[in] user_stack_size Size (in bytes) of the stack usage in User mode.
  @param[in] root_stack_size Size (in bytes) of the stack usage in Guest mode.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_stack_size2 (qurt_thread_attr_t *attr, unsigned short user_stack_size, unsigned short root_stack_size)
{
	union qurt_thread_stack_info{
		unsigned int raw_size;
		struct{
			unsigned short user_stack;
			unsigned short root_stack;
		};
	}user_root_stack_size;
	user_root_stack_size.user_stack = user_stack_size;
	user_root_stack_size.root_stack = root_stack_size;
	
    attr->stack_size = user_root_stack_size.raw_size;
}

/**@ingroup func_qurt_thread_attr_set_stack_addr
  @xreflabel{sec:set_stack_addr}
  Sets the thread stack address attribute. \n
  Specifies the base address of the memory area to use for a call stack of a thread.

  stack_addr must contain an address value that is 8-byte aligned.

  The thread stack address and stack size (Section @xref{sec:set_stack_size}) specify the memory area used as a
  call stack for the thread. \n
  @note1hang The user is responsible for allocating the memory area used for the thread
             stack. The memory area must be large enough to contain the stack that the thread
			 creates.

  @datatypes
  #qurt_thread_attr_t
  
  @param[in,out] attr Pointer to the thread attribute structure.
  @param[in] stack_addr  Pointer to the 8-byte aligned address of the thread stack.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_stack_addr (qurt_thread_attr_t *attr, void *stack_addr)
{
    attr->stack_addr = stack_addr;
}

/**@ingroup func_qurt_thread_attr_set_bus_priority
   Sets the internal bus priority state in the Hexagon core for this software thread attribute. 
   Memory requests generated by the thread with bus priority enabled are
   given priority over requests generated by the thread with bus priority disabled. 
   The default value of bus priority is disabled.

   @note1hang Sets the internal bus priority for Hexagon processor version V60 or greater. 
              The priority is not propagated to the bus fabric.
  
   @datatypes
   #qurt_thread_attr_t

   @param[in] attr Pointer to the thread attribute structure.

   @param[in] bus_priority Enabling flag. Values: \n 
         - #QURT_THREAD_BUS_PRIO_DISABLED \n
         - #QURT_THREAD_BUS_PRIO_ENABLED @tablebulletend

   @return
   None

   @dependencies
   None.
*/
static inline void qurt_thread_attr_set_bus_priority ( qurt_thread_attr_t *attr, unsigned short bus_priority)
{
    attr->bus_priority = bus_priority;
}

/**@ingroup func_qurt_thread_attr_set_autostack
   Enables autostack v2 feature for threads setting autostack bit to 1.
   If autostack enabled thread gets framelimit exception, kernel will
   allocate more stack for thread and return to normal execution. 
   If autostack disabled thread gets framelimit exception, exception will be error fatal.
  
   @datatypes
   #qurt_thread_attr_t

   @param[in] attr Pointer to the thread attribute structure.

   @param[in] Autostack enable, disable flag. Values: \n 
         - #QURT_THREAD_AUTOSTACK_DISABLED \n
         - #QURT_THREAD_AUTOSTACK_ENABLED @tablebulletend

   @return
   None

   @dependencies
   None.
*/
static inline void qurt_thread_attr_set_autostack ( qurt_thread_attr_t *attr, unsigned short autostack)
{
    attr->autostack = autostack;
}

static inline void qurt_thread_attr_enable_stid ( qurt_thread_attr_t *attr, char enable_stid)
{
    if (enable_stid) {
        attr->stid = enable_stid;
    }
    else
    {
        attr->stid = QURT_THREAD_ATTR_STID_DEFAULT;
    }
}


/**@ingroup func_qurt_thread_set_autostack
  Sets autostack enable in TCB\n
  

  @param[in] Pointer to UGP

  @return
  None.

  @dependencies
  None.
*/

void qurt_thread_set_autostack(void *);


/**@ingroup func_qurt_thread_get_name
  Gets the thread name of current thread.\n
  Returns the thread name of the current thread. 
  Thread names are assigned to threads as thread attributes (Section @xref{sec:threads}). They are used to
  identify a thread during debugging or profiling.

  @param[out] name Pointer to a character string, which specifies the address where the returned thread name is stored.
  @param[in] max_len Maximum length of the character string that can be returned.

  @return
  None.

  @dependencies
  None.
*/
void qurt_thread_get_name (char *name, unsigned char max_len);

/**@ingroup func_qurt_thread_create
  @xreflabel{hdr:qurt_thread_create}
  Creates a thread with the specified attributes, and makes it executable.

  @note1hang This function fails (with an error result) if the set of hardware threads
             specified in the thread attributes is invalid for the target processor version.

  @datatypes
  #qurt_thread_t \n
  #qurt_thread_attr_t
  
  @param[out]  thread_id    Returns a pointer to the thread identifier if the thread was 
                             successfully created.
  @param[in]   attr 	    Pointer to the initialized thread attribute structure that specifies 
                             the attributes of the created thread.
  @param[in]   entrypoint   C function pointer, which specifies the main function of a thread.
  @param[in]   arg  	     Pointer to a thread-specific argument structure
  
   
  @return 
  #QURT_EOK -- Thread created. \n
  #QURT_EFAILED -- Thread not created. 

  @dependencies
  None.
 */
int qurt_thread_create (qurt_thread_t *thread_id, qurt_thread_attr_t *attr, void (*entrypoint) (void *), void *arg);

/*
   Stops the current thread, frees the kernel TCB, and yields to the next highest ready thread. 
  
   @return
   void 

   @dependencies
   None.
 */
void qurt_thread_stop(void);


/**@ingroup func_qurt_thread_resume
   Resumes the execution of a suspended thread.
  
   @param[in]  thread_id Thread identifier.

   @return 
   #QURT_EOK -- Thread successfully resumed. \n
   #QURT_EFATAL -- Resume operation failed.

   @dependencies
   None.
 */
int qurt_thread_resume(unsigned int thread_id);

/**@ingroup func_qurt_thread_get_id
   Gets the identifier of the current thread.\n
   Returns the thread identifier for the current thread.
     
   @return 
   Thread identifier -- Identifier of the current thread. 

   @dependencies
   None.
 */
qurt_thread_t qurt_thread_get_id (void);


/**@ingroup func_qurt_thread_get_l2cache_partition
   Returns the current value of the L2 cache partition assigned to the caller thread.\n
     
   @return 
   Value of the data type #qurt_cache_partition_t.

   @dependencies
   None.
 */
qurt_cache_partition_t qurt_thread_get_l2cache_partition (void);

/**@ingroup func_qurt_thread_set_timetest_id
   Sets the timetest identifier of the current thread.
   Timetest identifiers are used to identify a thread during debugging or profiling.\n
   @note1hang Timetest identifiers differ from the kernel-generated thread identifiers used to
              specify threads in the API thread operations.

   @param[in]  tid  Timetest identifier.

   @return
   None.

   @dependencies
   None.
 */
void qurt_thread_set_timetest_id (unsigned short tid);

/**@ingroup func_qurt_thread_set_cache_partition
   Sets the cache partition for the current thread. This function uses the type qurt_cache_partition_t to select the cache partition 
            of the current thread for the L1 I cache, L1 D cache, and L2 cache.
  
   @datatypes
   #qurt_cache_partition_t 

   @param[in] l1_icache L1 I cache partition.
   @param[in] l1_dcache L1 D cache partition.
   @param[in] l2_cache L2 cache partition.
    
   @return
   None.

   @dependencies
   None.
 */
void qurt_thread_set_cache_partition(qurt_cache_partition_t l1_icache, qurt_cache_partition_t l1_dcache, qurt_cache_partition_t l2_cache);


#if 0
/**
   Sets the prefetch value in the SSR.
  
   @param   settings	The least significant 5 bits are used to set the prefetchability of the calling
                        thread. See @xhyperref{Q5,[Q5]} for hardware meanings.
    
   @return
   None.

   @dependencies
   None.
 */
void qurt_thread_set_prefetch(unsigned int settings);
#endif

/**@ingroup func_qurt_thread_get_timetest_id
   Gets the timetest identifier of the current thread.\n
   Returns the timetest identifier of the current thread.\n
   Timetest identifiers are used to identify a thread during debugging or profiling. \n
   @note1hang Timetest identifiers differ from the kernel-generated thread identifiers used to
              specify threads in the API thread operations.

   @return 
   Integer -- Timetest identifier. 

   @dependencies
   None.
 */
unsigned short qurt_thread_get_timetest_id (void);

/**@ingroup func_qurt_thread_exit
   @xreflabel{sec:qurt_thread_exit}
   Stops the current thread and awakens any threads joined to it, then destroys the stopped
   thread.

   Any thread that has been suspended on the current thread (by performing a thread join 
   -- Section @xref{sec:thread_join}) is awakened and passed a user-defined status value 
   indicating the status of the stopped thread.\n

   @note1hang Exit must be called in the context of the thread to stop.
  
   @param[in]   status User-defined thread exit status value.

   @return
   None.

   @dependencies
   None.
 */
void qurt_thread_exit(int status);

/**@ingroup func_qurt_thread_join
   @xreflabel{sec:thread_join}
   Waits for a specified thread to finish; the specified thread is another thread within
   the same process.
   The caller thread is suspended until the specified thread exits. When the unspecified thread
   exits, the caller thread is awakened. \n
   @note1hang If the specified thread has already exited, this function returns immediately
              with the result value #QURT_ENOTHREAD. \n
   @note1cont Two threads cannot call qurt_thread_join to wait for the same thread to finish.
              If this happens, QuRT generates an exception (see Section @xref{sec:exceptionHandling}).
  
   @param[in]   tid     Thread identifier.
   @param[out]  status  Destination variable for thread exit status. Returns an application-defined 
                        value indicating the termination status of the specified thread. 
  
   @return  
   #QURT_ENOTHREAD -- Thread has already exited. \n
   #QURT_EOK -- Thread successfully joined with valid status value. 

   @dependencies
   None.
 */
int qurt_thread_join(unsigned int tid, int *status);

/**@ingroup qurt_thread_detach
   @xreflabel{sec:thread_detach}
   Detaches a joinable thread. The specified thread is another thread within the 
   same process. Create the thread as a joinable thread; only joinable threads 
   can be detached.
   If a joinable thread is detached, it finishes its execution and exits.
  
   @param[in]   tid     Thread identifier.
   
   @return  
   #QURT_ENOTHREAD -- Thread specifed by TID does not exist. \n
   #QURT_EOK -- Thread successfully detached.

   @dependencies
   None.
 */
int qurt_thread_detach(unsigned int tid);

/**@ingroup func_qurt_thread_get_anysignal
   Gets the signal of the current thread.
   Returns the RTOS-assigned signal of the current thread.

   QuRT assigns every thread a signal to support communication between threads.
   
   @return  
   Signal object address -- Any-signal object assigned to the current thread.

   @dependencies
   None.
  
 */
unsigned int qurt_thread_get_anysignal(void);

/**@ingroup func_qurt_thread_get_priority 
   Gets the priority of the specified thread. \n 
   Returns the thread priority of the specified thread.\n
   Thread priorities are specified as numeric values in a range as large as 0 through 255, with lower
   values representing higher priorities. 0 represents the highest possible thread priority. \n
   @note1hang QuRT can be configured to have different priority ranges.

   @datatypes
   #qurt_thread_t
  
   @param[in]  threadid	   Thread identifier.	

   @return
   -1 -- Invalid thread identifier. \n
   0 through 255 -- Thread priority value.

   @dependencies
   None.
 */
int qurt_thread_get_priority (qurt_thread_t threadid);

/**@ingroup func_qurt_thread_set_priority
   Sets the priority of the specified thread.\n
   Thread priorities are specified as numeric values in a range as large as 0 through 255, with lower
   values representing higher priorities. 0 represents the highest possible thread priority.

   @note1hang QuRT can be configured to have different priority ranges. For more
              information see Section @xref{sec:AppDev}.
   
   @datatypes
   #qurt_thread_t

   @param[in] threadid	    Thread identifier.	
   @param[in] newprio 	    New thread priority value.

   @return
   0 -- Priority successfully set. \n
   -1 -- Invalid thread identifier. \n 
   
   @dependencies
   None.
 */
int qurt_thread_set_priority (qurt_thread_t threadid, unsigned short newprio);

/*
   Gets the QuRT API version.
 
  @return
  Qurt API version.

  @dependencies
  None.
 */
unsigned int qurt_api_version(void);

/**@ingroup func_qurt_thread_attr_get
  Gets the attributes of the specified thread.\n

  @datatypes
  #qurt_thread_t \n
  #qurt_thread_attr_t

  @param[in]  thread_id	    Thread identifier.
  @param[out] attr 	    Pointer to the destination structure for thread attributes.
  
  @return
  #QURT_EOK -- Success. \n
  #QURT_EINVALID -- Invalid argument.

  @dependencies
  None.
 */
int qurt_thread_attr_get (qurt_thread_t thread_id, qurt_thread_attr_t *attr);



/**@ingroup func_qurt_thread_get_tls_base
  Gets the base address of thread local storage (TLS) of a dynamically loaded module
  for the current thread.
  
  @datatypes
  #qurt_tls_info 

  @param[in]  info	   Pointer to the TLS information for a module.
  
  @return
   Pointer to the TLS object for the dynamically loaded module.\n
   NULL -- TLS information is invalid.

  @dependencies
  None.
 */
void * qurt_thread_get_tls_base(qurt_tls_info* info);

/**@ingroup func_qurt_thread_pktcount_get
  Gets the PKTCOUNT of a specified thread.\n

  @datatypes
  #qurt_thread_t \n

  @param[in]  thread_id	    Thread identifier.
  
  @return
  PKTCOUNT

  @dependencies
  None.
 */

long long int qurt_thread_pktcount_get (qurt_thread_t thread_id);

/**@ingroup func_qurt_thread_pktcount_set
  Sets the PKTCOUNT for the current QuRT thread.

  @return
  Value pktcount is set to.

  @dependencies
  None.
 */

long long int qurt_thread_pktcount_set (long long int);

/**@ingroup func_qurt_thread_stid_get
  Gets the STID for a specified thread.

  @datatypes
  #qurt_thread_t 

  @param[in]  thread_id	    Thread identifier.
  
  @return
  STID

  @dependencies
  None.
 */

char qurt_thread_stid_get(qurt_thread_t thread_id);

/**@ingroup func_qurt_thread_stid_set
  Sets the STID for a specified thread. \n

  @datatypes
  #qurt_thread_t \n

  @param[in]  stid	    Thread identifier.
  
  @return 
   #QURT_EOK -- STID set created. \n
   #QURT_EFAILED -- STID not set. 

  @dependencies
  None.
 */

int qurt_thread_stid_set(char stid);

/**@ingroup func_qurt_thread_get_running_ids
  Returns the thread IDs of the running threads in the system; use only during fatal error handling.
 
  @datatypes
  #qurt_thread_t 
 
  @param[in,out] * Array of thread identifier of size #QURT_MAX_HTHREAD_LIMIT + 1.
 
  @return
   #QURT_EINVALID -- Incorrect argument \n
   #QURT_ENOTALLOWED  -- API not called during error handling \n
   #QURT_EOK -- Success, returns a NULL-terminated array of thread_id
 
  @dependencies
  None.
 */
int qurt_thread_get_running_ids(qurt_thread_t *);


/**@ingroup func_qurt_thread_get_running_id
  Gets the thread identifier of the thread with the matching name in the same process
  of the caller.
 
  @datatypes
  #qurt_thread_t \n
 
  @param[out] Thread identifier
  @param[in]  name of the thread
 
  @return
   QURT_EINVALID -- No thread with the matching name in the caller's process
   QURT_EOK      -- Success  
 
  @dependencies
  None.
 */
int qurt_thread_get_thread_id (qurt_thread_t *thread_id, char *name);

/**@ingroup func_qurt_sleep
  Suspends the current thread for the specified amount of time.

  @note1hang Since QuRT timers are deferrable, this call is guaranteed to block
             at least for the specified amount of time. If power-collapse is 
             enabled, the maximum amount of time this call can block depends on
             the earliest wakeup from power-collapse past the specified duration.

  @param[in] duration  Duration (in microseconds) for which the thread is suspended.

  @return 
  None.

  @dependencies
  None.
 */
void qurt_sleep (unsigned long long int duration);

#endif /* QURT_THREAD_H */
