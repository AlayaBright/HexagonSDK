/**=============================================================================
@file
    test_b_vector_self_max_ab.c

@brief
    Accuracy test for qhblas_hvx_b_vector_self_max_ab function

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
    int8_t *input_arr;     // Pointer to input array
    int8_t *output;        // Pointer to output
    uint32_t length;       // Length of input arrays
    int32_t expect_res;    // Expected return value
} test_invalid;

static range_int8_t ranges0[] =
{
    {  INT8_MIN, -50       },
    { -50,        50       },
    {  50,        INT8_MAX }
};

static int8_t input_arr[ARR_LEN];
static int8_t output;

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

inline static int8_t generate_ref_output(uint32_t size)
{
    int8_t max = input_arr[0];

    for (uint32_t i = 0; i < size; ++i)
    {
        max = ((input_arr[i] > max) ? input_arr[i] : max);
    }

    return max;
}

int main(void)
{
    INIT_INPUT_INT8(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A1(qhblas_hvx_b_vector_self_max_ab)

    TEST_INPUT_SCALAR_INT16(qhblas_hvx_b_vector_self_max_ab, ARR_LEN, output, input_arr)

    return 0;
}
