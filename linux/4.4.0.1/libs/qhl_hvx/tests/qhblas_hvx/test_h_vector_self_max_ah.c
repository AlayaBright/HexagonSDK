/**=============================================================================
@file
    test_h_vector_self_max_ah.c

@brief
    Accuracy test for qhblas_hvx_h_vector_self_max_ah function

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
    int16_t *input_arr;    // Pointer to input array
    int16_t *output;       // Pointer to output
    uint32_t length;       // Length of input arrays
    int32_t expect_res;    // Expected return value
} test_invalid;

static range_int16_t ranges0[] =
{
    {  INT16_MIN, -1000      },
    { -1000,       1000      },
    {  1000,       INT16_MAX }
};

static int16_t input_arr[ARR_LEN];
static int16_t output;

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

inline static int16_t generate_ref_output(uint32_t size)
{
    int16_t max = input_arr[0];

    for (uint32_t i = 0; i < size; ++i)
    {
        max = ((input_arr[i] > max) ? input_arr[i] : max);
    }

    return max;
}

int main(void)
{
    INIT_INPUT_INT16(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A1(qhblas_hvx_h_vector_self_max_ah)

    TEST_INPUT_SCALAR_INT16(qhblas_hvx_h_vector_self_max_ah, ARR_LEN, output, input_arr)

    return 0;
}
