/**=============================================================================
@file
    bench_i_vector_norm_ah.c

@brief
    Benchmark for measuring performance of qhblas_hvx_i_vector_norm_ah function.

    Benchmark measures number of cycles needed for executing
    qhblas_hvx_i_vector_norm_aw function certain number of times
    on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include "qhblas_hvx.h"
#include "qhblas_test.h"

static range_int16_t ranges0[] =
{
    {  INT16_MIN, -1000      },
    { -1000,       1000      },
    {  1000,       INT16_MAX }
};

static range_uint32_t vector_ranges[] =
{
    { 0,     10     },
    { 10,    100    },
    { 100,   1000   },
    { 1000,  10000  },
    { 10000, 100000 }
};

static uint32_t lengths_of_array[VEC_LEN];
static int16_t input_arr[VEC_MAX_ARR_LEN];
static int64_t output;

static void clear_cache(uint32_t length)
{
#if __hexagon__
    hexagon_buffer_cleaninv((uint8_t *)input_arr, length*sizeof(int16_t));
#endif
}

#define RANGE_CNT0   (sizeof(ranges0)/sizeof(*ranges0))
#define RANGE_VECTOR (sizeof(vector_ranges)/sizeof(*vector_ranges))

BENCH_TFUN(qhblas_hvx_i_vector_norm_ah, input_arr, &output)

int main(void)
{
    BENCH_VECTOR_INIT_INPUT(lengths_of_array, VEC_LEN, vector_ranges, RANGE_VECTOR, RAND_MAX)

    INIT_INPUT_INT16(input_arr, VEC_MAX_ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    BENCH_TCALL(qhblas_hvx_i_vector_norm_ah, NR_OF_ITERATIONS, lengths_of_array, VEC_LEN)

    return 0;
}
