/**=============================================================================
@file
    bench_pow_af.c

@brief
    Benchmark for measuring performance of qhmath_pow_af function.

    Benchmark measures number of cycles needed for executing qhmath_pow_af function
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

range_float_t ranges0[] = {
    { -1000.2f, -88.2f    },
    { -88.2f,   -1.0f     },
    { -1.0f,    -0x1p-126 },
    {  0.0f,     0x1p-126 },
    {  0x1p-126, 1.0f     },
    {  1.0f,     88.2f    },
    {  88.2f,    1000.2f  }
};

range_float_t ranges1[] = {
    { -1000.2f, -88.2f    },
    { -88.2f,   -1.0f     },
    { -1.0f,    -0x1p-126 },
    {  0.0f,     0x1p-126 },
    {  0x1p-126, 1.0f     },
    {  1.0f,     88.2f    },
    {  88.2f,    1000.2f  }
};

#define RANGE_CNT0 (sizeof(ranges0) / sizeof(range_float_t))
#define RANGE_CNT1 (sizeof(ranges1) / sizeof(range_float_t))

static float a0[ARR_LEN];
static float a1[ARR_LEN];
static float a2[ARR_LEN];
static int32_t r;

BENCH_TFUN_POW_AF4(qhmath_pow_af, ARR_LEN);

int main(int argc, char const *argv[])
{
    performance_high();

    printf("Benchmark for qhmath_pow_af\n\n");

    // initialize test array
    BENCH_AF4_INPUT_INIT(ARR_LEN, RANGE_CNT0, RANGE_CNT1, RAND_MAX);

    TCALL(qhmath_pow_af);

    performance_normal();

    return 0;
}
