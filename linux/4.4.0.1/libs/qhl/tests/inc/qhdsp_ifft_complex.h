/**=============================================================================
@file
    qhdsp_ifft_complex.h

@brief
    Header file for complex 2^N IFFT routines.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _QHDSP_IFFT_COMPLEX_H
#define _QHDSP_IFFT_COMPLEX_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup fft_internal_routines FFT internal routines
  * @{
 */

/**
 * @brief           Complex 2^N IFFT for single precision float point
 * @param[in]       input - input samples in frequency domain
 * @param[in]       N - number of samples on which IFFT is performed
 * @param[in]       w - twiddle factors
 * @param[out]      output - IFFT output buffer
 * @note            
 *                  - Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 *                  - Assumptions:
 *                                  1. w - generated with qhdsp_gen_twiddles_complex_f() function
 */
void c1difft_f(const float complex* input, int32_t N, const float complex* w, float complex* output);

/**
 * @brief           Complex 2^N IFFT for 16x16 (complex number: bits 0:15-real part, bits 16:31-imag part) fixed-point
 * @param[in]       input - input samples in frequency domain
 * @param[in]       N - number of samples on which IFFT is performed
 * @param[in]       w - twiddle factors
 * @param[out]      output - IFFT output buffer
 * @note            
 *                  - Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 *                  - Scale factor [1/N] absent since scaling was done in FFT function
 *                  - input format Q<log2(N)>.<15-log2(N)>; example: N=16 -> log2(N) = 4 -> input format Q4.11, output format Q15 
 *                  - Assumptions:
 *                                  1. w - generated with qhdsp_fft_gen_twiddles_complex_ach() function
 */
void c1difft_h(const int32_t *input, int32_t N, const int32_t *w, int32_t *output);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _QHDSP_IFFT_COMPLEX_H */
