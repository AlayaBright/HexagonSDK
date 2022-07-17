/**=============================================================================
@file
    bench_clipping_ah.c

@brief
    Benchmark qhmath_clipping_ah function.

    Benchmark measures number of cycles needed for executing qhmath_clipping_ah function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhl_common.h"

typedef struct
{
    int16_t lowest_value;
    int16_t highest_value;
    int16_t lowest_clip_value;
    int16_t highest_clip_value;

} test_t;

test_t ranges[] = {
    { 0, 4096, 500, 3000 },
    { -25000, 25000, -4096, 4096},
};

#define RANGE_CNT (sizeof(ranges) / sizeof(*ranges))

static int16_t a[ARR_LEN];
static int16_t b[ARR_LEN];

int main(int argc, char const *argv[])
{
    performance_high();

    measurement_acc_clr();

    printf("\n----\nBenchmark for qhmath_clipping_ah\n\n");

    for (int range_idx = 0; range_idx < RANGE_CNT; range_idx++)
    {
        int16_t lo = ranges[range_idx].lowest_value;
        int16_t hi = ranges[range_idx].highest_value;

        // initialize test array
        for (int i = 0; i < ARR_LEN; ++i)
        {
            a[i] = lo + (rand()) / (RAND_MAX/(hi-lo));
        }

        // prefetch
        qhmath_clipping_ah(a, b, ARR_LEN, ranges[range_idx].lowest_clip_value, ranges[range_idx].highest_clip_value);

        measurement_start();

        for (int i = 0; i < NR_OF_ITERATIONS; ++i)
        {
            qhmath_clipping_ah(a, b, ARR_LEN, ranges[range_idx].lowest_clip_value, ranges[range_idx].highest_clip_value);
        }

        measurement_finish();

        measurement_acc(NR_OF_ITERATIONS);
    }

    measurement_acc_prn("qhmath_clipping_ah", RANGE_CNT, ARR_LEN);

    performance_normal();

    return 0;
}
