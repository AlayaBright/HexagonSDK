/**=============================================================================
@file
    bench_matrix_inverse3x3_ah.c

@brief
    Benchmark for measuring performance of matrix_inverse3x3_ah function.

    Benchmark measures number of cycles needed for executing matrix_inverse3x3_ah function
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
#define MATRIX_N 3

#undef VEC_LEN
#define VEC_LEN 10

range_uint16_t ranges0[] = {
    {INT16_MIN, -1000},
    {-1000, -100},
    {-100, 100},
    {100, 1000},
    {1000, INT16_MAX}
};

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(range_uint16_t))

uint32_t lengths_of_arrays[VEC_LEN];
int32_t a1[MATRIX_N*MATRIX_N];
int16_t **matrix_vector;
int32_t r;

BENCH_TFUN_AF2_MATRIX(__qhblas_matrix_inverse3x3_ah);

int main(int argc, char const *argv[])
{
    performance_high();

    printf("Benchmark for matrix inverse 3x3 16bit\n\n");
    BENCH_MATRIX_INPUT_INIT_H(VEC_LEN, MATRIX_N);
    // initialize test array
    BENCH_AH4_INPUT_INIT_MATRIX(RANGE_CNT0, RAND_MAX, lengths_of_arrays, VEC_LEN);

    BENCH_TCALL(__qhblas_matrix_inverse3x3_ah, NR_OF_ITERATIONS, lengths_of_arrays, VEC_LEN);

    BENCH_MATRIX_INPUT_CLEANUP(VEC_LEN, MATRIX_N);

    performance_normal();

    return 0;
}
