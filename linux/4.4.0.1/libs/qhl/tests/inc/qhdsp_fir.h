/**=============================================================================
@file
   qhdsp_fir.h

@brief
   Header file for C implementation of FIR routines.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _QHDSP_FIR_H
#define _QHDSP_FIR_H

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup fir_internal_routines FIR internal routines
  * @{
 */

/**
 * @brief           FIR filtering on a bulk of data - 16b fixed point.
 * @param[in]       in_samples - input samples in time domain <Q15>
 * @param[in]       coefs - FIR filter coefficients <Q15>
 * @param[in]       taps - number of filter taps
 * @param[in]       length - length of input/output data (number of samples)
 * @param[out]      out_samples - output buffer <Q15>
 * @note
 *                  - Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 *                  - Assumptions:
 *                                  1. coefficients arranged in reversed order
 *                                  2. in_samples buffer has to be of (taps + length) size
 *                                  3. new incoming samples stored at [taps-1] offset in in_samples buffer
 */
int32_t fir_h(int16_t *in_samples, int16_t *coefs, uint32_t taps, uint32_t length, int16_t *out_samples);

/**
 * @brief           FIR filtering on a bulk of data - float point.
 * @param[in]       in_samples - input samples in time domain
 * @param[in]       delay_line - delay buffer (size is taps samples)
 * @param[in]       coefs - FIR filter coefficients
 * @param[in]       taps - number of filter taps
 * @param[in]       length - length of input/output data (number of samples)
 * @param[out]      out_samples - output buffer
 * @note
 *                  - Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 */
int32_t fir_f(const float *in_samples, float *delay_line, const float *coefs, uint32_t taps, uint32_t length, float *out_samples);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _QHDSP_FIR_H */
