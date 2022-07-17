# Compute resource manager framework

The cDSP has several shared resources such as L2 cache, HVX, HMX (where available), VTCM, hardware threads, and memory
buses.  The compute resource manager framework exposes in @b HAP_compute_res.h a set of APIs for managing, requesting
and releasing some of these resources.  The resource manager also offers a virtualized serialization resource to aid
concurrencies in which constituent use cases are to run with mutually exclusive access to the entire cDSP, for example,
to avoid cache thrashing with each other. 

Participating use cases issue blocking acquires on the serialization resource when ready to run, and each use case runs
in turn when it is granted that resource. Acquiring the serialization resource only ensures mutual exclusion from other
cooperating use cases that also block on acquisition of that resource; it does not guarantee exclusion from concurrent
use cases that do not block on the serialization resource.

When a higher-priority client requests a resource already in use by a lower-priority client, the lower-priority client
will be notified by a callback to suspend its work and release the resource as long as it configured a callback using
HAP_compute_res_attr_set_release_callback().

While VTCM allocation APIs exposed under [VTCM Manager](../../doxygen/HAP_vtcm_mgr/index.html) are still supported, we
recommend using the compute resource APIs for VTCM management and allocation where available. Starting with Lahaina, the
compute resource manager is  expanded to provide options to:

* Manage application IDs, which control VTCM partitions and privileges.
* Send release callbacks, which might be invoked when a high-priority client needs a resource used by a lower-priority client.
* Release and reacquire the same VTCM size and page configuration.
* Request VTCM with granular sizes (minimum and maximum required) and specific page size requirements.
* Allocate from a specified VTCM partition. (The [VTCM Manager allocation API](../../doxygen/HAP_vtcm_mgr/index.html) is
  restricted to only the default VTCM partition.) 

## Usage examples

### Serialized VTCM acquisition

This example shows two threads requesting VTCM and both participating in serialization by invoking HAP_compute_res_attr_set_serialize().

@code
    /*
     * PROCESS/THREAD 1
     */
    compute_res_attr_t res_info;
    unsigned int context_id = 0;
    void *p_vtcm = NULL;
    /*
     * Initialize the attribute structure
     */
    if (0 != HAP_compute_res_attr_init(&res_info) )
        return;
    /*
     * Set serialization option
     */
    if (0 != HAP_compute_res_attr_set_serialize(&res_info, 1) )
        return;
    /*
     * Set VTCM request parameters - 256KB single page
     */
    if (0 != HAP_compute_res_attr_set_vtcm_param(&res_info,
                                                 (256 * 1024),
                                                 1) )
        return;
    /*
     * Call acquire with a timeout of 10 milliseconds.
     */
    if (0 != (context_id = HAP_compute_res_acquire(&res_info, 10000) ) )
    {
        /*
         * Successfully requested for serialization and acquired VTCM.
         * The serialization request from PROCESS/THREAD 2 waits
         * until the resource is released here.
         */
        p_vtcm = HAP_compute_res_attr_get_vtcm_ptr(&res_info);
        if (0 == p_vtcm)
        {
            /*
             * VTCM allocation failed, should not reach here as the acquire
             * returned with valid context ID.
             */
            HAP_compute_res_release(context_id);
            return;
        }
        //Do my work in process/thread 1
        /*
         * Done. Release the resource now using the acquired context ID.
         * This releases both the serialization request and VTCM allocation.
         */
        HAP_compute_res_release(context_id);
        p_vtcm = NULL;
    } else {
        /*
         * Unsuccessful allocation. Timeout would have triggered.
         * Implement a fallback or fail gracefully.
         */
    }

    ...

    /*
     * PROCESS/THREAD 2
     */
    compute_res_attr_t res_info;
    unsigned int context_id = 0;
    /*
     * Initialize the attribute structure.
     */
    if (0 != HAP_compute_res_attr_init(&res_info) )
        return;
    /*
     * Set serialization option.
     */
    if (0 != HAP_compute_res_attr_set_serialize(&res_info, 1) )
        return;
    /*
     * Call acquire with a timeout of 10 milliseconds.
     */
    if (0 != (context_id = HAP_compute_res_acquire(&res_info, 10000) ) )
    {
        /*
         * Successfully requested for serialization.
         * The serialization request from PROCESS/THREAD 1 waits
         * until the resource is released here even when the PROCESS/THREAD 1s
         * request for VTCM can be served.
         */
        //Do my work in process/thread 2
        /*
         * Done. Release the resource now using the acquired context ID.
         */
        HAP_compute_res_release(context_id);
    } else {
        /*
         * Unsuccessful allocation. Timeout would have triggered.
         * Implement a fallback or fail gracefully.
         */
    }
@endcode

### Non-serialized VTCM acquisition

This example shows two threads requesting VTCM alone without a serialization option.

If the total size requested by both threads exceeds the size of VTCM that is available, only one thread gets
access to VTCM while the other thread waits. In this case, the threads are serializing their workload
implicitly.

If enough VTCM memory is available to meet the requests of both threads, both threads acquire VTCM upon request
and can end up executing in parallel.

@code
    /*
     * PROCESS/THREAD 1
     */
    compute_res_attr_t res_info;
    unsigned int context_id = 0;
    void *p_vtcm = NULL;
    /*
     * Initialize the attribute structure.
     */
    if (0 != HAP_compute_res_attr_init(&res_info) )
        return;

	/* By not calling HAP_compute_res_attr_set_serialize, we enable thread 1 to acquire VTCM
	 * as long as enough memory is available	
	 */

    /*
     * Set VTCM request parameters - 256 KB single page
     */
    if (0 != HAP_compute_res_attr_set_vtcm_param(&res_info,
                                                 (256 * 1024),
                                                 1) )
        return;
    /*
     * Call acquire with a timeout of 10 milliseconds.
     */
    if (0 != (context_id = HAP_compute_res_acquire(&res_info, 10000) ) )
    {
        /*
         * Successfully acquired VTCM.
         * The VTCM request from PROCESS/THREAD 2 waits if enough
         * VTCM is not left to serve the request until the resource is released
         * here.
         */
        p_vtcm = HAP_compute_res_attr_get_vtcm_ptr(&res_info);
        if (0 == p_vtcm)
        {
            /*
             * VTCM allocation failed, should not reach this point as the acquire
             * returned with valid context ID.
             */
            HAP_compute_res_release(context_id);
            return;
        }
        //Do my work in process/thread 1
        /*
         * Done. Release the resource now using the acquired context ID.
         * This releases the VTCM allocation.
         */
        HAP_compute_res_release(context_id);
        p_vtcm = NULL;
    } else {
        /*
         * Unsuccessful allocation. Timeout would have triggered.
         * Implement a fallback or fail gracefully.
         */
    }

    ...

    /*
     * PROCESS/THREAD 2
     */
    compute_res_attr_t res_info;
    unsigned int context_id = 0;
    void *p_vtcm = NULL;
    /*
     * Initialize the attribute structure
     */
    if (0 != HAP_compute_res_attr_init(&res_info) )
        return;

	/* By not calling HAP_compute_res_attr_set_serialize, we enable thread 2 to acquire VTCM
	 * as long as enough memory is available	
	 */

    /*
     * Set VTCM request parameters - 256 KB single page.
     */
    if (0 != HAP_compute_res_attr_set_vtcm_param(&res_info,
                                                 (256 * 1024),
                                                 1) )
        return;
    /*
     * Call acquire with a timeout of 10 milliseconds.
     */
    if (0 != (context_id = HAP_compute_res_acquire(&res_info, 10000) ) )
    {
        /*
         * Successfully acquired VTCM.
         * The VTCM request from PROCESS/THREAD 1 waits if enough
         * VTCM is not left to serve the request until the resource is released
         * here.
         */
        p_vtcm = HAP_compute_res_attr_get_vtcm_ptr(&res_info);
        if (0 == p_vtcm)
        {
            /*
             * VTCM allocation failed, should not reach this point as the acquire
             * returned with valid context ID.
             */
            HAP_compute_res_release(context_id);
            return;
        }
        //Do work in PROCESS/THREAD 2
        /*
         * Done. Release the resource now using the acquired context ID.
         * This releases the VTCM allocation.
         */
        HAP_compute_res_release(context_id);
        p_vtcm = NULL;
    } else {
        /*
         * Unsuccessful allocation. Timeout would have triggered.
         * Implement a fallback or fail gracefully.
         */
    }
@endcode
