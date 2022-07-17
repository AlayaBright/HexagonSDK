/**=============================================================================
@file
    test_modexp_ahf.c

@brief
    Accuracy test for qhmath_hvx_modexp_ahf function

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

static __fp16 input_arr[ARR_LEN];
static int32_t k_arr[2] = { -5, 5 };
static __fp16 output[ARR_LEN];
static __fp16 ref_output[2][ARR_LEN];

#define RANGE_CNT0 (sizeof(ranges0)/sizeof(*ranges0))
#define KARR_CNT   (sizeof(k_arr)/sizeof(*k_arr))

inline static void generate_ref_output(uint32_t size)
{
    typedef union
    {
        __fp16 f;
        struct
        {
            uint32_t mantissa : 10;
            uint32_t exponent : 5;
            uint32_t sign : 1;
        } bits;
    } fp16;

    for (uint32_t i = 0; i < size; ++i)
    {
        fp16 ref1 = { .f = input_arr[i] };
        fp16 ref2 = { .f = input_arr[i] };

        ref1.bits.exponent = k_arr[0] + 15;
        ref2.bits.exponent = k_arr[1] + 15;

        ref_output[0][i] = ref1.f;
        ref_output[1][i] = ref2.f;
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhmath_hvx_modexp_ahf function:\n");

    INIT_INPUT_FLOAT16(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    generate_ref_output(ARR_LEN);

    for (uint32_t k = 0; k < KARR_CNT; ++k)
    {
        int32_t k_val = k_arr[k];

        for (uint32_t i = 1; i <= ARR_LEN; ++i)
        {
            memset(output, 0, sizeof(__fp16)*ARR_LEN);
            qhmath_hvx_modexp_ahf(input_arr, output, i, k_val);

            for (uint32_t j = 0; j < i; ++j)
            {
                if (ref_output[k][j] != output[j])
                {
                    printf("Error:\tin = %.7f | k = %.7ld "
                           "| qhmath_hvx_modexp_ahf = %.7f | OK = %.7f "
                           "| vector length = %lu | iter = %lu\n",
                           input_arr[j], k_val, output[j],
                           ref_output[k][j],i, j);
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
