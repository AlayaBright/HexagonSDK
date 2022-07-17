#ifndef QURT_ISR_FRAME_H
#define QURT_ISR_FRAME_H

/*=====================================================================
 
  @file  qurt_isr_frame.h

  @brief  Prototypes of Qurt ISR Frame functions.
          All the APIs in this file are deprecated.
          Alternative APIs are available in qurt_isr.h

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2021 by Qualcomm Technologies, Inc.  All Rights Reserved.
 ======================================================================*/

#include <string.h>
#include <qurt_thread.h>

/*=====================================================================
 Constants and macros
======================================================================*/

#define QURT_ISR_ATTR_FRAME_NON_SHAREABLE       0
#define QURT_ISR_ATTR_FRAME_SHAREABLE           1
#define QURT_ISR_ATTR_FRAME_SHAREABLE_DEFAULT   QURT_ISR_ATTR_FRAME_SHAREABLE
#define QURT_ISR_ATTR_TCB_PARTITION_RAM         0
#define QURT_ISR_ATTR_TCB_PARTITION_TCM         1 
#define QURT_ISR_ATTR_TCB_PARTITION_DEFAULT     QURT_ISR_ATTR_TCB_PARTITION_RAM
#define QURT_ISR_ATTR_NON_DELAYED_ACK           0
#define QURT_ISR_ATTR_DELAYED_ACK               1
#define QURT_ISR_ATTR_ACK_DEFAULT               QURT_ISR_ATTR_NON_DELAYED_ACK
#define QURT_ISR_ATTR_STID_DEFAULT              0
#define QURT_ISR_ATTR_DRV_DEFAULT               0
#define QURT_ISR_ATTR_TIMETEST_ID_DEFAULT       (-2) 
#define QURT_ISR_ATTR_PRIORITY_DEFAULT_MAGIC    0x1ff
#define QURT_ISR_ATTR_BUS_PRIO_DISABLED         0
#define QURT_ISR_ATTR_BUS_PRIO_ENABLED          1 
#define QURT_ISR_ATTR_BUS_PRIO_DEFAULT          255
#define QURT_ISR_ATTR_DEFAULT_STACK_SIZE        2048
#define QURT_ISR_ATTR_NAME_MAXLEN               16

// Qurt ISR interrupt type

/* Trigger type bit fields for a PDC interrupt:
 *  Polarity  Edge  Output
 *  0         00  Level Sensitive Active Low
 *  0         01  Rising Edge Sensitive
 *  0         10  Falling Edge Sensitive
 *  0         11  Dual Edge Sensitive
 *  1         00  Level Sensitive Active High
 *  1         01  Falling Edge Sensitive
 *  1         10  Rising Edge Sensitive
 *  1         11  Dual Edge Sensitive */

#define QURT_ISR_PDC_TRIGGER_TYPE_SET(pol, edge)   (((pol & 0x01) << 2) | (edge & 0x03))

#define QURT_ISR_ATTR_INT_TRIGGER_LEVEL_LOW     QURT_ISR_PDC_TRIGGER_TYPE_SET(0, 0x00) 
#define QURT_ISR_ATTR_INT_TRIGGER_LEVEL_HIGH    QURT_ISR_PDC_TRIGGER_TYPE_SET(1, 0x00) 
#define QURT_ISR_ATTR_INT_TRIGGER_RISING_EDGE   QURT_ISR_PDC_TRIGGER_TYPE_SET(1, 0x02)  
#define QURT_ISR_ATTR_INT_TRIGGER_FALLING_EDGE  QURT_ISR_PDC_TRIGGER_TYPE_SET(0, 0x02)  
#define QURT_ISR_ATTR_INT_TRIGGER_DUAL_EDGE     QURT_ISR_PDC_TRIGGER_TYPE_SET(0, 0x03)  
#define QURT_ISR_ATTR_INT_TRIGGER_USE_DEFAULT   0xff 


// Qurt ISR attribute
#define QURT_ISR_ATTR_NULL                      0      
#define QURT_ISR_NOBLKING_MAGIC                 1      // for legacy fastint

#if defined(__clang__)
  #define ISR_FRAME_WARN_DEPRECATED(x)        __attribute__((deprecated("ISR Frame APIs are deprecated, alternative APIs are available in qurt_isr.h",x)))
#else
  #define ISR_FRAME_WARN_DEPRECATED(x)
#endif

/*=============================================================================
                                                TYPEDEFS
=============================================================================*/

#define qurt_isr_frame_attr_t qurt_thread_attr_t


/** ISR callback attributes */
typedef struct _qurt_isr_cb_attr {
    char           name[QURT_ISR_ATTR_NAME_MAXLEN];   /**< ISR frame/group name. */
    unsigned short priority;                          /**< ISR priority. */
    unsigned char  ack_type;                          /**< ISR ack type. */
    unsigned char  interrupt_type;                    /**< ISR interrupt type. */
    unsigned int   stack_size;                        /**< ISR stack size, only useful when frame name=NULL */
} qurt_isr_cb_attr_t;


#define qurt_isr_frame_attr_init qurt_thread_attr_init


/**@ingroup func_qurt_isr_cb_attr_init
  Initializes the structure used to set the ISR callback attributes when a Qurt ISR callback 
  is registered.  After an attribute structure is initialized, the individual attributes in 
  the structure can be explicitly set using the ISR attribute APIs.

  The default attribute values set by the initialize operation are the following: \n
  - priority       -- #QURT_ISR_ATTR_PRIORITY_DEFAULT_MAGIC \n
  - frameshareable -- #QURT_ISR_ATTR_FRAME_SHARABLE_DEFAULT
  - stack_size     -- 0  meaning no stack is provided by caller
  - ack_type       -- QURT_ISR_ATTR_ACK_DEFAULT
  - drv_number     -- QURT_ISR_ATTR_DRV_DEFAULT 
  - interrupt_type -- QURT_ISR_ATTR_INT_TRIGGER_USE_DEFAULT
  - name           -- 0  meaning no frame name assigned

  @datatypes
  #qurt_isr_attr_t
  
  @param[in,out] attr Pointer to the ISR attribute structure.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_isr_cb_attr_init (qurt_isr_cb_attr_t *attr)
{
    attr->priority        = QURT_ISR_ATTR_PRIORITY_DEFAULT_MAGIC;
    attr->ack_type        = QURT_ISR_ATTR_ACK_DEFAULT;
    attr->name[0]         = 0;    // meaning no frame name assigned
    attr->interrupt_type  = QURT_ISR_ATTR_INT_TRIGGER_USE_DEFAULT;
    attr->stack_size      = 0;    // meaning no stack is provided by caller
}


#define qurt_isr_frame_attr_set_stack_addr qurt_thread_attr_set_stack_addr
#define qurt_isr_frame_attr_set_stack_size qurt_thread_attr_set_stack_size 
#define qurt_isr_frame_attr_set_tcb_partition qurt_thread_attr_set_tcb_partition


/**@ingroup func_qurt_isr_cb_attr_set_priority
  Sets the priority to ISR callback.
  The priorities are specified as numeric values in the range 1 to 255, with 1 representing
  the highest priority.

  @datatypes
  #qurt_isr_cb_attr_t

  @param[in,out] attr  - Pointer to the ISR attribute structure.
  @param[in] priority  - Priority of the ISR callback.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_isr_cb_attr_set_priority (qurt_isr_cb_attr_t *attr, unsigned short priority)
{
    attr->priority = priority;
}


#define qurt_isr_cb_attr_set_stack_size


/**@ingroup func_qurt_isr_cb_attr_set_group
   Sets the name of the ISR frame that this ISR callback wants to join and use the ISR frame for processing the callback. 
   The ISR frame needs to be created beforehand. 

   @datatypes
   #qurt_isr_cb_attr_t

   @param[in] isr attribute structure.

   @param[in] frame_name. Name of the ISR frame that the callback want to use/join.

   @return
   None

   @dependencies
   None.
*/
static inline void qurt_isr_cb_attr_set_group (qurt_isr_cb_attr_t *attr, char *frame_name)
{
    strlcpy (attr->name, frame_name, QURT_ISR_ATTR_NAME_MAXLEN);
    attr->name[QURT_ISR_ATTR_NAME_MAXLEN - 1] = 0;
}


/**@ingroup func_qurt_isr_cb_attr_set_interrupt_type
   Sets interrupt type on the interrupt used for the ISR callback. 
   If there is no special setting on the interrupt type, by default, Qurt ISR 
   uses the interrupt type information defined in Qurt XML file.

   @datatypes
   #qurt_isr_cb_attr_t

   @param[in] isr attribute structure.

   @param[in] interrupt_type.  Values: \n 
         - QURT_ISR_ATTR_INT_TRIGGER_USE_DEFAULT
         - QURT_ISR_ATTR_INT_TRIGGER_LEVEL_HIGH 
         - QURT_ISR_ATTR_INT_TRIGGER_LEVEL_LOW 
         - QURT_ISR_ATTR_INT_TRIGGER_RISING_EDGE 
         - QURT_ISR_ATTR_INT_TRIGGER_FALLING_EDGE

   @return
   None

   @dependencies
   None.
*/
static inline void qurt_isr_cb_attr_set_interrupt_type (qurt_isr_cb_attr_t *attr, unsigned char interrupt_type)
{
    attr->interrupt_type = interrupt_type;
}


/**@ingroup func_qurt_isr_cb_attr_set_ack
   Sets ISR attribute for interrupt acknowledgement type.
   Setting to delayed interrupt acknowledgement disables Qurt kernel from re-enabling the interrupt
   after the ISR callback returns. The client needs to explicitly re-enable the interrupt afterwards.

   @datatypes
   #qurt_isr_cb_attr_t

   @param[in] isr attribute structure.

   @param[in] ack_type.  Values: \n 
         - #QURT_ISR_ATTR_NON_DELAYED_ACK  
         - #QURT_ISR_ATTR_DELAYED_ACK        

   @return
   None

   @dependencies
   None.
*/
static inline void qurt_isr_cb_attr_set_ack (qurt_isr_cb_attr_t *attr, unsigned char ack_type)
{
    if(ack_type <= QURT_ISR_ATTR_DELAYED_ACK) {
        attr->ack_type = ack_type;
    }
}


/**@ingroup func_qurt_isr_frame_create
  Creates a Qurt ISR frame with the specified frame attributes and the frame name.

  @datatypes
   qurt_isr_frame_attr_t
  
  @param[in]   attr           Pointer to the initialized ISR frame attribute structure
  @param[in]   frame_name     name of the ISR frame to be created
   
  @return 
   QURT_EOK     -- Qurt ISR frame created successfully. \n
   QURT_EFAILED -- Failure. 

  @deprecated
  Use qurt_isr_create to create ISR thread

  @dependencies
   None.
 */

int qurt_isr_frame_create(qurt_isr_frame_attr_t *pAttr, char *frame_name) ISR_FRAME_WARN_DEPRECATED("Migrate to qurt_isr_create for creating ISR thread");


/**@ingroup func_qurt_isr_frame_delete
  Deletes a Qurt ISR frame with its frame name 
  The ISR frame thread is deleted when function returns success.

  @param[in]   frame_name     name of the ISR frame to be deleted
   
  @return 
  #QURT_EOK     -- Qurt ISR frame deleted successfully. \n
  #QURT_EFAILED -- Failure. 

  @deprecated
  Use qurt_isr_delete to delete ISR thread

  @dependencies
  None.
 */

int qurt_isr_frame_delete(char *frame_name) ISR_FRAME_WARN_DEPRECATED("Migrate to qurt_isr_delete for deleting ISR thread");


/**@ingroup func_qurt_isr_callback_register
  Registers a Qurt ISR callback with the specified attributes.
  The interrupt is enabled when this function returns success.

  @datatypes
   qurt_isr_cb_attr_t
  
  @param[in]   attr         Pointer to the initialized ISR callback attribute structure. 
                            The pointer can be set as zero (NULL) when the client wants to rely on Qurt to use 
                            the default ISR setting to create a non-shareable ISR frame for the ISR callback.
  @param[in]   int_num      ISR interrupt number.
  @param[in]   callback     ISR callback function. 
                            There are 2 arguments passed in to the callback function when it is called. The first 
                            one is the *arg, and the second one is the int_num. 
  @param[in]   arg          Pointer to a ISR-specific argument structure.
   
  @return 
   QURT_EOK     -- Qurt ISR callback registered successfully. \n
   QURT_EFAILED -- Failure. 

  @deprecated
  Use qurt_isr_register2 to register ISR callback

  @dependencies
   None.
 */

int qurt_isr_callback_register(qurt_isr_cb_attr_t *pAttr, int int_num, void (*callback) (void *, int), void *arg) \
                        ISR_FRAME_WARN_DEPRECATED("Migrate to qurt_isr_register2 for registering ISR callback");


/**@ingroup func_qurt_isr_callback_deregister
  De-registers the ISR callback registered for the specified interrupt.
  The interrupt is disabled when this function returns success.

  @param[in]   int_num      interrupt number for the ISR callback.
   
  @return 
  #QURT_EOK     -- ISR callback deregistered successfully. \n
  #QURT_EFAILED -- Failure. 

  @deprecated
  Use qurt_isr_deregister2 to de-register ISR callback

  @dependencies
  None.
 */

int qurt_isr_callback_deregister(int int_num) ISR_FRAME_WARN_DEPRECATED("Migrate to qurt_isr_deregister2 for de-registering ISR callback");

#endif /* QURT_ISR_FRAME_H */
