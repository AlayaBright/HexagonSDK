/**=============================================================================
@file
    test_div_aw.c

@brief
    Accuracy test for qhmath_hvx_div_aw function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include "qhmath_hvx.h"
#include "qhblas_test.h"
#include "qhmath.h"

static range_int32_t ranges0[] =
{
     { -999200000, -499600000 },
     { -499600000, -249800000 },
     { -249800000, 0          },
     { 0,          249800000  },
     { 249800000,  499600000  },
     { 499600000,  999200000  }
};

static int32_t input_arr1[ARR_LEN] __attribute__((aligned(128))) ;
static int32_t input_arr2[ARR_LEN] __attribute__((aligned(128))) ;
static int32_t output[ARR_LEN] __attribute__((aligned(128))) ;
static int32_t ref_output[ARR_LEN] __attribute__((aligned(128))) ;

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(range_int32_t))

static const float SCALE_Q31 = 2147483648.0; // 2^31 in float
static const float SCALE_Q16 = 65536.0;      // 2^16 in float

#define QFORMAT_IN  SCALE_Q31
#define QFORMAT_OUT SCALE_Q16

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        float numerator = (float)(input_arr1[i]/QFORMAT_IN);
        float denominator = (float)(input_arr2[i]/QFORMAT_IN);
        float q = numerator / denominator;
        ref_output[i] = (int32_t)(q*QFORMAT_OUT);
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;
    int32_t tolerance = 256;

    printf("Test for qhmath_hvx_div_aw function:\n");

    INIT_INPUT_INT32(input_arr1, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT32(input_arr2, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    generate_ref_output(ARR_LEN);

    for (uint32_t i = 1; i <= ARR_LEN; i++)
    {
        memset(output, 0, sizeof(__typeof__(*output)) * ARR_LEN);

        qhmath_hvx_div_aw(input_arr1, input_arr2, output, i);

        for (uint32_t j = 0; j < i; ++j)
        {
            int32_t error = ref_output[j] - output[j];
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
                       "qhmath_hvx_div_aw(x) = %.7d (%f)| OK = %.7d (%f) "
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
