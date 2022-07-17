/**=============================================================================
@file
    dsp_capabilities_utils.h

@brief
    Wrapper functions for FastRPC Capability APIs.

Copyright (c) 2020-2021 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/
#ifndef DSP_CAPABILITIES_UTILS_H
#define DSP_CAPABILITIES_UTILS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <inttypes.h>
#include <stdbool.h>
#include "AEEStdErr.h"

 /**
 * Wrapper for FastRPC Capability API: Allow users to query DSP support.
 *
 * @param[out]  domain pointer to supported domain.
 * @return      0          if query is successful.
 *              non-zero   if error, return value points to the error.
 */
int get_dsp_support(int *domain);

 /**
 * Wrapper for FastRPC Capability API: Allow users to query VTCM information.
 *
 * @param[in]   domain value of domain in the queried.
 * @param[out]  capability capability value of the attribute queried.
 * @param[in]   attr value of the attribute to the queried.
 * @return      0          if query is successful.
 *              non-zero   if error, return value points to the error.
 */
int get_vtcm_info(int domain, uint32_t *capability, uint32_t attr);

 /**
 * Wrapper for FastRPC Capability API: Allow users to query unsigned pd support.
 *
 * @return      true          if unsigned pd is supported.
 *              false         if unsigned pd is not supported, capability query failed.
 */
bool get_unsignedpd_support(void);

#ifdef __cplusplus
}
#endif

#endif  //DSP_CAPABILITIES_UTILS_H
