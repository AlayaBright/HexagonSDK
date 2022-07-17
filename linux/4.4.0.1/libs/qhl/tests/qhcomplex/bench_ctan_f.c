/**=============================================================================
@file
    bench_ctan_f.c

@brief
    Benchmark of qhcomplex_ctan_f function.

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
    extern float complex ctanf(float complex);
#elif BENCH_HSDK || BENCH_GLIBC
    #include <complex.h>
#endif

typedef struct
{
    float lo; // Lowest range
    float hi; // Highest range
} range;

range real_ranges[] = {
    { -0.001f, 0.001f },
    { 0.001f, 10.0f },
    { 10.0f,  100.0f },
    { 100.0f, 1000.0f },
    { 10.0f,  100.0f },
    { 100.0f, 1000.0f }
};

range imag_ranges[] = {
    { -10.0f, 10.0f },
    { -10.0f, 10.0f },
    { -10.0f, 10.0f },
    { -10.0f, 10.0f },
    { 11.0f, 88.0f },
    { 88.0f, 192.0f }
};

#define RANGE_CNT (sizeof(real_ranges) / sizeof(*real_ranges))

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
    TFUN(qhcomplex_ctan_f);
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
    TFUN(ctanf);
#endif

int main(int argc, char const *argv[])
{
    performance_high();

    // initialize array contents
    for (int i = 0; i < ARR_LEN; ++i)
    {
        size_t range_index = i % RANGE_CNT;
        float real_lo = real_ranges[range_index].lo;
        float real_hi = real_ranges[range_index].hi;
        float imag_lo = imag_ranges[range_index].lo;
        float imag_hi = imag_ranges[range_index].hi;

        float real = real_lo + (real_hi - real_lo) * ((float)rand() / (float)RAND_MAX);
        float imag = imag_lo + (imag_hi - imag_lo) * ((float)rand() / (float)RAND_MAX);
        a[i] = _F_COMPLEX_(real, imag);
    }

    #ifdef BENCH_QHL
        TCALL(qhcomplex_ctan_f);
    #elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
        TCALL(ctanf);
    #endif

    performance_normal();

    return 0;
}