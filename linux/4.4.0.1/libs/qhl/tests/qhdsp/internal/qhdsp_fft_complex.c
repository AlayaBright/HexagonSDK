/**=============================================================================
@file
   qhdsp_fft_complex.c

@brief
   C implementation of complex 2^N FFT routines.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/
#if __hexagon__
#include <hexagon_protos.h>     // intrinsics
#include "qhcomplex.h"
#else
#ifdef __ARM_ACLE
#include <arm_acle.h>
#endif /* __ARM_ACLE */
#include <math.h>
#endif

#include "qhdsp_fft_internal.h"

/**
 * @brief           Complex 2^N FFT for single precision float point
 * @param[in]       input - input samples in time domain (complex)
 * @param[in]       N - number of samples on which FFT is performed
 * @param[in]       w - twiddle factors
 * @param[out]      output - FFT output buffer
 * @note
 *                  - Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 */
void c1dfft_f(const float complex *input, int32_t N, const float complex *w, float complex *output)
{
    int32_t i, j, k1, k2, n, m, LOG2N;
    float complex Wa, Wb, Wc;
    float complex A[4];

    const float complex *ptr;

    int32_t iBitrevBuffIdx;

#if __hexagon__
    LOG2N = Q6_R_ct0_R(N);
#else
    LOG2N = __builtin_ctz(N);
#endif


    if(N>=4)
    {
        // Stage 1
        // read input in bit-reversed order
        for (i = 0, m = 0; i < N; i += 4) {
            iBitrevBuffIdx = brev(i << (32-LOG2N));
            A[0] = input[iBitrevBuffIdx];
            iBitrevBuffIdx = brev((i+1) << (32-LOG2N));
            A[1] = input[iBitrevBuffIdx];
            iBitrevBuffIdx = brev((i+2) << (32-LOG2N));
            A[2] = input[iBitrevBuffIdx];
            iBitrevBuffIdx = brev((i+3) << (32-LOG2N));
            A[3] = input[iBitrevBuffIdx];

            radix4_btfly_f(A);

            Wb = w[m++]; // Wb = w[j];
            Wa = w[m++]; // Wa = w[2*j+1];
            Wc = w[m++]; // Wc = cmult_r(Wa,Wb);

            output[i] = A[0];
            output[i + 1] = A[1] * Wa;
            output[i + 2] = A[2] * Wb;
            output[i + 3] = A[3] * Wc;
        }
    }

    // Other Radix-4 stages
    k1 =  4; // # in each group
    k2 = N / 16; // # of groups

    for (n = LOG2N - 2; n > 2; n -= 2)
    {
        for (i = 0, m = 0; i < k2; i++)
        {
            Wb = w[m++]; // Wb = w[i];
            Wa = w[m++]; // Wa = w[2*i+1];
            Wc = w[m++]; // Wc = cmult_r(Wa,Wb);

            for (j = 0; j < k1; j++)
            {
                A[0] = output[(4 * i + 0) * k1 + j];
                A[1] = output[(4 * i + 1) * k1 + j];
                A[2] = output[(4 * i + 2) * k1 + j];
                A[3] = output[(4 * i + 3) * k1 + j];

                radix4_btfly_f(A);

                output[(4 * i + 0) * k1 + j] = A[0];
                output[(4 * i + 1) * k1 + j] = A[1] * Wa;
                output[(4 * i + 2) * k1 + j] = A[2] * Wb;
                output[(4 * i + 3) * k1 + j] = A[3] * Wc;
            }
        }
        k1 = k1 << 2;
        k2 = k2 >> 2;
    }

    if (n == 2)
    {
        // last Radix-4 stage
        for (j = 0; j < N / 4; j++)
        {
            A[0] = output[0 * (N / 4) + j];
            A[1] = output[1 * (N / 4) + j];
            A[2] = output[2 * (N / 4) + j];
            A[3] = output[3 * (N / 4) + j];

            radix4_btfly_f(A);

            // No multiplcations needed
            output[0 * (N / 4) + j] = A[0];
            output[1 * (N / 4) + j] = A[1];
            output[2 * (N / 4) + j] = A[2];
            output[3 * (N / 4) + j] = A[3];
        }
    }
    else
    {
        if(N==2)
            ptr = input;
        else
            ptr = output;

        if(N!=4)
        {
            // last Radix-2 stage
            for (i = 0; i < N / 2; i++)
            {
                A[0] = ptr[i];
                A[1] = ptr[i + N / 2];

                radix2_btfly_f(A);

                output[i] = A[0];
                output[i + N / 2] = A[1];
            }
        }
    }
}

/**
 * @brief           Complex 2^N FFT for 16x16 (complex number: bits 0:15-real part, bits 16:31-imag part) fixed-point
 * @param[in]       input - input samples in time domain (complex)
 * @param[in]       N - number of samples on which FFT is performed
 * @param[in]       w - twiddle factors
 * @param[out]      output - FFT output buffer
 * @note
 *                  - Function doesn't have qhdsp prefix in order to distinguish from assembly implementation.
 *                  - Scale factor: 1/N
 *                  - input format Q15, output format Q<log2(N)>.<15-log2(N)>; example: N=16 -> log2(N)=4 -> output format Q4.11
 *                  - Assumptions:
 *                                  1. w - generated with qhdsp_fft_gen_twiddles_complex_ach() function
 */
void c1dfft_h(const int32_t *input, int32_t N, const int32_t *w, int32_t *output)
{
    int32_t i, j, k1, k2, n, m;
    int32_t LOG2N;
    int32_t Wa,Wb,Wc;
    int32_t A[4];

    const int32_t *ptr;

#if __hexagon__
    LOG2N = Q6_R_ct0_R(N);
#else
    LOG2N = __builtin_ctz(N);
#endif

    if(N>=4)
    {
        /*************************************/
        /*    Stage 1                        */
        /*  read input in bit-reversed order */
        /*************************************/
        for(i=0, m=0; i<N; i+=4)
        {
            A[0] = input[ brev(i << (32-LOG2N)) ];
            A[1] = input[ brev((i+1) << (32-LOG2N)) ];
            A[2] = input[ brev((i+2) << (32-LOG2N)) ];
            A[3] = input[ brev((i+3) << (32-LOG2N)) ];

            radix4_btfly_h_qv3( A );

            Wb = w[m++];      // Wb = w[j];
            Wa = w[m++];      // Wa = w[2*j+1];
            Wc = w[m++];      // Wc = cmult_r(Wa,Wb);

            output[i  ] = A[0];
            output[i+1] = cmult_r( A[1], Wa );
            output[i+2] = cmult_r( A[2], Wb );
            output[i+3] = cmult_r( A[3], Wc );
        }
    }

    /************************************/
    /*  Other Radix-4 stages            */
    /************************************/
    k1=  4;                           // # in each group
    k2 = N/16;                        // # of groups

    for(n = LOG2N-2; n >2 ; n -=2)
    {
        for (i=0, m=0; i< k2; i++)
        {
            Wb = w[m++];      // Wb = w[i];
            Wa = w[m++];      // Wa = w[2*i+1];
            Wc = w[m++];      // Wc = cmult_r(Wa,Wb);

            for(j=0; j< k1; j++)
            {
                A[0] = output[(4*i + 0)*k1 + j ];
                A[1] = output[(4*i + 1)*k1 + j ];
                A[2] = output[(4*i + 2)*k1 + j ];
                A[3] = output[(4*i + 3)*k1 + j ];

                radix4_btfly_h_qv3( A );

                output[(4*i + 0)*k1 + j] = A[0];
                output[(4*i + 1)*k1 + j] = cmult_r(A[1], Wa);
                output[(4*i + 2)*k1 + j] = cmult_r(A[2], Wb);
                output[(4*i + 3)*k1 + j] = cmult_r(A[3], Wc);
            }
        }
        k1 = k1 << 2;
        k2 = k2 >> 2;
    }

    if(n == 2)
    {
        /************************************/
        /*  last Radix-4 stage              */
        /************************************/
        for(j=0; j< N/4; j++)
        {
            A[0] = output[0*(N/4) + j];
            A[1] = output[1*(N/4) + j];
            A[2] = output[2*(N/4) + j];
            A[3] = output[3*(N/4) + j];

            radix4_btfly_h( A );

            // No multiplcations needed
            output[0*(N/4) + j] = A[0];
            output[1*(N/4) + j] = A[1];
            output[2*(N/4) + j] = A[2];
            output[3*(N/4) + j] = A[3];
        }
    }
    else
    {
        if(N==2)
            ptr = input;
        else
            ptr = output;

        if(N!=4)
        {
            /************************************/
            /*  last Radix-2 stage              */
            /************************************/
            for(i=0; i<N/2; i++)\
            {
                A[0] = ptr[i    ];
                A[1] = ptr[i+N/2];

                radix2_btfly_h( A );

                output[i    ] = A[0];
                output[i+N/2] = A[1];
            }
        }
    }
}
