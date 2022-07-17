/**=============================================================================
@file
    test_matrix_matrix_mpy_af.c

@brief
    Accuracy test for qhblas_hvx_matrix_matrix_mpy_af function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"

#include <hexagon_sim_timer.h>

#define M 128
#define K 128
#define N 512

#define TOLERANCE 0.001f

static range_float_t ranges0[] =
    {
        {-1.0f, -1.0f},
        {-0.001f, -0.0f},
        {0.0f, 0.001f},
        {0.001f, 1.0f}};

static float input_mtx1[M * K];
static float input_mtx2[K * N];
static float output[M * N];
static float ref_output[M * N];

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(*ranges0))

inline static void generate_ref_output(uint32_t n)
{
    int64_t result = 0;
    for (size_t i = 0; i < M; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            result = 0;
            for (size_t k = 0; k < K; k++)
            {
                ref_output[i * n + j] += input_mtx1[i * K + k] * input_mtx2[k * n + j];
            }
        }
    }
}

int main(void)
{
    INIT_INPUT_FLOAT(input_mtx1, M * K, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_FLOAT(input_mtx2, K * N, ranges0, RANGE_CNT0, RAND_MAX)

    float error;
    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhblas_hvx_matrix_matrix_mpy_af function:\n");

    for (uint32_t i = 1; i <= N; ++i)
    {
        printf("Running accuracy tests for input matrices %dx%d and %dx%lu...\n", M, K, K, i);
        generate_ref_output(i);

        qhblas_hvx_matrix_matrix_mpy_af(input_mtx1, input_mtx2, output, M, K, i);

        for (uint32_t j = 0; j < M * i; ++j)
        {
            error = ref_output[j] - output[j];
            if (error < 0)
                error = -error;
            if (error > TOLERANCE)
            {
                printf("Error: qhblas_hvx_matrix_matrix_mpy_af(x) = %.7f | OK = %.7f "
                       "| matrix 1 size = %u x %u | matrix 2 size = %u x %lu | i = %lu\n",
                       output[j], ref_output[j], M, K, K, i, j);
                ++fail;
            }
            else
            {
                ++pass;
            }
        }
        memset(ref_output, 0, M * i * sizeof(float));
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
