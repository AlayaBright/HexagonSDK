/**=============================================================================
@file
    test_acos_af.c

@brief
    Accuracy test for qhmath_hvx_acos_ahf function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qhmath_hvx.h"
#include "qhblas_test.h"
#include <math.h>

static range_float_t ranges0[] =
{
    {  -1.0f,   1.0f }
};


static __fp16 input[ARR_LEN];
static __fp16 ref_output[ARR_LEN];
static __fp16 output[ARR_LEN];

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(*ranges0))

inline static void Generate_input_float(__fp16* input1, uint32_t length, range_float_t* range)
{
    for (uint32_t i = 0; i < length; ++i)
    {
        uint32_t range_index = i % RANGE_CNT0;
        float lo = range[range_index].lo;
        float hi = range[range_index].hi;

        input1[i] = lo + (hi - lo) * i / length;
    }
}

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output[i] = (__fp16)acos(input[i]);
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhmath_hvx_acos_ahf function:\n");

    Generate_input_float(input, ARR_LEN, ranges0);

    generate_ref_output(ARR_LEN);

    for (uint32_t i = ARR_LEN; i <= ARR_LEN; ++i)
    {
        memset(output, 0, sizeof(__typeof__(*output)) * ARR_LEN);

        qhmath_hvx_acos_ahf(input, output, i);

        for (uint32_t j = 0; j < i; ++j)
        {
            __fp16 error = ref_output[j] - output[j];
            if (error < 0)
                error = -error;

            __fp16 error_percent = fabsf((ref_output[j]-output[j])/(ref_output[j]))*100.0f;

            // TODO: This error threshold is only for sdk 4.2, we will fix it in the next release.
            if (error_percent > 10.5)
            {
                printf("Error:\tx = { %.7f } | "
                       "qhmath_hvx_acos_ahf(x) = %.7e | OK = %.7e "
                       "| error_percent = %2.5f "
                       "| vector length = %u | i = %u\n",
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
    printf("(%u tests passed, %u failed)\n", pass, fail);

    return 0;
}
#else
int main(void)
{
    return 0;
}
#endif
