/**=============================================================================
@file
    test_lms_ahf.c

@brief
    Accuracy test for qhdsp_hvx_lms_ahf function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>
#include <hexagon_types.h>

#include "qhdsp_hvx.h"
#include "qhblas_test.h"

#define TOLERANCE 0.5f

static range_float_t ranges0[] =
{
    { -1.0f,   -0.001  },
    { -0.001f, -0.0f   },
    {  0.0f,    0.001f },
    {  0.001f,  1.0f   }
};

static __fp16 input[ARR_LEN];
static __fp16 ydes[ARR_LEN];
static __fp16 rate = 0.0001f;
static __fp16 __attribute__((aligned(128))) states[ARR_LEN];
static __fp16 __attribute__((aligned(128))) coeffs[ARR_LEN];
static __fp16 error[ARR_LEN];
static __fp16 output[ARR_LEN];
static __fp16 orig_coeffs[ARR_LEN];
static float ref_states[ARR_LEN];
static float ref_coeffs[ARR_LEN];
static float ref_error[ARR_LEN];
static float ref_output[ARR_LEN];

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))

static int32_t check_diff(float x, __fp16 *y, float tolerance)
{
    float diff = x - (float) *y;
    if (diff < 0) diff = -diff;

    return diff > tolerance;
}

static void lms(float input, float ydes, float rate, uint32_t taps,
                float *states, float *coeffs, float *error, float *output)
{
    float out, err, sum;

    states[0] = input;
    sum = 0;

    for(uint32_t i = 0; i < taps; i++)
    {
        sum += coeffs[i] * states[i];
    }

    out = sum;
    *output = out;

    err = ydes - out;
    *error = err;

    err = err * rate;

    for(uint32_t i = 0; i < taps; i++)
    {
        coeffs[i] = coeffs[i] + err * states[i];
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
        lms((float)input[i], (float)ydes[i], (float)rate, taps,
            ref_states, ref_coeffs, ref_error + i, ref_output + i);
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;
    int32_t failed = 0;

    printf("Test for qhdsp_hvx_lms_ahf function:\n");

    INIT_INPUT_FLOAT16(input, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_FLOAT16(orig_coeffs, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_FLOAT16(ydes, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    // Go through variable size of taps.
    for (uint32_t i = 64; i <= ARR_LEN; ++i)
    {
        failed = 0;

        // Clear buffers.
        memset(states, 0, sizeof(__fp16)*i);
        memset(output, 0, sizeof(__fp16)*ARR_LEN);
        memset(error, 0, sizeof(__fp16)*ARR_LEN);
        memset(ref_states, 0, sizeof(float)*i);

        // Copy original data.
        memcpy(coeffs, orig_coeffs, sizeof(__fp16)*i);
        for (uint32_t j = 0; j < i; ++j)
        {
            ref_coeffs[j] = (float) orig_coeffs[j];
        }

        // Call functions.
        generate_ref_output(i);
        qhdsp_hvx_lms_ahf(input, ydes, &rate, i, states, coeffs,
                          error, output, ARR_LEN);

        // Check errors and outputs.
        for (uint32_t j = 0; j < ARR_LEN; ++j)
        {
            if (check_diff(ref_error[j], error + j, TOLERANCE))
            {
                failed = 1;

                printf("Error:\tError buffer | got = %f | OK = %f | "
                       "input = %f | input length = %d | taps = %ld | "
                       "iter = %ld\n",
                       error[j], ref_error[j], input[j], ARR_LEN, i, j);
            }

            if (check_diff(ref_output[j], output + j, TOLERANCE))
            {
                failed = 1;

                printf("Error:\tOutput buffer | got = %f | OK = %f | "
                       "input = %f | input length = %d | taps = %ld | "
                       "iter = %ld\n",
                       output[j], ref_output[j], input[j], ARR_LEN, i, j);
            }
        }

        // Check states and coeffs.
        for (uint32_t j = 0; j < i; ++j)
        {
            if (check_diff(ref_states[j], states + j, 0) && j != 0)
            {
                failed = 1;

                printf("Error:\tStates | got = %f | OK = %f | "
                       "input length = %d | taps = %ld | iter = %ld\n",
                       states[j], ref_states[j], ARR_LEN, i, j);
            }

            if (check_diff(ref_coeffs[j], coeffs + j, TOLERANCE))
            {
                failed = 1;

                printf("Error:\tCoeffs | got = %f | OK = %f | "
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

#else /* __HEXAGON_ARCH__ >= 68 */
#include <stdio.h>
int main()
{
    printf("HVX float is not supported for this architecture.\n");
    return 0;
}
#endif /* __HEXAGON_ARCH__ >= 68 */
