/**=============================================================================
@file
    bench_matrix_matrix_mpy_ahf.c

@brief
    Benchmark for measuring performance of qhblas_hvx_matrix_matrix_mpy_ahf function.

    Benchmark measures number of cycles needed for executing qhblas_hvx_matrix_matrix_mpy_ahf function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and cycles per member (CPM) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls
    CPM = CPC / (height_of_output_mtx * width_of_output_mtx)

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>
#include "qhblas_hvx.h"
#include "qhblas_test.h"

#define M 128
#define K 128
#define N 5000

static range_float_t ranges0[] = {
    {-10000.0f, -0.001f},
    {-0.001f, -0.0f},
    {0.0f, 0.001f},
    {0.001f, 10000.0f},
};

static range_uint32_t vector_ranges[] = {
    {0, 100},
    {100, 500},
    {500, 1000},
    {1000, 5000},
};

static uint32_t lengths_of_arrays[VEC_LEN];
static __fp16 input_mtx1[M * K];
static __fp16 input_mtx2[K * N];
static __fp16 output[M * N];

static void clear_cache(uint32_t length)
{
#if __hexagon__
    hexagon_buffer_cleaninv((uint8_t *)input_mtx1, M * K * sizeof(__fp16));
    hexagon_buffer_cleaninv((uint8_t *)input_mtx2, K * length * sizeof(__fp16));
#endif
}

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(*ranges0))
#define RANGE_VECTOR (sizeof(vector_ranges) / sizeof(*vector_ranges))

int main(void)
{
    BENCH_VECTOR_INIT_INPUT(lengths_of_arrays, VEC_LEN, vector_ranges, RANGE_VECTOR, RAND_MAX)

    INIT_INPUT_FLOAT16(input_mtx1, M * K, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_FLOAT16(input_mtx2, K * N, ranges0, RANGE_CNT0, RAND_MAX)

    performance_high();

    printf("Benchmark for qhblas_hvx_matrix_matrix_mpy_ahf function:\n");

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
            clear_cache(length);
            measurement_start();
            qhblas_hvx_matrix_matrix_mpy_ahf(input_mtx1, input_mtx2, output, M, K, length);
            measurement_finish();
            cpc += measurement_get_cycles();
            uspc += measurement_get_us();
        }
        cpc /= (double)NR_OF_ITERATIONS;
        cpm = cpc / (double)(M * length);
        uspc /= (double)NR_OF_ITERATIONS;
        uspm = uspc / (double)(M * length);
        length_sum += M * length;
        cpc_sum += cpc;
        uspc_sum += uspc;
        printf("  %11.4f CPC %11.4f MPS |  %11.4f CPM  %11.4f MPSpm | matix1 size = %lux%lu | matix2 size = %lux%lu\n",
               cpc, 1.0 / uspc, cpm, 1.0 / uspm, M, K, K, length);
    }
    cpm_sum = cpc_sum / (double)length_sum;
    uspm_sum = uspc_sum / (double)length_sum;
    printf("\nSummary:\n");
    printf("  %11.4f CPC %11.4f MPS | %11.4f CPM %11.4f MPSPM\n",
           cpc_sum, 1.0 / uspc_sum, cpm_sum, 1.0 / uspm_sum);

    performance_normal();

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
