/*-----------------------------------------------------------------------------
   Copyright (c) 2019-2020 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------------*/

#ifndef HAP_COMPUTE_RES_H_
#define HAP_COMPUTE_RES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup types Macros and structures
 * @{
 */

/** Error code for unsupported features. */
#define HAP_COMPUTE_RES_NOT_SUPPORTED                           0x80000404

/**
 *  @file HAP_compute_res.h
 *  @brief Header file with APIs to allocate compute resources.
 */

/**
 * Structure containing attributes for compute resources.
 */
typedef struct {
    unsigned long long attributes[8]; /**< Attribute array. */
} compute_res_attr_t;

/**
 * Structure containing a VTCM page size and the number of pages with that size.
 */
typedef struct {
    unsigned int page_size;  /**< Page size in bytes. */
    unsigned int num_pages;  /**< Number of pages of size page_size. */
} compute_res_vtcm_page_def_t;

/**
 * Structure describing the VTCM memory pages.
 */
typedef struct {
    unsigned int block_size;           /**< Block size in bytes */
    unsigned int page_list_len;      /**< Number of valid elements in page_list array */
    compute_res_vtcm_page_def_t page_list[8];  /**< Array of pages. */
} compute_res_vtcm_page_t;

/**
 * @}
 */
 
/**
 * @cond DEV
 */
int __attribute__((weak)) compute_resource_attr_init(
                                        compute_res_attr_t* attr);

int __attribute__((weak)) compute_resource_attr_set_serialize(
                                        compute_res_attr_t* attr,
                                        unsigned char b_enable);

int __attribute__((weak)) compute_resource_attr_set_hmx_param(
                                        compute_res_attr_t* attr,
                                        unsigned char b_enable);

int __attribute__((weak)) compute_resource_attr_set_vtcm_param(
                                        compute_res_attr_t* attr,
                                        unsigned int vtcm_size,
                                        unsigned char b_single_page);

int __attribute__((weak)) compute_resource_attr_set_vtcm_param_v2(
                                        compute_res_attr_t* attr,
                                        unsigned int vtcm_size,
                                        unsigned int min_page_size,
                                        unsigned int min_vtcm_size);

int __attribute__((weak)) compute_resource_attr_set_app_type(
                                        compute_res_attr_t* attr,
                                        unsigned int application_id);

int __attribute__((weak)) compute_resource_attr_set_cache_mode(
                                        compute_res_attr_t* attr,
                                        unsigned char b_enable);

int __attribute__((weak)) compute_resource_attr_set_release_callback(
                                        compute_res_attr_t* attr,
                                        int (*release_callback)(
                                            unsigned int context_id,
                                            void* client_context),
                                        void* client_context);

void* __attribute__((weak)) compute_resource_attr_get_vtcm_ptr(
                                        compute_res_attr_t* attr);

int __attribute__((weak)) compute_resource_attr_get_vtcm_ptr_v2(
                                        compute_res_attr_t* attr,
                                        void** vtcm_ptr,
                                        unsigned int* vtcm_size);

int __attribute__((weak)) compute_resource_query_VTCM(
                                unsigned int application_id,
                                unsigned int* total_block_size,
                                compute_res_vtcm_page_t* total_block_layout,
                                unsigned int* avail_block_size,
                                compute_res_vtcm_page_t* avail_block_layout);

unsigned int __attribute__((weak)) compute_resource_acquire(
                                        compute_res_attr_t* attr,
                                        unsigned int timeout_us);

int __attribute__((weak)) compute_resource_release(
                                        unsigned int context_id);

int __attribute__((weak)) compute_resource_acquire_cached(
                                        unsigned int context_id,
                                        unsigned int timeout_us);

int __attribute__((weak)) compute_resource_release_cached(
                                        unsigned int context_id);

int __attribute__((weak)) compute_resource_hmx_lock(
                                        unsigned int context_id);

int __attribute__((weak)) compute_resource_hmx_unlock(
                                        unsigned int context_id);

int __attribute__((weak)) compute_resource_check_release_request(
                                        unsigned int context_id);
/**
 * @endcond
 */

/**
 * @defgroup attributes Manage attributes
 * Manage parameters affecting the requested shared resources
 * @{
 */

/**
 * Initializes the attribute structure for a resource request.
 *
 * The user must call this function before setting any specific resource property
 * via other helper functions.
 *
 * @param[in] attr Pointer to compute resource attribute structure,
 *                 #compute_res_attr_t.
 *
 * @return
 * 0 upon success. \n
 * Nonzero upon failure. \n
 * HAP_COMPUTE_RES_NOT_SUPPORTED if unsupported.
 */
static inline int HAP_compute_res_attr_init(compute_res_attr_t* attr)
{
    if (compute_resource_attr_init)
        return compute_resource_attr_init(attr);

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * Sets or clears the serialization option in the request resource structure.
 *
 * Serialization allows participating use cases to run with mutually exclusive
 * access to the entire cDSP which helps, for example, in avoiding cache
 * thrashing while trying to run simultaneously on different hardware threads.
 * Participating use cases issue blocking acquires on the serialization
 * resource when ready to run, and each runs in turn when it is granted that
 * resource.
 *
 * Acquiring the serialization resource only ensures
 * mutual exclusion from other cooperating use cases that also block on
 * acquisition of that resource, it does not guarantee exclusion from
 * concurrent use cases that do not request the serialization
 * resource.
 *
 * @param[in] attr Pointer to the compute resource attribute structure,
 *                 #compute_res_attr_t.
 * @param[in] b_serialize 1 (TRUE) to participate in serialization. \n
 *                        0 (FALSE) otherwise.
 *
 * @return
 * 0 upon success \n
 * Nonzero upon failure.
 */
static inline int HAP_compute_res_attr_set_serialize(
                                                compute_res_attr_t* attr,
                                                unsigned char b_serialize)
{
    if (compute_resource_attr_set_serialize)
    {
        return compute_resource_attr_set_serialize(attr,
                                                   b_serialize);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * Sets VTCM request parameters in the provided resource attribute structure.
 *
 * The user calls this function to request the specified VTCM size in the acquire call.
 * These VTCM request attributes are reset to 0 (no VTCM request) in the
 * resource attribute structure by HAP_compute_res_attr_init().
 *
 * @param[in] attr Pointer to compute resource attribute structure,
 *                 #compute_res_attr_t.
 * @param[in] vtcm_size Size of the VTCM request in bytes;
                        0 if VTCM allocation is not required.
 * @param[in] b_single_page    1 - Requested VTCM size to be allocated in a
 *                                 single page. \n
 *                             0 - No page requirement (allocation can spread
 *                                 across multiple pages. VTCM manager
 *                                 always attempts the best fit).
 *
 * @return
 * 0 upon success. \n
 * Non-zero upon failure.
 */
static inline int HAP_compute_res_attr_set_vtcm_param(
                                             compute_res_attr_t* attr,
                                             unsigned int vtcm_size,
                                             unsigned char b_single_page)
{
    if (compute_resource_attr_set_vtcm_param)
    {
        return compute_resource_attr_set_vtcm_param(attr,
                                                    vtcm_size,
                                                    b_single_page);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * Reads the VTCM memory pointer from the given attribute structure.
 *
 * On a successful VTCM resource request placed via #HAP_compute_res_acquire()
 * using HAP_compute_res_attr_set_vtcm_param(), a user can invoke this helper
 * function to retrieve the allocated VTCM address by passing the same attribute
 * structure used in the respective HAP_compute_res_acquire() call.
 *
 * @param[in] attr Pointer to compute the resource attribute structure
 *                 #compute_res_attr_t.
 *
 * @return
 * Void pointer to the allocated VTCM section. \n
 * 0 signifies no allocation.
 */
static inline void* HAP_compute_res_attr_get_vtcm_ptr(compute_res_attr_t* attr)
{
    if (compute_resource_attr_get_vtcm_ptr)
    {
        return compute_resource_attr_get_vtcm_ptr(attr);
    }

    return 0;
}

/**
 * Sets an extended set of VTCM request parameters in the attribute structure,
 * specifically VTCM Size, the minimum required page size, and the minimum
 * required VTCM size.
 *
 * This function cannot be used with HAP_compute_res_attr_set_vtcm_param().
 * Call this function after HAP_compute_res_attr_init().
 *
 * Supported starting with Lahaina.
 *
 * @param[in] attr Pointer to compute the resource attribute structure,
 *                 #compute_res_attr_t.
 * @param[in] vtcm_size Size of the VTCM request in bytes. 0 if VTCM allocation
 *                      is NOT required.
 * @param[in] min_page_size Minimum page size required in bytes. Valid pages include
 *                          4 KB, 16 KB, 64 KB, 256 KB, 1 MB, 4 MB, 16 MB. Setting 0
 *                          will select best possible fit (least page mappings)
 * @param[in] min_vtcm_size Minimum VTCM size in bytes, if the specified size
 *                          (vtcm_size) is not available. 0 means the
 *                          size is an absolute requirement.
 *
 * @return
 * 0 for success. \n
 * Non-zero for failure. \n
 * HAP_COMPUTE_RES_NOT_SUPPORTED when not supported.
 */
static inline int HAP_compute_res_attr_set_vtcm_param_v2(
                                             compute_res_attr_t* attr,
                                             unsigned int vtcm_size,
                                             unsigned int min_page_size,
                                             unsigned int min_vtcm_size)
{
    if (compute_resource_attr_set_vtcm_param_v2)
    {
        return compute_resource_attr_set_vtcm_param_v2(attr,
                                                       vtcm_size,
                                                       min_page_size,
                                                       min_vtcm_size);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * On a successful VTCM resource request placed via
 * HAP_compute_res_acquire() or HAP_compute_res_acquire_cached() using
 * HAP_compute_res_attr_set_vtcm_param_v2(), users invoke this helper function
 * to retrieve the allocated VTCM address and size by passing the same
 * attribute structure used in the respective acquire call.
 *
 * Supported starting with Lahaina.
 *
 * @param[in] attr Pointer to compute the resource attribute structure
 *                 #compute_res_attr_t.
 * @param[out] vtcm_ptr Assigned VTCM address; NULL for no allocation.
 * @param[out] vtcm_size Size of the allocated VTCM memory from the assigned pointer.
 *
 * @return
 * 0 upon success. \n
 * Nonzero upon failure. \n
 * HAP_COMPUTE_RES_NOT_SUPPORTED when not supported.
 */
static inline int HAP_compute_res_attr_get_vtcm_ptr_v2(
                                        compute_res_attr_t* attr,
                                        void** vtcm_ptr,
                                        unsigned int* vtcm_size)
{
    if (compute_resource_attr_get_vtcm_ptr_v2)
    {
        return compute_resource_attr_get_vtcm_ptr_v2(attr,
                                                     vtcm_ptr,
                                                     vtcm_size);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * On chipsets with HMX, sets/resets the HMX request parameter in the attribute
 * structure for acquiring the HMX resource.
 *
 * Call this function after HAP_compute_res_attr_init().
 *
 * Supported starting with Lahaina.
 *
 * @param[in] attr Pointer to compute the resource attribute structure,
 *                 #compute_res_attr_t.
 * @param[in] b_enable 0 - do not request HMX resource (resets option). \n
 *                     1 - request HMX resource (sets option).
 * @return
 * 0 upon success. \n
 * Nonzero upon failure.
 */
static inline int HAP_compute_res_attr_set_hmx_param(
                                                compute_res_attr_t* attr,
                                                unsigned char b_enable)
{
    if (compute_resource_attr_set_hmx_param)
    {
        return compute_resource_attr_set_hmx_param(attr,
                                                   b_enable);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * Sets or resets cacheable mode in the attribute structure.
 *
 * A cacheable request allows users to allocate and release based on the
 * context ID of the request. On a successful cacheable request via
 * HAP_compute_res_acquire(), users get the same VTCM address and
 * size across calls of HAP_compute_res_acquire_cached() and
 * HAP_compute_res_release_cached() until the context is explicitly
 * released via HAP_compute_res_release().
 *
 * After a successful cacheable request via HAP_compute_res_acquire(),
 * users can get the assigned VTCM pointer (if requested) by passing
 * the attribute structure to HAP_compute_res_attr_get_vtcm_ptr()
 * for v1 and HAP_compute_res_attr_get_vtcm_ptr_v2() for v2,
 * and they must call HAP_compute_res_acquire_cached() before using the
 * assigned resources.
 *
 * Supported starting with Lahaina.
 *
 * @param[in] attr Pointer to compute resource attribute structure,
 *                 #compute_res_attr_t.
 * @param[in] b_enable  0 - Do not request cacheable mode (resets option). \n
 *                      1 - Request cacheable mode (sets option).
 *
 * @return
 * 0 upon success. \n
 * Nonzero upon failure.\n
 * HAP_COMPUTE_RES_NOT_SUPPORTED when not supported.
 */
static inline int HAP_compute_res_attr_set_cache_mode(
                                              compute_res_attr_t* attr,
                                              unsigned char b_enable)
{
    if (compute_resource_attr_set_cache_mode)
    {
        return compute_resource_attr_set_cache_mode(attr,
                                                    b_enable);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * Sets the application ID parameter in the resource structure used to
 * select the appropriate VTCM partition.
 *
 * If this application ID parameter is not explicitly set, the default partition is selected.
 * The default application ID (0) is set when the attribute structure is initialized.
 * Application IDs are defined in the kernel device tree configuration.
 * If the given ID is not specified in the tree, the primary VTCM partition is selected.
 *
 * Call this function after HAP_compute_res_attr_init().
 *
 * Supported starting with Lahaina.
 *
 * @param[in] attr Pointer to compute the resource attribute structure
 *                 #compute_res_attr_t.
 * @param[in] application_id Application ID used to specify the VTCM partition.
 *
 * @return
 * 0 upon success. \n
 * Nonzero upon failure.
 */
static inline int HAP_compute_res_attr_set_app_type(
                                              compute_res_attr_t* attr,
                                              unsigned int application_id)
{
    if (compute_resource_attr_set_app_type)
    {
        return compute_resource_attr_set_app_type(attr,
                                                  application_id);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * Returns the total and available VTCM sizes and page layouts
 * for the given application type.
 *
 * Supported starting with Lahaina.
 *
 * @param[in] application_id Application ID used to specify the VTCM partition.
 * @param[out] total_block_size Total VTCM size assigned for this application type.
 * @param[out] total_block_layout Total VTCM size (total_block_size)
 *                                represented in pages.
 * @param[out] avail_block_size Largest contiguous memory chunk available in
                                VTCM for this application type.
 * @param[out] avail_block_layout Available block size (avail_block_size)
 *                                represented in pages.
 *
 * @return
 * 0 upon success. \n
 * Nonzero upon failure. \n
 * HAP_COMPUTE_RES_NOT_SUPPORTED when not supported.
 */
static inline int HAP_compute_res_query_VTCM(
                                unsigned int application_id,
                                unsigned int* total_block_size,
                                compute_res_vtcm_page_t* total_block_layout,
                                unsigned int* avail_block_size,
                                compute_res_vtcm_page_t* avail_block_layout)
{
    if (compute_resource_query_VTCM)
    {
        return compute_resource_query_VTCM(application_id,
                                           total_block_size,
                                           total_block_layout,
                                           avail_block_size,
                                           avail_block_layout);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * @}
 */

/**
 * @defgroup acquire_release Acquire and release
 * Manage the process of resource acquisition and release
 * @{
 */

/**
 * Checks the release request status for the provided context.
 * When a context is acquired by providing a release callback, the callback
 * can be invoked by the compute resource manager when a high priority client
 * is waiting for the resource(s). If a client defers a release request waiting
 * for an outstanding work item, this API can be used to check if a release is
 * still required before releasing the context once the work is done.
 *
 * Note: It is not mandatory to call this API once a release request via
 * the registered callback is received. The context can be released and reacquired
 * if necessary. This API can be useful to avoid a release and reacquire in cases
 * where the high priority client times out and is no longer waiting for the
 * resource(s).
 *
 * Supported starting with Lahaina.
 *
 * @param[in] context_id  Context ID returned by HAP_compute_res_acquire call().
 *
 * @return
 * 0 if the provided context need not be released. \n
 * Nonzero up on failure or if the context needs to be released. \n
 * HAP_COMPUTE_RES_NOT_SUPPORTED when not supported. \n
 */
static inline int HAP_compute_res_check_release_request(
                                                    unsigned int context_id)
{
    if (compute_resource_check_release_request)
    {
        return compute_resource_check_release_request(context_id);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * Accepts a prepared attribute structure (attr) and returns a context ID
 * for a successful request within the provided timeout (microseconds).
 *
 * @param[in] attr Pointer to compute the resource attribute structure
 *                 #compute_res_attr_t.
 * @param[in] timeout_us Timeout in microseconds; 0 specifies no timeout
 *                       i.e., requests with unavailable resources
 *                       immediately return failure. If nonzero, should
 *                       be at least 200.
  *
 * @return
 * Nonzero context ID upon success. \n
 * 0 upon failure (i.e., unable to acquire requested resource
 * in a given timeout duration).
 */
static inline unsigned int HAP_compute_res_acquire(
                                              compute_res_attr_t* attr,
                                              unsigned int timeout_us)
{
    if (compute_resource_acquire)
    {
        return compute_resource_acquire(attr, timeout_us);
    }

    return 0;
}

/**
 * Releases all the resources linked to the given context ID.
 *
 * Call this function with the context_id returned by a successful
 * HAP_compute_res_acquire call().
 *
 * @param[in] context_id Context ID returned by HAP_compute_res_acquire call().
 *
 * @return
 * 0 upon success. \n
 * Nonzero upon failure. \n
 * HAP_COMPUTE_RES_NOT_SUPPORTED when not supported.
 */
static inline int HAP_compute_res_release(unsigned int context_id)
{
    if (compute_resource_release)
    {
        return compute_resource_release(context_id);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * Acquires or reacquires the resources pointed to the context_id returned by
 * a successful HAP_compute_res_acquire() call. If VTCM resource was requested,
 * the VTCM address, size, and page configuration remain the same.
 *
 * Supported from Lahaina.
 *
 * @param[in] context_id Context ID returned by HAP_compute_res_acquire().
 * @param[in] timeout_us Timeout in microseconds; 0 specifies no timeout
 *                       i.e., requests with unavailable resources
 *                       immediately return failure. If nonzero, should
 *                       be at least 200.
 *
 * @return
 * 0 upon success. \n
 * Nonzero upon failure. \n
 * HAP_COMPUTE_RES_NOT_SUPPORTED when not supported.
 */
static inline int HAP_compute_res_acquire_cached(
                                              unsigned int context_id,
                                              unsigned int timeout_us)
{
    if (compute_resource_acquire_cached)
    {
        return compute_resource_acquire_cached(context_id, timeout_us);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * Releases all the resources pointed to by the context_id acquired
 * by a successful HAP_compute_res_acquire_cached() call, while allowing the
 * user to reacquire the same resources via HAP_compute_res_acquire_cached()
 * in the future until the context is released via HAP_compute_res_release().
 *
 * Supported starting with Lahaina.
 *
 * @param[in] context_id Context ID returned by
 *                       #HAP_compute_res_acquire().
 *
 * @return
 * 0 upon success. \n
 * Nonzero upon failure. \n
 * HAP_COMPUTE_RES_NOT_SUPPORTED when not supported.
 */
static inline int HAP_compute_res_release_cached(unsigned int context_id)
{
    if (compute_resource_release_cached)
    {
        return compute_resource_release_cached(context_id);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * Sets the release callback function in the attribute structure.

 * The compute resource manager calls the release_callback function when any of the
 * resources reserved by the specified context are required by a higher priority
 * client. Clients act on the release request by explicitly calling the release
 * function HAP_compute_res_release() or HAP_compute_res_release_cached()
 * to release all acquired resources of the given context_id.
 *
 * Client-provided context (client_context) is passed to the release callback. On
 * receiving a release request via the provided callback, clients should call the
 * release function within 5 milliseconds. The release_callback function
 * should not have any blocking wait.
 *
 * Call this function after HAP_compute_res_attr_init().
 *
 * Supported starting with Lahaina.
 *
 * @param[in] attr Pointer to compute the resource attribute structure,
 *                 #compute_res_attr_t.
 * @param[in] release_callback Function pointer to the registered callback to
                               receive the release request.
 * @param[in] client_context User-provided client context.
 *
 * @return
 * 0 upon success. \n
 * Nonzero upon failure. \n
 * HAP_COMPUTE_RES_NOT_SUPPORTED when not supported.
 */
static inline int HAP_compute_res_attr_set_release_callback(
                                        compute_res_attr_t* attr,
                                        int (*release_callback)(
                                            unsigned int context_id,
                                            void* client_context),
                                        void* client_context)
{
    if (compute_resource_attr_set_release_callback)
    {
        return compute_resource_attr_set_release_callback(attr,
                                                          release_callback,
                                                          client_context);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * @}
 */

/**
 * @defgroup HMX HMX lock and unlock
 * Manage HMX lock once HMX has been acquired
 *
 * @{
 */

/**
 * Locks the HMX unit to the current thread and prepares the thread to
 * execute HMX instructions. The client must have already acquired the
 * HMX resource with HAP_compute_res_acquire() or HAP_compute_res_acquire_cached(),
 * and context_id must refer to the corresponding resource manager context.
 *
 * Before executing HMX instructions, a client must call this function from
 * the same software thread used for HMX processing. Only the calling thread
 * with a valid HMX lock may execute HMX instructions.
 *
 * Supported starting with Lahaina.
 *
 * @param[in] context_id Context ID returned by
 *                       #HAP_compute_res_acquire().
 *
 * @return
 * 0 upon success. \n
 * Nonzero upon failure. \n
 * HAP_COMPUTE_RES_NOT_SUPPORTED when not supported.
 */
static inline int HAP_compute_res_hmx_lock(unsigned int context_id)
{
    if (compute_resource_hmx_lock)
    {
        return compute_resource_hmx_lock(context_id);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * Unlocks the HMX unit from the calling thread. The HMX unit can then be
 * locked to another thread or released with HAP_compute_res_release().
 *
 * This function must be called from the same thread as the previous
 * HMX_compute_res_hmx_lock() call.
 *
 * Supported starting with Lahaina.
 *
 * @param[in] context_id Context ID returned by
 *                       #HAP_compute_res_acquire().
 *
 * @return
 * 0 upon success. \n
 * Nonzero upon failure. \n
 * HAP_COMPUTE_RES_NOT_SUPPORTED when not supported.
 */
static inline int HAP_compute_res_hmx_unlock(unsigned int context_id)
{
    if (compute_resource_hmx_unlock)
    {
        return compute_resource_hmx_unlock(context_id);
    }

    return HAP_COMPUTE_RES_NOT_SUPPORTED;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //HAP_COMPUTE_RES_H_
