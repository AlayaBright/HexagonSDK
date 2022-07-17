/**=============================================================================
@file
    test_sin_af.c

@brief
    Accuracy test for qhmath_hvx_sin_af function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>
#include "qhmath_hvx.h"
#include "qhblas_test.h"
#include <math.h>

static range_float_t ranges0[] =
{
    { -100.0f,  -1.0f },
    { -3.14f,     3.14f },
    {  1.0f,   100.0f }
};

static float input[ARR_LEN];
static float ref_output[ARR_LEN];
static float output[ARR_LEN];

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(*ranges0))

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output[i] = sinf(input[i]);
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;
    float tolerance = 0.000016;

    printf("Test for qhmath_hvx_sin_af function:\n");

    INIT_INPUT_FLOAT(input, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    generate_ref_output(ARR_LEN);

    for (uint32_t i = 1; i <= ARR_LEN; ++i)
    {
        memset(output, 0, sizeof(__typeof__(*output)) * ARR_LEN);

        qhmath_hvx_sin_af(input, output, i);

        for (uint32_t j = 0; j < i; ++j)
        {
            float error = ref_output[j] - output[j];
            if (error < 0)
                error = -error;
            if (error > tolerance)
            {
                printf("Error:\tx = { %.7f } | "
                       "qhmath_hvx_sin_af(x) = %.7f | OK = %.7f "
                       "| vector length = %u | i = %u\n",
                       input[j], output[j],
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
    printf("(%u tests passed, %u failed)\n", pass, fail);

    return 0;
}
#else
int main(void)
{
    return 0;
}
#endif
