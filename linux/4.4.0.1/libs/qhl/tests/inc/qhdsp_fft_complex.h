/**=============================================================================
@file
    qhdsp_fft_complex.h

@brief
    Header file for C implementation of complex 2^N FFT routines.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _QHDSP_FFT_COMPLEX_H
#define _QHDSP_FFT_COMPLEX_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup fft_internal_routines FFT internal routines
  * @{
 */

/**
 * @brief           Complex 2^N FFT for single precision float point
 * @param[in]       input - input samples in time domain (complex)
 * @param[in]       N - number of samples on which FFT is performed
 * @param[in]       w - twiddle factors
 * @param[out]      output - FFT output buffer
 * @note
 *                  - Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 *                  - Assumptions:
 *                                  1. w - generated with qhdsp_gen_twiddles_complex() function
 */
void c1dfft_f(const float complex *input, int32_t N, const float complex *w, float complex *output);

/**
 * @brief           Complex 2^N FFT for 16x16 (complex number: bits 0:15-real part, bits 16:31-imag part) fixed-point
 * @param[in]       input - input samples in time domain (complex)
 * @param[in]       N - number of samples on which FFT is performed
 * @param[in]       w - twiddle factors
 * @param[out]      output - FFT output buffer
 * @note
 *                  - Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 *                  - Scale factor: 1/N
 *                  - input format Q15, output format Q<log2(N)>.<15-log2(N)>; example: N=16 -> log2(N) = 4 -> output format Q4.11
 *                  - Assumptions:
 *                                  1. w - generated with qhdsp_fft_gen_twiddles_complex_ach() function
 */
void c1dfft_h(const int32_t *input, int32_t N, const int32_t *w, int32_t *output);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _QHDSP_FFT_COMPLEX_H */
