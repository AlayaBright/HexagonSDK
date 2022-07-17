/**=============================================================================
@file
    bench_copysign_f.c

@brief
    Benchmark for measuring performance of qhmath_copysign_f function.

    Benchmark measures number of cycles needed for executing qhmath_copysign_f function
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
  float copysignf(float, float);
#endif

range_float_t ranges_a0[] = {
  {-10000.0f, -1.0f},
  {-1.0f, 0.0f},
  {0.0f, 1.0f},
  {1.0f, 10000.0f}
};

range_float_t ranges_a1[] = {
  {-1000.0f, -1.0f},
  {0.0f, 1.0f},
  {-1000.0f, -1.0f},
  {1.0f, 1000.0f}
};

#define RANGE_CNT (sizeof(ranges_a0) / sizeof(*ranges_a0))

static float a0[ARR_LEN];
static float a1[ARR_LEN];
static float r[ARR_LEN];

#ifdef BENCH_QHL
  BENCH_TFUN_F2(qhmath_copysign_f, ARR_LEN);
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
  BENCH_TFUN_F2(copysignf, ARR_LEN);
#endif

int main(int argc, char const *argv[])
{
    performance_high();

    // initialize test array
    BENCH_TFUN_F2_INIT(ARR_LEN, RANGE_CNT, RAND_MAX)

#ifdef BENCH_QHL
  TCALL(qhmath_copysign_f);
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
  TCALL(copysignf);
#endif

    performance_normal();

    return 0;
}
