/*
 * Copyright (c) 2012-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

#ifndef HAP_MEM_H
#define HAP_MEM_H
#include <stdlib.h>
#include "AEEStdDef.h"
#include "AEEStdErr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file HAP_mem.h
 * @brief HAP Memory APIs
 */


/*
 * Protections are chosen from these bits, or-ed together
 */


 /*! @name HAP_PROT
     \brief These macros define the permissions on memory block described by the file descriptor.

     It is passed as input parameter 'prot' to HAP_mmap(). These can be ORed to set the required permissions.


*/

///@{

/*!     \def HAP_PROT_NONE
        \brief Passing HAP_PROT_NONE as input results in setting 'NO' permissions on the buffer.
*/
#define HAP_PROT_NONE   0x00    /* no permissions */

/*!      \def HAP_PROT_READ
         \brief Passing HAP_PROT_READ as input results in setting 'Read' permissions on the buffer.
*/
#define HAP_PROT_READ   0x01    /* pages can be read */
/*!      \def HAP_PROT_WRITE
     \brief Passing HAP_PROT_WRITE as input results in setting 'Write' permissions on the buffer.
*/

#define HAP_PROT_WRITE  0x02    /* pages can be written */

/*!
     \def HAP_PROT_EXEC
     \brief Passing HAP_PROT_EXEC as input results in setting 'Execute' permissions on the buffer. Currently not supported.
*/
#define HAP_PROT_EXEC   0x04    /* pages can be executed */


///@}

/*
 * Cache policy or-ed with protections parameter
 */

 /*! @name HAP_MEM_CACHE
     \brief These macros define the cache policies for mapping memory pages to DSP MMU. Default cache policy is cache writeback.

     It is passed as input parameter 'prot', or-ed with page protections to HAP_mmap().

*/

///@{

/*!     \def HAP_MEM_CACHE_WRITEBACK
        \brief Passing HAP_MEM_CACHE_WRITEBACK as input results in mapping memory as cache writeback
*/
#define HAP_MEM_CACHE_WRITEBACK    (0x10)    /* cache write back */

/*!      \def HAP_MEM_CACHE_NON_SHARED
         \brief Passing HAP_MEM_CACHE_NON_SHARED as input results in mapping memory as uncached
*/
#define HAP_MEM_CACHE_NON_SHARED   (0x20)    /* normal uncached memory */

/*!      \def HAP_MEM_CACHE_WRITETHROUGH
     \brief Passing HAP_MEM_CACHE_WRITETHROUGH as input results in mapping memory as cache write through
*/

#define HAP_MEM_CACHE_WRITETHROUGH  (0x40)    /* write through memory */

///@}


/**
 * Allocate a block of memory.
 * @param[in] bytes size of memory block in bytes.
 * @param[out] pptr pointer to the memory block
 * @return int AEE_SUCCESS for success and AEE_ENOMEMORY for failure.
 */

static inline int HAP_malloc(uint32 bytes, void** pptr)
{
    *pptr = malloc(bytes);
    if (*pptr) {
        return AEE_SUCCESS;
    }
    return AEE_ENOMEMORY;
}

/**
 * Free the memory block allocated through HAP_malloc().
 * @param[in] ptr pointer to the memory block
 * @return int AEE_EBADCLASS if ptr is NULL
               AEE_SUCCESS if ptr is not NULL

 */

static inline int HAP_free(void* ptr)
{
    if(ptr == NULL)
        return AEE_EBADCLASS;
    free(ptr);
    return AEE_SUCCESS;
}
/**
 * @brief Statistics of user heap memory
 */
struct HAP_mem_stats {
   uint64 bytes_free; /** number of bytes free from all the segments,
                        * may not be available for a single alloc
                        */
   uint64 bytes_used; /** number of bytes used */
   uint64 seg_free;   /** number of segments free */
   uint64 seg_used;   /** number of segments used */
   uint64 min_grow_bytes; /** minimum number of bytes to grow the heap by when creating a new segment */
};

/**
 * Get the current statistics from the heap.
 *
 * @param[in,out] stats pointer to stats structure
 * @retval AEE_SUCCESS
 */
int HAP_mem_get_stats(struct HAP_mem_stats *stats);

/**
 * Set the minimum and maximum size by which the user heap should grow when
 * it is exhausted and needs to add memory from the HLOS on the Apps processor.
 *
 * This API allows to configure the minimum and maximum size that should
 * be added to the DSP user heap when an allocation fails and more memory
 * needs to be obtained from the HLOS. Using this API is optional. If not
 * used, the runtime will try to choose reasonable growth sizes based on
 * allocation history.
 *

 * @param[in] min minimum bytes to grow the heap by when requesting a new segment
 * @param[in] max maximum bytes to grow the heap by when requesting a new segment
 * @retval AEE_SUCCESS
 *
 */
int HAP_mem_set_grow_size(uint64 min, uint64 max);

/**
 * Set low and high memory thresholds for heap
 *
 * Thresholds must be tuned according to the memory requirements
 *
 * Improper thresholds might led to heap failure
 *
 * @param[in] low_largest_block_size (in bytes) - the heap will grow if size of the largest free block is less than this threshold.
 *                                Currently, setting this parameter will have no impact on the heap.
 * @param[in] high_largest_block_size (in bytes) - the heap manager will release all unused sections if size of the largest free block is greater than this threshold.
 *                                 The recommended value for this, is the size of largest single allocation possible in your application.
 * @return  AEE_SUCCESS on success
 *         AEE_EBADPARM on failure
 */
int HAP_mem_set_heap_thresholds(unsigned int low_largest_block_size, unsigned int high_largest_block_size);


/**
 * Map buffer associated with the file descriptor to DSP memory. The reference
 * count gets incremented if the file descriptor is already mapped.
 *
 * @param[in] addr mapping at fixed address, not supported currently. This has to be set to NULL
 * @param[in] len size of the buffer to be mapped
 * @param[in] prot protection flags - supported are only HAP_PROT_READ and HAP_PROT_WRITE. HAP_PROT_EXEC is not supported
 * @param[in] flags HAP_MAP_NO_MAP - Increment reference count with no mapping
 *               0 - map the buffer and increment the reference count
 * @param[in] fd file descriptor for the buffer
 * @param[in] offset offset into the buffer
 * @retval  mapped address
 *          -1 on failure
 */
void* HAP_mmap(void *addr, int len, int prot, int flags, int fd, long offset);

/**
 * Decrements the reference count and unmaps the buffer from memory if the reference count goes to 0.
 *
 * @param[in] addr mapped address
 * @param[in] len size of the mapped buffer
 * @return  0 on success
 *         AEE_NOSUCHMAP in input addr is invalid
 */
int HAP_munmap(void *addr, int len);

/**
 * Get virtual and physical address associated with the buffer and increments
 * the reference count.
 *
 * @param[in] fd file descriptor for the buffer
 * @param[out] vaddr virtual address associated with the buffer
 * @param[out] paddr physical address associated with the buffer
 * @retval 0 on success
 *          AEE_ENOSUCHMAP if fd is invalid
 */
int HAP_mmap_get(int fd, void **vaddr, uint64 *paddr);

/**
 * Decrements the reference count of the file descriptor.
 *
 *@param[in] fd file descriptor of the buffer
 *@retval 0 on success
 *         AEE_ENOSUCHMAP if fd is invalid
 *         AEE_EBADMAPREFCNT if map refcount is <=0
 */
int HAP_mmap_put(int fd);

/**
 * Get the stack size (in bytes) available for current thread
 * Supported only on Lahaina and Cedros
 * @return  available stack for current thread, on success
 *          AEE_EINVALIDTHREAD if unable to get current thread id
 *          AEE_ERESOURCENOTFOUND if unable to get stack for current thread
 */
uint64 HAP_mem_available_stack();

#ifdef __cplusplus
}
#endif

#endif // HAP_MEM_H

