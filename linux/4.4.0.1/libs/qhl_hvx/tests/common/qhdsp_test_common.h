/**=============================================================================
@file
   qhdsp_test_common.h

@brief
   Header file for common routines used internally in QHDSP testing.

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _QHDSP_TEST_COMMON_H
#define _QHDSP_TEST_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include "qhcomplex.h"
#include "qhmath.h"
//#include "qhdsp_fft_internal.h"

#define   creal_f(x)      (qhcomplex_creal_f( (x) ) )
#define   cimag_f(x)      (qhcomplex_cimag_f( (x) ) )
#define   LOG10(x)            (qhmath_log10_f( (x) ))

float calculate_SNR32(int complex* REF, int complex* DUT, uint32_t n_points);
float calculate_SNR8(const char complex* REF, char complex* DUT, uint32_t n_points);
float calculate_SNR_sf(float complex* REF, float complex* DUT, int nPoints);

#ifdef __cplusplus
}
#endif

#endif /* _QHDSP_TEST_COMMON_H */
