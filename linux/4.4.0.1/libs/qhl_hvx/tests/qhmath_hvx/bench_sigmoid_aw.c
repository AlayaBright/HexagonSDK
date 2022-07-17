/**=============================================================================
@file
    bench_sigmoid_aw.c

@brief
    Benchmark for measuring performance of qhmath_hvx_sigmoid_aw function.

    Benchmark measures number of cycles needed for executing
    qhmath_hvx_sigmoid_aw function certain number of times
    on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include "qhmath_hvx.h"
#include "qhblas_test.h"

static range_int32_t ranges0[] =
{
    { -999200000, -499600000 },
    { -499600000, -249800000 },
    { -249800000, 0          },
    { 0,          249800000  },
    { 249800000,  499600000  },
    { 499600000,  999200000  }
};
static range_uint32_t vector_ranges[] =
{
    { 0,     10     },
    { 10,    100    },
    { 100,   1000   },
    { 1000,  10000  },
    { 10000, 100000 }
};

static uint32_t lengths_of_arrays[VEC_LEN];
static int32_t input_arr1[VEC_MAX_ARR_LEN];
static int32_t output[VEC_MAX_ARR_LEN];

static void clear_cache(uint32_t length)
{
#if __hexagon__
    hexagon_buffer_cleaninv((uint8_t *)input_arr1, length*sizeof(int32_t));
#endif
}

#define RANGE_CNT0   (sizeof(ranges0)/sizeof(*ranges0))
#define RANGE_VECTOR (sizeof(vector_ranges)/sizeof(*vector_ranges))

BENCH_TFUN(qhmath_hvx_sigmoid_aw, input_arr1, output)

int main(void)
{
    BENCH_VECTOR_INIT_INPUT(lengths_of_arrays, VEC_LEN, vector_ranges, RANGE_VECTOR, RAND_MAX)

    INIT_INPUT_INT32(input_arr1, VEC_MAX_ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    BENCH_TCALL(qhmath_hvx_sigmoid_aw, NR_OF_ITERATIONS, lengths_of_arrays, VEC_LEN)

    return 0;
}
