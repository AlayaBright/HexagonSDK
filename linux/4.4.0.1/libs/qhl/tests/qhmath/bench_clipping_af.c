/**=============================================================================
@file
    bench_clipping_af.c

@brief
    Benchmark qhmath_clipping_af function.

    Benchmark measures number of cycles needed for executing qhmath_clipping_af function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhl_common.h"

#ifdef BENCH_QHL
    #include "qhmath.h"
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
    #include <math.h>

    typedef float __attribute__((__aligned__(8))) float_a8_t;

    __attribute__((noinline))
    int32_t qhmath_clipping_af(float_a8_t *input,
                            float_a8_t *output,
                            uint32_t size,
                            float low_level,
                            float high_level)
    {
        if (low_level >= high_level)
        {
            return -1;
        }

        if (size <= 0)
        {
            return -1;
        }

        for (uint32_t i = 0; i < size; i++)
        {
            float actual_val = input[i];

            if (low_level > actual_val)
                actual_val = low_level;
            else if (actual_val > high_level)
                actual_val = high_level;

            output[i] = actual_val;
        }

        return 0;
    }

#endif



typedef struct
{
    float lowest_value;
    float highest_value;
    float lowest_clip_value;
    float highest_clip_value;

} test_t;

test_t ranges[] = {
    { -10000.0, -1000.0, -3000.0, -2000.0 },
    { 0, 3.3, 1.8, 2.7},
};

#define RANGE_CNT (sizeof(ranges) / sizeof(*ranges))

static float a[ARR_LEN] __attribute__((__aligned__(8)));
static float b[ARR_LEN] __attribute__((__aligned__(8)));

int main(int argc, char const *argv[])
{
    performance_high();

    measurement_acc_clr();

    printf("Benchmark for qhmath_clipping_af(x)\n\n");

    for (int range_idx = 0; range_idx < RANGE_CNT; range_idx++)
    {
        float lo = ranges[range_idx].lowest_value;
        float hi = ranges[range_idx].highest_value;

        // initialize test array
        for (int i = 0; i < ARR_LEN; ++i)
        {
            a[i] = lo + (float)(rand()) / ((float)RAND_MAX/(hi-lo));
        }

        // prefetch
        qhmath_clipping_af(a, b, ARR_LEN, ranges[range_idx].lowest_clip_value, ranges[range_idx].highest_clip_value);

        measurement_start();

        for (int i = 0; i < NR_OF_ITERATIONS; ++i)
        {
            qhmath_clipping_af(a, b, ARR_LEN, ranges[range_idx].lowest_clip_value, ranges[range_idx].highest_clip_value);
        }

        measurement_finish();

        measurement_acc(NR_OF_ITERATIONS);
    }

    measurement_acc_prn("qhmath_clipping_af", RANGE_CNT, ARR_LEN);

    performance_normal();

    return 0;
}
