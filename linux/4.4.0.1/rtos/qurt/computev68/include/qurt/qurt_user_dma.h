#ifndef QURT_USER_DMA_H
#define QURT_USER_DMA_H

/**
  @file qurt_user_dma.h
  @brief  Definitions, macros, and prototypes used for handling user dma

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2021  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/


/**@ingroup qurt_user_dma_dmsyncht
  send DMSyncht command to the user dma engine.
   
   This is to be called to ensure all posted DMA memory operations are
   complete. 
   
   Note that this will stall the current thread till the instruction
   is complete and returns.

  @return
  QURT_EOK - on dmsyncht completion
  QURT_ENOTSUPPORTED - in case of user dma not supported
  
  @dependencies
  None.
*/
int qurt_user_dma_dmsyncht(void);

#endif
