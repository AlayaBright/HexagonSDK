/**=============================================================================
@file
    bench_bkfir_aw.c

@brief
    Benchmark for measuring performance of qhdsp_hvx_bkfir_aw function.

    This function measures the number of cycles needed for executing
    qhdsp_hvx_bkfir_aw a number of times on randomly generated values and
    reports the average cycle count.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhdsp_hvx.h"
#include "qhblas_test.h"

#undef ARR_LEN
#define ARR_LEN 4096

static range_int32_t ranges0[] =
{
    { -800000, -10000  },
    { -10000,  -1000   },
    { -1000,    1000   },
    {  1000,    10000  },
    {  10000,   800000 }
};

#define MAX_TAPS_SIZE (4096)

static int32_t __attribute__((aligned(128))) input[ARR_LEN + MAX_TAPS_SIZE];
static int32_t __attribute__((aligned(128))) coeffs[ARR_LEN];
static int32_t __attribute__((aligned(128))) output[ARR_LEN];

#define RANGE_CNT0   (sizeof(ranges0)/sizeof(*ranges0))

int main(void)
{
    double cpc, cpm, uspc, uspm;

    printf("Benchmark for qhdsp_hvx_bkfir_aw function:\n");

    INIT_INPUT_INT32(input, ARR_LEN + MAX_TAPS_SIZE, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_INT32(coeffs, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    // Go through variable size of inputs.
    for (uint32_t i = 256; i <= ARR_LEN; i += i)
    {
        // Go through variable size of taps.
        for (uint32_t j = 64; j <= i; j += 64)
        {
            cpc = 0;
            uspc = 0;

            for (uint32_t k = 0; k < NR_OF_ITERATIONS; ++k)
            {
                hexagon_buffer_cleaninv((uint8_t *)input, (i + j)*sizeof(int32_t));
                measurement_start();

                qhdsp_hvx_bkfir_aw(input, coeffs, j, i, output);

                measurement_finish();
                cpc += measurement_get_cycles();
                uspc += measurement_get_us();
            }

            cpc /= (double) NR_OF_ITERATIONS;
            uspc /= (double) NR_OF_ITERATIONS;
            cpm = cpc / (double) i;
            uspm = uspc / (double) i;
            printf("  %11.4f CPC %11.4f MPS | %11.4f CPM %11.4f MPSPM | "
                   "input length = %ld | taps = %ld\n",
                   cpc, 1.0 / uspc, cpm, 1.0 / uspm, i, j);
        }

        printf("\n");
    }

    return 0;
}
