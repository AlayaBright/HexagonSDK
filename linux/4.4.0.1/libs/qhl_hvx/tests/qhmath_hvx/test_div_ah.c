/**=============================================================================
@file
    test_div_ah.c

@brief
    Accuracy test for qhmath_hvx_div_ah function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include "qhmath_hvx.h"
#include "qhblas_test.h"
#include "qhmath.h"

static range_uint16_t ranges0[] = {
    {  INT16_MIN, INT16_MAX }
};

//static int16_t input_arr1[ARR_LEN];
//static int16_t input_arr2[ARR_LEN];
//static int16_t output[ARR_LEN];
//static int16_t ref_output[ARR_LEN];


static int16_t input_arr1[ARR_LEN] __attribute__((aligned(128))) ;
static int16_t input_arr2[ARR_LEN] __attribute__((aligned(128))) ;
static int16_t output[ARR_LEN] __attribute__((aligned(128))) ;
static int16_t ref_output[ARR_LEN] __attribute__((aligned(128))) ;


static test_invalid_h2 test_data[] = {
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

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(range_uint16_t))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(test_invalid_h2))

static const float SCALE_Q15 = 32768.0;  // 2^15 in float
//static const float SCALE_Q14 = 16384.0;  // 2^14 in float
static const float SCALE_Q8 = 256.0;     // 2^8 in float

#define QFORMAT_IN  SCALE_Q15
#define QFORMAT_OUT SCALE_Q8

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        float numerator = (float)(input_arr1[i]/QFORMAT_IN);
        float denominator = (float)(input_arr2[i]/QFORMAT_IN);
        float q = numerator / denominator;
        if (input_arr2[i])
            ref_output[i] = (int16_t)(q*QFORMAT_OUT);
        else
            ref_output[i] = (int16_t) 0x7fff;
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;
    int16_t tolerance = 32;

    INIT_INPUT_INT16(input_arr1, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT16(input_arr2, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A2(qhmath_hvx_div_ah)

    generate_ref_output(ARR_LEN);

    for (uint32_t i = 1; i < ARR_LEN; ++i)
    {
        memset(output, 0, sizeof(__typeof__(*output)) * ARR_LEN);

        qhmath_hvx_div_ah(input_arr1, input_arr2, output, i);

        for (uint32_t j = 0; j < i; ++j)
        {
            int16_t error = ref_output[j] - output[j];
            if (error < 0)
                error = -error;
            if (error > tolerance)
            {
                float numerator = (float) (input_arr1[j] / QFORMAT_IN);
                float denominator = (float) (input_arr2[j] / QFORMAT_IN);
                float ref_f = (float) (ref_output[j] / QFORMAT_OUT);
                float output_f = (float) (output[j] / QFORMAT_OUT);

                printf("Error:\tx = { %.7d / %.7d } |\t"
                       " {%7f / %7f}\t"
                       "qhmath_hvx_div_ah(x) = %.7d (%f)| OK = %.7d (%f) "
                       "| vector length = %lu | i = %lu\n",
                       input_arr1[j], input_arr2[j],
                       numerator, denominator,
                       output[j], output_f,
                       ref_output[j], ref_f, i, j);
                ++fail;
            }
            else
            {
                ++pass;
            }
        }
    }

    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");
    printf("(%lu tests passed, %lu failed)\n", pass, fail);

    return 0;
}
