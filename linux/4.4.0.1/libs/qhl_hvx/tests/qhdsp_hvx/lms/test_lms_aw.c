/**=============================================================================
@file
    test_lms_aw.c

@brief
    Accuracy test for qhdsp_hvx_lms_aw function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include <hexagon_types.h>

#include "qhdsp_hvx.h"
#include "qhblas_test.h"

static range_int32_t ranges0[] =
{
    { -800000, -10000  },
    { -10000,  -1000   },
    { -1000,    1000   },
    {  1000,    10000  },
    {  10000,   800000 }
};

static int32_t input[ARR_LEN];
static int32_t ydes[ARR_LEN];
static int32_t rate = 0x26660000;
static int32_t __attribute__((aligned(128))) states[ARR_LEN];
static int32_t __attribute__((aligned(128))) coeffs[ARR_LEN];
static int32_t error[ARR_LEN];
static int32_t output[ARR_LEN];
static int32_t orig_coeffs[ARR_LEN];
static int32_t ref_states[ARR_LEN];
static int32_t ref_coeffs[ARR_LEN];
static int32_t ref_error[ARR_LEN];
static int32_t ref_output[ARR_LEN];

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))

static int32_t mult(int32_t x, int32_t y)
{
    int32_t res = ((int64_t)x * (int64_t)y + 0x40000000) >> 31;
    return (res == 0x80000000 ? 0x7fffffff : res);
}

static void lms(int32_t input, int32_t ydes, int32_t rate, uint32_t taps,
                int32_t *states, int32_t *coeffs, int32_t *error,
                int32_t *output)
{
    int32_t out, err, sum;

    states[0] = input;
    sum = 0;

    for(uint32_t i = 0; i < taps; i++)
    {
        sum += mult(coeffs[i], states[i]);
    }

    out = sum;
    *output = out;

    err = Q6_R_sub_RR_sat(ydes, out);
    *error = err;

    err = mult(err, rate);

    for(uint32_t i = 0; i < taps; i++)
    {
        coeffs[i] = Q6_R_add_RR_sat(coeffs[i], mult(err, states[i]));
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
    printf("Test for qhdsp_hvx_lms_aw function:\n");

    INIT_INPUT_INT32(input, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT32(orig_coeffs, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT32(ydes, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    uint32_t pass = 0;
    uint32_t fail = 0;
    int32_t failed = 0;

    // Go through variable size of taps.
    for (uint32_t i = 64; i <= ARR_LEN; ++i)
    {
        failed = 0;

        // Clear buffers.
        memset(states, 0, sizeof(int32_t)*i);
        memset(output, 0, sizeof(int32_t)*ARR_LEN);
        memset(error, 0, sizeof(int32_t)*ARR_LEN);
        memset(ref_states, 0, sizeof(int32_t)*i);

        // Copy original data.
        memcpy(coeffs, orig_coeffs, sizeof(int32_t)*i);
        memcpy(ref_coeffs, orig_coeffs, sizeof(int32_t)*i);

        // Call functions.
        generate_ref_output(i);
        qhdsp_hvx_lms_aw(input, ydes, rate, i, states, coeffs,
                         error, output, ARR_LEN);

        // Check errors and outputs.
        for (uint32_t j = 0; j < ARR_LEN; ++j)
        {
            if (ref_error[j] != error[j])
            {
                failed = 1;

                printf("Error:\tError buffer | got = %ld | OK = %ld | "
                       "input = %ld | input length = %d | taps = %ld | "
                       "iter = %ld\n",
                       error[j], ref_error[j], input[j], ARR_LEN, i, j);
            }

            if (ref_output[j] != output[j])
            {
                failed = 1;

                printf("Error:\tOutput buffer | got = %ld | OK = %ld | "
                       "input = %ld | input length = %d | taps = %ld | "
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

                printf("Error:\tStates | got = %ld | OK = %ld | "
                       "input length = %d | taps = %ld | iter = %ld\n",
                       states[j], ref_states[j], ARR_LEN, i, j);
            }

            if (ref_coeffs[j] != coeffs[j])
            {
                failed = 1;

                printf("Error:\tCoeffs | got = %ld | OK = %ld | "
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
