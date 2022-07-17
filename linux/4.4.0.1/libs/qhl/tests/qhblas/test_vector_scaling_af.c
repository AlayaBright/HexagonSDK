/**=============================================================================
@file
    test_vector_scaling_af.c

@brief
    Accuracy test for qhblas_vector_scaling_af function

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
static float_a8_t input_arr[ARR_LEN];
static float_a8_t output[ARR_LEN];
static float_a8_t ref_output[ARR_LEN];
static const float scale_factor = 2.348f;

static test_invalid_fscale test_data[] = {
    { input_arr, scale_factor, output, 0,       -1 },
    { input_arr, scale_factor, NULL,   ARR_LEN, -1 },
    { input_arr, scale_factor, NULL,   0,       -1 },
    { NULL,      scale_factor, output, ARR_LEN, -1 },
    { NULL,      scale_factor, output, 0,       -1 },
    { NULL,      scale_factor, NULL,   ARR_LEN, -1 },
    { NULL,      scale_factor, NULL,   0,       -1 },
    { input_arr, scale_factor, output, ARR_LEN,  0 }
};

#define NUM_SPEC_VALUE (sizeof(spec_value)/sizeof(float))
#define RANGE_CNT0     (sizeof(ranges0)/sizeof(range_float_t))
#define NUM_TEST_TYPE  (sizeof(test_data)/sizeof(test_invalid_fscale))

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output[i] = input_arr[i] * scale_factor;
    }
}

int main(void)
{
    TEST_INIT_SPEC_VAL(input_arr, spec_value, NUM_SPEC_VALUE)

    INIT_INPUT_FLOAT_INDEX(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX, NUM_SPEC_VALUE)

    TEST_INVALID_INPUT_SCALE(qhblas_vector_scaling_af)

    TEST_INPUT_SCALE_FLOAT(qhblas_vector_scaling_af)

    return 0;
}
