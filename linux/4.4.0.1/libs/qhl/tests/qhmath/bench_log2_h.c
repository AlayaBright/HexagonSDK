/**=============================================================================
@file
    bench_log2_h.c

@brief
    Benchmark for measuring performance of qhmath_log2_h function.

    Benchmark measures number of cycles needed for executing qhmath_log2_h function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

@version
    v1.0.0

@date
    August 13th, 2019

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhmath_test.h"

uint16_t a[ARR_LEN];
uint16_t r[ARR_LEN];

range_uint16_t ranges[] = {
    {0, 65535},
};

#define RANGE_CNT (sizeof(ranges) / sizeof(range_uint16_t))

BENCH_TFUN_F1(qhmath_log2_h, ARR_LEN);

int main(int argc, char const *argv[])
{
    performance_high();


    BENCH_H1_INPUT_INIT_U16B(ARR_LEN, RANGE_CNT, RAND_MAX);

    TCALL(qhmath_log2_h);

    performance_normal();

    return 0;
}
