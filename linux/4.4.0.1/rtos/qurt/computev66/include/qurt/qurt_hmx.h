#ifndef QURT_HMX_H
#define QURT_HMX_H
/**
  @file qurt_hmx.h 
  @brief   Prototypes of Qurt HMX API.  

Copyright (c) 2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/


/*=============================================================================
                        TYPEDEFS
=============================================================================*/


/* HMX locking status */


/*=============================================================================
                        FUNCTIONS
=============================================================================*/

/**@ingroup func_qurt_hmx_lock
  Lock a HMX unit.
  If a HMX unit is available, this function locks the unit and returns right away;
  if there is no HMX unit available, the caller is blocked until a HMX is available 
  and gets locked by the function.
  
  @param[in]
  None.

  @param[out] 
  None.

  @return
  QURT_EOK - HMX locked successful
  Others - Failure (no HMX hardware supported)

  @dependencies
  None.
  
 */
int qurt_hmx_lock(void);


/**@ingroup func_qurt_hmx_unlock
  Unlock a HMX unit.
  If there is a HMX unit locked by the caller thread, unlock the HMX unit and clear the 
  accumulators(assuming fixed point type). 
  If there is no HMX unit locked by the caller thread, return failure. 
  
  @param[in]
  None.

  @param[out] 
  None.

  @return
  QURT_EOK - HMX unlocked successful
  Others - Failure 

  @dependencies
  None.
  
 */
int qurt_hmx_unlock(void);


/**@ingroup func_qurt_hmx_try_lock
  Lock a HMX unit.
  If a HMX unit is available, this function locks the unit and returns right away;
  if there is no HMX unit available, the function returns failure without blocking the caller.
  
  @param[in]
  None.

  @param[out] 
  None.

  @return
  QURT_EOK - HMX locked successful
  Others - Failure (no HMX available)

  @dependencies
  None.
  
 */
int qurt_hmx_try_lock(void);


#endif /* QURT_HMX_H */

