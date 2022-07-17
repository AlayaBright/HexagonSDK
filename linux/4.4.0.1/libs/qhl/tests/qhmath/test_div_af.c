/**=============================================================================
@file
    test_div_af.c

@brief
    Accuracy test for qhmath_div_af function

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhmath_test.h"

typedef struct
{
    float lo;  // Lowest value
    float hi;  // Highest value
} range0;

typedef struct
{
    float lo;  // Lowest value
    float hi;  // Highest value
} range1;

range0 ranges0[] = {
    {-1e+3f, 1e+3f}
};

range1 ranges1[] = {
    {-1e+3f, 1e+3f}
};

typedef struct test_t
{
    float *input1_arr;     // Pointer to array of numerators
    float *input2_arr;     // Pointer to array of denominators
    float *output_arr;        // Pointer to output array
    uint32_t length;          // Length of input arrays
    int32_t expect_res;       // Expected return value
} test_t;

float spec_value[] = { 0.0, -0.0, -INFINITY, INFINITY, NAN, -NAN};

float input_data1[ARR_LEN];
float input_data2[ARR_LEN];
float output_data[ARR_LEN];
float ref_data[ARR_LEN];

test_t test_data[] = {
                      {input_data1, input_data2, output_data, 0, -1},
                      {input_data1, input_data2, NULL, ARR_LEN, -1},
                      {input_data1, input_data2, NULL, 0, -1},
                      {input_data1, NULL, output_data, ARR_LEN, -1},
                      {input_data1, NULL, output_data, 0, -1},
                      {input_data1, NULL, NULL, ARR_LEN, -1},
                      {input_data1, NULL, NULL, 0, -1},
                      {NULL, input_data2, output_data, ARR_LEN, -1},
                      {NULL, input_data2, output_data, 0, -1},
                      {NULL, input_data2, NULL, ARR_LEN, -1},
                      {NULL, input_data2, NULL, 0, -1},
                      {NULL, NULL, output_data, ARR_LEN, -1},
                      {NULL, NULL, output_data, 0, -1},
                      {NULL, NULL, NULL, ARR_LEN, -1},
                      {NULL, NULL, NULL, 0, -1},
                      {input_data1, input_data2, output_data, ARR_LEN, 0},
                      };

#define NUM_SPEC_VALUE     (sizeof(spec_value)/sizeof(float))
#define RANGE_CNT0         (sizeof(ranges0) / sizeof(range0))
#define RANGE_CNT1         (sizeof(ranges1) / sizeof(range1))
#define NUM_TEST_TYPE      (sizeof(test_data)/sizeof(test_t))

int main(void)
{
    // initialize starting elements of test array by special values (INF, NAN, 0.0 etc...)
    TEST_AF4_INIT_SPEC_VAL(NUM_SPEC_VALUE);

    // initialize array contents
    TEST_AF4_INIT_INPUT(NUM_SPEC_VALUE * NUM_SPEC_VALUE, ARR_LEN);

    TEST_GEN_REF_AF4(qhmath_div_f)

    TEST_EXEC_PARAM_AF4(qhmath_div_af)

    TEST_EXEC_ACC_AF4(qhmath_div_af)

    return 0;
}
