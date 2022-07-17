#ifndef QURT_FUTEX_H
#define QURT_FUTEX_H
/**
  @file  qurt_futex.h

  @brief  Prototypes of Qurt futex API functions      
  
 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013, 2020  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/

/*=====================================================================
 Functions
======================================================================*/


/**@ingroup func_qurt_futex_wait
  If a memory object address contains a value that is same as a specified 
  value, the caller thread is moved into waiting state. 

   @param[in]  lock  Pointer to the object memory. 
   @param[in]  val   value to check against the object content. 

   @return
   QURT_EOK   success
   Others     failure

   @dependencies
   None.
 */
int qurt_futex_wait(void *lock, int val);


/**@ingroup func_qurt_futex_wait_cancellable
  If a memory object address contains a value that is same as a specified 
  value, the caller thred is moved into waiting state. 
  The waiting state can be canceled by kernel when there is a special need. 

   @param[in]  lock  Pointer to the object memory. 
   @param[in]  val   value to check against the object content. 

   @return
   QURT_EOK   success
   Others     failure

   @dependencies
   None.
 */
int qurt_futex_wait_cancellable(void *lock, int val);


/**@ingroup func_qurt_futex_wake
  Wakes up a specified number of threads that have been waiting 
  for the object change with qurt_futex_wait().

   @param[in]  lock        Pointer to the object memory. 
   @param[in]  n_to_wake   The maximum number of threads to wake up.

   @return
   number of threads woken up by this function

   @dependencies
   None.
 */
int qurt_futex_wake(void *lock, int n_to_wake);

#endif /* QURT_FUTEX_H */

