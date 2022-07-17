/**=============================================================================
@file
    bench_modexp_af.c

@brief
    Benchmark for measuring performance of qhmath_modexp_af function.

    Benchmark measures number of cycles needed for executing qhmath_modexp_af function
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

typedef struct
{
    float lo;  // Lowest value
    float hi;  // Highest value
} range1;

range0 ranges0[] = {
    {-10000.0f, -1.0f},
    {-1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 10000.0f}
};

range_int32_t ranges1[] = {
  {-127, -1},
  {-1, 0},
  {0, 1},
  {1, 127}
};

const int32_t new_exponent = 5;

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(*ranges0))
#define RANGE_CNT1 (sizeof(ranges1) / sizeof(range_int32_t))

static float_a8_t a0[ARR_LEN];
static float_a8_t a1[ARR_LEN];
static float_a8_t b[ARR_LEN];
static float r;

int main(int argc, char const *argv[])
{
    performance_high();

    printf("Benchmark for qhmath_modexp_af\n\n");

    // initialize test array
    for (uint32_t i = 0; i < ARR_LEN; ++i)
    {
        uint32_t range_index0 = i % RANGE_CNT0;
        float lo0 = ranges0[range_index0].lo;
        float hi0 = ranges0[range_index0].hi;

        // Generate random float number in range
        a0[i] = lo0 + (hi0 - lo0) * ((float)rand() / (float)RAND_MAX);

        uint32_t range_index1 = i % RANGE_CNT1;
        int32_t lo1 = ranges1[range_index1].lo;
        int32_t hi1 = ranges1[range_index1].hi;

        // Generate random float number in range
        a1[i] = lo1 +  (int32_t) ((float) hi1 - lo1) * ((float)rand() / (float)RAND_MAX);
    }

    measurement_start();

    for (int i = 0; i < NR_OF_ITERATIONS; ++i)
    {
        r = qhmath_modexp_af(a0, b, ARR_LEN, new_exponent);
    }

    measurement_finish();

    measurement_print_af("qhmath_modexp_af", NR_OF_ITERATIONS, ARR_LEN);

    performance_normal();

    return 0;
}
