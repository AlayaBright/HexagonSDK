/**=============================================================================
@file
    bench_cholesky_decomposition_aw.c

@brief
    Benchmark for measuring performance of qhblas_hvx_cholesky_decomposition_aw function.

    Benchmark measures number of cycles needed for executing qhblas_hvx_cholesky_decomposition_aw function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and cycles per member (CPM) and it is calculated as

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

static range_int32_t ranges0[] = {
    {0, 10},
    {10, 100},
    {100, 1000},
};

static range_uint32_t vector_ranges[] = {
    {0, 10},
    {10, 500},
    {500, 3000},
};

static uint32_t lengths_of_arrays[VEC_LEN];
static int32_t input_arr1[M * M];
static int32_t output[M * M];

static void clear_cache(uint32_t length)
{
#if __hexagon__
    hexagon_buffer_cleaninv((uint8_t *)input_arr1, length * sizeof(int32_t));
#endif
}

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(range_int32_t))
#define RANGE_VECTOR (sizeof(vector_ranges) / sizeof(range_uint32_t))

int main(void)
{
    BENCH_VECTOR_INIT_INPUT(lengths_of_arrays, VEC_LEN, vector_ranges, RANGE_VECTOR, RAND_MAX)

    INIT_INPUT_INT16(input_arr1, M * M, ranges0, RANGE_CNT0, RAND_MAX)

    performance_high();
    printf("Benchmark for qhblas_hvx_cholesky_decomposition_aw function:\n");

    double cpc_sum = 0.0;
    double cpm_sum = 0.0;
    double uspc_sum = 0.0;
    double uspm_sum = 0.0;
    uint64_t length_sum = 0;
    for (uint32_t i = 0; i < RANGE_VECTOR; ++i)
    {
        double cpc = 0.0;
        double cpm = 0.0;
        double uspc = 0.0;
        double uspm = 0.0;
        uint32_t length = lengths_of_arrays[i];
        for (uint32_t j = 0; j < NR_OF_ITERATIONS; ++j)
        {
            clear_cache(length * length);
            measurement_start();
            qhblas_hvx_cholesky_decomposition_aw(input_arr1, output, length);
            measurement_finish();
            cpc += measurement_get_cycles();
            uspc += measurement_get_us();
        }
        cpc /= (double)NR_OF_ITERATIONS;
        cpm = cpc / (double)(length * length);
        uspc /= (double)NR_OF_ITERATIONS;
        uspm = uspc / (double)(length * length);
        length_sum += length * length;
        cpc_sum += cpc;
        uspc_sum += uspc;
        printf("  %11.4f CPC %11.4f MPS |  %11.4f CPM  %11.4f MPSpm | matix size = %lux%lu\n",
               cpc, 1.0 / uspc, cpm, 1.0 / uspm, length, length);
    }
    cpm_sum = cpc_sum / (double)length_sum;
    uspm_sum = uspc_sum / (double)length_sum;
    printf("\nSummary:\n");
    printf("  %11.4f CPC %11.4f MPS | %11.4f CPM %11.4f MPSPM\n",
           cpc_sum, 1.0 / uspc_sum, cpm_sum, 1.0 / uspm_sum);

    performance_normal();
    return 0;
}
