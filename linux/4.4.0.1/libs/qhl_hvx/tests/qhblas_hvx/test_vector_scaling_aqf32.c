/**=============================================================================
@file
    test_vector_scaling_aqf32.c

@brief
    Accuracy test for qhblas_hvx_vector_scaling_aqf32 function

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhmath_hvx.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"

typedef struct test_invalid
{
    qf32_t *input_arr;        // Pointer to first input array
    qf32_t scaling_factor;    // Scaling factor
    qf32_t *output;           // Pointer to output array
    uint32_t length;         // Length of input arrays
    int32_t expect_res;      // Expected return value
} test_invalid;

static range_float_t ranges0[] =
{
    { -1000.0f, -100.0f  },
    { -100.0f,   100.0f  },
    {  100.0f,   1000.0f }
};

static float *input_arr;
static float *scale_factor; //  = 0.3f;
static float *output;
static qf32_t *input_arr_qf32;
static qf32_t *scale_factor_qf32; // 0.3 = 0x2666667e;
static qf32_t *output_qf32;
static float ref_output[ARR_LEN];


#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))
#define NUM_TEST_TYPE (sizeof(test_data)/sizeof(*test_data))

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output[i] = input_arr[i] * (*scale_factor);
    }
}

int main(void)
{
    input_arr_qf32 = (qf32_t *) memalign(128, sizeof(qf32_t) * ARR_LEN);
    output_qf32 = (qf32_t *) memalign(128, sizeof(qf32_t) * ARR_LEN);
    scale_factor_qf32 = (qf32_t *) memalign(128, sizeof(qf32_t) * 1);

    input_arr = (float *) memalign(128, sizeof(float) * ARR_LEN);
    output = (float *) memalign(128, sizeof(float) * ARR_LEN);
    scale_factor = (float *) memalign(128, sizeof(float) * 1);


    INIT_INPUT_FLOAT(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    // Convert input data from IEEE to qfloat
    qhmath_hvx_aqf32_convert_asf (input_arr, input_arr_qf32, ARR_LEN);
    qhmath_hvx_aqf32_convert_asf (scale_factor, scale_factor_qf32, 1);


    float error;
    uint32_t pass = 0;
    uint32_t fail = 0;
    generate_ref_output(ARR_LEN);

    printf("Test for qhblas_hvx_vector_scaling_aqf32 function:\n");

    for (uint32_t i = 1; i <= ARR_LEN; ++i)
    {
        memset(output, 0, sizeof(__typeof__(*output))*ARR_LEN);
        memset(output_qf32, 0, sizeof(__typeof__(*output_qf32))*ARR_LEN);
        qhblas_hvx_vector_scaling_aqf32(input_arr_qf32, *scale_factor_qf32, output_qf32, i);

        // Convert qfloat to IEEE Float
        qhmath_hvx_asf_convert_aqf32 (output_qf32, output, i);

        for (uint32_t j = 0; j < i; ++j)
        {
            error = ref_output[j] - output[j];
            if (error < 0) error = -error;
            if (error >  0.0001f)
            {
                printf("Error: input(x) = {%.7f, %.7f} | %s(x) = %.7f | OK = %.7f "
                       "| vector length = %lu | i = %lu | tolerance = %f\n",
                       input_arr[j], *scale_factor, qhblas_hvx_vector_scaling_aqf32, output[j],
                       ref_output[j], i, j, 0.0001f);
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
