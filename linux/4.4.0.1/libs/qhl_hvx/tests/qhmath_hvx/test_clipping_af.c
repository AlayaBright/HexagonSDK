/**=============================================================================
@file
    test_clipping_af.c

@brief
    Accuracy test for qhmath_hvx_clipping_af function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhmath_hvx.h"
#include "qhblas_test.h"

static range_float_t ranges0[] =
{
    { -1000.0f, -100.0f  },
    { -100.0f,   100.0f  },
    {  100.0f,   1000.0f }
};

static float input_arr[ARR_LEN];
static float low_level = -110.0f;
static float high_level = 110.0f;
static float output[ARR_LEN];
static float ref_output[ARR_LEN];

#define RANGE_CNT0     (sizeof(ranges0)/sizeof(*ranges0))

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        float val = input_arr[i];

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

    printf("Test for qhmath_hvx_clipping_af function:\n");

    INIT_INPUT_FLOAT(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    generate_ref_output(ARR_LEN);

    for (uint32_t i = 1; i <= ARR_LEN; ++i)
    {
        memset(output, 0, sizeof(float)*ARR_LEN);
        qhmath_hvx_clipping_af(input_arr, output, i, low_level, high_level);

        for (uint32_t j = 0; j < i; ++j)
        {
            if (ref_output[j] != output[j])
            {
                printf("Error:\tin = %.7f | low_level = %.7f "
                       "| high_level = %.7f | qhmath_hvx_clipping_af = %.7f "
                       "| OK = %.7f | vector length = %lu | iter = %lu\n",
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

#else /* __HEXAGON_ARCH__ >= 68 */
#include <stdio.h>
int main()
{
    printf("HVX float is not supported for this architecture.\n");
    return 0;
}
#endif /* __HEXAGON_ARCH__ >= 68 */
