/**=============================================================================
@file
    bench_abs_af.c

@brief
    Benchmark for measuring performance of qhmath_abs_af function.

    Benchmark measures number of cycles needed for executing qhmath_abs_af function
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
    float lo;  // Lowest value
    float hi;  // Highest value
} range;


range ranges[] = {
    {-1.0f, 1.0f},
    {-10000.0f, 10000.0f},
    {-1e+30f, 1e+30f}
};

#define RANGE_CNT (sizeof(ranges) / sizeof(range))

static float a[ARR_LEN];
static float_a8_t b[ARR_LEN];
static int32_t r;

#define TFUN(FNAME)                                                               \
    void test_##FNAME(long n)                                                     \
    {                                                                             \
        measurement_start();                                                      \
        for (int i = 0; i < n; ++i)                                               \
        {                                                                         \
            r = FNAME(a, b, ARR_LEN);                                             \
        }                                                                         \
        measurement_finish();                                                     \
        measurement_print_af(#FNAME, n, ARR_LEN);                                 \
    }

TFUN(qhmath_abs_af);

int main(int argc, char const *argv[])
{
    performance_high();

    printf("Benchmark for qhmath_abs_af\n\n");

    // initialize test array
    for (int i = 0; i < ARR_LEN; ++i)
    {
        uint32_t range_index = i % RANGE_CNT;
        float lo = ranges[range_index].lo;
        float hi = ranges[range_index].hi;

        a[i] = lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);
    }

    TCALL(qhmath_abs_af);

    performance_normal();

    return 0;
}
