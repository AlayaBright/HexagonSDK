/**=============================================================================
@file
    bench_sin_h.c

@brief
    Benchmark for measuring performance of qhmath_sin_h function.

    Benchmark measures number of cycles needed for executing qhmath_sin_h function
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

#define N_SAMPLES            (1<<16)
#define MAX_INT_16_NUM_F     32768.0

/* Input/Output array for calculation of accuracy */
static int16_t a[N_SAMPLES];
static int16_t r[N_SAMPLES];
static int16_t expect_output_array[N_SAMPLES];
static int16_t error[N_SAMPLES];

/* Function for generating of all possible inputs and outputs */
void gen_sin_test(void)
{
    float x_input_f_0_1;
    int16_t x_input_int;
    float result_float;

    for (int i = -N_SAMPLES/2; i < N_SAMPLES/2; i++)
    {
        x_input_int = (int16_t) i;
        a[i + N_SAMPLES/2] = i;
        x_input_f_0_1 = ((float) x_input_int)/MAX_INT_16_NUM_F;
        result_float = qhmath_sin_f(x_input_f_0_1 * 2 * M_PI);
        expect_output_array[i + N_SAMPLES/2] = (int16_t) (result_float * MAX_INT_16_NUM_F);
    }
}

BENCH_TFUN_F1(qhmath_sin_h, N_SAMPLES);

int main(int argc, char const *argv[])
{
    performance_high();

    float avg_error = 0.0;

    puts("Benchmark of qhmath_sin_h(x)");

    /* Generate input and output test array for calculating g accuracy */
    gen_sin_test();

    TCALL(qhmath_sin_h);

    /* Computation of error for each sample and calculates max_error and average error */
    int16_t max_error = 0;
    for (int i = 0; i  < N_SAMPLES; i++)
    {
        error[i] = abs(r[i] - expect_output_array[i]);
        avg_error += error[i];

        if (max_error < error[i])
        {
            max_error = error[i];
        }
    }
    avg_error /= N_SAMPLES;

    printf("MAX error: %d \n", max_error);
    printf("AVG error: %f \n", avg_error);

    performance_normal();

    return 0;
}
