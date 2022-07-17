#ifndef QURT_PROCESS_H
#define QURT_PROCESS_H
/**
  @file qurt_process.h
  Prototypes of QuRT process control APIs.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
#include "qurt_callback.h"


/** @cond */
#define QURT_PROCESS_ATTR_NAME_MAXLEN QURT_MAX_NAME_LEN   /**< Maximum length for process name. */
/** QuRT process thread attributes */
#define QURT_PROCESS_DEFAULT_CEILING_PRIO 0        /**< Default ceiling priority of the threads in the new process. */
#define QURT_PROCESS_DEFAULT_MAX_THREADS  -1       /**< Default number of threads in the new process.
                                                        -1 indicates that the limit is set to the maximum supported by the system. */
/** @endcond */

/** QuRT process flags */
#define QURT_PROCESS_NON_SYSTEM_CRITICAL (1u << 1) /**< Starts the new process as non system-critical. */
#define QURT_PROCESS_UNTRUSTED           (1u << 7) /**< Starts the new process as unsigned process. */


/** @addtogroup process_types
@{ */
/* QuRT process types */
typedef enum {
    QURT_PROCESS_TYPE_RESERVED,            /**< Process type is reserved */
    QURT_PROCESS_TYPE_KERNEL,              /**< Indicates kernel process */
    QURT_PROCESS_TYPE_SRM,                 /**< Indicates SRM process    */
    QURT_PROCESS_TYPE_SECURE,              /**< Indicates secure process */
    QURT_PROCESS_TYPE_ROOT,                /**< Indicates root process   */
    QURT_PROCESS_TYPE_USER,                /**< Indicates user process   */
}qurt_process_type_t;

/** QuRT process callaback types */
typedef enum {
   QURT_PROCESS_DUMP_CB_ROOT,             /**< Register callback which executes in
                                               root process context */
   QURT_PROCESS_DUMP_CB_ERROR,            /**< Register user process callback which gets 
                                               called after threads in the process are frozen */
   QURT_PROCESS_DUMP_CB_PRESTM,           /**< Register user process callback which gets
                                               called before threads in the process are frozen */
   QURT_PROCESS_DUMP_CB_MAX               /**< Reserved for error checking */
}qurt_process_dump_cb_type_t;

/** QuRT process dump attributes. */
typedef struct _qurt_pd_dump_attr{
  /** @cond */
  unsigned int enabled;                    /**< Process dump is enabled */
  char *path;                              /**< Process dump path */
  unsigned int path_len;                   /**< Length of process dump path */
  /** @endcond */
}qurt_pd_dump_attr_t;                    


/** QuRT process attributes. */
typedef struct _qurt_process_attr {
    /** @cond */
    char name[QURT_PROCESS_ATTR_NAME_MAXLEN]; /**< Name of the new process */
    int flags;                                /**< Flags as indicated by QuRT process flags */
    unsigned sid;                             /**< StreamID of the process being spawned */
    unsigned max_threads;                     /**< Maximum threads that can be created by new process */
    unsigned short ceiling_prio;              /**< Maximum priority at which threads can be 
                                                   created by new process */
    qurt_process_type_t type;                 /**< Type of the process indicated by 
                                                   qurt_process_type_t */
    qurt_pd_dump_attr_t dump_attr;            /**< Process dump attributes for the new process 
                                                   as indicated by qurt_pd_dump_attr_t */ 
    /** @endcond */
} qurt_process_attr_t; 

/** @} */ /* end_addtogroup process_types */

/*=============================================================================
FUNCTIONS
=============================================================================*/
 /** @cond rest_reg_dist */
/**@ingroup func_qurt_process_create
  Creates a process with the specified attributes, and starts the process.

  The process executes the code in the specified executable ELF file.

  @datatypes
  #qurt_process_attr_t

  @param[out] attr Accepts an initialized process attribute structure, which specifies
                   the attributes of the created process.

  @return
  In case of negative return value,error is returned:
  #-QURT_EPRIVILEGE                     Caller does not have enough privilege for this operation.
  #-QURT_EMEM                           Not enough memory to perform the operation.
  #-QURT_EFAILED                        Operation failed.
  #-QURT_ENOTALLOWED                    Operation not allowed.
  #-QURT_ENOREGISTERED                  Not Registered.   
  #-QURT_ENORESOURCE                    Resource exhaustion  
  #-QURT_EINVALID                       Invalid argument value 
  
  
  In case of positive return value, Process ID is returned.


  @dependencies
  None.
*/
int qurt_process_create (qurt_process_attr_t *attr);

/**@ingroup func_qurt_process_get_id
  Returns the process identifier for the current thread. 

  @return
  None.

  @dependencies
  Process identifier for the current thread..
*/
int qurt_process_get_id (void);
/** @endcond */
/** @cond internal_only*/
/**@ingroup func_qurt_process_get_uid
  Returns the user identifier for the current thread. 

  @return
  None.

  @dependencies
  User identifier for the current thread.
*/
int qurt_process_get_uid (void);
/** @endcond */
/** @cond rest_reg_dist */
/**@ingroup func_qurt_process_attr_init
  Initializes the structure that sets the process attributes when a thread is created.

  After an attribute structure is initialized, the individual attributes in the structure can 
  be explicitly set using the process attribute operations.

  Table @xref{tbl:processAttrDefaults} lists the default attribute values set by the initialize 
  operation.

  @inputov{table_process_attribute_defaults}

  @datatypes
  #qurt_process_attr_t

  @param[out] attr Pointer to the structure to initialize.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_process_attr_init (qurt_process_attr_t *attr)
{
    attr->name[0] = 0;
    attr->flags = 0;
    attr->sid = 0;
    attr->max_threads = QURT_PROCESS_DEFAULT_MAX_THREADS;
    attr->ceiling_prio = QURT_PROCESS_DEFAULT_CEILING_PRIO;
    attr->type = QURT_PROCESS_TYPE_RESERVED;
    attr->dump_attr.enabled = 0;
    attr->dump_attr.path = NULL;
    attr->dump_attr.path_len = 0;
}

/**@ingroup func_qurt_process_attr_set_executable
  Sets the process name in the specified process attribute structure.

  Process names identify process objects that are already 
  loaded in memory as part of the QuRT system.

  @note1hang Process objects are incorporated into the QuRT system at build time.

  @datatypes
  #qurt_process_attr_t

  @param[in] attr Pointer to the process attribute structure.
  @param[in] name Pointer to the process name.
 
  @return
  None.

  @dependencies
  None.
*/
void qurt_process_attr_set_executable (qurt_process_attr_t *attr, char *name);

/**@ingroup func_qurt_process_attr_set_flags
Sets the process properties in the specified process attribute structure.
Process properties are represented as defined symbols that map into bits 
0 through 31 of the 32-bit flag value. Multiple properties are specified by OR'ing 
together the individual property symbols.

@datatypes
#qurt_process_attr_t

@param[in] attr  Pointer to the process attribute structure.
@param[in] flags QURT_PROCESS_SUSPEND_ON_STARTUP suspends the process after creating it.

@return
None.

@dependencies
None.
*/
static inline void qurt_process_attr_set_flags (qurt_process_attr_t *attr, int flags)
{
    attr->flags = flags;
}
/** @endcond */
/** @cond internal_only*/
/**@ingroup func_qurt_process_attr_set_sid
Sets the process streamID in the specified process attribute structure.

@datatypes
#qurt_process_attr_t

@param[in] attr  Pointer to the process attribute structure.
@param[in] sid   streamID to set for this process.

@return
None.

@dependencies
None.
*/
static inline void qurt_process_attr_set_sid (qurt_process_attr_t *attr, unsigned sid)
{
    attr->sid = sid;
}
/** @endcond */
/** @cond rest_reg_dist */
/**@ingroup func_qurt_process_attr_set_max_threads
Sets the maximum number of threads allowed in the specified process attribute structure.

@datatypes
#qurt_process_attr_t

@param[in] attr          Pointer to the process attribute structure.
@param[in] max_threads   Maximum number of threads allowed for this process.

@return
None.

@dependencies
None.
*/
static inline void qurt_process_attr_set_max_threads (qurt_process_attr_t *attr, unsigned max_threads)
{
    attr->max_threads = max_threads;
}

/**@ingroup func_qurt_process_attr_set_ceiling_prio
Sets the highest thread priority allowed in the specified process attribute structure.

@datatypes
#qurt_process_attr_t

@param[in] attr          Pointer to the process attribute structure.
@param[in] prio          Priority.

@return
None.

@dependencies
None.
*/
static inline void qurt_process_attr_set_ceiling_prio (qurt_process_attr_t *attr, unsigned short prio)
{
    attr->ceiling_prio = prio;
}
/** @endcond */
/** @cond internal_only*/
/**@ingroup func_qurt_process_attr_set_dump_status
Sets the process domain dump-enabled field in process domain dump attributes.

@datatypes
#qurt_process_attr_t

@param[in] attr          Pointer to the process attribute structure.
@param[in] enabled       1 -- Process domain dump is collected \n
                         0 -- Process domain dump is not collected

@return
None.

@dependencies
None.
*/
static inline void qurt_process_attr_set_dump_status(qurt_process_attr_t *attr, unsigned int enabled)
{
    attr->dump_attr.enabled = enabled;
}

/**@ingroup func_qurt_process_attr_set_dump_path
Sets the process domain dump path and its type.

@datatypes
#qurt_process_attr_t

@param[in] attr          Pointer to the process attribute structure.
@param[in] path          Path where the process domain dumps must be saved.
@param[in] path_len      Length of the path string.

@return
None. 

@dependencies
None.
*/
static inline void qurt_process_attr_set_dump_path(qurt_process_attr_t *attr, const char *path, int path_len)
{
    attr->dump_attr.path = (char*)path;
    attr->dump_attr.path_len = path_len;
}
/** @endcond */
/** @cond rest_reg_dist */
/**@ingroup func_qurt_process_cmdline_get
Gets the command line string associated with the current process.
The Hexagon simulator command line arguments are retrieved using 
this function as long as the call is made
in the process of the QuRT installation, and with the 
requirement that the program is running in a simulation environment.

If the function modifies the provided buffer, it zero-terminates
the string. It is possible that the function does not modify the
provided buffer, so the caller must set buf[0] to a NULL
byte before making the call. A truncated command line is returned when
the command line is longer than the provided buffer.

@param[in] buf      Pointer to a character buffer that must be filled in.
@param[in] buf_siz  Size (in bytes) of the buffer pointed to by buf.

@return
None.

@dependencies
None.
*/
void qurt_process_cmdline_get(char *buf, unsigned buf_siz);

/**@ingroup func_qurt_process_get_thread_count
Gets the number of threads present in the process indicated by PID. 
 
@param[in] pid PID of the process for which the information is required.

@return
Number of threads in the process indicated by PID.

@dependencies
None.
*/
int qurt_process_get_thread_count(unsigned int pid);

/**@ingroup func_qurt_process_get_thread_ids
Gets all the thread IDs for a process indicated by PID. 

@param[in] pid      PID of the process for which the information is required.
@param[in] ptr      Pointer to a user passed buffer that must be filled in with thread IDs.
@param[in] thread_num  Number of thread IDs requested.

@return
None.

@dependencies
None.
 */
int qurt_process_get_thread_ids(unsigned int pid, unsigned int *ptr, unsigned thread_num);
/** @endcond */
/** @cond internal_only*/
/**@ingroup func_qurt_process_dump_get_mem_mappings_count
Gets the number of mappings present in the process indicated by PID. 
 
@param[in] pid PID of the process for which the information is required.

@return
Number of mappings for the process indicated by PID.

@dependencies
None.
*/
int qurt_process_dump_get_mem_mappings_count(unsigned int pid);

/**@ingroup func_qurt_process_dump_get_mappings
Gets all the mappings for a specified PID.

@note1hang Device type mappings or mappings created by setting the QURT_PERM_NODUMP attribute
           are skipped by this API.

@param[in] pid      PID of the process for which the information is required.
@param[in] ptr      Pointer to a buffer that must be filled in with mappings.
@param[in] count    Count of mappings requested.

@return
Number of mappings filled in the buffer passed by the user.

@dependencies
None.
*/
int qurt_process_dump_get_mappings(unsigned int pid, unsigned int *ptr, unsigned count);
/** @endcond */
/** @cond rest_reg_dist */
/**@ingroup func_qurt_process_attr_get
Gets the attributes of the process with which it was created. 
 
@datatypes
#qurt_process_attr_t

@param[in]     pid  PID of the process for which the information is required.
@param[in,out] attr Pointer to the user allocated attribute structure.

@return
None.

@dependencies
None.
*/
int qurt_process_attr_get(unsigned int pid, qurt_process_attr_t *attr);

/**@ingroup func_qurt_process_dump_register_cb
Registers process domain dump callback. 
 
@datatypes
#qurt_cb_data_t \n
#qurt_process_dump_cb_type_t

@param[in] cb_data Pointer to the callback information.
@param[in] type Callback type; these callbacks are called in the context of the user process domain: \n
   #QURT_PROCESS_DUMP_CB_PRESTM -- Before threads of the exiting process are frozen. \n
   #QURT_PROCESS_DUMP_CB_ERROR  -- After threads are frozen and captured. \n
   #QURT_PROCESS_DUMP_CB_ROOT   -- After threads are frozen and captured, and CB_ERROR type of callbacks
                                   are called.
@param[in] priority Priority.

@return
#QURT_EOK -- Success \n
Other values -- Failure
 
@dependencies
None.
*/
int qurt_process_dump_register_cb(qurt_cb_data_t *cb_data, qurt_process_dump_cb_type_t type, unsigned short priority);

/**@ingroup func_qurt_process_dump_deregister_cb
Deregisters process domain dump callback.

@datatypes
#qurt_cb_data_t \n
#qurt_process_dump_cb_type_t

@param[in] cb_data Pointer to the callback information to deregister.
@param[in] type    Callback type.

@return
#QURT_EOK -- Success.\n
Other values -- Failure.
 
@dependencies
None.
*/
int qurt_process_dump_deregister_cb(qurt_cb_data_t *cb_data,qurt_process_dump_cb_type_t type);
/** @endcond */
/** @cond internal_only*/
/**@ingroup func_qurt_process_set_rtld_debug
Sets rtld_debug for a process. 
 
@param[in] pid     PID of the process for which rtld_debug must be set.
@param[in] address rtld_debug address.

@return
None 
 
@dependencies
None
*/
int qurt_process_set_rtld_debug(unsigned int pid,unsigned int address);

/**@ingroup func_qurt_process_get_rtld_debug
Gets rtld_debug for a process.

@param[in] pid         PID of the process for which rtld_debug must be set.
@param[in,out] address Pointer to the user passed address in which the rtld_debug address must be returned.

@return
None.

@dependencies
None.
*/
int qurt_process_get_rtld_debug(unsigned int pid,unsigned int *address);
/**@ingroup func_qurt_process_exit
Exit Current User Process with exit code
@param[in] exitcode
 
@return
QURT_EFATAL - No Client found with given PID value
QURT_EINVALID - Client was invalid
QURT_ENOTALLOWED - User does not have permission to perform this operation
QURT_EOK - Success
@dependencies
None
*/
int qurt_process_exit(int exitcode);

/**@ingroup func_qurt_process_kill
Kill the process represented by PID with exit code
@param[in] pid  of the process to be killed
@param[in] exitcode 
 
@return
QURT_EFATAL - No Client found with given PID value
QURT_EINVALID - Client was invalid
QURT_ENOTALLOWED - User does not have permission to perform this operation
QURT_EOK - Success
@dependencies
None
*/
int qurt_process_kill(int pid, int exitcode);
 
 
/**@ingroup func_qurt_debugger_register_process
Register the process indicated by PID with Debug Monitor
@param[in] pid  of the process
@param[in] address
 
@return
QURT_EOK if success
@dependencies
None
*/
int qurt_debugger_register_process(int pid, unsigned int adr);
 
 
/**@ingroup func_qurt_debugger_deregister_process
De-register the process indicated by PID with Debug Monitor
@param[in] pid  of the process
 
@return
QURT_EOK if success
@dependencies
None
*/
int qurt_debugger_deregister_process(int pid);
 
/**@ingroup func_qurt_process_exec_callback
Execute callbacks in the user process indicates by client_handle.
@param[in] client_handle Obtained from the current invocation function (Section 3.4.1).
@param[in] Callback function to be executed.
@param[in] stack adr to be used
@param[in] stack size
 
@return
QURT_EOK if success
@dependencies
None
*/
int qurt_process_exec_callback(int client_handle,
                                     unsigned callback_fn,
                                     unsigned stack_base,
                                     unsigned stack_size);
 
/**@ingroup func_qurt_process_get_pid
Get process ID of the process represented by client_handle
@param[in] client_handle Obtained from the current invocation function (Section 3.4.1).
@param[in] adr to store PID
 
@return
QURT_EOK if success
@dependencies
None
*/
int qurt_process_get_pid(int client_handle, int * pid);
#endif
