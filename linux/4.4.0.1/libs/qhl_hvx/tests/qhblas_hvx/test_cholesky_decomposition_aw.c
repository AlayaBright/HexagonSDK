/**=============================================================================
@file
    test_cholesky_decomposition_aw.c

@brief
    Accuracy test for qhblas_hvx_cholesky_decomposition_aw function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"

#include "qhblas_hvx.h"
#include "qhblas_test.h"

#define M 300

static int32_t input_mtx1[M * M];
static int32_t output[M * M];
static int32_t ref_output[M * M];

// generate m by m symmetric positive definite matrix
inline static void generate_input(uint32_t m)
{
    int32_t lo = 100000;
    int32_t hi = 900000;
    int32_t random_value = 0;

    for (size_t i = 0; i < m; i++)
    {
        for (size_t j = 0; j < m; j++)
        {
            if (j <= i)
            {
                random_value = (int32_t)(lo + (hi - lo) * ((float)rand() / (float)RAND_MAX));
                if (j == i)
                    random_value *= (i + m);
                input_mtx1[i * m + j] = random_value;
                input_mtx1[j * m + i] = random_value;
            }
        }
    }
}

int32_t dotprod(int32_t *a, int32_t *b, int begin, int end)
{
    int32_t sum = 0;
    int32_t mpy = 0;

    for (uint32_t i = begin; i < end; ++i)
    {
        mpy = ((int64_t)a[i] * (int64_t)b[i] + 0x40000000) >> 31;
        sum += (mpy == 0x80000000 ? 0x7fffffff : mpy);
    }

    return sum;
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

            if (i == j)
            {
                sum = input_mtx1[i * m + i] - sum;
                float_sum = (float)sum / (float)2147483648UL;
                float_sum = qhmath_sqrt_f(float_sum) * (float)2147483648UL;
                ref_output[i * m + j] = (int32_t)float_sum;
            }
            else
            {
                sum = input_mtx1[i * m + j] - sum;
                float_sum = (float)sum / (float)2147483648UL;
                float_sum = float_sum / ((float)ref_output[j * m + j] / (float)2147483648UL);
                float_sum *= 2147483648UL;
                ref_output[i * m + j] = (int32_t)float_sum;
            }
        }
    }
}

int main(void)
{
    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhblas_hvx_cholesky_decomposition_aw function:\n");

    for (uint32_t i = 1; i <= M; ++i)
    {
        printf("Running accuracy tests for input matrix %dx%d...\n", i, i);

        generate_input(i);
        generate_ref_output(i);

        qhblas_hvx_cholesky_decomposition_aw(input_mtx1, output, i);

        for (uint32_t j = 0; j < i * i; ++j)
        {
            if (ref_output[j] != output[j])
            {
                printf("Error: qhblas_hvx_cholesky_decomposition_aw(x) = %d | OK = %d "
                       "| matrix size = %u x %u | i = %u\n",
                       output[j], ref_output[j], i, i, j);
                ++fail;
            }
            else
            {
                ++pass;
            }
        }
        memset(output, 0, i * i * sizeof(int32_t));
        memset(ref_output, 0, i * i * sizeof(int32_t));
    }

    printf("%s ", (fail == 0) ? "PASSED" : "FAILED");
    printf("(%u tests passed, %u failed)\n", pass, fail);

    return 0;
}
