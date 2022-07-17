/**=============================================================================
@file
    test_sqrt_aqf32.c

@brief
    Accuracy test for qhmath_hvx_sqrt_aqf32 function

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhmath_hvx.h"
#include "qhblas_test.h"

#include "hvx_internal.h"

#include <math.h>

typedef struct test_invalid
{
    qf32_t *input_arr;   // Pointer to input array
    qf32_t *output;      // Pointer to output
    uint32_t length;    // Length of input arrays
    int32_t expect_res; // Expected return value
} test_invalid;

static range_float_t ranges0[] =
{
    {1.0E-38, 1.175494351E-38},
    {1.0E-37, 2.0E-37}, {1.0E-35, 2.0E-35}, {1.0E-30, 2.0E-30},
    {1.0E-25, 2.0E-25}, {1.0E-20, 2.0E-20}, {1.0E-15, 2.0E-15},
    {1.0E-10, 2.0E-10}, {1.0E-05, 2.0E-05}, {1.0E-01, 2.0E-01},
    {1.0E-00, 2.0E-00}, {1.0E+05, 2.0E+05}, {1.0E+10, 2.0E+10},
    {1.0E+15, 2.0E+15}, {1.0E+20, 2.0E+20}, {1.0E+25, 2.0E+25},
    {1.0E+30, 2.0E+30}, {1.0E+35, 2.0E+35}, {1.0E+37, 2.0E+37},
    {2.0E+38, 2.55211754908E+38},
};

static float input_arr[ARR_LEN];
static float ref_output[ARR_LEN];
static float output[ARR_LEN];

static uint32_t input_arr_qf32[ARR_LEN];
static uint32_t output_qf32[ARR_LEN];

static test_invalid test_data[] =
    {{input_arr_qf32, output_qf32, 0, -1},
        {input_arr_qf32, NULL, ARR_LEN, -1},
        {input_arr_qf32, NULL, 0, -1},
        {NULL, output_qf32, ARR_LEN, -1},
        {NULL, output_qf32, 0, -1},
        {NULL, NULL, ARR_LEN, -1},
        {NULL, NULL, 0, -1},
        {input_arr_qf32, output_qf32, ARR_LEN, 0}};

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(*ranges0))
#define NUM_TEST_TYPE (sizeof(test_data) / sizeof(*test_data))


inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output[i] = sqrtf(input_arr[i]);
    }
}

static float convert_qf32_to_float_helper(qf32_t qf)
{
    uint32_t  __attribute__((aligned(128))) temp_in[32];
    float __attribute__((aligned(128))) temp_out[32];

    temp_in[0] = *(unsigned int*)&qf;
    HVX_Vector in = *(HVX_Vector*)temp_in;

    HVX_Vector out = Q6_Vsf_equals_Vqf32(in);

    *(HVX_Vector *)temp_out = out;
    return temp_out[0];
}

static qf32_t convert_float_to_qf32_helper(float sf)
{
    uint32_t  __attribute__((aligned(128))) temp_in[32];
    qf32_t __attribute__((aligned(128))) temp_out[32];

    temp_in[0] = *(unsigned int*)&sf;
    HVX_Vector in = *(HVX_Vector*)temp_in;

    HVX_Vector out = Q6_Vqf32_vadd_VsfVsf(in, Q6_V_vzero());

    *(HVX_Vector *)temp_out = out;
    return temp_out[0];
}

static void convert_qf32_to_float(qf32_t *in, float *out)
{
    for(size_t i = 0; i < ARR_LEN; i++)
    {
        out[i] = convert_qf32_to_float_helper(in[i]);
    }
}

static void convert_float_to_qf32(float *in, qf32_t *out)
{
    for(size_t i = 0; i < ARR_LEN; i++)
    {
        out[i] = convert_float_to_qf32_helper(in[i]);
    }

}

int main(void)
{
    INIT_INPUT_FLOAT(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    TEST_INVALID_INPUT_A1(qhmath_hvx_sqrt_aqf32)

    uint32_t pass = 0;
    uint32_t fail = 0;
    float tolerance = 0.001;
    generate_ref_output(ARR_LEN);

    convert_float_to_qf32(input_arr, input_arr_qf32);
    for (uint32_t i = 1; i <= ARR_LEN; ++i)
    {
        memset(output, 0, sizeof(__typeof__(*output)) * ARR_LEN);
        memset(output_qf32, 0, sizeof(__typeof__(*output_qf32)) * ARR_LEN);

        qhmath_hvx_sqrt_aqf32(input_arr_qf32, output_qf32, i);

        convert_qf32_to_float(output_qf32, output);

        for (uint32_t j = 0; j < i; ++j)
        {
            float error = ref_output[j] - output[j];
            float relative_err = (error/ref_output[j])*100;
            if (fabs(relative_err) > tolerance)
            {
                printf("Error:\trelative err(%) = {%.7f} input = %.7f | qhmath_hvx_sqrt_aqf32(x) = %.7f | OK = %.7f "
                        "| vector length = %lu | i = %lu\n",
                        relative_err, input_arr[j], output[j],
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
