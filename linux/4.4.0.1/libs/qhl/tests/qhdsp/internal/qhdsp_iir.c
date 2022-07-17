/**=============================================================================
@file
   qhdsp_iir.c

@brief
   C implementation of IIR routines.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdint.h>
#include <stddef.h>
#include <hexagon_protos.h>     // intrinsics

#include "qhdsp_iir.h"

// cascaded biquad - transposed direct form 2 - float
int32_t iir_f(const float *in_samples, const float *coefs, float *states, uint32_t num_biquads, uint32_t length, float *out_samples)
{
    if(in_samples == NULL || coefs == NULL || states == NULL || num_biquads == 0 || length == 0 || out_samples == NULL)
    {
        return -1;
    }

    float y_n, s1_n, s2_n, x_n;
    float b0, b1, a1, b2, a2;

    float *in_samples_ptr;
    float *out_samples_ptr;

    in_samples_ptr = (float *)in_samples;

    for(uint32_t j = 0; j < num_biquads; j++)
    {
        s1_n = *states++;               // s1[n-1]
        s2_n = *states--;               // s2[n-1]
        b0 = *coefs++;
        b1 = *coefs++;
        a1 = *coefs++;
        b2 = *coefs++;
        a2 = *coefs++;

        out_samples_ptr = out_samples;

        for(uint32_t i = 0; i < length; i++)
        {
            x_n = *in_samples_ptr++;
            y_n = b0 * x_n + s1_n;             // y[n] = b0 * x[n] + s1[n-1]
            *out_samples_ptr++ = y_n;

            s1_n = s2_n + b1 * x_n - a1 * y_n; // s1[n] = s2[n-1] + b1 * x[n] - a1 * y[n]
            s2_n = b2 * x_n - a2 * y_n;        // s2[n] = b2 * x[n] - a2 * y[n]
        }

        in_samples_ptr = out_samples;

        *states++ = s1_n;
        *states++ = s2_n;
    }

    return 0;
}

// cascaded biquad - transposed direct form 2 - 16bit fixed point
#ifndef DOUBLE_PRECISION_IIR
int32_t iir_h(const int16_t *in_samples, const int16_t *coefs, int32_t *states, uint32_t num_biquads, uint32_t length, int16_t *out_samples)
{
    if(in_samples == NULL || coefs == NULL || states == NULL || num_biquads == 0 || length == 0 || out_samples == NULL)
    {
        return -1;
    }

    const int16_t *coef_ptr;
    int32_t *state_ptr;
    int32_t y_n, s1_n, s2_n;

    int16_t b0, b1, a1, b2, a2;
    int32_t x_n;
    int16_t shift_amt;

    for(uint32_t i = 0; i < length; i++)
    {
        coef_ptr = coefs;
        state_ptr = states;

        x_n = (int32_t)Q6_R_aslh_R(*in_samples++);  // align 16b PCM to MSB bits in 32b register

        for(uint32_t j = 0; j < num_biquads; j++)
        {
            s1_n = *state_ptr++;               // s1[n-1] - Q31
            s2_n = *state_ptr--;               // s2[n-1] - Q31

            b0 = *coef_ptr++;                  // Q15
            b1 = *coef_ptr++;                  // Q15
            a1 = *coef_ptr++;                  // Q15 / 2
            b2 = *coef_ptr++;                  // Q15
            a2 = *coef_ptr++;                  // Q15 / 2
            shift_amt = *coef_ptr++;           // shift amount

    //      y_n = b0 * x_n + s1_n;             // y[n] = b0 * x[n] + s1[n-1]
            y_n = s1_n;                        // y[n] = s1[n-1]
            y_n = Q6_R_mpyacc_RhRl_s1_sat(y_n, x_n, (int32_t) b0);    // y[n] += (b0 * x[n])<<1

    //      s1_n = s2_n + b1 * x_n - a1 * y_n; // s1[n] = s2[n-1] + b1 * x[n] - a1 * y[n]
            s1_n = Q6_R_mpy_RhRl(x_n, (int32_t) b1);    // b1 * x[n] - no shift
            s1_n = Q6_R_mpyacc_RhRl_s1_sat(s1_n, y_n, (int32_t) a1);    // b1 * x[n] + (-a1 * y[n])<<1
            s1_n = Q6_R_asl_RI(s1_n, 1);       // (b1 * x[n])<<1 - (a1 * y[n])<<2
            s1_n += s2_n;                      // s1[n] = s2[n-1] + (b1 * x[n])<<1 - (a1 * y[n])<<2

    //      s2_n = b2 * x_n - a2 * y_n;        // s2[n] = b2 * x[n] - a2 * y[n]
            s2_n = Q6_R_mpy_RhRl(x_n, b2);     // b2 * x[n] - no shift
            s2_n = Q6_R_mpyacc_RhRl_s1_sat(s2_n, y_n, (int32_t) a2);    // b2 * x[n] + (-a2 * y[n])<<1
            s2_n = Q6_R_asl_RI(s2_n, 1);       // s2[n] = s2[n] << 1

            x_n = (int32_t)Q6_P_asl_PR(y_n, shift_amt);    // set input for next cascade

            *state_ptr++ = s1_n;
            *state_ptr++ = s2_n;
        }

        *out_samples++ = (int16_t)Q6_R_asrh_R(x_n + 0x8000);    // add 0.5
    }

    return 0;
}
#else   // #ifndef DOUBLE_PRECISION_IIR
int32_t iir_h(const int16_t *in_samples, const int16_t *coefs, int64_t *states, uint32_t num_biquads, uint32_t length, int16_t *out_samples)
{
    if(in_samples == NULL || coefs == NULL || states == NULL || num_biquads == 0 || length == 0 || out_samples == NULL)
    {
        return -1;
    }

    const int16_t *coef_ptr;
    int64_t *state_ptr;
    int64_t y_n, y_n_p1, s1_n, s2_n;

    int16_t b0, b1, a1, b2, a2;
    int32_t x_n, x_n_p1;

    int64_t a_tmp, b_tmp, c_tmp, d_tmp;
    int16_t shift_amt;

    for(uint32_t i = 0; i < length; i+=2)
    {
        coef_ptr = coefs;
        state_ptr = states;

        x_n = (int32_t)Q6_R_aslh_R(*in_samples++);  // align 16b PCM to MSB bits in 32b register
        x_n_p1 = (int32_t)Q6_R_aslh_R(*in_samples++);  // align 16b PCM to MSB bits in 32b register

        for(uint32_t j = 0; j < num_biquads; j++)
        {
            s1_n = *state_ptr++;               // s1[n-1] - Q47
            s2_n = *state_ptr--;               // s2[n-1] - Q47

            b0 = *coef_ptr++;                  // Q15
            b1 = *coef_ptr++;                  // Q15
            a1 = *coef_ptr++;                  // Q15 / 2
            b2 = *coef_ptr++;                  // Q15
            a2 = *coef_ptr++;                  // Q15 / 2
            shift_amt = *coef_ptr++;           // shift amount

    //      y_n = b0 * x_n + s1_n;             // y[n] = b0 * x[n] + s1[n-1]
            y_n = s1_n;                        // y[n] = s1[n-1]
            y_n = Q6_P_vrmpywehacc_PP_s1(y_n, x_n, (int32_t) b0);    // y[n] = b0 * x[n] + s1[n-1]
            y_n = Q6_P_extract_PII(y_n, 32, 16);    // get 32 MSB bits out of 48bit result

    //      s1_n = s2_n + b1 * x_n - a1 * y_n; // s1[n] = s2[n-1] + b1 * x[n] - a1 * y[n]
            s1_n = Q6_P_vrmpyweh_PP(x_n, (int32_t) b1);    // b1 * x[n] - no shift
            s1_n = Q6_P_vrmpywehacc_PP_s1(s1_n, y_n, (int32_t) a1);    // b1 * x[n] + (-a1 * y[n])<<1
            s1_n = Q6_P_asl_PI(s1_n, 1);       // (b1 * x[n])<<1 - (a1 * y[n])<<2
            s1_n += s2_n;                      // s1[n] = s2[n-1] + (b1 * x[n])<<1 - (a1 * y[n])<<2

            y_n_p1 = s1_n;                        // y[n] = s1[n-1]
            y_n_p1 = Q6_P_vrmpywehacc_PP_s1(y_n_p1, x_n_p1, (int32_t) b0);    // y[n] = b0 * x[n] + s1[n-1]
            y_n_p1 = Q6_P_extract_PII(y_n_p1, 32, 16);    // get 32 MSB bits out of 48bit result

    //      s2_n = b2 * x_n - a2 * y_n;        // s2[n] = b2 * x[n] - a2 * y[n]
            #if 0
            s2_n = Q6_P_vrmpyweh_PP(x_n, (int32_t) b2);    // b2 * x[n] - no shift
            s2_n = Q6_P_vrmpywehacc_PP_s1(s2_n, y_n, (int32_t) a2);    // b2 * x[n] + (-a2 * y[n])<<1
            s2_n = Q6_P_asl_PI(s2_n, 1);       // s2[n] = s2[n] << 1

            s1_n = Q6_P_vrmpyweh_PP(x_n_p1, (int32_t) b1);    // b1 * x[n] - no shift
            s1_n = Q6_P_vrmpywehacc_PP_s1(s1_n, y_n_p1, (int32_t) a1);    // b1 * x[n] + (-a1 * y[n])<<1
            s1_n = Q6_P_asl_PI(s1_n, 1);       // (b1 * x[n])<<1 - (a1 * y[n])<<2
            s1_n += s2_n;                      // s1[n] = s2[n-1] + (b1 * x[n])<<1 - (a1 * y[n])<<2
            #else
            a_tmp = Q6_P_combine_RR(x_n_p1, x_n);
            b_tmp = Q6_P_combine_RR((int32_t) b1, (int32_t) b2);
            c_tmp = Q6_P_combine_RR(y_n_p1, y_n);
            d_tmp = Q6_P_combine_RR((int32_t) a1, (int32_t) a2);

            s1_n = Q6_P_vrmpyweh_PP(a_tmp, b_tmp);
            s1_n = Q6_P_vrmpywehacc_PP_s1(s1_n, c_tmp, d_tmp);
            s1_n = Q6_P_asl_PI(s1_n, 1);
            #endif

            s2_n = Q6_P_vrmpyweh_PP(x_n_p1, (int32_t) b2);    // b2 * x[n] - no shift
            s2_n = Q6_P_vrmpywehacc_PP_s1(s2_n, y_n_p1, (int32_t) a2);    // b2 * x[n] + (-a2 * y[n])<<1
            s2_n = Q6_P_asl_PI(s2_n, 1);       // s2[n] = s2[n] << 1

            x_n = (int32_t)Q6_P_asl_PR(y_n, shift_amt);    // set input for next cascade
            x_n_p1 = (int32_t)Q6_P_asl_PR(y_n_p1, shift_amt);    // set input for next cascade

            *state_ptr++ = s1_n;
            *state_ptr++ = s2_n;
        }

        *out_samples++ = (int16_t)Q6_R_asrh_R(x_n + 0x8000);    // add 0.5
        *out_samples++ = (int16_t)Q6_R_asrh_R(x_n_p1 + 0x8000);    // add 0.5
    }

    return 0;
}
#endif  // #ifndef DOUBLE_PRECISION_IIR
