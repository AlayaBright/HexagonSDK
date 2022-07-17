/**=============================================================================
@file
    test_tanh_ahf.c

@brief
    Accuracy test for qhmath_hvx_tanh_ahf function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>
#include "qhmath_hvx.h"
#include "qhblas_test.h"
#include <math.h>


static range_float16_t ranges0[] =
{
    {  -1000.0f, 1000.0f  }
};



static __fp16 input[ARR_LEN];
static __fp16 ref_output[ARR_LEN];
static __fp16 output[ARR_LEN];

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(*ranges0))

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output[i] = (__fp16) tanhf((float)input[i]);
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhmath_hvx_tanh_ahf function:\n");

    INIT_INPUT_FLOAT16(input, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    generate_ref_output(ARR_LEN);

    for (uint32_t i = ARR_LEN; i <= ARR_LEN; ++i)
    {
        memset(output, 0, sizeof(__typeof__(*output)) * ARR_LEN);

        qhmath_hvx_tanh_ahf(input, output, i);

        for (uint32_t j = 0; j < i; ++j)
        {
            float error = (float)ref_output[j] - (float)output[j];
            if (error < 0)
                error = -error;

            float error_percent = (ref_output[j]-output[j])/(ref_output[j])*100.0f;

            if (error_percent>5)
            {
                printf("Error:\tx = { %.7f } | "
                       "qhmath_hvx_tanh_vhf(x) = %.7f | OK = %.7f "
                       "| error_percent = %2.5f "
                       "| vector length = %lu | i = %lu\n",
                       input[j], output[j],
                       ref_output[j],
                       error_percent, i, j);
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
