/**=============================================================================
@file
   bench_div_h.c

@brief
    Benchmark for measuring performance of qhmath_div_h function.

    Benchmark measures number of cycles needed for executing qhmath_div_h function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhmath_test.h"

static int16_t a0[ARR_LEN];
static int16_t a1[ARR_LEN];
static int16_t r[ARR_LEN];

range_int16_t ranges[] = {
    {256, 32512}
};

#define RANGE_CNT (sizeof(ranges) / sizeof(range_int16_t))

BENCH_TFUN_F2(qhmath_div_h, ARR_LEN);

int main(int argc, char const *argv[])
{
    performance_high();

    BENCH_H2_INPUT_INIT_16B(ARR_LEN, RANGE_CNT, RAND_MAX);

    TCALL(qhmath_div_h);

    performance_normal();

    return 0;
}
