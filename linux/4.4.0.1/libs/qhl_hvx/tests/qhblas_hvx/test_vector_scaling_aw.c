/**=============================================================================
@file
    test_vector_scaling_aw.c

@brief
    Accuracy test for qhblas_hvx_vector_scaling_aw function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"

typedef struct test_invalid
{
    int32_t *input_arr;        // Pointer to first input array
    int32_t scaling_factor;    // Scaling factor
    int32_t *output;           // Pointer to output array
    uint32_t length;           // Length of input arrays
    int32_t expect_res;        // Expected return value
} test_invalid;

static range_int32_t ranges0[] =
{
    { -200000, -10000  },
    { -10000,  -1000   },
    { -1000,    1000   },
    {  1000,    10000  },
    {  10000,   200000 }
};

static int32_t input_arr[ARR_LEN];
static const int32_t scale_factor = (int32_t)(0.3*2147483648);
static int32_t output[ARR_LEN];
static int32_t ref_output[ARR_LEN];

static test_invalid test_data[] =
{
    { input_arr, scale_factor, output, 0,       -1 },
    { input_arr, scale_factor, NULL,   ARR_LEN, -1 },
    { input_arr, scale_factor, NULL,   0,       -1 },
    { NULL,      scale_factor, output, ARR_LEN, -1 },
    { NULL,      scale_factor, output, 0,       -1 },
    { NULL,      scale_factor, NULL,   ARR_LEN, -1 },
    { NULL,      scale_factor, NULL,   0,       -1 },
    { input_arr, scale_factor, output, ARR_LEN,  0 }
};

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(*test_data))

inline static void generate_ref_output(uint32_t size)
{
    int32_t mpy = 0;

    for (uint32_t i = 0; i < size; ++i)
    {
        mpy = ((int64_t)input_arr[i] * scale_factor + 0x40000000) >> 31;
        ref_output[i] = (mpy == 0x80000000 ? 0x7fffffff : mpy);
    }
}

int main(void)
{
    INIT_INPUT_INT32(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_SCALE(qhblas_hvx_vector_scaling_aw)

    TEST_INPUT_SCALE_INT16(qhblas_hvx_vector_scaling_aw)

    return 0;
}
