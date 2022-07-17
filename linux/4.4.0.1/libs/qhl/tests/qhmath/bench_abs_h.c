 /**=============================================================================
@file
   bench_abs_h.c

@brief   Benchmark for measuring performance of qhmath_abs_h function.

        Benchmark measures number of cycles needed for executing qhmath_abs_h function
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
    int16_t lo;  // Lowest value
    int16_t hi;  // Highest value
} range;


range ranges[] = {
    {-10000, -1000},
    {-1000, -500},
    {-500, -100},
    {-100, -50},
    {-50, -10},
    {-10, -1},
    {-1, 0},
    {0, 1},
    {1, 10},
    {10, 50},
    {50, 100},
    {100, 500},
    {500, 1000},
    {1000, 10000}
};

#define RANGE_CNT (sizeof(ranges) / sizeof(*ranges))

static int16_t a[ARR_LEN];
static uint16_t r[ARR_LEN];

#define TFUN(FNAME)                                                               \
    void test_##FNAME(long n)                                                     \
    {                                                                             \
        measurement_start();                                                      \
        for (int i = 0; i < n; ++i)                                               \
        {                                                                         \
            for (int j = 0; j < ARR_LEN; ++j)                                     \
            {                                                                     \
                r[j] = FNAME(a[j]);                                                               \
            }                                                                     \
        }                                                                         \
        measurement_finish();                                                     \
        measurement_print_f(#FNAME, n, ARR_LEN);                                  \
    }

TFUN(qhmath_abs_h);

int main(int argc, char const *argv[])
{
    performance_high();

    printf("Benchmark for qhmath_abs_h\n\n");

    // initialize test array
    for (uint32_t i = 0; i < ARR_LEN; ++i)
    {
        uint32_t range_index = i % RANGE_CNT;
        int16_t lo = ranges[range_index].lo;
        int16_t hi = ranges[range_index].hi;

        // Generate random integer number in range
        a[i] = (int16_t) (lo + (rand() % (hi-lo+1)));
    }

TCALL(qhmath_abs_h);

    performance_normal();

    return 0;
}
