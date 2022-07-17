/**=============================================================================
@file
    test_vector_add_af.c

@brief
    Accuracy test for qhblas_vector_add_af function

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

static float spec_value[] = { 0, -0, -INFINITY, INFINITY, NAN};
static float_a8_t input_arr1[ARR_LEN];
static float_a8_t input_arr2[ARR_LEN];
static float_a8_t output[ARR_LEN];
static float_a8_t ref_output[ARR_LEN];

static test_invalid_f2 test_data[] = {
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

#define NUM_SPEC_VALUE (sizeof(spec_value)/sizeof(float))
#define RANGE_CNT0     (sizeof(ranges0)/sizeof(range_float_t))
#define NUM_TEST_TYPE  (sizeof(test_data)/sizeof(test_invalid_f2))

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output[i] = input_arr1[i] + input_arr2[i];
    }
}

int main(void)
{
    TEST_INIT_SPEC_VAL(input_arr1, spec_value, NUM_SPEC_VALUE)
    TEST_INIT_SPEC_VAL(input_arr2, spec_value, NUM_SPEC_VALUE)

    INIT_INPUT_FLOAT_INDEX(input_arr1, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX, NUM_SPEC_VALUE)
    INIT_INPUT_FLOAT_INDEX(input_arr2, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX, NUM_SPEC_VALUE)

    TEST_INVALID_INPUT_A2(qhblas_vector_add_af)

    TEST_INPUT_AF2(qhblas_vector_add_af)

    return 0;
}
