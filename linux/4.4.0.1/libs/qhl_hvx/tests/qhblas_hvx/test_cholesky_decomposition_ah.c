/**=============================================================================
@file
    test_cholesky_decomposition_ah.c

@brief
    Accuracy test for qhblas_hvx_cholesky_decomposition_ah function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"

#include "qhblas_hvx.h"
#include "qhblas_test.h"

#define M 300

static int16_t input_mtx1[M * M];
static int16_t output[M * M];
static int16_t ref_output[M * M];

// generate m by m symmetric positive definite matrix
inline static void generate_input(uint32_t m)
{
    int16_t lo = 10;
    int16_t hi = 90;
    int16_t random_value = 0;

    for (size_t i = 0; i < m; i++)
    {
        for (size_t j = 0; j < m; j++)
        {
            if (j <= i)
            {
                random_value = (int16_t)(lo + (hi - lo) * ((float)rand() / (float)RAND_MAX));
                if (j == i)
                    random_value *= (i + m * 10);
                input_mtx1[i * m + j] = random_value;
                input_mtx1[j * m + i] = random_value;
            }
        }
    }
}

int32_t dotprod(int16_t *a, int16_t *b, int begin, int end)
{
    int32_t sum = 0;

    for (uint32_t i = begin; i < end; ++i)
    {
        sum += (int32_t)a[i] * (int32_t)b[i];
    }

    return sum << 1;
}

inline static void generate_ref_output(uint32_t m)
{
    int32_t sum = 0;
    float float_sum = 0;
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < (i + 1); ++j)
        {
            sum = dotprod(&ref_output[i * m], &ref_output[j * m], 0, j);
            sum >>=16;

            if (i == j)
            {
                sum = input_mtx1[i * m + i] - sum;
                float_sum = (float)sum / (float)32768;
                float_sum = qhmath_sqrt_f(float_sum) * (float)32768;
                ref_output[i * m + j] = (int16_t)float_sum;
            }
            else
            {
                sum = input_mtx1[i * m + j] - sum;
                float_sum = (float)sum / (float)32768;
                float_sum = float_sum / ((float)ref_output[j * m + j] / (float)32768);
                float_sum *= 32768;
                ref_output[i * m + j] = (int16_t)float_sum;
            }
        }
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhblas_hvx_cholesky_decomposition_ah function:\n");

    for (uint32_t i = 1; i <= M; ++i)
    {
        printf("Running accuracy tests for input matrix %dx%d...\n", i, i);

        generate_input(i);
        generate_ref_output(i);

        qhblas_hvx_cholesky_decomposition_ah(input_mtx1, output, i);

        for (uint32_t j = 0; j < i * i; ++j)
        {
            if (ref_output[j] != output[j])
            {
                printf("Error: qhblas_hvx_cholesky_decomposition_ah(x) = %d | OK = %d "
                       "| matrix size = %u x %u | i = %u\n",
                       output[j], ref_output[j], i, i, j);
                ++fail;
            }
            else
            {
                ++pass;
            }
        }
        memset(output, 0, i * i * sizeof(int16_t));
        memset(ref_output, 0, i * i * sizeof(int16_t));
    }

    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");
    printf("(%u tests passed, %u failed)\n", pass, fail);

    return 0;
}
