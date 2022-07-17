/**=============================================================================
@file
    bench_sigmoid_f.c

@brief
    Benchmark for measuring performance of qhmath_sigmoid_f function.

    Benchmark measures number of cycles needed for executing qhmath_sigmoid_f function
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
    #include <math.h>
    float __attribute__ ((noinline)) qhmath_sigmoid_f(float x) { return 1/(expf(-x)+1); };
#endif

range_float_t ranges[] = {
  {-100.0f, -88.0f},
  {-88.0f, -10.0f},
  {-10.0f, -1.0f},
  {-1.0f, 0.0f},
  {0.0f, 1.0f},
  {1.0f, 10.0f},
  {10.0f, 88.0f},
  {88.0f, 100.0f},
};

#define RANGE_CNT (sizeof(ranges) / sizeof(*ranges))

static float a[ARR_LEN];
static float r[ARR_LEN];

BENCH_TFUN_F1(qhmath_sigmoid_f, ARR_LEN);

int main(int argc, char const *argv[])
{
    performance_high();

    // initialize test arrays
    BENCH_TFUN_F1_INIT(ARR_LEN, RANGE_CNT, RAND_MAX)

    TCALL(qhmath_sigmoid_f);

    performance_normal();

    return 0;
}
