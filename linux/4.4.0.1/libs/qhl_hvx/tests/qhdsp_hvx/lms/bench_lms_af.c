/**=============================================================================
@file
    bench_lms_af.c

@brief
    Benchmark for measuring performance of qhdsp_hvx_lms_af function.

    Benchmark measures number of cycles needed for executing
    qhdsp_hvx_lms_af function certain number of times
    on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2020 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#if __HEXAGON_ARCH__ >= 68

#include <stdio.h>
#include <stdlib.h>

#include "qhdsp_hvx.h"
#include "qhblas_test.h"

#undef ARR_LEN
#define ARR_LEN 4096

static range_float_t ranges0[] =
{
    { -1.0f,   -0.001  },
    { -0.001f, -0.0f   },
    {  0.0f,    0.001f },
    {  0.001f,  1.0f   }
};

static float input[ARR_LEN];
static float ydes[ARR_LEN];
static float rate = 0.001f;
static float __attribute__((aligned(128))) states[ARR_LEN];
static float __attribute__((aligned(128))) coeffs[ARR_LEN];
static float error[ARR_LEN];
static float output[ARR_LEN];
static float orig_coeffs[ARR_LEN];

#define RANGE_CNT0   (sizeof(ranges0)/sizeof(*ranges0))

int main(void)
{
    double cpc, cpm, uspc, uspm;

    printf("Benchmark for qhdsp_hvx_lms_af function:\n");

    INIT_INPUT_FLOAT(input, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)
    INIT_INPUT_FLOAT(orig_coeffs, ARR_LEN, ranges0, RANGE_CNT0, RAND_MAX)

    // Go through variable size of inputs.
    for (uint32_t i = 256; i <= ARR_LEN; i += i)
    {
        // Go through variable size of taps.
        for (uint32_t j = 64; j <= i; j += j)
        {
            cpc = 0;
            uspc = 0;
            memset(states, 0, sizeof(float)*ARR_LEN);
            memcpy(coeffs, orig_coeffs, sizeof(float)*ARR_LEN);

            for (uint32_t k = 0; k < NR_OF_ITERATIONS; ++k)
            {
                hexagon_buffer_cleaninv((uint8_t *)input, i*sizeof(float));
                measurement_start();
                qhdsp_hvx_lms_af(input, ydes, rate, j, states,
                                  coeffs, error, output, i);
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

#else /* __HEXAGON_ARCH__ >= 68 */
#include <stdio.h>
int main()
{
    printf("HVX float is not supported for this architecture.\n");
    return 0;
}
#endif /* __HEXAGON_ARCH__ >= 68 */
