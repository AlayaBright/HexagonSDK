/**=============================================================================
@file
    bench_matrix_inverse2x2_af.c

@brief
    Benchmark for measuring performance of matrix_inverse2x2_af function.

    Benchmark measures number of cycles needed for executing matrix_inverse2x2_af function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include "qhblas.h"
#include "qhblas_test.h"

#undef MATRIX_N
#define MATRIX_N 2

range_float_t ranges0[] = {
    {-1000.0, -100.0},
    {-100.0, -1.0},
    {-1.0, 1.0},
    {1.0, 100.0},
    {100, 1000.0}
};

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(range_float_t))

uint32_t lengths_of_arrays[VEC_LEN];
float_a8_t a1[MATRIX_N*MATRIX_N];
float_a8_t **matrix_vector;
int32_t r;

BENCH_TFUN_AF2_MATRIX(__qhblas_matrix_inverse2x2_af);

int main(int argc, char const *argv[])
{
    performance_high();

    printf("Benchmark for matrix inverse 2x2 float\n\n");
    BENCH_MATRIX_INPUT_INIT(VEC_LEN, MATRIX_N);
    // initialize test array
    BENCH_AF4_INPUT_INIT_MATRIX(RANGE_CNT0, RAND_MAX, lengths_of_arrays, VEC_LEN);

    BENCH_TCALL(__qhblas_matrix_inverse2x2_af, NR_OF_ITERATIONS, lengths_of_arrays, VEC_LEN);

    BENCH_MATRIX_INPUT_CLEANUP(VEC_LEN, MATRIX_N);

    performance_normal();

    return 0;
}
