/**=============================================================================
@file
    test_qf32_vector_dot_aqf32.c

@brief
    Accuracy test for qhblas_hvx_qf32_vector_dot_aqf32 function

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhblas_hvx.h"
#include "qhmath_hvx.h"
#include "qhblas_test.h"

typedef struct test_invalid
{
    qf32_t *input_arr1;    // Pointer to first input array
    qf32_t *input_arr2;    // Pointer to second input array
    qf32_t *output;        // Pointer to output
    uint32_t length;       // Length of input arrays
    int32_t expect_res;    // Expected return value
} test_invalid;

static range_float_t ranges0[] =
{
    { -1.0f,   -1.0f },
    { -0.001f, -0.0f   },
    {  0.0f,    0.001f },
    {  0.001f,  1.0f   }
};

static float *input_arr1;
static float *input_arr2;
static float *output;
static qf32_t *input_arr1_qf32;
static qf32_t *input_arr2_qf32;
static qf32_t *output_qf32;

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
    input_arr1_qf32 = (qf32_t *) memalign(128, sizeof(qf32_t) * ARR_LEN);
    input_arr2_qf32 = (qf32_t *) memalign(128, sizeof(qf32_t) * ARR_LEN);
    output_qf32 = (qf32_t *) memalign(128, sizeof(qf32_t) * 1);

    input_arr1 = (float *) memalign(128, sizeof(float) * ARR_LEN);
    input_arr2 = (float *) memalign(128, sizeof(float) * ARR_LEN);
    output = (float *) memalign(128, sizeof(float) * 1);

    INIT_INPUT_FLOAT(input_arr1, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_FLOAT(input_arr2, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    // Convert input data from IEEE float to qf32
    qhmath_hvx_aqf32_convert_asf (input_arr1, input_arr1_qf32, ARR_LEN);
    qhmath_hvx_aqf32_convert_asf (input_arr2, input_arr2_qf32, ARR_LEN);


    float error;
    uint32_t pass = 0;
    uint32_t fail = 0;
    float ref_output = 0;

    printf("Test for qhblas_hvx_qf32_vector_dot_aqf32 function:\n");

    for (uint32_t i = 1; i <= ARR_LEN; ++i)
    {
        ref_output = generate_ref_output(i);

        qhblas_hvx_qf32_vector_dot_aqf32(input_arr1_qf32, input_arr2_qf32, output_qf32, i);
        // convert qf32 to IEEE float
        qhmath_hvx_asf_convert_aqf32 (output_qf32, output, 1);

        error = ref_output - (float)*output;
        if (error < 0) error = -error;
        if (error > 0.001f)
        {
            printf("Error: qhblas_hvx_qf32_vector_dot_aqf32(x) = %.7f | OK = %.7f | vector length = %lu\n",
                   *output, ref_output, i);
            ++fail;
        }
        else
        {
            ++pass;
        }
        ref_output = 0;
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
