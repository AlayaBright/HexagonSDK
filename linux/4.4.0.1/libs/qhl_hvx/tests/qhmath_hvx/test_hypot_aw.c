/**=============================================================================
@file
    test_hypot_aw.c

@brief
    Accuracy test for qhmath_hvx_hypot_aw function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include "qhmath_hvx.h"
#include "qhblas_test.h"
#include "qhmath.h"

static range_int32_t ranges0[] = {
    {  INT32_MIN,  -1000      },
    { -1000,        1000      },
    {  1000,        INT32_MAX }
};

static int32_t input_arr1[ARR_LEN];
static int32_t input_arr2[ARR_LEN];
static int32_t output[ARR_LEN];
static int32_t ref_output[ARR_LEN];

static test_invalid_w2 test_data[] = {
    { input_arr1, input_arr2, output, 0,       -1 },
    { input_arr1, input_arr2, NULL,   ARR_LEN, -1 },
    { input_arr1, input_arr2, NULL,   0,       -1 },
    { input_arr1, NULL,       output, ARR_LEN, -1 },
    { input_arr1, NULL,       output, 0,       -1 },
    { input_arr1, NULL,       NULL,   ARR_LEN, -1 },
    { input_arr1, NULL,       NULL,   0,       -1 },
    { NULL,       input_arr2, output, ARR_LEN, -1 },
    { NULL,       input_arr2, output, 0,       -1 },
    { NULL,       input_arr2, NULL,   ARR_LEN, -1 },
    { NULL,       input_arr2, NULL,   0,       -1 },
    { NULL,       NULL,       output, ARR_LEN, -1 },
    { NULL,       NULL,       output, 0,       -1 },
    { NULL,       NULL,       NULL,   ARR_LEN, -1 },
    { NULL,       NULL,       NULL,   0,       -1 },
    { input_arr1, input_arr2, output, ARR_LEN,  0 }
};

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(range_int32_t))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(test_invalid_w2))

static const float SCALE_Q31 = 2147483648.0;  // 2^31 in float
static const float SCALE_Q30 = 1073741824.0;  // 2^30 in float

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        float square_a = ((float)input_arr1[i]/SCALE_Q31) * ((float)input_arr1[i]/SCALE_Q31);
        float square_b = ((float)input_arr2[i]/SCALE_Q31) * ((float)input_arr2[i]/SCALE_Q31);
        ref_output[i] = (int32_t)(qhmath_sqrt_f(square_a + square_b)*SCALE_Q30);
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;
    int32_t tolerance = 26977042;

    INIT_INPUT_INT32(input_arr1, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT32(input_arr2, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A2(qhmath_hvx_hypot_aw)

    generate_ref_output(ARR_LEN);

    for (uint32_t i = 1; i <= ARR_LEN; ++i)
    {
        memset(output, 0, sizeof(__typeof__(*output)) * ARR_LEN);

        qhmath_hvx_hypot_aw(input_arr1, input_arr2, output, i);

        for (uint32_t j = 0; j < i; ++j)
        {
            int32_t error = ref_output[j] - output[j];

            if (error < 0)
                error = -error;
            if (error > tolerance)
            {
                printf("Error:\tx = { %.7ld, %.7ld } | "
                       "qhmath_hvx_hypot_aw(x) = %.7ld | OK = %.7ld "
                       "| vector length = %lu | i = %lu\n",
                       input_arr1[j], input_arr2[j], output[j],
                       ref_output[j], i, j);
                ++fail;
            }
            else
            {
                ++pass;
            }
        }
    }

    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");
    printf("(%lu tests passed, %lu failed)\n", pass, fail);

    return 0;
}
