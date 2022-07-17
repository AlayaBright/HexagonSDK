/**=============================================================================
@file
    test_modexp_af.c

@brief
    Accuracy test for qhmath_hvx_modexp_af function

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
static int32_t k_arr[2] = { -25, 25 };
static float output[ARR_LEN];
static float ref_output[2][ARR_LEN];

#define RANGE_CNT0 (sizeof(ranges0)/sizeof(*ranges0))
#define KARR_CNT   (sizeof(k_arr)/sizeof(*k_arr))

inline static void generate_ref_output(uint32_t size)
{
    typedef union
    {
        float f;
        struct
        {
            uint32_t mantissa : 23;
            uint32_t exponent : 8;
            uint32_t sign : 1;
        } bits;
    } fp32;

    for (uint32_t i = 0; i < size; ++i)
    {
        fp32 ref1 = { .f = input_arr[i] };
        fp32 ref2 = { .f = input_arr[i] };

        ref1.bits.exponent = k_arr[0] + 127;
        ref2.bits.exponent = k_arr[1] + 127;

        ref_output[0][i] = ref1.f;
        ref_output[1][i] = ref2.f;
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhmath_hvx_modexp_af function:\n");

    INIT_INPUT_FLOAT(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    generate_ref_output(ARR_LEN);

    for (uint32_t k = 0; k < KARR_CNT; ++k)
    {
        int32_t k_val = k_arr[k];

        for (uint32_t i = 1; i <= ARR_LEN; ++i)
        {
            memset(output, 0, sizeof(float)*ARR_LEN);
            qhmath_hvx_modexp_af(input_arr, output, i, k_val);

            for (uint32_t j = 0; j < i; ++j)
            {
                if (ref_output[k][j] != output[j])
                {
                    printf("Error:\tin = %.7f | k = %.7ld "
                           "| qhmath_hvx_modexp_af = %.7f | OK = %.7f "
                           "| vector length = %lu | iter = %lu\n",
                           input_arr[j], k_val, output[j],
                           ref_output[k][j], i, j);
                    ++fail;
                }
                else
                {
                    ++pass;
                }
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
