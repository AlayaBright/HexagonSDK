#ifndef QURT_PIMUTEX_H
#define QURT_PIMUTEX_H 1
/**
  @file qurt_pimutex.h   
  Prototypes of qurt_pimutex API.  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2021  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/


#include <qurt_futex.h>
#include <qurt_mutex.h>

/*=============================================================================
                        FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_pimutex_init
  Initializes a priority inheritance mutex object.
  The priority inheritance mutex is initially unlocked.

  This function works the same as qurt_mutex_init().

   @note1hang Each pimutex-based object has one or more kernel resources associated with it;
              to prevent resource leaks, call qurt_pimutex_destroy()
              when this object is not used anymore

  @datatypes
  #qurt_mutex_t
  
  @param[out]  lock  Pointer to the priority inheritance mutex object.

  @return
  None.

  @dependencies
  None.
 
 */
void qurt_pimutex_init(qurt_mutex_t *lock);

/**@ingroup func_qurt_pimutex_destroy
   Destroys the specified priority inheritance mutex.  

   @note1hang Priority inheritance mutexes must be destroyed when they are no longer in
              use. Failure to do this causes resource leaks in the QuRT kernel.\n
   @note1cont Priority inheritance mutexes must not be destroyed while they are still in use.
              If this happens, the behavior of QuRT is undefined.
  
   @datatypes
   #qurt_mutex_t
   
   @param[in]  lock  Pointer to the priority inheritance mutex object to destroy.

   @return
   None.

   @dependencies
   None.
  
 */
void qurt_pimutex_destroy(qurt_mutex_t *lock);

/**@ingroup func_qurt_pimutex_lock
  Requests access to a shared resources. If a thread performs a lock operation on a mutex 
  that is not in use, the thread gains access to the shared resource that the mutex protects, 
  and continues executing.
 
  If a thread performs a lock operation on a mutex that is already being used by another
  thread, the thread is suspended. When the mutex becomes available again (because the 
  other thread has unlocked it), the thread is awakened and given access to the shared resource.

  If a thread is suspended on a priority inheritance mutex, and the priority of the suspended
  thread is higher than the priority of the thread that has locked the mutex, the thread
  with the mutex acquires the higher priority of the suspended thread. The locker thread blocks
  until the lock is available.
 
  @note1hang  A thread is not suspended if it locks a priority inheritance mutex that it has 
              already locked by itself. However, the mutex does not become available to other 
			  threads until the thread performs a balanced number of unlocks on the mutex.\n
  @note1cont  When multiple threads are competing for a mutex, the lock operation for a priority
              inheritance mutex is slower than it is for a recursive mutex. 
			  In particular, it is about 10 times slower when the mutex is available for locking,
			  and slower (with greatly varying times) when the mutex is already locked.

  @datatypes
  #qurt_mutex_t
  
  @param[in]  lock  Pointer to the priority inheritance mutex object to lock.

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_pimutex_lock(qurt_mutex_t *lock);

/**@ingroup func_qurt_pimutex_unlock
   Releases access to a shared resource; unlocks the specified priority inheritance mutex.  \n
   More than one thread can be suspended on a priority inheritance mutex. When the mutex
   is unlocked, only the highest-priority thread waiting on the mutex is awakened. If the
   awakened thread has higher priority than the current thread, a context switch occurs.

   When a thread unlocks a priority inheritance mutex, its thread priority is restored to its
   original value from any higher priority value that it acquired from another thread
   suspended on the mutex.
  
   @datatypes
   #qurt_mutex_t
   
   @param[in]  lock  Pointer to the priority inheritance mutex object to unlock.

   @return 
   None.

   @dependencies
   None.
 
 */
void qurt_pimutex_unlock(qurt_mutex_t *lock);

/**@ingroup func_qurt_pimutex_try_lock
  Request access to a shared resource (without suspend). Attempts to lock the specified priority inheritance mutex.\n
  If a thread performs a try_lock operation on a priority inheritance mutex that is not in
  use, the thread gains access to the shared resource that is protected by the mutex, and
  continues executing.
  If a thread performs a try_lock operation on a priority inheritance mutex that is already
  being used by another thread, qurt_pimutex_try_lock immediately returns with a
  nonzero result value.
  
  @datatypes
  #qurt_mutex_t
  
  @param[in]  lock  Pointer to the priority inheritance mutex object to lock.

  @return
  0 -- Success. \n
  Nonzero -- Failure. 

  @dependencies
  None. 
 */
int qurt_pimutex_try_lock(qurt_mutex_t *lock);

#endif /* QURT_PIMUTEX_H */