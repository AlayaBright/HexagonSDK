/**=============================================================================
@file
    test_clipping_ah.c

@brief
    Accuracy test for qhmath_hvx_clipping_ah function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath_hvx.h"
#include "qhblas_test.h"

static range_int16_t ranges0[] =
{
    {  INT16_MIN, -100       },
    { -100,        100       },
    {  100,        INT16_MAX }
};

static int16_t input_arr[ARR_LEN];
static int16_t low_level = -1010;
static int16_t high_level = 1010;
static int16_t output[ARR_LEN];
static int16_t ref_output[ARR_LEN];

#define RANGE_CNT0 (sizeof(ranges0)/sizeof(*ranges0))

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        int16_t val = input_arr[i];

        if (val < low_level)
        {
            val = low_level;
        } else if (val > high_level)
        {
            val = high_level;
        }

        ref_output[i] = val;
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhmath_hvx_clipping_ah function:\n");

    INIT_INPUT_INT16(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    generate_ref_output(ARR_LEN);

    for (uint32_t i = 1; i <= ARR_LEN; ++i)
    {
        memset(output, 0, sizeof(int16_t)*ARR_LEN);
        qhmath_hvx_clipping_ah(input_arr, output, i, low_level, high_level);

        for (uint32_t j = 0; j < i; ++j)
        {
            if (ref_output[j] != output[j])
            {
                printf("Error:\tin = %.7d | low_level = %.7d "
                       "| high_level = %.7d | qhmath_hvx_clipping_ah = %.7d "
                       "| OK = %.7d | vector length = %lu | iter = %lu\n",
                       input_arr[j], low_level, high_level, output[j],
                       ref_output[j], i, j);
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
