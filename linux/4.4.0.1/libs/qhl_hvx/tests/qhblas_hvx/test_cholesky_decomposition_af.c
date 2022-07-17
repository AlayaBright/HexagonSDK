/**=============================================================================
@file
    test_cholesky_decomposition_af.c

@brief
    Accuracy test for qhblas_hvx_cholesky_decomposition_af function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"

#define M 300
#define TOLERANCE 0.001f

static float input_mtx1[M * M];
static float output[M * M];
static float ref_output[M * M];

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
                input_mtx1[i * m + j] = random_value;
                input_mtx1[j * m + i] = random_value;
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

            float sum = dotprod(&ref_output[i * m], &ref_output[j * m], 0, j);

            if (i == j)
            {
                ref_output[i * m + j] = qhmath_sqrt_f(input_mtx1[i * m + i] - sum);
            }
            else
            {
                ref_output[i * m + j] = (1.0 / ref_output[j * m + j] * (input_mtx1[i * m + j] - sum));
            }
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

        qhblas_hvx_cholesky_decomposition_af(input_mtx1, output, i);

        for (uint32_t j = 0; j < i * i; ++j)
        {
            error = ref_output[j] - output[j];
            if (error < 0)
                error = -error;
            if (error > TOLERANCE)
            {
                printf("Error: qhblas_hvx_cholesky_decomposition_af(x) = %.7f | OK = %.7f "
                       "| matrix size = %u x %u | i = %u\n",
                       output[j], ref_output[j], i, i, j);
                ++fail;
            }
            else
            {
                ++pass;
            }
        }
        memset(output, 0, i * i * sizeof(float));
        memset(ref_output, 0, i * i * sizeof(float));
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
