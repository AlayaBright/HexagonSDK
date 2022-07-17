/**=============================================================================
@file
    bench_cproj_f.c

@brief
    Benchmark of qhcomplex_cproj_f function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhl_common.h"

#ifdef BENCH_QHL
    #include "qhcomplex.h"
#elif BENCH_OPENLIBM || BENCH_MUSL
    #define complex _Complex
    extern float complex cprojf(float complex);
#elif BENCH_HSDK || BENCH_GLIBC
    #include <complex.h>
#endif

#define MAX_FLT __FLT_MAX__
#define INFINITY  __builtin_inff()

typedef struct
{
    float lo; // Lowest value
    float hi; // Highest value
} range;

range ranges[] = {
    { -INFINITY,  0.0f    },
    {  INFINITY,  0.0f    },
    { -MAX_FLT,  -0.0f    },
    {  0.0f,      MAX_FLT }
};

#define RANGE_CNT (sizeof(ranges) / sizeof(*ranges))

static float complex a[ARR_LEN];
static float complex r[ARR_LEN];

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
    TFUN(qhcomplex_cproj_f);
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
    TFUN(cprojf);
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
        TCALL(qhcomplex_cproj_f);
    #elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
        TCALL(cprojf);
    #endif

    performance_normal();

    return 0;
}
