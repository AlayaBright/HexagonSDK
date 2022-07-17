/**=============================================================================
@file
    bench_carg_f.c

@brief
    Benchmark of qhcomplex_carg_f function.

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
    extern float cargf(float complex);
#elif BENCH_HSDK || BENCH_GLIBC
    #include <complex.h>
#endif

typedef struct
{
    float lo;  // Lowest value
    float hi;  // Highest value
} range;

range ranges_real[] = {
    {  1.71,  3.14 },
    { -1.71,  1.71 },
    { -1.71, -3.14 }
};

range ranges_imag[] = {
    { -10000.0, 0.0     },
    { -1.0,     1.0     },
    {  0.0 ,    10000.0 }
};

#define RANGE_CNT (sizeof(ranges_real) / sizeof(*ranges_real))

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
    TFUN(qhcomplex_carg_f);
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
    TFUN(cargf);
#endif

int main(int argc, char const *argv[])
{
    performance_high();

    // initialize array contents
    for (int i = 0; i < ARR_LEN; ++i)
    {
        uint32_t range_index = i % RANGE_CNT;
        float real_lo = ranges_real[range_index].lo;
        float real_hi = ranges_real[range_index].hi;
        float imag_lo = ranges_imag[range_index].lo;
        float imag_hi = ranges_imag[range_index].hi;

        // Generate random complex number in range
        float real = real_lo + (real_hi - real_lo) * ((float)rand() / (float)RAND_MAX);
        float imag = imag_lo + (imag_hi - imag_lo) * ((float)rand() / (float)RAND_MAX);
        a[i] = _F_COMPLEX_(real, imag);
    }


    #ifdef BENCH_QHL
        TCALL(qhcomplex_carg_f);
    #elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
        TCALL(cargf);
    #endif

    performance_normal();

    return 0;
}
