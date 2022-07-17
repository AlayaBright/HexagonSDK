/**=============================================================================
@file
    test_aw_vector_scaling_ah.c

@brief
    Accuracy test for qhblas_hvx_aw_vector_scaling_ah function

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
    int16_t *input_arr;        // Pointer to first input array
    int16_t scaling_factor;    // Scaling factor
    int32_t *output;           // Pointer to output array
    uint32_t length;           // Length of input arrays
    int32_t expect_res;        // Expected return value
} test_invalid;

static range_int16_t ranges0[] =
{
    {  INT16_MIN, -1000      },
    { -1000,      -100       },
    { -100,        100       },
    {  100,        1000      },
    {  1000,       INT16_MAX }
};

static int16_t input_arr[ARR_LEN];
static const int16_t scale_factor = (int16_t)(0.3*32768);
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
    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output[i] = ((int32_t)input_arr[i] * (int32_t)scale_factor) << 1;
    }
}

int main(void)
{
    INIT_INPUT_INT16(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_SCALE(qhblas_hvx_aw_vector_scaling_ah)

    TEST_INPUT_SCALE_INT16(qhblas_hvx_aw_vector_scaling_ah)

    return 0;
}
