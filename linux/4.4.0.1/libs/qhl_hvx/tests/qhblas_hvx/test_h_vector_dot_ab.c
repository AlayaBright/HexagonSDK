/**=============================================================================
@file
    test_h_vector_dot_ab.c

@brief
    Accuracy test for qhblas_hvx_h_vector_dot_ab function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/
#include <stdio.h>
#include <stdlib.h>

#include "qhblas_hvx.h"
#include "qhblas_test.h"

typedef struct test_invalid
{
    int8_t *input_arr1;    // Pointer to first input array
    int8_t *input_arr2;    // Pointer to second input array
    int16_t *output;       // Pointer to output
    uint32_t length;       // Length of input arrays
    int32_t expect_res;    // Expected return value
} test_invalid;

static range_int8_t ranges0[] =
{
    {  INT8_MIN, -50       },
    { -50,        50       },
    {  50,        INT8_MAX }
};

static int8_t input_arr1[ARR_LEN];
static int8_t input_arr2[ARR_LEN];
static int16_t output;

static test_invalid test_data[] =
{
    { input_arr1, input_arr2, &output, 0,       -1 },
    { input_arr1, input_arr2, NULL,    ARR_LEN, -1 },
    { input_arr1, input_arr2, NULL,    0,       -1 },
    { input_arr1, NULL,       &output, ARR_LEN, -1 },
    { input_arr1, NULL,       &output, 0,       -1 },
    { input_arr1, NULL,       NULL,    ARR_LEN, -1 },
    { input_arr1, NULL,       NULL,    0,       -1 },
    { NULL,       input_arr2, &output, ARR_LEN, -1 },
    { NULL,       input_arr2, &output, 0,       -1 },
    { NULL,       input_arr2, NULL,    ARR_LEN, -1 },
    { NULL,       input_arr2, NULL,    0,       -1 },
    { NULL,       NULL,       &output, ARR_LEN, -1 },
    { NULL,       NULL,       &output, 0,       -1 },
    { NULL,       NULL,       NULL,    ARR_LEN, -1 },
    { NULL,       NULL,       NULL,    0,       -1 },
    { input_arr1, input_arr2, &output, ARR_LEN,  0 }
};

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(*test_data))

inline static int32_t generate_ref_output(uint32_t size)
{
    int16_t output = 0;

    for (uint32_t i = 0; i < size; ++i)
    {
        output += (int16_t)input_arr1[i] * (int16_t)input_arr2[i];
    }

    return output << 1;
}

int main(void)
{
    INIT_INPUT_INT8(input_arr1, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT8(input_arr2, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A2(qhblas_hvx_h_vector_dot_ab)

    TEST_INPUT_SCALAR_INT16(qhblas_hvx_h_vector_dot_ab, ARR_LEN, output, input_arr1, input_arr2)

    return 0;
}
