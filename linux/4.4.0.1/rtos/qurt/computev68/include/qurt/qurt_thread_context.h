#ifndef QURT_THREAD_CONTEXT_H
#define QURT_THREAD_CONTEXT_H
/**
  @file qurt_thread_context.h 
  @brief Kernel thread context structure
			
EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2018-2020  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/



#include <qurt_qdi_constants.h>

/** @cond internal_only */

/** @cond */
#define THREAD_ITERATOR_END ((qurt_thread_t)(-1))  /**< thread iterator has completed. */   
/** @endcond */

/**@ingroup func_qurt_thread_iterator_create
qurt_thread_iterator_create gives ability to the caller to enumerate threads in the system.
@datatypes

@param None

@return returns handle of the newly created iterator. The handle needs to be passed for
        subsequent operations on the iterator       
		  
None
@dependencies
None
*/
static inline int qurt_thread_iterator_create(void)
{
   return qurt_qdi_handle_invoke(QDI_HANDLE_GENERIC, QDI_OS_THREAD_ITERATOR_CREATE);
}

/**@ingroup func_qurt_thread_iterator_next
qurt_thread_iterator_next is used to iterate over the list of threads in the system
@datatypes

@param iter[in] iterator handle that is returned by qurt_thread_iterator_create()

@return THREAD_ITERATOR_END indicates that iterator has reached the end of the thread list.
        Any other value indicates a valid thread_id.
		  
None
@dependencies
None
*/
static inline qurt_thread_t qurt_thread_iterator_next(int iter)
{
   return qurt_qdi_handle_invoke(iter, QDI_OS_THREAD_ITERATOR_NEXT);
}

/**@ingroup func_qurt_thread_iterator_destroy
qurt_thread_iterator_destroy is used to cleanup thread iterator resources
@datatypes

@param iter[in] iterator handle that is returned by qurt_thread_iterator_create()

@return QURT_EOK on successful completion of operation
        QURT_EFATAL indicates the handle passed was invalid.
		  
None
@dependencies
None
*/
static inline int qurt_thread_iterator_destroy(int iter)
{
   return qurt_qdi_close(iter);
}

/**@ingroup func_qurt_thread_context_get_tname
qurt_thread_context_get_tname is used to get name of the thread from given thread id.
@datatypes

@param thread_id[in]   Represents the thread for which name will be returned
       name[in/out]    Pointer to the local buffer where name will be copied back
       max_len[in]     Size of the local buffer

@return  QURT_EOK on success, failure otherwise.
		  
None
@dependencies
None
*/
int qurt_thread_context_get_tname(unsigned int thread_id, char *name, unsigned char max_len);

/**@ingroup func_qurt_thread_context_get_prio
qurt_thread_context_get_prio is used to get priority for the given thread.
@datatypes

@param thread_id[in]   Represents the thread for which priority will be returned
       prio[in/out]    Pointer to the local variable where priority will be written

@return  QURT_EOK on success, failure otherwise.
		  
None
@dependencies
None
*/
int qurt_thread_context_get_prio(unsigned int thread_id, unsigned char *prio);

/**@ingroup func_qurt_thread_context_get_pcycles
qurt_thread_context_get_pcycles is used to get pcycles for the given thread.
@datatypes

@param thread_id[in]   Represents the thread for which processor cycles will be returned
       pcycles[in/out] Pointer to the local variable where processor cycles will be written

@return  QURT_EOK on success, failure otherwise.
		  
None
@dependencies
None
*/
int qurt_thread_context_get_pcycles(unsigned int thread_id, unsigned long long int *pcycles);

/**@ingroup func_qurt_thread_context_get_stack_base
qurt_thread_context_get_stack_base is used to get stack base address for the given thread.
@datatypes

@param thread_id[in]   Represents the thread for which stack base address will be returned
       sbase[in/out]   Pointer to the local variable where stack base address will be written

@return  QURT_EOK on success, failure otherwise.
		  
None
@dependencies
None
*/
int qurt_thread_context_get_stack_base(unsigned int thread_id, unsigned int *sbase);

/**@ingroup func_qurt_thread_context_get_stack_size
qurt_thread_context_get_stack_size is used to get stack size for the given thread.
@datatypes

@param thread_id[in]   Represents the thread for which stack size will be returned
       ssize[in/out]   Pointer to the local variable where stack size will be written

@return  QURT_EOK on success, failure otherwise.
		  
None
@dependencies
None
*/
int qurt_thread_context_get_stack_size(unsigned int thread_id, unsigned int *ssize);

/**@ingroup func_qurt_thread_context_get_pid
qurt_thread_context_get_pid is used to get process id for the given thread.
@datatypes

@param thread_id[in]  Represents the thread for which process id will be returned
       pid[in/out]    Pointer to the local variable where process id will be written

@return  QURT_EOK on success, failure otherwise.
		  
None
@dependencies
None
*/
int qurt_thread_context_get_pid(unsigned int thread_id, unsigned int *pid);

/**@ingroup func_qurt_thread_context_get_pname
qurt_thread_context_get_pname is used to get process name for the given thread.
@datatypes

@param thread_id[in]  Represents the thread for which process name will be returned
       name[in/out]   Pointer to the local buffer where process name will be copied back
       len[in]        Length allocated to the local buffer.

@return  QURT_EOK on success, failure otherwise.
		  
None
@dependencies
None
*/
int qurt_thread_context_get_pname(unsigned int thread_id, char *name, unsigned int len);


/* new structure defining how tcb is interpreted to crash dump tools
 * definitions of keys in consts.h */
struct qurt_debug_thread_info {
/** @cond */
    char name[QURT_MAX_NAME_LEN];     /**< Name of the thread. */
    struct {
        unsigned key;                 
        unsigned val;
    } os_info[40];  
    unsigned gen_regs[32];            /**< General mode registers. */
    unsigned user_cregs[32];          /**< User mode registers. */
    unsigned guest_cregs[32];         /**< Guest mode registers. */
    unsigned monitor_cregs[64];       /**< Monitor mode registers. */
/** @endcond */
}; /* should add up to 1K */


/**@ingroup func_qurt_system_tcb_dump_get
qurt_system_tcb_dump_get can be used to cleanup thread iterator resources
@datatypes

@param thread_id[in] Thread on which the operation needs to be performed
       ptr[in/out]   Pointer to the local buffer where contents will be written
       size[in]      Size of the debug thread info structure obtained by calling
                     qurt_system_tcb_dump_get_size().
	   
@return QURT_EOK for success, failure otherwise
		  
None
@dependencies
None
*/
int qurt_system_tcb_dump_get(qurt_thread_t thread_id, void *ptr, size_t size);
/** @endcond */

#endif /* QURT_THREAD_CONTEXT_H */
