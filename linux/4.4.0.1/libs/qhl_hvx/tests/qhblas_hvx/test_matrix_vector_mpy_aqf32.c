/**=============================================================================
@file
    test_matrix_vector_mpy_aqf32.c

@brief
    Accuracy test for qhblas_hvx_matrix_vector_mpy_aqf32 function

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhblas_hvx.h"
#include "qhmath_hvx.h"
#include "qhblas_test.h"

#define M 100
#define N 100

#define TOLERANCE 0.001f

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

static float ref_output[N];

static qf32_t *input_arr1_qf32;
static qf32_t *input_arr2_qf32;
static qf32_t *output_qf32;

#define RANGE_CNT0    (sizeof(ranges0)/sizeof(*ranges0))

inline static void generate_ref_output(uint32_t m)
{
    memset(ref_output, 0, sizeof(float) * N);
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < m; j++)
        {
            ref_output[i] += input_arr1[i * m + j] * input_arr2[j];
        }
    }
}

int main(void)
{
    input_arr1_qf32 = (qf32_t *) memalign(128, sizeof(qf32_t) * M * N);
    input_arr2_qf32 = (qf32_t *) memalign(128, sizeof(qf32_t) * M);
    output_qf32 = (qf32_t *) memalign(128, sizeof(qf32_t) * N);

    input_arr1 = (float *) memalign(128, sizeof(float) * M * N);
    input_arr2 = (float *) memalign(128, sizeof(float) * M);
    output = (float *) memalign(128, sizeof(float) * N);

    INIT_INPUT_FLOAT(input_arr1, M * N, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_FLOAT(input_arr2, M, ranges0, RANGE_CNT0, RAND_MAX)
    // Convert input from IEEE float to qf32
    qhmath_hvx_aqf32_convert_asf (input_arr1, input_arr1_qf32, M * N);
    qhmath_hvx_aqf32_convert_asf (input_arr2, input_arr2_qf32, M);

    float error;
    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhblas_hvx_matrix_vector_mpy_aqf32 function:\n");

    for (uint32_t i = 1; i <= M; ++i)
    {
        generate_ref_output(i);

        qhblas_hvx_matrix_vector_mpy_aqf32(input_arr1_qf32, input_arr2_qf32, output_qf32, i, N);

        // Convert Output from qf32 to IEEE float
        qhmath_hvx_asf_convert_aqf32 (output_qf32, output, N);

        for (uint32_t j = 0; j < N; ++j)
        {
            error = ref_output[j] - output[j];
            if (error < 0)
                error = -error;
            if (error > TOLERANCE)
            {
                printf("Error:\t got = %.7f | OK = %.7f "
                       "| matrix width = %lu | matrix height = %u | i = %lu\n", output[j],
                       ref_output[j], i, N, j);
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
