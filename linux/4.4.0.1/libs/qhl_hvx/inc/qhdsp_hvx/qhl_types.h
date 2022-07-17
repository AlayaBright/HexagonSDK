/**=============================================================================
@file
   qhl_types.h

@brief
   Header file for custom typedefs used in QHL libraries.

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _QHL_TYPES_H
#define _QHL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Complex 16-bit floating-point type
 */
typedef struct
{
    __fp16 r; /**< real part */
    __fp16 i; /**< imaginary part */
}qhl_cfloat16_t;

#ifdef __cplusplus
}
#endif

#endif /* _QHL_TYPES_H */
