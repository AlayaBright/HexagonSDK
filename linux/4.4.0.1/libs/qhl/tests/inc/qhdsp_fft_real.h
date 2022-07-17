/**=============================================================================
@file
   qhdsp_fft_real.h

@brief
   Header file for real 2^N FFT routines.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _QHDSP_FFT_REAL_H
#define _QHDSP_FFT_REAL_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup fft_internal_routines FFT internal routines
  * @{
 */

/**
 * @brief           Real 2^N FFT for single precision float point
 * @param[in]       input - input samples in time domain (real)
 * @param[in]       N - number of samples on which FFT is performed
 * @param[in]       Wt1 - twiddle factors - for N/2-point complex FFT
 * @param[in]       Wt2 - twiddle factors - for last stage
 * @param[out]      output - FFT output buffer
 * @note
 *                  - Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 *                  - Assumptions:
 *                                  1. Wt1 & Wt2 - generated with qhdsp_fft_gen_twiddles_real_acf() function
 *                  - Constraints:
 *                                  1. N has to be >= 4 (and power of 2)
 */
void r1dfft_f(const float* input, int32_t N, const float complex *Wt1, const float complex *Wt2, float complex *output);

/**
 * @brief           Real 2^N FFT for 16x16 (complex number: bits 0:15-real part, bits 16:31-imag part) fixed-point
 * @param[in]       input - input samples in time domain (real)
 * @param[in]       N - number of samples on which FFT is performed
 * @param[in]       Wt1 - twiddle factors - for N/2-point complex FFT
 * @param[in]       Wt2 - twiddle factors - for last stage
 * @param[out]      output - FFT output buffer
 * @note            
 *                  - Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 *                  - input format Q15, output format Q<log2(N)>.<15-log2(N)>; example: N=16 -> log2(N) = 4 -> output format Q4.11
 *                  - Assumptions:
 *                                  1. Wt1 & Wt2 - generated with qhdsp_fft_gen_twiddles_real_ach() function
 *                  - Constraints:
 *                                  1. N has to be >= 4 (and power of 2)
 */
void r1dfft_h(const int16_t* input, int32_t N, const int32_t* Wt1, const int32_t* Wt2, int32_t* output);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _QHDSP_FFT_REAL_H */
