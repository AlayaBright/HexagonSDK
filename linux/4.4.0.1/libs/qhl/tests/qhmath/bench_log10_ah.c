/**=============================================================================
@file
   bench_log10_ah.c

@brief   Benchmark for measuring performance of qhmath_log10_ah function.

           Benchmark measures number of cycles needed for executing
           qhmath_log10_ah function certain number of times on randomly generated values.
           Performance is shown in cycles per call (CPC) and it is calculated as

           CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhmath_test.h"

range_uint16_t ranges[] =
{
    {1, 65535}
};

static uint16_t input[ARR_LEN];
static uint16_t output[ARR_LEN];
static int32_t r;

BENCH_TFUN_AF(qhmath_log10_ah, NR_OF_ITERATIONS, ARR_LEN, input, output, ARR_LEN);

int main(int argc, char const *argv[])
{
    BENCH_INPUT_INIT_TYPE(uint16_t, ranges, ARR_LEN, input);

    BENCH_TCALL(qhmath_log10_ah);

    return 0;
}
