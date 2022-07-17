/**=============================================================================
@file
    test_vector_dot_af.c

@brief
    Accuracy test for qhblas_f_vector_dot_af function

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/
#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas.h"
#include "qhblas_test.h"

static range_float_t ranges0[] = {
    { -1.0f,   -0.001f },
    { -0.001f, -0.0f   },
    {  0.0f,    0.001f },
    {  0.001f,  1.0f   }
};

static float_a8_t input_arr1[ARR_LEN];
static float_a8_t input_arr2[ARR_LEN];
static float output;

static test_invalid_f2 test_data[] = {
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

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(range_float_t))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(test_invalid_f2))

inline static float generate_ref_output(uint32_t size)
{
    float output = 0;
    for (uint32_t i = 0; i < size; ++i)
    {
        output += input_arr1[i] * input_arr2[i];
    }

    return output;
}

int main(void)
{
    INIT_INPUT_FLOAT(input_arr1, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_FLOAT(input_arr2, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A2(qhblas_f_vector_dot_af)

    TEST_INPUT_SCALAR_FLOAT(qhblas_f_vector_dot_af, ARR_LEN, output, input_arr1, input_arr2)

    return 0;
}
