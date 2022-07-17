/**=============================================================================
@file
    qhmath_cos_ah.c

@brief
    Computes cosine over the elements of 16-bit input array.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdint.h>
#include "qhmath.h"
#include "qhmath_macros.h"

int32_t qhmath_cos_ah(int16_t *input, int16_t *output, uint32_t size)
{
    QHMATH_VECTORIZE_FUN (qhmath_cos_h, output, size, input)
}