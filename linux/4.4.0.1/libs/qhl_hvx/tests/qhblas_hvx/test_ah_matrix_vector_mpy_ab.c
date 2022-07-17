/**=============================================================================
@file
    test_ah_matrix_vector_mpy_ab.c

@brief
    Accuracy test for qhblas_hvx_ah_matrix_vector_mpy_ab function

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhblas_hvx.h"
#include "qhblas_test.h"

#define M 100
#define N 100

static range_int8_t ranges0[] = {
    {INT8_MIN, -100},
    {-100, 100},
    {100, INT8_MAX}};

static int8_t input_arr1[M * N];
static int8_t input_arr2[M];
static int16_t output[N];
static int16_t ref_output[N];

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(range_int8_t))

inline static void generate_ref_output(uint32_t m)
{
    memset(ref_output, 0, sizeof(int16_t) * N);
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < m; j++)
        {
            ref_output[i] += input_arr1[i * m + j] * input_arr2[j];
        }
        ref_output[i] <<= 1;
    }
}

int main(void)
{
    INIT_INPUT_INT8(input_arr1, M * N, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT8(input_arr2, M, ranges0, RANGE_CNT0, RAND_MAX)

    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhblas_hvx_ah_matrix_vector_mpy_ab function:\n");

    for (uint32_t i = 1; i <= M; ++i)
    {
        generate_ref_output(i);
        qhblas_hvx_ah_matrix_vector_mpy_ab(input_arr1, input_arr2, output, i, N);

        for (uint32_t j = 0; j < N; ++j)
        {
            if (ref_output[j] != output[j])
            {
                printf("Error:\tx = {%.7d, %.7d} | qhblas_hvx_ah_matrix_vector_mpy_ab(x) = %.7d | OK = %.7d "
                       "| matrix width = %lu | matrix height = %u | i = %lu\n",
                       input_arr1[j], input_arr2[j], output[j],
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
