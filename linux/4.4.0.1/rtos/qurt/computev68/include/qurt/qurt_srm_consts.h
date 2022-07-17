#ifndef QURT_SRM_CONSTS_H
#define QURT_SRM_CONSTS_H
/**
  @file qurt_srm_consts.h 

  @brief  Type definitions for srm

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2020  by Qualcomm Technologies, Inc.  All Rights Reserved
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/** @cond */
#define QURT_SRM_WAKEUP_REQUEST       1 << 0          /* Value = 1:  Send wakeup request to SRM server */
#define QURT_SRM_SET_HANDLE           1 << 1          /* Value = 2:  Set the client handle for new SRM client */
#define QURT_SRM_ALLOC_KERNEL_PAGES   1 << 2          /* Value = 4:  Allocate pages from kernel VA space */
/** @endcond */

#endif /* QURT_SRM_CONSTS_H */
