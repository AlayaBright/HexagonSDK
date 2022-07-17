/**=============================================================================
@file
    test_matrix_vector_mpy_aw.c

@brief
    Accuracy test for qhblas_hvx_matrix_vector_mpy_aw function

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

static range_int32_t ranges0[] =
{
    { -200000, -10000  },
    { -10000,  -1000   },
    { -1000,    1000   },
    {  1000,    10000  },
    {  10000,   200000 }
};

static int32_t input_arr1[M * N];
static int32_t input_arr2[M];
static int32_t output[N];
static int32_t ref_output[N];

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(range_int32_t))

inline static void generate_ref_output(uint32_t m)
{
    memset(ref_output, 0, sizeof(int32_t) * N);
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < m; j++)
        {
            ref_output[i] += (((int64_t) input_arr1[i * m + j] * (int64_t) input_arr2[j] + 0x40000000) >> 31);
        }
    }
}

int main(void)
{
    INIT_INPUT_INT32(input_arr1, M * N, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT32(input_arr2, M, ranges0, RANGE_CNT0, RAND_MAX)

    uint32_t pass = 0;
    uint32_t fail = 0;

    printf("Test for qhblas_hvx_matrix_vector_mpy_aw function:\n");

    for (uint32_t i = 1; i <= M; ++i)
    {
        generate_ref_output(i);

        qhblas_hvx_matrix_vector_mpy_aw(input_arr1, input_arr2, output, i, N);

        for (uint32_t j = 0; j < N; ++j)
        {
            if (ref_output[j] != output[j])
            {
                printf("Error:\tx = {%.7ld, %.7ld} | qhblas_hvx_matrix_vector_mpy_aw(x) = %.7ld | OK = %.7ld "
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
