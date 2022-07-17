/**=============================================================================
@file
    test_f_vector_dot_af.c

@brief
    Accuracy test for qhblas_hvx_f_vector_dot_af function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhblas_hvx.h"
#include "qhblas_test.h"

typedef struct test_invalid
{
    float *input_arr1;    // Pointer to first input array
    float *input_arr2;    // Pointer to second input array
    float *output;        // Pointer to output
    uint32_t length;      // Length of input arrays
    int32_t expect_res;   // Expected return value
} test_invalid;

static range_float_t ranges0[] =
{
    { -1.0f,   -1.0f },
    { -0.001f, -0.0f   },
    {  0.0f,    0.001f },
    {  0.001f,  1.0f   }
};

static float input_arr1[ARR_LEN];
static float input_arr2[ARR_LEN];
static float output;

static test_invalid test_data[] =
{
    { input_arr1, input_arr2, &output, 0,       -1 },
    { input_arr1, input_arr2, NULL,    ARR_LEN, -1 },
    { input_arr1, input_arr2, NULL,    0,       -1 },
    { input_arr1, NULL,       &output, ARR_LEN, -1 },
    { input_arr1, NULL,       &output, 0,       -1 },
    { input_arr1, NULL,       NULL,    ARR_LEN, -1 },
    { input_arr1, NULL,       NULL,    0,       -1 },
    { NULL,       input_arr2, &output, ARR_LEN, -1 },
    { NULL,       input_arr2, &output, 0,       -1 },
    { NULL,       input_arr2, NULL,    ARR_LEN, -1 },
    { NULL,       input_arr2, NULL,    0,       -1 },
    { NULL,       NULL,       &output, ARR_LEN, -1 },
    { NULL,       NULL,       &output, 0,       -1 },
    { NULL,       NULL,       NULL,    ARR_LEN, -1 },
    { NULL,       NULL,       NULL,    0,       -1 },
    { input_arr1, input_arr2, &output, ARR_LEN,  0 }
};

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(*test_data))

inline static float generate_ref_output(uint32_t size)
{
    float ref_output = 0.0f;

    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output += input_arr1[i] * input_arr2[i];
    }

    return ref_output;
}

int main(void)
{
    INIT_INPUT_FLOAT(input_arr1, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_FLOAT(input_arr2, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A2(qhblas_hvx_f_vector_dot_af)

    TEST_INPUT_SCALAR_QFLOAT32(qhblas_hvx_f_vector_dot_af, ARR_LEN, output,
                               0.001f, input_arr1, input_arr2)

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
