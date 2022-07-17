/**=============================================================================
@file
    test_exp2_ahf.c

@brief
    Accuracy test for qhmath_hvx_exp2_ahf function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhmath_hvx.h"
#include "qhblas_test.h"

#include <math.h>

typedef struct test_invalid
{
    __fp16 *input_arr;   // Pointer to input array
    __fp16 *output;      // Pointer to output
    uint32_t length;    // Length of input arrays
    int32_t expect_res; // Expected return value
} test_invalid;

static range_float_t ranges0[] =
{
        {-10.0f, -5.0f},
        {-5.0f, 0.0f},
        {0.0f, 5.0f},

};

static __fp16 input_arr[ARR_LEN];
static __fp16 ref_output[ARR_LEN];
static __fp16 output[ARR_LEN];

static test_invalid test_data[] =
    {
        {input_arr, output, 0, -1},
        {input_arr, NULL, ARR_LEN, -1},
        {input_arr, NULL, 0, -1},
        {NULL, output, ARR_LEN, -1},
        {NULL, output, 0, -1},
        {NULL, NULL, ARR_LEN, -1},
        {NULL, NULL, 0, -1},
        {input_arr, output, ARR_LEN, 0}};

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(*ranges0))
#define NUM_TEST_TYPE (sizeof(test_data) / sizeof(*test_data))

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output[i] =  (__fp16)exp2f((float)input_arr[i]);
    }
}

int main(void)
{
    INIT_INPUT_FLOAT16(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A1(qhmath_hvx_exp2_ahf)

    uint32_t pass = 0;
    uint32_t fail = 0;
    float tolerance = 8.0;
    generate_ref_output(ARR_LEN);

    for (uint32_t i = 1; i <= ARR_LEN; ++i)
    {
        memset(output, 0, sizeof(__typeof__(*output)) * ARR_LEN);

        qhmath_hvx_exp2_ahf(input_arr, output, i);

        for (uint32_t j = 0; j < i; ++j)
        {
            float error = ref_output[j] - output[j];
            if (error < 0)
                error = -error;
            if (error > tolerance)
            {
                printf("Error:\tx = {%.7f} | qhmath_hvx_exp2_ahf(x) = %.7f | OK = %.7f "
                       "| vector length = %lu | i = %lu\n",
                       input_arr[j], output[j],
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
