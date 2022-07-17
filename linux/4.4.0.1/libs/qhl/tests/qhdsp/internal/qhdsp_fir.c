/**=============================================================================
@file
   qhdsp_fir.c

@brief
   C implementation of FIR routines.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdint.h>
#include <string.h>
#include <hexagon_protos.h> // intrinsics

// taken from q6basic_op.h
#define   LL_shr(x, n)       (Q6_P_asr_PR((x),(n)))
#define   extract_h(x)       ((int16_t)Q6_R_asrh_R( (x) ))
#define   L_sat(x)           (Q6_R_sat_P( (x) ))
#define   LL_shl(x, n)       (Q6_P_asl_PR((x),(n)))

int32_t fir_f(const float *in_samples, float *delay_line, const float *coefs, uint32_t taps, uint32_t length, float *out_samples)
{
    if(in_samples == NULL || delay_line == NULL || coefs == NULL || taps == 0 || length == 0 || out_samples == NULL)
    {
        return -1;
    }

    float sum1, sum2;
    uint32_t dly_wr_idx = 0;
    uint32_t dly_rd1_idx;
    uint32_t dly_rd2_idx;

    for(uint32_t i = 0; i < length; i+=2)
    {
        sum2 = 0;

        delay_line[dly_wr_idx] = in_samples[i];

        dly_rd1_idx = dly_wr_idx;

        // update delay_line write index (++)
        dly_wr_idx = (dly_wr_idx + 1) < taps ? dly_wr_idx + 1 : 0;

        sum1 = coefs[taps-1] * delay_line[dly_wr_idx];

        dly_rd2_idx = dly_wr_idx;

        delay_line[dly_wr_idx] = in_samples[i+1];

        // update delay_line write index (++)
        dly_wr_idx = (dly_wr_idx + 1) < taps ? dly_wr_idx + 1 : 0;

        for(uint32_t j = 0; j < taps-1; j++)
        {
            sum1 += coefs[j] * delay_line[dly_rd1_idx];
            sum2 += coefs[j] * delay_line[dly_rd2_idx];

            // update delay_line read index (--)
            dly_rd1_idx = (dly_rd1_idx - 1) < 0 ? taps - 1 : dly_rd1_idx - 1;
            // update delay_line read index (--)
            dly_rd2_idx = (dly_rd2_idx - 1) < 0 ? taps - 1 : dly_rd2_idx - 1;
        }

        sum2 += coefs[taps-1] * delay_line[dly_rd2_idx];

        out_samples[i] = sum1;
        out_samples[i+1] = sum2;
    }

    return 0;
}

int32_t fir_h(int16_t *in_samples, int16_t *coefs, uint32_t taps, uint32_t length, int16_t *out_samples)
{
    if(in_samples == NULL || coefs == NULL || length == 0 || out_samples == NULL)
    {
        return -1;
    }

    int64_t sum;

    for(uint32_t i = 0; i < length; i++)
    {
        sum = 0;
        for(uint32_t j = 0; j < taps; j++)
        {
            sum += coefs[j] * in_samples[i+j];
        }

        sum = LL_shl(sum, 1);
        out_samples[i] = extract_h(L_sat(sum));
    }

    memmove((void *)in_samples, (const void *)&in_samples[taps], (length-1)*sizeof(int16_t));

    return 0;
}
