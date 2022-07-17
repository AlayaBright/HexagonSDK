/**=============================================================================
@file
    test_vector_add_ah.c

@brief
    Accuracy test for qhblas_hvx_vector_add_ah function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"

static range_int16_t ranges0[] =
{
    {  INT16_MIN, -100       },
    { -100,        100       },
    {  100,        INT16_MAX }
};

static int16_t input_arr1[ARR_LEN];
static int16_t input_arr2[ARR_LEN];
static int16_t output[ARR_LEN];
static int16_t ref_output[ARR_LEN];

static test_invalid_h2 test_data[] =
{
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

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(*test_data))

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        int32_t sum = input_arr1[i] + input_arr2[i];
        sum = ((sum > 32767) ? 32767 : ((sum < -32767) ? -32768 : sum));

        ref_output[i] = sum;
    }
}

int main(void)
{
    INIT_INPUT_INT16(input_arr1, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT16(input_arr2, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A2(qhblas_hvx_vector_add_ah)

    TEST_INPUT_AH2(qhblas_hvx_vector_add_ah)

    return 0;
}
