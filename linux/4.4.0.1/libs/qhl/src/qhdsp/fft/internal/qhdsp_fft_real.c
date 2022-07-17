/**=============================================================================
@file
   qhdsp_fft_real.c

@brief
   C implementation of real 2^N FFT routines.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdint.h>
#include "qhdsp_fft_internal.h"
#include "qhdsp_fft_complex.h"

/** @defgroup fft_real_routines Real 2^N FFT routines
  * @{
 */

/**
 * @brief           Real 2^N FFT for single precision float point
 * @param[in]       input - input samples in time domain (real)
 * @param[in]       n - number of samples on which FFT is performed
 * @param[in]       Wt1 - twiddle factors - for N/2-point complex FFT
 * @param[in]       Wt2 - twiddle factors - for last stage
 * @param[out]      output - FFT output buffer
 * @note
 *                  Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 *                  Slight modifications were done in order to improve performance.
 */
void r1dfft_f(const float* input, int32_t N, const float complex *Wt1, const float complex *Wt2, float complex *output)
{
    int32_t i;
    float complex X, Y;
    float complex *Z = output;

    // Construct complex array with even
    // input as real and odd input as
    // imaginary parts
    // Then do N/2-point complex FFT
    c1dfft_f((float complex*)input, N / 2, Wt1, Z);

    // Calculate last stage butterflies
    // calculate FFT at k=0, k=N/2
    X = creal_f(Z[0]) + 1i * 0;
    Y = cimag_f(Z[0]) + 1i * 0;

    output[0] = X + Y;
    output[N / 2] = X - Y;

    for (i = 1; i <= N / 4; i++)
    {
        X = Z[i] + conj_f(Z[N / 2 - i]);
        Y = Z[i] - conj_f(Z[N / 2 - i]);

        X = 0.5      * X;
        Y = Wt2[i - 1] * Y;

        output[i] = X + Y;

        if (i != N / 4)
        {
          output[N / 2 - i] = conj_f(X - Y);
        }
    }

    for (i = 1; i < N / 2; i++)
    {
        output[N - i] = conj_f(output[i]);
    }
}

/**
 * @brief           Real 2^N FFT for 16x16 (complex number: bits 0:15-real part, bits 16:31-imag part) fixed-point
 * @param[in]       input - input samples in time domain (real)
 * @param[in]       N - number of samples on which FFT is performed
 * @param[in]       Wt1 - twiddle factors - for N/2-point complex FFT
 * @param[in]       Wt2 - twiddle factors - for last stage
 * @param[out]      output - FFT output buffer
 * @note
 *                  Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 *                  Slight modifications were done in order to improve performance.
 */
void r1dfft_h(const int16_t* input, int32_t N, const int32_t* Wt1, const int32_t* Wt2, int32_t* output)
{
    int32_t i;
    int32_t X, Y;
    int32_t *Z = output;

    /*************************************/
    /* Construct complex array with even */
    /* input as real and odd input as    */
    /* imaginary parts                   */
    /* Then do N/2-point complex FFT     */
    /*************************************/

    c1dfft_h((int32_t *)input, N/2, Wt1, Z);

    /*************************************/
    /*  Calculate last stage butterflies */
    /**************************************/
    // calculate FFT at k=0, k=N/2
    X = create_complex(real(Z[0]), 0);
    Y = create_complex(imag(Z[0]), 0);

    output[0  ] = cavg(X,Y);
    output[N/2] = cnavg(X,Y);

    for (i=1; i<= N/4; i++)
    {
        X =  cavg(Z[i], conjugate(Z[N/2-i]));
        Y = cnavg(Z[i], conjugate(Z[N/2-i]));

        Y = cmult_r(Wt2[i-1],Y);
        output[i] = cnavg(X, Y);

        if (i!= N/4)
        {
            output[N/2-i] = conjugate(cavg(X, Y));
        }
    }

    for (i = 1; i < N / 2; i++)
    {
        output[N - i] = conjugate(output[i]);
    }
}

/**
  * @}
  */
