/**=============================================================================
@file
    test_matrix_vector_mpy_ah.c

@brief
    Accuracy test for qhblas_hvx_matrix_vector_mpy_ah function

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

static range_int16_t ranges0[] = {
    {INT16_MIN, -100},
    {-100, 100},
    {100, INT16_MAX}};

static int16_t input_arr1[M * N];
static int16_t input_arr2[M];
static int16_t output[N];
static int16_t ref_output[N];

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(range_int16_t))

inline static void generate_ref_output(uint32_t m)
{
    for (size_t i = 0; i < N; i++)
    {
        int32_t result = 0;
        for (size_t j = 0; j < m; j++)
        {
            result += input_arr1[i * m + j] * input_arr2[j];
        }
        ref_output[i] = (int16_t)((result >> 15) & 0xFFFF);
    }
}

int main(void)
{
    INIT_INPUT_INT16(input_arr1, M * N, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT16(input_arr2, M, ranges0, RANGE_CNT0, RAND_MAX)

    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhblas_hvx_matrix_vector_mpy_ah function:\n");

    for (uint32_t i = 1; i <= N; ++i)
    {
        generate_ref_output(i);

        qhblas_hvx_matrix_vector_mpy_ah(input_arr1, input_arr2, output, i, N);

        for (uint32_t j = 0; j < N; ++j)
        {
            if (ref_output[j] != output[j])
            {
                printf("Error:\tx = {%.7d, %.7d} | qhblas_hvx_matrix_vector_mpy_ah(x) = %.7d | OK = %.7d "
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
