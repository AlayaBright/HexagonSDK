/**=============================================================================
@file
    test_hf_vector_norm_ahf.c

@brief
    Accuracy test for qhblas_hvx_hf_vector_norm_ahf function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhblas_hvx.h"
#include "qhblas_test.h"
#include "qhmath.h"

typedef struct test_invalid
{
    __fp16 *input_arr;       // Pointer to input array
    __fp16 *output;          // Pointer to output
    uint32_t length;         // Length of input arrays
    int32_t expect_res;      // Expected return value
} test_invalid;

static range_float_t ranges0[] =
{
    { -1.0f,   -0.001f },
    { -0.001f, -0.0f   },
    {  0.0f,    0.001f },
    {  0.001f,  1.0f   }
};

static __fp16 input_arr[ARR_LEN];
static __fp16 output;

static test_invalid test_data[] =
{
    { input_arr, &output, 0,       -1 },
    { input_arr, NULL,    ARR_LEN, -1 },
    { input_arr, NULL,    0,       -1 },
    { NULL,      &output, ARR_LEN, -1 },
    { NULL,      &output, 0,       -1 },
    { NULL,      NULL,    ARR_LEN, -1 },
    { NULL,      NULL,    0,       -1 },
    { input_arr, &output, ARR_LEN,  0 }
};

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(*test_data))

inline static float generate_ref_output(uint32_t size)
{
    float ref_output = 0.0f;

    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output += (float)input_arr[i] * (float)input_arr[i];
    }

    return qhmath_sqrt_f(ref_output);
}

int main(void)
{
    INIT_INPUT_FLOAT16(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A1(qhblas_hvx_hf_vector_norm_ahf)

    TEST_INPUT_SCALAR_QFLOAT16(qhblas_hvx_hf_vector_norm_ahf, ARR_LEN, output,
                               0.3f, input_arr)

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
