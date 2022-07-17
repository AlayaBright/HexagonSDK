/**=============================================================================
@file
    test_vector_norm_ah.c

@brief
    Accuracy test for qhblas_h_vector_norm_ah function

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/
#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas.h"
#include "qhblas_test.h"

static range_uint16_t ranges0[] = {
    {  INT16_MIN, -1000      },
    { -1000,      -100       },
    { -100,        100       },
    {  100,        1000      },
    {  1000,       INT16_MAX }
};

static int16_a8_t input_arr[ARR_LEN];
static int32_t output;

static test_invalid_hnorm test_data[] = {
    { input_arr, &output, 0,       -1 },
    { input_arr, NULL,    ARR_LEN, -1 },
    { input_arr, NULL,    0,       -1 },
    { NULL,      &output, ARR_LEN, -1 },
    { NULL,      &output, 0,       -1 },
    { NULL,      NULL,    ARR_LEN, -1 },
    { NULL,      NULL,    0,       -1 },
    { input_arr, &output, ARR_LEN,  0 }
};

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(range_uint16_t))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(test_invalid_hnorm))

inline static int32_t generate_ref_output(uint32_t size)
{
    int64_t output = 0;
    for (uint32_t i = 0; i < size; i++)
    {
        output += (int64_t)(input_arr[i] * input_arr[i]);
    }

    output <<= 1;
    float result_of_norm = (qhmath_sqrt_f((float)(output)/(float)(2147483648)))*(1 << 16);
    return (((result_of_norm-(int32_t)result_of_norm)>=0.5) ? ((int32_t)result_of_norm+1) : ((int32_t)(result_of_norm))); /* 2147483648 = 2 ^ 31 */
}

int main(void)
{
    INIT_INPUT_INT16(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A1(qhblas_h_vector_norm_ah)

    TEST_INPUT_SCALAR_INT16(qhblas_h_vector_norm_ah, ARR_LEN, output, input_arr)

    return 0;
}
