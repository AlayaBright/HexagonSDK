/**=============================================================================
@file
    test_exp2_af.c

@brief
    Accuracy test for qhmath_exp2_af function

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

range0 ranges0[] = {
    {-200.0f, -38.5f},
    {-38.5f, 0.0f},
    {0.0f, 38.5f},
    {38.5f, 200.0f}
};

typedef struct test_t
{
    float *input_arr;     // Pointer to input array
    float *output_arr;    // Pointer to output array
    uint32_t length;      // Length of input array
    int32_t expect_res;   // Expected return value
} test_t;


float spec_value[] = { 0.0, -0.0, -INFINITY, INFINITY, NAN, -NAN};
float input_data[ARR_LEN];
float output_data[ARR_LEN];
float ref_data[ARR_LEN];

test_t test_data[] = {
                      {input_data, output_data, NULL, -1},
                      {input_data, NULL, ARR_LEN, -1},
                      {input_data,  NULL, 0, -1},
                      {NULL, output_data, ARR_LEN, -1},
                      {NULL, output_data, 0, -1},
                      {NULL, NULL, ARR_LEN, -1},
                      {NULL, NULL, 0, -1},
                      {input_data, output_data, ARR_LEN, 0},
                      };

#define NUM_SPEC_VALUE     (sizeof(spec_value)/sizeof(float))
#define RANGE_CNT0         (sizeof(ranges0) / sizeof(range0))
#define NUM_TEST_TYPE      (sizeof(test_data)/sizeof(test_t))

int main(void)
{
    // initialize start elements of test array by special value (INF, NAN, 0.0 etc...)
    TEST_AF3_INIT_SPEC_VAL(NUM_SPEC_VALUE);

    // initialize array contents
    TEST_AF3_INIT_INPUT(NUM_SPEC_VALUE, ARR_LEN);

    TEST_GEN_REF_AF3(qhmath_exp2_f)

    TEST_EXEC_PARAM_AF3(qhmath_exp2_af)

    TEST_EXEC_ACC_AF3(qhmath_exp2_af)
    return 0;
}
