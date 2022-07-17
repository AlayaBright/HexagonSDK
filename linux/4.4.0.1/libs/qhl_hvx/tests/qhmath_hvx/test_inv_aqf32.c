/**=============================================================================
@file
    test_inv_aqf32.c

@brief
    Accuracy test for qhmath_hvx_inv_aqf32 function

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>


#include "qhmath_hvx.h"
#include "qhblas_test.h"

#include "hexagon_types.h"

static range_float_t ranges0[] =
{
    { -1000.0f, -100.0f  },
    { -100.0f,   100.0f  },
    {  100.0f,   1000.0f }
};

static float input_arr[ARR_LEN];
static float output[ARR_LEN];
static float ref_output[ARR_LEN];

static uint32_t input_arr_qf32[ARR_LEN];
static uint32_t output_qf32[ARR_LEN];

#define RANGE_CNT0     (sizeof(ranges0)/sizeof(*ranges0))

inline static void generate_ref_output(uint32_t size)
{
    for (uint32_t i = 0; i < size; ++i)
    {
        ref_output[i] = 1.0f/input_arr[i];
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
    uint32_t pass = 0;
    uint32_t fail = 0;
    float tolerance = 0.05;
    float error;

    printf("Test for qhmath_hvx_inv_aqf32 function:\n");

    INIT_INPUT_FLOAT(input_arr, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    generate_ref_output(ARR_LEN);

    convert_float_to_qf32(input_arr, input_arr_qf32);
    for (uint32_t i = 1; i <= ARR_LEN; ++i)
    {
        memset(output_qf32, 0, sizeof(float)*ARR_LEN);
        qhmath_hvx_inv_aqf32(input_arr_qf32, output_qf32, i);

        convert_qf32_to_float(output_qf32, output);
        for (uint32_t j = 0; j < i; ++j)
        {
            error = (float)ref_output[j] - (float)output[j];
            if (error < 0)
                error = -error;

            if (error > tolerance)
            {
                printf("Error:\tin = %.7f | qhmath_hvx_inv_aqf32 = %.7f "
                       "| OK = %.7f | vector length = %lu | iter = %lu\n",
                       input_arr[j], output[j], ref_output[j], i, j);
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
