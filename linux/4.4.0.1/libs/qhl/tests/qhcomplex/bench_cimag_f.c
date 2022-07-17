/**=============================================================================
@file
    bench_cimagf.c

@brief
    Benchmark of qhcomplex_cimag_f function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhl_common.h"

#define MAX_FLT __FLT_MAX__

#ifdef BENCH_QHL
    #include "qhcomplex.h"
#elif BENCH_OPENLIBM || BENCH_MUSL
    #define complex _Complex
    extern float cimagf(float complex);
#elif BENCH_HSDK || BENCH_GLIBC
    #include <complex.h>
#endif

typedef struct
{
    float lo; // Lowest value
    float hi; // Highest value
} range;

range ranges[] = {
    { -MAX_FLT, -0.0     },
    {  0.0,      MAX_FLT }
};

#define RANGE_CNT (sizeof(ranges) / sizeof(*ranges))

static float complex a[ARR_LEN];
static float r[ARR_LEN];

#define TFUN(FNAME)                                                               \
    void test_##FNAME(long n)                                                     \
    {                                                                             \
        for (int j = 0; j < ARR_LEN; ++j)                                         \
            r[j] = FNAME(a[j]);                                                   \
        measurement_start();                                                      \
        for (int i = 0; i < n; ++i)                                               \
        {                                                                         \
            for (int j = 0; j < ARR_LEN; ++j)                                     \
                r[j] = FNAME(a[j]);                                               \
        }                                                                         \
        measurement_finish();                                                     \
        measurement_print_f(#FNAME, n, ARR_LEN);                                  \
    }

#ifdef BENCH_QHL
    TFUN(qhcomplex_cimag_f);
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
    TFUN(cimagf);
#endif

int main(int argc, char const *argv[])
{
    performance_high();

    // initialize array contents
    for (int i = 0; i < ARR_LEN; ++i)
    {
        size_t range_index = i % RANGE_CNT;
        float lo = ranges[range_index].lo;
        float hi = ranges[range_index].hi;

        // Generate random complex number in range
        float real = lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);
        float imag = lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);
        a[i] = _F_COMPLEX_(real, imag);
    }

    #ifdef BENCH_QHL
        TCALL(qhcomplex_cimag_f);
    #elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
        TCALL(cimagf);
    #endif

    performance_normal();

    return 0;
}
