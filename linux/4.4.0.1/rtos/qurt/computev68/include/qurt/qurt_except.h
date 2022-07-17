#ifndef QURT_EXCEPT_H
#define QURT_EXCEPT_H

/**
  @file qurt_except.h 
  Defines Cause and Cause2 codes for error-handling.  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2021 by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc..
 ======================================================================*/



/*
  QuRT supports error handling to handle CPU detected exceptions and software errors. 
  QuRT treats all errors as either fatal errors or nonfatal errors. 

  @section sec1 Fatal errors
  All supervisor mode exceptions are treated as fatal errors. 
  If a registered exception handler calls qurt_exit(), it is treated as a fatal error.
  Fatal errors result in saving the context of primary hardware thread to QURT_error_info and the rest of the thread contexts to the corresponding TCBs. 
  All hardware threads are eventually stopped and the cache is flushed.
  NMI exception is treated little differently from other fatal errors. QuRT saves the contexts of all the hardware threads into QURT_error_info.\n

  @subsection subsection1 Debugging fatal errors
  - QURT_error_info.status.status	 -- Indicates that an error occured.
  - QURT_error_info.status.cause	 -- Cause code for fatal error; Cause and Cause 2 details are listed below.
  - QURT_error_info.status.cause2	 -- Cause2 code for fatal error; Cause and Cause 2 details are listed below.
  - QURT_error_info.status.fatal	 -- Indicates whether a fatal error occurred. A user error can result in a fatal error if the exceptional handler is not registered.
  - QURT_error_info.status.hw_tnum -- Indicates the index of QURT_error_info.locregs[], where the context is saved when the error is fatal error.
  - QURT_error_info.global_regs    -- Contains the values of the global registers of Q6
  - QURT_error_info.local_regs[QURT_error_info.status.hw_tnum] -- Provides the CPU context when the error is a supervisor error.
    


  @subsection subsection2 Debugging nonfatal errors
  - QURT_error_info.user_errors                                    -- All user errors are logged here.
  - QURT_error_info.user_errors.counter                            -- Index to last logged error.
  - QURT_error_info.user_errors.entry[0...counter]	               -- Structure for logged error.
  - QURT_error_info.user_errors.entry[0...counter].error_tcb       -- TCB for the user error.
  - QURT_error_info.user_errors.entry[0...counter].error_tcb.error -- Information about the error; Cause, Cause2, Badva and hardware thread ID.
  - QURT_error_info.user_errors.entry[0...counter].error_code      -- ((cause2 << 8) 'Logical Or' (cause) ); Cause and Cause 2 details are listed below.
  - QURT_error_info.user_errors.entry[0...counter].hw_thread	   -- Hardware thread ID for error.
  - QURT_error_info.user_errors.entry[0...counter].pcycle	       -- Pcycle for error.

@note  
  Important usage note:
  Cause and Cause2 are error codes to distinguish multiple errors.
  SSR and BADAVA are inconclusive without the vector number.
  All cause and cause2 can range from 1 to 255 and every cause can have 1 to 255 error code.
  Hence the system can have up to 255 * 255 unique error codes.
  The cominations is representated as ((cause2 << 8) 'Logical OR' (cause) )
  Some Cause2 codes are statically defined, whereas some are obtaned from SSR[7:0] cause codes. It depends on cause codes.
  SSR cause codes are defined in Hexagon reference manual.
  All possible combinations are listed below.
*/
/** @addtogroup chapter_error
@{ */
/* cause - error type - 8-bits*/
#define QURT_EXCEPT_PRECISE             0x01 /**< A precise exception occurred. For this particular cause code, Cause2 is SSR[7:0].*/
#define QURT_EXCEPT_NMI                 0x02 /**< An NMI occurred; Cause2 is not defined. */
#define QURT_EXCEPT_TLBMISS             0x03 /**< TLBMISS RW occurred; for this particular cause code, Cause2 is SSR[7:0]. */
#define QURT_EXCEPT_RSVD_VECTOR         0x04 /**< Interrupt was raised on reserved vector, this must never happen. Cause2 is not defined. */
#define QURT_EXCEPT_ASSERT              0x05 /**< Kernel assert. Cause2 QURT_ABORT_* are listed below  */
#define QURT_EXCEPT_BADTRAP             0x06 /**< trap0(num) called with unsupported num. Cause2 is 0 */
#define QURT_EXCEPT_UNDEF_TRAP1         0x07 /**< Trap1 is not supported. Using Trap1 causes this error. Cause2 is not defined */
#define QURT_EXCEPT_EXIT                0x08 /**< Application called qurt_exit() (or called qurt_exception_raise_nonfatal()). Could be called from C library. Cause2 is "[Argument passed to qurt_exception_raise_nonfatal() & 0xFF]" */
#define QURT_EXCEPT_TLBMISS_X           0x0A /**< TLBMISS X (execution) occurred. Cause2 is not defined */
#define QURT_EXCEPT_STOPPED             0x0B /**< Running thread stopped due to Fatal error on other hardware thread. Cause2 is not defined */
#define QURT_EXCEPT_FATAL_EXIT          0x0C /**< Application called qurt_fatal_exit(). Cause2 is not defined */
#define QURT_EXCEPT_INVALID_INT         0x0D /**< Kernel received an invalid L1 interrupt. Cause2 is not defined */
#define QURT_EXCEPT_FLOATING_POINT      0x0E /**< Kernel received an floating point error. Cause2 is not defined  */
#define QURT_EXCEPT_DBG_SINGLE_STEP     0x0F /**< Cause2 is not defined */
#define QURT_EXCEPT_TLBMISS_RW_ISLAND   0x10 /**< RW miss in Island mode. Cause2 QURT_TLB_MISS_RW_MEM* are listed below */
#define QURT_EXCEPT_TLBMISS_X_ISLAND    0x11 /**< Execute miss in Island mode. For this particular cause code, Cause2 is SSR[7:0] */
#define QURT_EXCEPT_SYNTHETIC_FAULT     0x12 /**< Synthetic fault with user request that kernel detected. Cause2 QURT_SYNTH_* are listed below. */
#define QURT_EXCEPT_INVALID_ISLAND_TRAP 0x13 /**< Invalid trap in Island mode. Cause2 is trap number */
#define QURT_EXCEPT_UNDEF_TRAP0         0x14 /**< trap0(num) was called with unsupported num. Cause2 is trap number */
#define QURT_EXCEPT_PRECISE_DMA_ERROR   0x28 /**< Precise DMA error. Cause2 is DM4[15:8]. Badva is DM5 register */

#define QURT_ECODE_UPPER_LIBC         (0 << 16)  /**< Upper 16 bits is 0 for libc. */
#define QURT_ECODE_UPPER_QURT         (0 << 16)  /**< Upper 16 bits is 0 for QuRT. */
#define QURT_ECODE_UPPER_ERR_SERVICES (2 << 16)  /**< Upper 16 bits is 2 for error service. */
#define QURT_ECODE_ISLAND_INVALID_QDI  3         /**< Passing invalid QDI method in island */

/** @addtogroup chapter_error
@{ */
/* Cause2 for QURT_EXCEPT_SYNTHETIC_FAULT cause- 8bits */
#define  QURT_SYNTH_ERR                         0x01
#define  QURT_SYNTH_INVALID_OP                  0x02
#define  QURT_SYNTH_DATA_ALIGNMENT_FAULT        0x03
#define  QURT_SYNTH_FUTEX_INUSE                 0x04
#define  QURT_SYNTH_FUTEX_BOGUS                 0x05
#define  QURT_SYNTH_FUTEX_ISLAND                0x06
#define  QURT_SYNTH_FUTEX_DESTROYED             0x07
#define  QURT_SYNTH_PRIVILEGE_ERR               0x08

/* Cause2 - Abort cause reason - 8 bits */
/* ERR_ASSERT cause */
#define   QURT_ABORT_FUTEX_WAKE_MULTIPLE           0x01   /**<  futex_asm.s: Abort cause - futex wake multiple. */
#define   QURT_ABORT_WAIT_WAKEUP_SINGLE_MODE       0x02   /**<  power.c: Abort cause - Thread waiting to wake up in single threaded mode. */
#define   QURT_ABORT_TCXO_SHUTDOWN_NOEXIT          0x03   /**<  power.c : Abort cause - call TCXO shutdown without exit. */
#define   QURT_ABORT_FUTEX_ALLOC_QUEUE_FAIL        0x04   /**<  futex.c: Abort cause - futex allocation queue failure -  QURTK_futexhash_lifo empty. */
#define   QURT_ABORT_INVALID_CALL_QURTK_WARM_INIT  0x05   /**<  init_asm.S: Abort cause - invalid call QURTK_warm_init() in NONE CONFIG_POWER_MGMT mode. */
#define   QURT_ABORT_THREAD_SCHEDULE_SANITY        0x06   /**<  switch.S: Abort cause - sanity schedule thread is not supposed to run on the current hardware thread. */
#define   QURT_ABORT_REMAP                         0x07   /**<  Remap in the page table; the correct behavior must remove mapping if necessary. */
#define   QURT_ABORT_NOMAP                         0x08   /**<  No mapping in page table when removing a user mapping. */
#define   QURT_ABORT_OUT_OF_SPACES                 0x09
#define   QURT_ABORT_INVALID_MEM_MAPPING_TYPE      0x0A   /**<  Invalid memory mapping type when creating qmemory. */
#define   QURT_ABORT_NOPOOL                        0x0B   /**<  No pool available to attach. */
#define   QURT_ABORT_LIFO_REMOVE_NON_EXIST_ITEM    0x0C   /**<  Cannot allocate more futex waiting queue. */
#define   QURT_ABORT_ARG_ERROR                     0x0D
#define   QURT_ABORT_ASSERT                        0x0E   /**<  Assert abort. */
#define   QURT_ABORT_FATAL                         0x0F   /**<  FATAL error; must never happen. */
#define   QURT_ABORT_FUTEX_RESUME_INVALID_QUEUE    0x10   /**<  futex_asm.s: Abort cause - invalid queue ID in futex resume. */
#define   QURT_ABORT_FUTEX_WAIT_INVALID_QUEUE      0x11   /**<  futex_asm.s: Abort cause - invalid queue ID in futex wait. */
#define   QURT_ABORT_FUTEX_RESUME_INVALID_FUTEX    0x12   /**<  futex.c: Abort cause - invalid futex object in hashtable. */
#define   QURT_ABORT_NO_ERHNDLR                    0x13   /**<  No registered error handler. */
#define   QURT_ABORT_ERR_REAPER                    0x14   /**<  Exception in reaper thread itself. */
#define   QURT_ABORT_FREEZE_UNKNOWN_CAUSE          0x15   /**<  Abort in thread freeze operation. */
#define   QURT_ABORT_FUTEX_WAIT_WRITE_FAILURE      0x16   /**<  During futex wait processing, could not perform a necessary write operation to userland data; most likely due to a DLPager eviction. */
#define   QURT_ABORT_ERR_ISLAND_EXP_HANDLER        0x17   /**<  Exception in Island exception handler task. */
#define   QURT_ABORT_L2_TAG_DATA_CHECK_FAIL        0x18   /**<  Detected error in L2 tag/data during warm boot. The L2 tag/data check is done when CONFIG_DEBUG_L2_POWER_COLLAPSE is enabled */
#define   QURT_ABORT_ERR_SECURE_PROCESS            0x19   /**<  Abort error in secure process. */
#define   QURT_ABORT_ERR_EXP_HANDLER               0x20   /**<  Either no exception handler or handler itself caused an exception. */
#define   QURT_ABORT_ERR_NO_PCB                    0x21   /**<  PCB of the thread context failed initialization, PCB was NULL. */
#define   QURT_ABORT_NO_PHYS_ADDR                  0x22   /**<  Unable to find Physical Address for Virtual Address */
#define   QURT_ABORT_OUT_OF_FASTINT_CONTEXTS       0x23   /**<  Fast Interrupt contexts exhausted */
#define   QURT_ABORT_CLADE_ERR                     0x24   /**<  Fatal Error seen with Clade interrupt */
#define   QURT_ABORT_ETM_ERR                       0x25   /**<  Fatal Error seen with ETM interrupt */
#define   QURT_ABORT_ECC_DED_ASSERT                0x26   /**<  ECC 2 bit DED error */
#define   QURT_ABORT_VTLB_ERR                      0x27   /**<  Fatal error in VTLB layer */

/* Cause2 - TLB-miss_X - 8bits */
#define  QURT_TLB_MISS_X_FETCH_PC_PAGE             0x60
#define  QURT_TLB_MISS_X_2ND_PAGE                  0x61
#define  QURT_TLB_MISS_X_ICINVA                    0x62

/* Cause2 - TLB-miss_RW - 8bits */
#define  QURT_TLB_MISS_RW_MEM_READ                 0x70
#define  QURT_TLB_MISS_RW_MEM_WRITE                0x71

/** @cond  */
/* Cause2 - Floating point exception - 8 bits */
#define  QURT_FLOATING_POINT_EXEC_ERR              0xBF    /**<  Excecute floating-point */
/** @endcond */

/* Cause2 - autostackv2 - 8bits */
#define  QURT_AUTOSTACKV2_CANARY_NOT_MATCH         0xC1
#define  QURT_AUTOSTACKV2_POOL_IDX_OFF_RANGE       0xC2

/** @cond rest_reg_dist*/
/* Enable floating point exceptions */
#define QURT_FP_EXCEPTION_ALL        0x1F << 25 /**< */
#define QURT_FP_EXCEPTION_INEXACT    0x1 << 29 /**< */
#define QURT_FP_EXCEPTION_UNDERFLOW  0x1 << 28 /**< */
#define QURT_FP_EXCEPTION_OVERFLOW   0x1 << 27 /**< */
#define QURT_FP_EXCEPTION_DIVIDE0    0x1 << 26 /**< */
#define QURT_FP_EXCEPTION_INVALID    0x1 << 25 /**< */

/** @endcond */
/** @} */ /* end_addtogroup chapter_error */

#endif /* QURT_EXCEPT_H */
