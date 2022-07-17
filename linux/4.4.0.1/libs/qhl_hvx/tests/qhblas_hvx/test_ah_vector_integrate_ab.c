/**=============================================================================
@file
    test_ab_vector_integrate_ah.c

@brief
    Accuracy test for qhblas_hvx_ah_vector_integrate_ab function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"

static range_int8_t ranges0[] = {
    {  INT8_MIN, -100       },
    { -100,        100       },
    {  100,        INT8_MAX }
};

static int8_t input_arr[ARR_LEN];
static int16_t output[ARR_LEN];
static int16_t ref_output[ARR_LEN];

static test_invalid_bh test_data[] = {
    { input_arr, output, 0,       -1 },
    { input_arr, NULL,   ARR_LEN, -1 },
    { input_arr, NULL,   0,       -1 },
    { NULL,       output, ARR_LEN, -1 },
    { NULL,       output, 0,       -1 },
    { NULL,       NULL,   ARR_LEN, -1 },
    { NULL,       NULL,   0,       -1 },
    { input_arr, output, ARR_LEN,  0 }
};

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(range_int8_t))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(test_invalid_bh))

inline static void generate_ref_output(uint32_t size)
{
    int16_t sum = 0;
    for (int32_t i = 0; i < size; ++i)
    {
        sum = sum + input_arr[i];
        ref_output[i] = (int16_t)sum;
    }

}

int main(void)
{
    INIT_INPUT_INT8(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A1(qhblas_hvx_ah_vector_integrate_ab)

    TEST_INPUT_AH(qhblas_hvx_ah_vector_integrate_ab)

    return 0;
}
