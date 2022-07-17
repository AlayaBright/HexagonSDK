#ifndef QURT_SPACE_H
#define QURT_SPACE_H
/**
  @file qurt_space.h
  @brief Prototypes of QuRT process control APIs

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/

#include <qurt_signal.h>

/** This flag is a request to the OS to suspend the processes just before calling main() */
#define SPAWNN_FLAG_SUSPEND_ON_STARTUP 0x1

/**
 * The qurt_spawn_args() function create and start a process from ELF file of given name. The slash symbols 
 * "/" or "\" will be ignored. Please do not include directory name in input. Additionally this function
 * accepts the the SPAWN flags. Multiple SPAWN flags can be specified by OR'ing the the flags.
 *
 * @param name      ELF file name of the executable. Name shall not contain directories, e.g.
 *                  use "dsp2.elf", instead of "/prj/qct/.../dsp2.elf"
 *
 * @param return    Process ID if operation successful.
 *                  negative error code if failed:
                      #-QURT_EPRIVILEGE                     Caller does not have enough privilege for this operation.
                      #-QURT_EMEM                           Not enough memory to perform the operation.
                      #-QURT_EFAILED                        Operation failed.
                      #-QURT_ENOTALLOWED                    Operation not allowed.
                      #-QURT_ENOREGISTERED                  Not Registered.   
                      #-QURT_ENORESOURCE                    Resource exhaustion  
                      #-QURT_EINVALID                       Invalid argument value
                     
 *
*/

int qurt_spawn_flags(const char * name, int flags);

/**
   The qurt_spawn() function creates and starts a process from an ELF file of the specified name. The slash symbols 
   "/" or "\" will be ignored. Do not include the directory name in the input. 
  
   @param name      ELF file name of the executable. Name shall not contain directories, e.g.
                    use "dsp2.elf", instead of "/prj/qct/.../dsp2.elf"
  
   @return
   Address space ID (0-32) -- Success. \m
   Negative error code -- Failure.
  
*/
#define qurt_spawn(name) qurt_spawn_flags(name,0)

/**
 * The qurt_getpid() function returns process ID (address space ID) of the current process.
 *
 * @return    address space ID (0-32) 
 *
*/
#define qurt_getpid qurt_process_get_id

/**
 * The qurt_space_switch() function context switches to the process of given process ID. This is a priviledged
 * API. It only works if the caller is from Guest OS.
 *
 * @param asid      [0..31] process ID (address space ID) of destination process space
 *
 * @return    address space ID of previous process if operation successful. 
 *                  negative error code if failed
 *
*/
int qurt_space_switch(int asid);

/**
 * The qurt_wait() function  waits for status change in a child process. It could be used by parent
 * process to block on any child process terminates.
 *
 * @param status    Pointer to status variable. The variable provides the status value of child process. 
 *                  The value comes from exit() system call made by child process.
 *
 * @return    address space ID of child process that changing status if operation successful. 
 *                  negative error code if failed
 *
*/

int qurt_wait(int *status);


/** @cond */
/* APIs that allow registering callbacks on spawn of user pd */
typedef void (*QURT_SPAWN_PFN)(int, void *);  //no return, since we won't be error checking it in spawn 
/** @endcond */


/** @cond internal_only */

/**@ingroup func_qurt_event_register
The qurt_event_register() will set the specified bits by mask in the signal passed by the caller. The signal gets set
when the client handle indicated by value goes away (at process exit). Multiple clients can register for the signal
to be set.

@datatypes

@param[in] type    QURT_PROCESS_EXIT is the only event that can be registered for. 
           value   Indicates the client handle of the process that the event is being registered for.
           signal  Pointer to the signal object that needs to be set when the event occurs
           mask    Mask bits which need to be set in the signal
           data    pointer to the variable which would receive the exitcode of the exiting process
		   datasize size of the data variable. 

@return   QURT_EOK  successful execution
          QURT_EMEM Not enough memory to allocate resources
          QURT_EVAL Invalid values being passed to the api
		  
None
@dependencies
None
*/
int qurt_event_register(int type, int value, qurt_signal_t *signal, unsigned int mask, void *data, unsigned int data_size);

/**@ingroup func_qurt_callback_register_onspawn
The qurt_callback_register_onspawn() will allow registering for a callback on spawn of any user process.

@datatypes

@param[in] pFn         Callback function to be called when any user process is spawned
           user_data   Pointer to the argument that the callback needs to be called with


@return   handle to be used while deregistering the callback. Mutliple clients can register for callback
          on spawn and some clients might choose to deregister.
		  
None
@dependencies
None
*/
int qurt_callback_register_onspawn(QURT_SPAWN_PFN pFn, void *user_data);

/**@ingroup func_qurt_callback_deregister_onspawn
qurt_callback_deregister_onspawn() will allow de-registering callback on spawn.

@datatypes

@param[in] callback_handle   handle which was returned by qurt_callback_register_onspawn
           
@return   QURT_EOK if de-registering was successful
		  
None
@dependencies
None
*/
int qurt_callback_deregister_onspawn(int callback_handle);
/** @endcond */

#endif /* QURT_SPACE_H */
