/**=============================================================================
@file
   qhdsp_iir.h

@brief
   Header file for C implementation of IIR routines.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _QHDSP_IIR_H
#define _QHDSP_IIR_H

#ifdef __cplusplus
extern "C" {
#endif

#define DOUBLE_PRECISION_IIR

/** @defgroup iir_internal_routines IIR internal routines
  * @{
 */

/**
 * @brief           IIR (cascaded biquad) filtering on a bulk of data - float point.
 * @param[in]       in_samples - input samples in time domain
 * @param[in]       coefs - IIR filter coefficients - arranged as b0, b1, a1, b2, a2
 * @param[in,out]   states - pointer to filter states buffer
 * @param[in]       num_biquads - number of consecutive IIR filter calls (with different coeffs)
 * @param[in]       length - length of input/output data (number of samples)
 * @param[out]      out_samples - output buffer
 * @note
 *                  - Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 */
int32_t iir_f(const float *in_samples, const float *coefs, float *states, uint32_t num_biquads, uint32_t length, float *out_samples);

/**
 * @brief           IIR (cascaded biquad) filtering on a bulk of data - 16-bit fixed point.
 * @param[in]       in_samples - input samples in time domain <Q15>
 * @param[in]       coefs - IIR filter coefficients - arranged as b0, b1, a1, b2, a2 <Q15>
 * @param[in,out]   states - pointer to filter states buffer <Q31>/<Q47>
 * @param[in]       num_biquads - number of consecutive IIR filter calls (with different coeffs)
 * @param[in]       length - length of input/output data (number of samples)
 * @param[out]      out_samples - output buffer <Q15>
 * @note
 *                  - Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 */
#ifndef DOUBLE_PRECISION_IIR
int32_t iir_h(const int16_t *in_samples, const int16_t *coefs, int32_t *states, uint32_t num_biquads, uint32_t length, int16_t *out_samples);
#else
int32_t iir_h(const int16_t *in_samples, const int16_t *coefs, int64_t *states, uint32_t num_biquads, uint32_t length, int16_t *out_samples);
#endif

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _QHDSP_IIR_H */
