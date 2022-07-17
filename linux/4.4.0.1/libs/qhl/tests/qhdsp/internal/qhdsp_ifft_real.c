/**=============================================================================
@file
   qhdsp_ifft_real.c

@brief
   Real 2^N IFFT routines.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdint.h>
#if __hexagon__
#include <stdlib.h>
#else
#include <malloc.h>
#endif
#include "qhdsp_fft_internal.h"
#include "qhdsp_ifft_complex.h"
#include "qhdsp_internal.h"

/**
 * @brief           Real 2^N IFFT for single precision float point
 * @param[in]       input - input samples in frequency domain
 * @param[in]       n - number of samples on which FFT is performed
 * @param[in]       Wt1 - twiddle factors - for N/2-point complex FFT
 * @param[in]       Wt2 - twiddle factors - for last stage
 * @param[out]      output - IFFT output buffer
 * @note            Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 */
void r1difft_f(const float complex* input, int32_t N, const float complex* Wt1, const float complex* Wt2, float complex* output)
{
    float complex re, im;
    float complex *c1difft_f_input_ptr;

    if(N > MAX_COMPLEX_FLOATS_ON_STACK)
    {
        // handle malloc failure - TBD!
        c1difft_f_input_ptr = (float complex *)memalign(N/2*sizeof(float complex), N/2*sizeof(float complex));
    }
    else
    {
        c1difft_f_input_ptr = __builtin_alloca(N*sizeof(float complex));
        int32_t alignment = N/2*sizeof(float complex);
        int32_t offset = alignment - 1;
        c1difft_f_input_ptr = (float complex *)(((int32_t)c1difft_f_input_ptr + offset) & ~(alignment - 1));
    }

    re = input[0] + conj_f(input[N/2]);
    im = input[0] - conj_f(input[N/2]);

    re = 0.5 * re;
    im = (float complex)(0 + 0.5*1i) * im;    // e^((j*2*Pi*k)/N) = 1 when k=0

    c1difft_f_input_ptr[0] = re + im;

    for (int32_t i = 1; i < N / 2; i++)
    {
        re = input[i] + conj_f(input[N/2-i]);
        im = input[i] - conj_f(input[N/2-i]);
        re = 0.5 * re;
        im = im * conj_f(Wt2[i-1]);

        c1difft_f_input_ptr[i] = re + im;
    }

    c1difft_f(c1difft_f_input_ptr, N/2, Wt1, output);

    if(N > MAX_COMPLEX_FLOATS_ON_STACK)
    {
        free(c1difft_f_input_ptr);
    }
}

/**
 * @brief           Real 2^N IFFT for 16x16 (complex number: bits 0:15-real part, bits 16:31-imag part) fixed-point
 * @param[in]       input - input samples in frequency domain
 * @param[in]       N - number of samples on which FFT is performed
 * @param[in]       Wt1 - twiddle factors - for N/2-point complex FFT
 * @param[in]       Wt2 - twiddle factors - for last stage
 * @param[out]      output - IFFT output buffer
 * @note            Function doesn't have qhcomplex prefix in order to distinguish from assembly implementation.
 */
void r1difft_h(const int32_t *input, int32_t N, const int32_t *Wt1, const int32_t *Wt2, int16_t *output)
{
    int32_t X, Y;
    int32_t *complex_ifft_in_buff;
    int32_t Y_multiplier;

    if(N > MAX_COMPLEX_FXPs_ON_STACK/2)
    {
        // handle malloc failure - TBD!
        complex_ifft_in_buff = (int32_t *)memalign(N/2*sizeof(int32_t), N/2*sizeof(int32_t));
    }
    else
    {
        complex_ifft_in_buff = __builtin_alloca(2*N*sizeof(int32_t));   // 2*N to cover worst case 
        int32_t alignment = N/2*sizeof(int32_t)*2;  // sizeof(int32_t)*2 - required 8 byte alignment
        int32_t offset = alignment - 1;
        complex_ifft_in_buff = (int32_t *)(((int32_t)complex_ifft_in_buff + offset) & ~(alignment - 1));
    }

    Y_multiplier = create_complex(0,0x7fff);

    for (int32_t i = 0; i < N / 2; i++)
    {
        X = cadd(input[i], conjugate(input[N/2-i]));
        Y = csub(input[i], conjugate(input[N/2-i]));

        Y = cmult_cr(Y,Y_multiplier);
        Y_multiplier = Wt2[i];

        complex_ifft_in_buff[i] = csub(X,Y);
    }

    c1difft_h(complex_ifft_in_buff, N/2, Wt1, (int32_t *)output);

    if(N > MAX_COMPLEX_FXPs_ON_STACK/2)
    {
        free(complex_ifft_in_buff);
    }
}
