/**=============================================================================
@file
    bench_exp2_f.c

@brief
    Benchmark for measuring performance of qhmath_exp2_f function.

    Benchmark measures number of cycles needed for executing qhmath_exp2_f function
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
  float exp2f(float);
#endif

range_float_t ranges[] = {
  {-200.0f,-128.0f},
  {-128.0, 0.0f},
  {0.0f, 128.0f},
  {128.0f,200.0f},
};

#define RANGE_CNT (sizeof(ranges) / sizeof(*ranges))

static float a[ARR_LEN];
static float r[ARR_LEN];

#ifdef BENCH_QHL
  BENCH_TFUN_F1(qhmath_exp2_f, ARR_LEN);
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
  BENCH_TFUN_F1(exp2f, ARR_LEN);
#endif

int main(int argc, char const *argv[])
{
    performance_high();

    // initialize test array
    BENCH_TFUN_F1_INIT(ARR_LEN, RANGE_CNT, RAND_MAX);

#ifdef BENCH_QHL
  TCALL(qhmath_exp2_f);
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
  TCALL(exp2f);
#endif

    performance_normal();

    return 0;
}
