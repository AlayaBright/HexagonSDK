/**=============================================================================
@file
    bench_ceil_f.c

@brief
    Benchmark qhmath_ceil_f function.

    Benchmark measures number of cycles needed for executing qhmath_exp_f function
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
    extern float ceilf(float);
#endif

#define MAX_FLT __FLT_MAX__

range_float_t ranges[] = {
    { -MAX_FLT,    -4194304.0f },
    { -4194304.0f, -1024.0f    },
    { -1024.0f,    -1.0f       },
    { -1.0f,       -0.25f      },
    {  0.25f,       1.0f       },
    {  1.0f,        1024.0f    },
    {  1024.0f,     4194304.0f },
    {  4194304.0f,  MAX_FLT    }
};

#define RANGE_CNT (sizeof(ranges) / sizeof(*ranges))

static float a[ARR_LEN];
static float r[ARR_LEN];

#ifdef BENCH_QHL
    BENCH_TFUN_F1(qhmath_ceil_f, ARR_LEN);
#elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
    BENCH_TFUN_F1(ceilf, ARR_LEN);
#endif

int main(int argc, char const *argv[])
{
    performance_high();

    // initialize test array
    BENCH_TFUN_F1_INIT(ARR_LEN, RANGE_CNT, RAND_MAX);

    #ifdef BENCH_QHL
        TCALL(qhmath_ceil_f);
    #elif BENCH_OPENLIBM || BENCH_MUSL || BENCH_GLIBC || BENCH_HSDK
        TCALL(ceilf);
    #endif

    performance_normal();

    return 0;
}
