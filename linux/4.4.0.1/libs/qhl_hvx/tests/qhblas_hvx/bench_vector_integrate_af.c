/**=============================================================================
@file
    bench_vector_integrate_af.c

@brief
    Benchmark for measuring performance of qhblas_hvx_vector_integrate_af function.

    Benchmark measures number of cycles needed for executing qhblas_hvx_vector_integrate_af function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>
#include "qhblas_hvx.h"
#include "qhblas_test.h"

static range_float_t ranges0[] = {
    {-1000.0,    -100.0      },
    { -100.0,     100.0      },
    {  100.0,     1000.0     }
};

static range_uint32_t vector_ranges[] = {
    { 0,   10      },
    { 10,   100    },
    { 100,   1000  },
    { 1000,  10000 },
    { 10000, 100000}
};

static uint32_t lengths_of_arrays[VEC_LEN];
static float input_arr[VEC_MAX_ARR_LEN];
static float output[VEC_MAX_ARR_LEN];

static void clear_cache(uint32_t length)
{
#if __hexagon__
    hexagon_buffer_cleaninv((uint8_t *)input_arr, length*sizeof(float));
#endif
}

#define RANGE_CNT0   (sizeof(ranges0)/sizeof(range_float_t))
#define RANGE_VECTOR (sizeof(vector_ranges)/sizeof(range_uint32_t))

BENCH_TFUN(qhblas_hvx_vector_integrate_af, input_arr, output)

int main(void)
{
    BENCH_VECTOR_INIT_INPUT(lengths_of_arrays, VEC_LEN, vector_ranges, RANGE_VECTOR, RAND_MAX)

    INIT_INPUT_FLOAT(input_arr, VEC_MAX_ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    BENCH_TCALL(qhblas_hvx_vector_integrate_af, NR_OF_ITERATIONS, lengths_of_arrays, VEC_LEN)

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