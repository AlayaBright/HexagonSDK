/**=============================================================================
@file
   bench_sigmoid_ah.c

@brief   Benchmark for measuring performance of qhmath_sigmoid_ah function.

           Benchmark measures number of cycles needed for executing
           qhmath_sigmoid_ah function certain number of times on randomly generated values.
           Performance is shown in cycles per call (CPC) and it is calculated as

           CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhmath_test.h"

range_int16_t ranges[] =
{
    {-32768, 32767}
};

static int16_a8_t input[ARR_LEN];
static int16_a8_t output[ARR_LEN];
static int32_t r;

BENCH_TFUN_AF(qhmath_sigmoid_ah, NR_OF_ITERATIONS, ARR_LEN, input, output, ARR_LEN);

int main(int argc, char const *argv[])
{
    BENCH_INPUT_INIT_TYPE(int16_t, ranges, ARR_LEN, input);

    BENCH_TCALL(qhmath_sigmoid_ah);

    return 0;
}
