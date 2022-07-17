/**=============================================================================
@file
    test_ah_vector_integrate_aw.c

@brief
    Accuracy test for qhblas_hvx_aw_vector_integrate_ah function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"

static range_int16_t ranges0[] = {
    {  INT16_MIN, -100       },
    { -100,        100       },
    {  100,        INT16_MAX }
};

static int16_t input_arr[ARR_LEN];
static int32_t output[ARR_LEN];
static int32_t ref_output[ARR_LEN];

static test_invalid_hw test_data[] = {
    { input_arr, output, 0,       -1 },
    { input_arr, NULL,   ARR_LEN, -1 },
    { input_arr, NULL,   0,       -1 },
    { NULL,       output, ARR_LEN, -1 },
    { NULL,       output, 0,       -1 },
    { NULL,       NULL,   ARR_LEN, -1 },
    { NULL,       NULL,   0,       -1 },
    { input_arr, output, ARR_LEN,  0 }
};

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(range_int16_t))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(test_invalid_hw))

inline static void generate_ref_output(uint32_t size)
{
    int32_t sum = 0;
    for (int32_t i = 0; i < size; ++i)
    {
        sum = sum + input_arr[i];
        ref_output[i] = (int32_t)sum;
    }

}

int main(void)
{
    INIT_INPUT_INT32(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A1(qhblas_hvx_aw_vector_integrate_ah)

    TEST_INPUT_AW(qhblas_hvx_aw_vector_integrate_ah)

    return 0;
}
