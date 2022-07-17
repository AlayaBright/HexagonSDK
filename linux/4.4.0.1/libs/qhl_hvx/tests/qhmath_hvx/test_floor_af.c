/**=============================================================================
@file
    test_floor_af.c

@brief
    Accuracy test for qhmath_hvx_floor_af function

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
    float *input_arr;   // Pointer to input array
    float *output;      // Pointer to output
    uint32_t length;    // Length of input arrays
    int32_t expect_res; // Expected return value
} test_invalid;

static range_float_t ranges0[] =
    {{-100.0f, -10.0f},
     {-10.0f, -0.001f},
     {-0.001f, 0.001f},
     {0.001f, 10.0f},
     {10.0f, 100.0f}};

static float input_arr[ARR_LEN];
static float ref_output[ARR_LEN];
static float output[ARR_LEN];

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
        ref_output[i] = floorf(input_arr[i]);
    }
}

int main(void)
{
    INIT_INPUT_FLOAT(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A1(qhmath_hvx_floor_af)

    TEST_INPUT_QFLOAT_A1(qhmath_hvx_floor_af)

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
