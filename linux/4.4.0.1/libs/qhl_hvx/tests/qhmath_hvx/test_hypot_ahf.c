/**=============================================================================
@file
    test_hypot_ahf.c

@brief
    Accuracy test for qhmath_hvx_hypot_ahf function

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
    { -10.0f, -1.0f  },
    { -1.0f,   1.0f  },
    {  1.0f,   10.0f }
};

static __fp16 input_a[ARR_LEN];
static __fp16 input_b[ARR_LEN];
static __fp16 output[ARR_LEN];
static __fp16 ref_output[ARR_LEN];

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(*ranges0))

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        float square_a = (float)input_a[i] * (float)input_a[i];
        float square_b = (float)input_b[i] * (float)input_b[i];
        ref_output[i] = (__fp16)sqrtf(square_a + square_b);
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;
    float tolerance = 0.2;

    printf("Test for qhmath_hvx_hypot_ahf function:\n");

    INIT_INPUT_FLOAT16(input_a, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_FLOAT16(input_b, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    generate_ref_output(ARR_LEN);

    for (uint32_t i = 1; i <= ARR_LEN; ++i)
    {
        memset(output, 0, sizeof(__typeof__(*output)) * ARR_LEN);

        qhmath_hvx_hypot_ahf(input_a, input_b, output, i);

        for (uint32_t j = 0; j < i; ++j)
        {
            float error = (float)ref_output[j] - (float)output[j];
            if (error < 0)
                error = -error;
            if (error > tolerance)
            {
                printf("Error:\tx = { %.7f, %.7f } | "
                       "qhmath_hvx_hypot_ahf(x) = %.7f | OK = %.7f "
                       "| vector length = %lu | i = %lu\n",
                       input_a[j], input_b[j], output[j],
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

#else /* __HEXAGON_ARCH__ >= 68 */
#include <stdio.h>
int main()
{
    printf("HVX float is not supported for this architecture.\n");
    return 0;
}
#endif /* __HEXAGON_ARCH__ >= 68 */
