/**=============================================================================
@file
    test_cholesky_decomposition_aqf32.c

@brief
    Accuracy test for qhblas_hvx_cholesky_decomposition_aqf32 function

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"
#include "qhmath_hvx.h"
#include "hvx_internal.h"

#define M 300
#define TOLERANCE 0.001f

static float input_mtx1_float[M * M];
static uint32_t input_mtx1_qf32[M * M];
static uint32_t output_qf32[M * M];
static float output_float[M * M];
static float ref_output_float[M * M];

// generate m by m symmetric positive definite matrix
inline static void generate_input(uint32_t m)
{
    float lo = 1.0f;
    float hi = 2.0f;
    float random_value = 0;

    for (size_t i = 0; i < m; i++)
    {
        for (size_t j = 0; j < m; j++)
        {
            if (j <= i)
            {
                random_value = (float)(lo + (hi - lo) * ((float)rand() / (float)RAND_MAX));
                if (j == i)
                    random_value *= (i + m);
                input_mtx1_float[i * m + j] = random_value;
                input_mtx1_float[j * m + i] = random_value;
            }
        }
    }
}


float dotprod(float *a, float *b, int begin, int end)
{
    float sum = 0;

    for (int i = begin; i < end; ++i)
    {
        sum += a[i] * b[i];
    }
    return sum;
}

inline static void generate_ref_output(uint32_t m)
{
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < (i + 1); ++j)
        {
            float sum = dotprod(&ref_output_float[i * m], &ref_output_float[j * m], 0, j);
            if (i == j)
            {
                ref_output_float[i * m + j] = qhmath_sqrt_f(input_mtx1_float[i * m + i] - sum);
            }
            else
            {
                ref_output_float[i * m + j] = (1.0 / ref_output_float[j * m + j] * (input_mtx1_float[i * m + j] - sum));
            }
        }
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


void convert_to_float(qf32_t *in, float *out)
{
    for(size_t i = 0; i < M; i++)
    {
        for(size_t j = 0; j < M; j++)
        {
            out[i*M+j] = convert_qf32_to_float_helper(in[i*M+j]);
        }
    }

}

void convert_to_qf32(float *in, qf32_t *out)
{
    for(size_t i = 0; i < M; i++)
    {
        for(size_t j = 0; j < M; j++)
        {
            out[i*M+j] = convert_float_to_qf32_helper(in[i*M+j]);
        }
    }

}


int main(void)
{
    float error = 0;
    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhblas_hvx_cholesky_decomposition_af function:\n");

    for (uint32_t i = M; i <= M; ++i)
    {
        printf("Running accuracy tests for input matrix %dx%d...\n", i, i);

        generate_input(i);
        generate_ref_output(i);

        convert_to_qf32(input_mtx1_float, input_mtx1_qf32);
        qhblas_hvx_cholesky_decomposition_aqf32(input_mtx1_qf32, output_qf32, i);
        convert_to_float((uint32_t*)output_qf32, (float*)output_float);

        for (uint32_t j = 0; j < i * i; ++j)
        {
            error = output_float[j] - ref_output_float[j];
            if (error < 0)
                error = -error;
            if (error > TOLERANCE)
            {
                printf("Error: qhblas_hvx_cholesky_decomposition_af(x) = %.7f | OK = %.7f "
                       "| matrix size = %u x %u | i = %u\n",
                       output_float[j], ref_output_float[j], i, i, j);
                ++fail;
            }
            else
            {
                ++pass;
            }
        }
        memset(output_qf32, 0, i * i * sizeof(float));
        memset(ref_output_float, 0, i * i * sizeof(float));
    }

    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");
    printf("(%u tests passed, %u failed)\n", pass, fail);

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
