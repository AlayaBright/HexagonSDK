/**=============================================================================
@file
    test_vector_scaling_af.c

@brief
    Accuracy test for qhblas_hvx_vector_scaling_af function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"

typedef struct test_invalid
{
    float *input_arr;        // Pointer to first input array
    float scaling_factor;    // Scaling factor
    float *output;           // Pointer to output array
    uint32_t length;         // Length of input arrays
    int32_t expect_res;      // Expected return value
} test_invalid;

static range_float_t ranges0[] =
{
    { -1000.0f, -100.0f  },
    { -100.0f,   100.0f  },
    {  100.0f,   1000.0f }
};

static float input_arr[ARR_LEN];
static const float scale_factor = 0.3f;
static float output[ARR_LEN];
static float ref_output[ARR_LEN];

static test_invalid test_data[] =
{
    { input_arr, scale_factor, output, 0,       -1 },
    { input_arr, scale_factor, NULL,   ARR_LEN, -1 },
    { input_arr, scale_factor, NULL,   0,       -1 },
    { NULL,      scale_factor, output, ARR_LEN, -1 },
    { NULL,      scale_factor, output, 0,       -1 },
    { NULL,      scale_factor, NULL,   ARR_LEN, -1 },
    { NULL,      scale_factor, NULL,   0,       -1 },
    { input_arr, scale_factor, output, ARR_LEN,  0 }
};

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(*test_data))

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output[i] = input_arr[i] * scale_factor;
    }
}

int main(void)
{
    INIT_INPUT_FLOAT(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_SCALE(qhblas_hvx_vector_scaling_af)

    TEST_INPUT_SCALE_QFLOAT32_T(qhblas_hvx_vector_scaling_af, 0.0001f)

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
