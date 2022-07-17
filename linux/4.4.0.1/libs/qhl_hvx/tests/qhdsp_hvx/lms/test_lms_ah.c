/**=============================================================================
@file
    test_lms_ah.c

@brief
    Accuracy test for qhdsp_hvx_lms_ah function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include <hexagon_types.h>

#include "qhdsp_hvx.h"
#include "qhblas_test.h"

static range_int16_t ranges0[] =
{
    { -1000, -100  },
    { -100,   100  },
    {  100,   1000 }
};

static int16_t input[ARR_LEN];
static int16_t ydes[ARR_LEN];
static int16_t rate = 0x2666;
static int16_t __attribute__((aligned(128))) states[ARR_LEN];
static int16_t __attribute__((aligned(128))) coeffs[ARR_LEN];
static int16_t error[ARR_LEN];
static int16_t output[ARR_LEN];
static int16_t orig_coeffs[ARR_LEN];
static int16_t ref_states[ARR_LEN];
static int16_t ref_coeffs[ARR_LEN];
static int16_t ref_error[ARR_LEN];
static int16_t ref_output[ARR_LEN];

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))
#define mult_r(x, y)  Q6_R_asrh_R(Q6_R_mpy_RlRl_s1_rnd_sat(x, y))

static void lms(int16_t input, int16_t ydes, int16_t rate, uint32_t taps,
                int16_t *states, int16_t *coeffs, int16_t *error,
                int16_t *output)
{
    int16_t out, err;
    int32_t sum;

    states[0] = input;
    sum = 0;

    for(uint32_t i = 0; i < taps; i++)
    {
        sum += coeffs[i] * states[i];
    }

    out = Q6_R_sath_R(Q6_P_asr_PR(sum, 15));
    *output = out;

    err = Q6_R_sub_RlRl_sat(ydes, out);
    *error = err;

    err = mult_r(err, rate);

    for(uint32_t i = 0; i < taps; i++)
    {
        coeffs[i] = Q6_R_add_RlRl_sat(coeffs[i], mult_r(err, states[i]));
    }

    for(int32_t i = taps - 1; i > 0; i--)
    {
        states[i] = states[i-1];
    }
}

inline static void generate_ref_output(uint32_t taps)
{
    for (uint32_t i = 0; i < ARR_LEN; ++i)
    {
        lms(input[i], ydes[i], rate, taps, ref_states,
            ref_coeffs, ref_error + i, ref_output + i);
    }
}

int main(void)
{
    printf("Test for qhdsp_hvx_lms_ah function:\n");

    INIT_INPUT_INT16(input, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT16(orig_coeffs, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT16(ydes, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    uint32_t pass = 0;
    uint32_t fail = 0;
    int32_t failed = 0;

    // Go through variable size of taps.
    for (uint32_t i = 64; i <= ARR_LEN; ++i)
    {
        failed = 0;

        // Clear buffers.
        memset(states, 0, sizeof(int16_t)*i);
        memset(output, 0, sizeof(int16_t)*ARR_LEN);
        memset(error, 0, sizeof(int16_t)*ARR_LEN);
        memset(ref_states, 0, sizeof(int16_t)*i);

        // Copy original data.
        memcpy(coeffs, orig_coeffs, sizeof(int16_t)*i);
        memcpy(ref_coeffs, orig_coeffs, sizeof(int16_t)*i);

        // Call functions.
        generate_ref_output(i);
        qhdsp_hvx_lms_ah(input, ydes, rate, i, states, coeffs,
                         error, output, ARR_LEN);

        // Check errors and outputs.
        for (uint32_t j = 0; j < ARR_LEN; ++j)
        {
            if (ref_error[j] != error[j])
            {
                failed = 1;

                printf("Error:\tError buffer | got = %d | OK = %d | "
                       "input = %d | input length = %d | taps = %ld | "
                       "iter = %ld\n",
                       error[j], ref_error[j], input[j], ARR_LEN, i, j);
            }

            if (ref_output[j] != output[j])
            {
                failed = 1;

                printf("Error:\tOutput buffer | got = %d | OK = %d | "
                       "input = %d | input length = %d | taps = %ld | "
                       "iter = %ld\n",
                       output[j], ref_output[j], input[j], ARR_LEN, i, j);
            }
        }

        // Check states and coeffs.
        for (uint32_t j = 0; j < i; ++j)
        {
            if ((ref_states[j] != states[j]) && j != 0)
            {
                failed = 1;

                printf("Error:\tStates | got = %d | OK = %d | "
                       "input length = %d | taps = %ld | iter = %ld\n",
                       states[j], ref_states[j], ARR_LEN, i, j);
            }

            if (ref_coeffs[j] != coeffs[j])
            {
                failed = 1;

                printf("Error:\tCoeffs | got = %d | OK = %d | "
                       "input length = %d | taps = %ld | iter = %ld\n",
                       coeffs[j], ref_coeffs[j], ARR_LEN, i, j);
            }
        }

        fail += failed;
        pass += !failed;
    }

    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");
    printf("(%lu tests passed, %lu failed)\n", pass, fail);

    return 0;
}
