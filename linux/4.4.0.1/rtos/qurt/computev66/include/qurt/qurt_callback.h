#ifndef QURT_CALLBACK_H
#define QURT_CALLBACK_H

/**
  @file qurt_callback.h
    Definitions, macros, and prototypes for qurt callback frameowrk.

  qurt callback framework provides a safe mechanism for root process drivers 
  to execute callback functions in a user process. The framework hosts 
  dedicated worker threads in corresponding processes that handle the execution
  of the callback fuction. This ensures that the callbacks occur in context of
  the appropriate process thread, in result maintaining privilege boundaries. 

  Prerequisites for use of this framework are:
  1. Driver is a QDI driver and client communicates with drivers using QDI 
     invocations.
  2. Callback configuration is specified in cust_config.xml for the user process 
     that intends to use this framework.

  qurt_cb_data_t is the public data structure that allows client to store all
  the required information about the callback, including the callback function
  and the arguments to be passed to this function when it executes.
  The client uses QDI interface to register this structure with root driver.
  
  Callback framework provides two APIs that a root driver can use to invoke callback.
  These functions are described in qurt_qdi.h header file.

  qurt_qdi_callback_invoke_async() invokes asynchronous callback wherein the
  invoking driver does not wait for the callback to finish executing.

  qurt_qdi_callback_invoke_sync()  invokes synchronous callback. Upon invocation
  the invoking thread gets suspended till the callback function finishes execution.
  
     
 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2021  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/

#include "qurt_qdi.h"
#include "qurt_qdi_constants.h"
#include "qurt_qdi_imacros.h"
#include "qurt_mutex.h"

/** Callback framework error codes.*/
typedef enum{
  QURT_CB_ERROR=-1,               /* CB registration failed.*/
  QURT_CB_OK=0,                   /* Success.*/
  QURT_CB_MALLOC_FAILED=-2,       /* QuRTOS malloc failure.*/
  QURT_CB_WAIT_CANCEL=-3,         /* Process exit cancelled wait operation.*/
  QURT_CB_CONFIG_NOT_FOUND=-4,    /* CB configuration for process was not found.*/
  QURT_CB_QUEUE_FULL=-5,          /* CB queue is serving at maximum capacity.*/
}qurt_cb_result_t;

/** Callback registration data structure*/
typedef struct {
  /** @cond */
  void* cb_func;             /* Pointer to callback function. */
  unsigned cb_arg;           /* Not interpreted by the framework.*/
  /** @endcond */
} qurt_cb_data_t;

/** @cond */
/*These defines are used as default, if cust_config does not specify them*/
#define CALLBACK_WORKER_STACK_SIZE 0x2000

/*Callback driver's private methods*/
#define QDI_CALLBACK_CBDINFO_GET          (QDI_PRIVATE + 1)
#define QDI_CALLBACK_CBDATA_GET           (QDI_PRIVATE + 2)
#define QDI_CALLBACK_WORKER_REGISTER      (QDI_PRIVATE + 3)
/** @endcond */

/**@ingroup func_qurt_cb_data_init 
  Initializes the callback data structure.
  Call this function by the entity that registers callback with the root process driver.

  @datatypes 
  #qurt_cb_data_t

  @param[in]  cb_data         Pointer to callback data structure.

  @return
  None.

  @dependencies
  None.
 */
static inline void qurt_cb_data_init (qurt_cb_data_t* cb_data){
    cb_data->cb_func = NULL;
    cb_data->cb_arg = 0;
}

/**@ingroup func_qurt_cb_data_set_cbfunc
  Sets up the callback function.
  Call this function by the entity that registers the callback with the root process driver.
  This sets up the callback function that executes when the callback is executed.
  
  @datatypes 
  #qurt_cb_data_t

  @param[in]  cb_data         Pointer to callback data structure.
  @param[in] cb_func         Pointer to callback function.

  @return
  None.

  @dependencies
  None.
 */
static inline void qurt_cb_data_set_cbfunc (qurt_cb_data_t* cb_data, void* cb_func){
  cb_data->cb_func = cb_func;
}

/**@ingroup func_qurt_cb_data_set_cbarg
  Sets up the callback argument.
  Call this function by the entity that registers the callback with the root process driver.
  This sets up the argument passed to the callback function when executing the callback.
  
  @datatypes 
  #qurt_cb_data_t

  @param[in]  cb_data         Pointer to callback data structure.
  @param[in] cb_arg          Argument for the callback function.

  @return
  None.

  @dependencies
  None.
 */
static inline void qurt_cb_data_set_cbarg (qurt_cb_data_t* cb_data, unsigned cb_arg){
  cb_data->cb_arg = cb_arg;
}

#endif