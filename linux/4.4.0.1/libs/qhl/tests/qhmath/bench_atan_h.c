/**=============================================================================
@file
    bench_atan_h.c

@brief
    Benchmark for measuring performance of qhmath_atan_h function.

    Benchmark measures number of cycles needed for executing qhmath_atan_h function
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

int32_t a[ARR_LEN];
uint32_t r[ARR_LEN];

range_int32_t ranges[] = {
    {-2147483648, -65535},
    {-65535, 0},
    {0, 65535},
    {65535, 2147483647}
};

#define RANGE_CNT (sizeof(ranges) / sizeof(range_int32_t))

BENCH_TFUN_F1(qhmath_atan_h, ARR_LEN);

int main(void)
{

    BENCH_H1_INPUT_INIT_32B(ARR_LEN, RANGE_CNT, RAND_MAX);

    TCALL(qhmath_atan_h);

    return 0;
}
