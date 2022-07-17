/**=============================================================================
@file
    bench_copysign_af.c

@brief
    Benchmark for measuring performance of qhmath_copysign_af function.

    Benchmark measures number of cycles needed for executing qhmath_copysign_af function
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
} range0;

range0 ranges0[] = {
    {-10000.0f, -1.0f},
    {-1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 10000.0f}
};

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(*ranges0))

static float copy_sign_from = -1.0;
static float_a8_t a[ARR_LEN];
static float_a8_t b[ARR_LEN];
static float r;

#define TFUN(FNAME)                                                               \
    void test_##FNAME(long n)                                                     \
    {                                                                             \
        measurement_start();                                                      \
        for (int i = 0; i < n; ++i)                                               \
        {                                                                         \
            r = FNAME(a, b, ARR_LEN, copy_sign_from);                               \
        }                                                                         \
        measurement_finish();                                                     \
        measurement_print_af(#FNAME, n, ARR_LEN);                                 \
    }

TFUN(qhmath_copysign_af);

int main(int argc, char const *argv[])
{
    performance_high();

    printf("Benchmark for qhmath_copysign_af\n\n");

    // initialize test array
    for (uint32_t i = 0; i < ARR_LEN; ++i)
    {
        uint32_t range_index0 = i % RANGE_CNT0;
        float lo0 = ranges0[range_index0].lo;
        float hi0 = ranges0[range_index0].hi;

        // Generate random float number in range
        a[i] = lo0 + (hi0 - lo0) * ((float)rand() / (float)RAND_MAX);
    }

    TCALL(qhmath_copysign_af);

    performance_normal();

    return 0;
}
