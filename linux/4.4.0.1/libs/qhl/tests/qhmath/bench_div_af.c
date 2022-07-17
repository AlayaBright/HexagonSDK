/**=============================================================================
@file
    bench_div_af.c

@brief
    Benchmark for measuring performance of qhmath_div_af function.

    Benchmark measures number of cycles needed for executing qhmath_div_af function
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

typedef struct
{
    float lo;  // Lowest value
    float hi;  // Highest value
} range1;

range0 ranges0[] = {
    {-1e+25f, 1e+25f},
    {-1e+25f, 1e+25f}
};

range1 ranges1[] = {
    {-1e+25f, 1e+25f},
    {-1e+25f, 1e+25f}
};

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(range0))
#define RANGE_CNT1 (sizeof(ranges1) / sizeof(range1))

static float a0[ARR_LEN];
static float a1[ARR_LEN];
static float a2[ARR_LEN];
static int32_t r;

BENCH_TFUN_AF4(qhmath_div_af, ARR_LEN);

int main(int argc, char const *argv[])
{
    performance_high();

    printf("Benchmark for qhmath_div_af\n\n");

    // initialize test array
    BENCH_AF4_INPUT_INIT(ARR_LEN, RANGE_CNT0, RANGE_CNT1, RAND_MAX);

    TCALL(qhmath_div_af);

    performance_normal();

    return 0;
}
