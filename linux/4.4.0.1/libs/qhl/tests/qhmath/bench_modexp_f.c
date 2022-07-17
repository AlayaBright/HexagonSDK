/**=============================================================================
@file
    bench_modexp_f.c

@brief
    Benchmark for measuring performance of qhmath_modexp_f function.

    Benchmark measures number of cycles needed for executing qhmath_modexp_f function
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
  float __attribute__((noinline)) qhmath_modexp_f(float x, int32_t new_exponent)
  {
    typedef union {
      float value;
      struct
      {
        uint32_t mantissa : 23;
        uint32_t exponent : 8;
        uint32_t sign : 1;
      };
    } fp_iee754_t;

    ((fp_iee754_t *)&x)->exponent = new_exponent + 0x7F;
    return ((fp_iee754_t *)&x)->value;
  };
#endif

range_float_t ranges_a0[] = {
  {-1000.0f, -1.0f},
  {-1.0f, 0.0f},
  {0.0f, 1.0f},
  {1.0f, 1000.0f}
};
range_int32_t ranges_a1[] = {
  {-127, -1},
  {-1, 0},
  {0, 1},
  {1, 127}
};

#define RANGE_CNT (sizeof(ranges_a0) / sizeof(*ranges_a0))
#define FNAME

static float a0[ARR_LEN];
static int32_t a1[ARR_LEN];
static float r[ARR_LEN];
const int32_t new_exponent = 5;

BENCH_TFUN_MODEXP(qhmath_modexp_f)

int main(int argc, char const *argv[])
{
    performance_high();

    // initialize test arrays
    BENCH_MOD_EXP_INIT(ARR_LEN, RANGE_CNT, RAND_MAX)

    TCALL(qhmath_modexp_f);

    performance_normal();

    return 0;
}
