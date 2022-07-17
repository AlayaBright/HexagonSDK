/**=============================================================================
@file
    bench_aw_matrix_vector_mpy_ah.c

@brief
    Benchmark for measuring performance of qhblas_hvx_aw_matrix_vector_mpy_ah function.

    Benchmark measures number of cycles needed for executing qhblas_hvx_aw_matrix_vector_mpy_ah function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls
    CPM = CPC / (height_of_output_mtx * width_of_output_mtx)

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include "qhblas_hvx.h"
#include "qhblas_test.h"

#define M 1000
#define N 10000

static range_int16_t ranges0[] = {
    {INT16_MIN, -1000},
    {-1000, 1000},
    {1000, INT16_MAX}};

static range_uint32_t vector_ranges[] = {
    {0, 10},
    {10, 100},
    {100, 1000},
    {1000, 10000},
};

static uint32_t lengths_of_arrays[VEC_LEN];
static int16_t input_arr1[M * N];
static int16_t input_arr2[M];
static int32_t output[N];

static void clear_cache(uint32_t length)
{
#if __hexagon__
    hexagon_buffer_cleaninv((uint8_t *)input_arr1, length*sizeof(int8_t));
    hexagon_buffer_cleaninv((uint8_t *)input_arr2, length*sizeof(int8_t));
#endif
}

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(range_int16_t))
#define RANGE_VECTOR (sizeof(vector_ranges) / sizeof(range_uint32_t))

BENCH_TFUN(qhblas_hvx_aw_matrix_vector_mpy_ah, input_arr1, input_arr2, output, M)

int main(void)
{
    BENCH_VECTOR_INIT_INPUT(lengths_of_arrays, VEC_LEN, vector_ranges, RANGE_VECTOR, RAND_MAX)

    INIT_INPUT_INT16(input_arr1, M * N, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT16(input_arr2, M, ranges0, RANGE_CNT0, RAND_MAX)

    BENCH_TCALL(qhblas_hvx_aw_matrix_vector_mpy_ah, NR_OF_ITERATIONS, lengths_of_arrays, VEC_LEN)

    return 0;
}
