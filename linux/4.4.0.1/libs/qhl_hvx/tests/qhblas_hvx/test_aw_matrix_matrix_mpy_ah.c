/**=============================================================================
@file
    test_aw_matrix_matrix_mpy_ah.c

@brief
    Accuracy test for qhblas_hvx_aw_matrix_matrix_mpy_ah function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"

#define M 128
#define K 128
#define N 512

static range_int16_t ranges0[] = {
    {-10000, -1000},
    {-1000, 1000},
    {1000, 10000}};

static int16_t input_mtx1[M * K];
static int16_t input_mtx2[K * N];
static int32_t output[M * N];
static int32_t ref_output[M * N];

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(range_int16_t))

static void inline sat32(int64_t *number)
{
    if (*number > INT32_MAX)
    {
        *number = INT32_MAX;
    }
    if (*number < INT32_MIN)
    {
        *number = INT32_MIN;
    }
}

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
                result = (input_mtx1[i * K + k]) * (input_mtx2[k * n + j]);
                result <<= 1;
                result = ref_output[i * n + j] + result;
                sat32(&result);
                ref_output[i * n + j] = result;
            }
        }
    }
}

int main(void)
{
    INIT_INPUT_INT16(input_mtx1, M * K, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT16(input_mtx2, K * N, ranges0, RANGE_CNT0, RAND_MAX)

    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhblas_hvx_aw_matrix_matrix_mpy_ah function:\n");

    for (uint32_t i = 1; i <= N; ++i)
    {
        printf("Running accuracy tests for input matrices %dx%d and %dx%lu...\n", M, K, K, i);

        generate_ref_output(i);

        qhblas_hvx_aw_matrix_matrix_mpy_ah(input_mtx1, input_mtx2, output, M, K, i);

        for (uint32_t j = 0; j < M * i; ++j)
        {
            if (ref_output[j] != output[j])
            {
                printf("Error: qhblas_hvx_aw_matrix_matrix_mpy_ah(x) = %.7ld | OK = %.7ld "
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
