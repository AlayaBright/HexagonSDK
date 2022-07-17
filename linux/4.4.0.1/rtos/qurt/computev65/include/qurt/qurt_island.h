#ifndef QURT_ISLAND_H
#define QURT_ISLAND_H

/**
  @file qurt_island.h
  @brief  Prototypes of power API
          The APIs allow entering and exiting island mode where the memory
          accesses are limited to local memory.

  EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2018-2020  by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/
#include <qurt_thread.h>
#include <qurt_memory.h>
#include <qurt_alloc.h>
#include <qurt_error.h>

#define QURT_ISLAND_ATTR_MEMORY_TYPE_L2LINELOCK 0
#define QURT_ISLAND_ATTR_MEMORY_TYPE_ONCHIP 1

//#define ISLAND_TLB_MGR_DEBUG
#ifdef ISLAND_TLB_MGR_DEBUG
	#define ISLAND_TLB_MGR_PRINT qurtos_printf
#else
	#define ISLAND_TLB_MGR_PRINT(...) ((void)0)
#endif

/** @cond */
enum qurt_island_attr_resource_type {
    QURT_ISLAND_ATTR_INVALID,
    QURT_ISLAND_ATTR_END_OF_LIST = QURT_ISLAND_ATTR_INVALID,
    QURT_ISLAND_ATTR_INT,
    QURT_ISLAND_ATTR_THREAD,
    QURT_ISLAND_ATTR_MEMORY,
    QURT_ISLAND_ATTR_MEMORY_PHYS,
    QURT_ISLAND_ATTR_POOL,
    QURT_ISLAND_ATTR_MEMORY_TYPE
};

typedef struct qurt_island_attr_resource {
    enum qurt_island_attr_resource_type type;
    union {
        struct {
            qurt_addr_t base_addr;
            qurt_size_t size;
        } memory;
        char poolname[32];
        unsigned int interrupt;
        unsigned int memory_type;
        qurt_thread_t thread_id;
    };
} qurt_island_attr_resource_t;

/** QuRT island attributes */
typedef struct qurt_island_attr {
    int max_attrs;
    struct qurt_island_attr_resource attrs[1];
} qurt_island_attr_t;
/** @endcond */

/** Qurt island specification type */
typedef struct {
   int qdi_handle;
} qurt_island_t;


/**@ingroup func_qurt_island_add_interrupt
  This function adds an interrupt to island spec dynamically.

  @datatypes
  #qurt_island_t \n

  @param[in] spec_id ID created for the island specification.
  @param[in] int_num Interrupt number to add to the spec

  @return
  QURT_EOK -- Successfully added interrupt to the spec. \n
  QURT_EFAILED -- Not enough slots to add interrupt to the spec

  @dependencies
  None.
 */
int qurt_island_add_interrupt (qurt_island_t spec_id, int int_num);

/**@ingroup func_qurt_island_remove_interrupt
  This function removes an interrupt to island spec dynamically.

  @datatypes
  #qurt_island_t \n

  @param[in] spec_id ID created for the island specification.
  @param[in] int_num Interrupt number to be removed from the spec

  @return
  QURT_EOK -- Successfully removed the interrupt from spec. \n

  @dependencies
  None.
 */
int qurt_island_remove_interrupt (qurt_island_t spec_id, int int_num);

/**@ingroup qurt_island_add_virtaddr
  This function adds a virtual address range to island spec dynamically.\n

  @datatypes
  #qurt_island_t \n
  #qurt_addr_t \n
  #qurt_size_t \n

  @param[in] spec_id ID created for the island specification.\n
  @param[in] Base address of the memory range \n
  @param[in] Size of the memory range \n
 
  @return
  QURT_EOK -- Successfully added memory range to the spec or \n
              Memory range exists in the spec already \n
  QURT_EFAILED -- Invalid input, size given as 0 \n
  QURT_EMEM -- Not enough memory to add memory range to the spec

  @dependencies
  Ensure that this API is invoked after Island manager is initialized.\n
  Mapping for this range should be defined in XML. Dynamic mappings will not be considered.\n
 */
int qurt_island_add_virtaddr (qurt_island_t spec_id, qurt_addr_t addr, qurt_size_t size);

/**@ingroup func_qurt_island_cancel
  Cancel island mode.\n
  This function bails out of Island Enter.

  @note1hang As enter can potentially take a long time, qurt_island_cancel can be called
             to cancel the entry sequence.

  @return
  None. \n

  @dependencies
  None.
 */
void qurt_island_cancel (void);


/**@ingroup func_qurt_island_get_status
  Get island mode status.

  Returns a value indicating whether the QuRT system is executing in island mode.

  @return
  0 - Normal mode. \n
  1 - Island mode. \n

  @dependencies
  None.
*/
unsigned int qurt_island_get_status (void);


/**@ingroup func_qurt_island_get_exit_status
  Get the reason for last island mode exit status.

  @param[out]    cause_code Return pointer that will have the cause code of the last
                 island exit reason.
                 QURT_EISLANDUSEREXIT: Island exit was due to user call for island exit
                 QURT_ENOISLANDENTRY: Never entered Island Mode
                 QURT_EISLANDINVALIDINT: Island exit was due to an invalid interrupt in island mode.

                 int_num Return pointer that will hold the invalid interrupt number that caused
                 island exit.
                 This will hold the interrupt number that caused the island exit if the cause
			        code is QURT_EISLANDINVALIDINT.
                 For all other cases, it will be -1.

  @return
  None. \n

  @dependencies
  None.
*/
void qurt_island_get_exit_status(unsigned int *cause_code, int *int_num);

/**@ingroup func_qurt_island_get_enter_timestamp
  Get the recent timestamp when the system exits STM during island enter.

  @param[out]    island_enter_timestamp return pointer will have the recent timestamp
                 recorded afte system exits STM during island enter. If the system never 
                 tried entering island island_enter_timestamp return pointer will hold a value 
                 of zero.
  
  @return
  None. \n

  @dependencies
  None.
*/
void qurt_island_get_enter_timestamp(unsigned long long *island_enter_timestamp);

/**@ingroup func_qurt_island_get_specid
  Get the island spec id

  @param[out] spec_id Return pointer that will have the spec_id.

  @return
  QURT_EOK: Valid spec id is returned
  QURT_EVAL: If the pointer passed is NULL
  QURT_EFAILED: If the spec id is not initialized.

  @dependencies
  None.
*/
int qurt_island_get_specid(qurt_island_t *spec_id);

/**@ingroup qurt_island_set_num_specs
  Set the maximum number of specs that need to be created

  @param[in] number of maximum specs that would be created.

  @return
  QURT_EOK: Operation succeeded 
  QURT_EFAILED: Operation failed 

  @dependencies
  This has to be called before any spec is created. Default we allow only one spec to be created
*/
int qurt_island_set_num_specs(unsigned number);
#endif /* QURT_ISLAND_H */
