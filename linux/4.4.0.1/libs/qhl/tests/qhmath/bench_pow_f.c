/**=============================================================================
@file
    bench_pow_f.c

@brief
    Benchmark of qhmath_pow_f function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include "qhmath_test.h"

#ifdef BENCH_QHL
    #include "qhmath.h"
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
    extern float powf(float, float);
#endif

range_float_t ranges_a0[] = {
    { -1000.2f, -88.2f    },
    { -88.2f,   -1.0f     },
    { -1.0f,    -0x1p-126 },
    {  0.0f,     0x1p-126 },
    {  0x1p-126, 1.0f     },
    {  1.0f,     88.2f    },
    {  88.2f,    1000.2f  }
};

range_float_t ranges_a1[] = {
    { -1000.2f, -88.2f    },
    { -88.2f,   -1.0f     },
    { -1.0f,    -0x1p-126 },
    {  0.0f,     0x1p-126 },
    {  0x1p-126, 1.0f     },
    {  1.0f,     88.2f    },
    {  88.2f,    1000.2f  }
};

#define RANGE_CNT (sizeof(ranges_a0) / sizeof(*ranges_a0))

static float a0[ARR_LEN];
static float a1[ARR_LEN];
static float r[ARR_LEN];

#ifdef BENCH_QHL
    BENCH_TFUN_F2(qhmath_pow_f, ARR_LEN);
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
    BENCH_TFUN_F2(powf, ARR_LEN);
#endif

int main(int argc, char const *argv[])
{
    performance_high();

    // initialize test arrays
    BENCH_TFUN_F2_INIT(ARR_LEN, RANGE_CNT, RAND_MAX)

    #ifdef BENCH_QHL
        TCALL(qhmath_pow_f);
    #elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
        TCALL(powf);
    #endif

    performance_normal();

    return 0;
}
