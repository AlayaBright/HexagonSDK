/**=============================================================================
@file
    test_ah_vector_scaling_ab.c

@brief
    Accuracy test for qhblas_hvx_ah_vector_scaling_ab function

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
    int8_t *input_arr;        // Pointer to first input array
    int8_t scaling_factor;    // Scaling factor
    int16_t *output;          // Pointer to output array
    uint32_t length;          // Length of input arrays
    int32_t expect_res;       // Expected return value
} test_invalid;

static range_int8_t ranges0[] =
{
    {  INT8_MIN, -50       },
    { -50,        50       },
    {  50,        INT8_MAX }
};

static int8_t input_arr[ARR_LEN];
static const int8_t scale_factor = (int8_t)(0.3*128);
static int16_t output[ARR_LEN];
static int16_t ref_output[ARR_LEN];

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
        ref_output[i] = ((int16_t)input_arr[i] * (int16_t)scale_factor) << 1;
    }
}

int main(void)
{
    INIT_INPUT_INT8(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_SCALE(qhblas_hvx_ah_vector_scaling_ab)

    TEST_INPUT_SCALE_INT16(qhblas_hvx_ah_vector_scaling_ab)

    return 0;
}
