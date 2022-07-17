/**=============================================================================
@file
    bench_sqrt_af.c

@brief
    Benchmark for measuring performance of qhmath_sqrt_af function.

    Benchmark measures number of cycles needed for executing qhmath_sqrt_af function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhmath_test.h"

typedef struct
{
    float lo;  // Lowest value
    float hi;  // Highest value
} range0;


range0 ranges0[] = {
    {0.0f, 1.0f},
    {1.0f, 10000.0f},
    {1e+25f, 1e+30f}
};


#define RANGE_CNT0 (sizeof(ranges0) / sizeof(range0))

static float a0[ARR_LEN];
static float a1[ARR_LEN];
static int32_t r;

BENCH_TFUN_AF3(qhmath_sqrt_af, ARR_LEN);

int main(int argc, char const *argv[])
{
    performance_high();

    printf("Benchmark for qhmath_sqrt_af\n\n");

    // initialize test array
    BENCH_AF3_INPUT_INIT(ARR_LEN, RANGE_CNT0, RAND_MAX);

    TCALL(qhmath_sqrt_af);

    performance_normal();

    return 0;
}
