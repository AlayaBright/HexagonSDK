/**=============================================================================
@file
    bench_cosh_f.c

@brief
    Benchmark of qhcomplex_ccos_f function.

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
    extern float complex ccosf(float complex);
#elif BENCH_HSDK || BENCH_GLIBC
    #include <complex.h>
#endif

typedef struct
{
    float lo; // Lowest value
    float hi; // Highest value
} range;

range ranges[] = {
    { 0.0f,    0.0002f },
    { 0.0002f, 0.2f    },
    { 0.2f,    0.69f   },
    { 0.69f,   10.0f   },
    { 10.0f,   88.72f  },
    { 88.72f,  200.0f  }
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
    TFUN(qhcomplex_ccos_f);
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
    TFUN(ccosf);
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
        TCALL(qhcomplex_ccos_f);
    #elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
        TCALL(ccosf);
    #endif

    performance_normal();

    return 0;
}
