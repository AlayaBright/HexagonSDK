/**=============================================================================
@file
    test_w_vector_dot_ah.c

@brief
    Accuracy test for qhblas_hvx_w_vector_dot_ah function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/
#include <stdio.h>
#include <stdlib.h>

#include "qhblas_hvx.h"
#include "qhblas_test.h"

typedef struct test_invalid
{
    int16_t *input_arr1;    // Pointer to first input array
    int16_t *input_arr2;    // Pointer to second input array
    int32_t *output;        // Pointer to output
    uint32_t length;        // Length of input arrays
    int32_t expect_res;     // Expected return value
} test_invalid;

static range_int16_t ranges0[] =
{
    { -20000, -1000  },
    { -1000,  -100   },
    { -100,    100   },
    {  100,    1000  },
    {  1000,   20000 }
};

static int16_t input_arr1[ARR_LEN];
static int16_t input_arr2[ARR_LEN];
static int32_t output;

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
    int32_t output = 0;

    for (uint32_t i = 0; i < size; ++i)
    {
        output += (int32_t)input_arr1[i] * (int32_t)input_arr2[i];
    }

    return output << 1;
}

int main(void)
{
    INIT_INPUT_INT16(input_arr1, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT16(input_arr2, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A2(qhblas_hvx_w_vector_dot_ah)

    TEST_INPUT_SCALAR_INT16(qhblas_hvx_w_vector_dot_ah, ARR_LEN, output, input_arr1, input_arr2)

    return 0;
}
