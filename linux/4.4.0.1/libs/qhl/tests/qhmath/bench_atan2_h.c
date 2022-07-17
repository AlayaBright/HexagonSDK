/**=============================================================================
@file
    bench_atan2_h.c

@brief
    Benchmark for measuring performance of qhmath_atan2_h function.

    Benchmark measures number of cycles needed for executing qhmath_atan2_h function
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

int16_t a0[ARR_LEN];
int16_t a1[ARR_LEN];

int16_t r[ARR_LEN];

range_int16_t ranges[] = {
    {-32768, 0},
    {0, 32767}
};

#define RANGE_CNT (sizeof(ranges) / sizeof(range_int16_t))

BENCH_TFUN_H2(qhmath_atan2_h, ARR_LEN);

int main(void)
{

    BENCH_H2_INPUT_INIT_16B(ARR_LEN, RANGE_CNT, RAND_MAX);

    TCALL(qhmath_atan2_h);

    return 0;
}
