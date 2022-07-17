#ifndef QURT_POWER_H
#define QURT_POWER_H
/**
  @file qurt_power.h
  @brief  Prototypes of power API

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2018-2020  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
12/12/12   cm      (Tech Pubs) Edited/added Doxygen comments and markup.
=============================================================================*/

/** @cond */
/**@ingroup func_qurt_power_shutdown_fail_exit
  Returns from Power Collapse mode when power collapse cannot proceed.

  This function unmasks the global interrupt. This operation is used only when the thread is
  recovering from a failed power collapse operation (Section @xref{sec:powerShutdownEnter}).

  @return
  QURT_EOK -- Operation was successfully performed.

  @dependencies
  None.
 */
#define  qurt_power_shutdown_fail_exit qurt_power_exit

/**@ingroup func_qurt_power_shutdown_exit
  Undoes state changes made preparing for power collapse.\n
  This function unmasks the global interrupts.

  @return
  QURT_EOK --Operation was successfully performed.

  @dependencies
  None.
 */
#define qurt_power_shutdown_exit qurt_power_exit
/**@endcond */

/**@ingroup func_qurt_system_ipend_get
  Gets the IPEND register.\n

  @note1hang Returns the current value of the Hexagon processor IPEND register. The return value
             is a mask value which identifies the individual interrupts that are pending. \n

  @note2hang The bit order of the mask value is identical to the order defined for the IPEND register. A
             mask bit value of 1 indicates that the corresponding interrupt is pending, and 0 indicates that the
             corresponding interrupt is not pending. \n

  @return
  Return the IPEND register value.

  @dependencies
  None.
 */
unsigned int qurt_system_ipend_get (void);

/** @cond */
/*
   Sets the AVS configuration register.

   @param avscfg_value    AVS configuration value to be set.

   @return
   None.

   @dependencies
   None.
 */
void qurt_system_avscfg_set(unsigned int avscfg_value);

/*
   Clears the IPEND register.
   Gets the AVS configuration register.

   @return
   AVS configuration register value.

   @dependencies
   None.
 */
unsigned int qurt_system_avscfg_get(void);
/**@endcond */


/**@ingroup func_qurt_system_vid_get
  Gets the VID register. \n

  @note1hang Returns the current value of the Hexagon processor VID register. The return value is
             the vector number of a second-level interrupt that has been accepted by the Hexagon
             processor core.\n

  @return
  Return the VID register value which is the L2 VIC interrupt number accepted by the processor.
  Valid range is 0-1023.

  @dependencies
  None.
 */
unsigned int qurt_system_vid_get(void);

/**@ingroup func_qurt_power_shutdown_get_pcycles
   Gets the number of power collapses and processor cycles for entering and exiting most recent
   power collapse.

   @note1hang If no power collapse has occured yet, processor cycle numbers will be zero.

   @param[out] enter_pcycles  Number of processor cycles for entering most
                              recent power collapse.
   @param[out] exit_pcycles  Number of processor cycles for exiting most
                             recent power collapse.
   @return
   Zero -- No power collapses have occurred.
   Non-zero -- Number of power collapses that have occurred since
                the processor was reset

   @dependencies
   None.
 */
int qurt_power_shutdown_get_pcycles( unsigned long long *enter_pcycles,  unsigned long long *exit_pcycles );

/**@ingroup func_qurt_system_tcm_set_size
   Set size of TCM memory to be saved during full power collapse.

   @note1hang The size will be aligned to 32 bytes. If size passed is greater than maximum size defined in
              xml, the size will be truncated to size defined in xml.

   @param[in] new_size Size of TCM memory to be saved

   @return
   Zero -- Size was successfully set
   -1 -- Size of 0 was passed

   @dependencies
   None.
 */
int qurt_system_tcm_set_size(unsigned int new_size);

#endif /* QURT_POWER_H */
