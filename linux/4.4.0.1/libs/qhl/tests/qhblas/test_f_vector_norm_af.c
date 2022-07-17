/**=============================================================================
@file
    test_vector_norm_af.c

@brief
    Accuracy test for qhblas_f_vector_norm_af function

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

static float_a8_t input_arr[ARR_LEN];
static float output;

static test_invalid_f1 test_data[] = {
    { input_arr, &output, 0,       -1 },
    { input_arr, NULL,    ARR_LEN, -1 },
    { input_arr, NULL,    0,       -1 },
    { NULL,      &output, ARR_LEN, -1 },
    { NULL,      &output, 0,       -1 },
    { NULL,      NULL,    ARR_LEN, -1 },
    { NULL,      NULL,    0,       -1 },
    { input_arr, &output, ARR_LEN,  0 }
};

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(range_float_t))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(test_invalid_f1))

inline static float generate_ref_output(uint32_t size)
{
    float output = 0;
    for (uint32_t i = 0; i < size; ++i)
    {
        output += input_arr[i] * input_arr[i];
    }

    return qhmath_sqrt_f(output);
}

int main(void)
{
    INIT_INPUT_FLOAT(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A1(qhblas_f_vector_norm_af)

    TEST_INPUT_SCALAR_FLOAT(qhblas_f_vector_norm_af, ARR_LEN, output, input_arr)

    return 0;
}
