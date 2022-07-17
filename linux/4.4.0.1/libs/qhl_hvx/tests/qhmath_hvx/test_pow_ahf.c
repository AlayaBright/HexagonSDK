/**=============================================================================
@file
    test_pow_ahf.c

@brief
    Accuracy test for qhmath_hvx_pow_ahf function

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
    { -5.0f, -1.0f },
    {  0.0f,  1.0f },
    {  1.0f,  5.0f },
    {  0.0f,  5.0f },
    {  0.0f,  2.0f },
    {  2.0f,  5.0f }
};

static range_float_t ranges1[] =
{
    {  3.0f,  3.0f },
    {  0.0f,  1.0f },
    {  1.0f,  5.0f },
    { -5.0f, -0.0f },
    {  2.0f,  5.0f },
    {  0.0f,  2.0f }
};

static __fp16 x_arr[ARR_LEN];
static __fp16 y_arr[ARR_LEN];
static __fp16 output[ARR_LEN];
static __fp16 ref_output[ARR_LEN];

#define RANGE_CNT0     (sizeof(ranges0)/sizeof(*ranges0))
#define RANGE_CNT1     (sizeof(ranges1)/sizeof(*ranges1))

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        float pow_f = qhmath_pow_f((float)x_arr[i], (float)y_arr[i]);
        ref_output[i] = (__fp16)(pow_f);
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;
    float tolerance = 0.02;
    float error;

    printf("Test for qhmath_hvx_pow_ahf function:\n");

    INIT_INPUT_FLOAT16(x_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_FLOAT16(y_arr, ARR_LEN, ranges1, RANGE_CNT1, RAND_MAX)

    generate_ref_output(ARR_LEN);

    for (uint32_t i = 1; i <= ARR_LEN; ++i)
    {
        memset(output, 0, sizeof(__fp16)*ARR_LEN);
        qhmath_hvx_pow_ahf(x_arr, y_arr, output, i);

        for (uint32_t j = 0; j < i; ++j)
        {
            error = (float)ref_output[j] - (float)output[j];
            if (error < 0)
                error = -error;

            if (error > tolerance)
            {
                printf("Error:\tin = %.7f %.7f | qhmath_hvx_pow_ahf = %.7f "
                       "| OK = %.7f | vector length = %lu | iter = %lu\n",
                       x_arr[j], y_arr[j], output[j], ref_output[j], i, j);
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
