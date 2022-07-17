/**=============================================================================
@file
    bench_divf.c

@brief
    Benchmark for measuring performance of qhmath_div_f function.

    Benchmark measures number of cycles needed for executing qhmath_div_f function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath_test.h"

#ifdef BENCH_QHL
  #include "qhmath.h"
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
  float __attribute__ ((noinline)) qhmath_div_f(float n, float d) { return n/d; };
#endif

#define MAX_FLT __FLT_MAX__

range_float_t ranges_a0[] = {
    { -MAX_FLT,  -0.0f        },
    {  0.0f,      -MAX_FLT    },
};

range_float_t ranges_a1[] = {
    { -MAX_FLT,  -0.0f        },
    {  0.0f,      -MAX_FLT    },
};

#define RANGE_CNT (sizeof(ranges_a0) / sizeof(*ranges_a0))

static float a0[ARR_LEN];
static float a1[ARR_LEN];
static float r[ARR_LEN];

BENCH_TFUN_F2(qhmath_div_f, ARR_LEN);

int main(int argc, char const *argv[])
{
    performance_high();

    // initialize test arrays
    BENCH_TFUN_F2_INIT(ARR_LEN, RANGE_CNT, RAND_MAX)

    TCALL(qhmath_div_f);

    performance_normal();

    return 0;
}
