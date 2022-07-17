/**=============================================================================
@file
    bench_matrix_integrate_aw.c

@brief
    Benchmark for measuring performance of qhblas_hvx_matrix_integrate_aw function.

    Benchmark measures number of cycles needed for executing qhblas_hvx_matrix_integrate_aw function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include "qhblas_hvx.h"
#include "qhblas_test.h"

#undef VEC_MAX_ARR_LEN
#define VEC_MAX_ARR_LEN 4000000

#undef VEC_LEN
#define VEC_LEN 20
static range_int32_t ranges0[] = {
    {  INT32_MIN, -1000      },
    { -1000,       1000      },
    {  1000,       INT32_MAX }
};

static range_uint32_t vector_ranges[] = {
    { 0,    128     },
    { 128,   512    },
    { 512,   1000   },
    { 1000,  2000   },
};

static uint32_t lengths_of_arrays[VEC_LEN];
static int32_t input_arr[VEC_MAX_ARR_LEN];
static int32_t output[VEC_MAX_ARR_LEN];

static void clear_cache(uint32_t length)
{
#if __hexagon__
    hexagon_buffer_cleaninv((uint8_t *)input_arr, length*sizeof(int32_t));
#endif
}

#define RANGE_CNT0   (sizeof(ranges0)/sizeof(range_int32_t))
#define RANGE_VECTOR (sizeof(vector_ranges)/sizeof(range_uint32_t))

BENCH_TFUN_HVX_MATRIX(qhblas_hvx_matrix_integrate_aw, input_arr, output)

int main(void)
{
    BENCH_VECTOR_INIT_INPUT(lengths_of_arrays, VEC_LEN, vector_ranges, RANGE_VECTOR, RAND_MAX)

    INIT_INPUT_INT32(input_arr, VEC_MAX_ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    BENCH_TCALL(qhblas_hvx_matrix_integrate_aw, NR_OF_ITERATIONS, lengths_of_arrays, VEC_LEN)

    return 0;
}
