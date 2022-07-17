/**=============================================================================
@file
    test_i_vector_norm_ah.c

@brief
    Accuracy test for qhblas_hvx_i_vector_norm_ah function.

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhblas_hvx.h"
#include "qhblas_test.h"
#include <math.h>

typedef struct test_invalid
{
    int16_t *input_arr;    // Pointer to input array
    int64_t *output;       // Pointer to output
    uint32_t length;       // Length of input arrays
    int32_t expect_res;    // Expected return value
} test_invalid;

static range_int16_t ranges0[] =
{
    { INT16_MIN, -10000     },
    { -10000,    -1000      },
    { -1000,      1000      },
    {  1000,      10000     },
    {  10000,     INT16_MAX }
};

static int16_t input_arr[ARR_LEN];
static int64_t output;

static test_invalid test_data[] =
{
    { input_arr, &output, 0,       -1 },
    { input_arr, NULL,    ARR_LEN, -1 },
    { input_arr, NULL,    0,       -1 },
    { NULL,      &output, ARR_LEN, -1 },
    { NULL,      &output, 0,       -1 },
    { NULL,      NULL,    ARR_LEN, -1 },
    { NULL,      NULL,    0,       -1 },
    { input_arr, &output, ARR_LEN,  0 }
};

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(*test_data))

inline static int64_t generate_ref_output(uint32_t size)
{
    double out_double, out_double_sqrt;
    int64_t ref_output = 0;

    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output += (int64_t)input_arr[i] * (int64_t)input_arr[i];
    }

    ref_output <<= 1;
    out_double = (double)ref_output / (double)9223372036854775808LLU;
    out_double_sqrt = sqrt(out_double) * (double)9223372036854775808LLU;
    return (int64_t) out_double_sqrt;
}

int main(void)
{
    INIT_INPUT_INT16(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A1(qhblas_hvx_i_vector_norm_ah)

    TEST_INPUT_SCALAR_INT16(qhblas_hvx_i_vector_norm_ah, ARR_LEN, output, input_arr)

    return 0;
}
