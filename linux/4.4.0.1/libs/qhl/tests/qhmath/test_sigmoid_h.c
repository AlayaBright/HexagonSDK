/**=============================================================================
@file
    test_sigmoid_h.c

@brief
    Benchmark for measuring performance of qhmath_sigmoid_h function.

    Benchmark measures number of cycles needed for executing qhmath_sigmoid_h function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>

#include "qhmath.h"
#include "qhl_common.h"

#define N_SAMPLES            (1<<16) // full
#define MAX_INT_16_NUM_F     32768.0

/* Input/Output array for calculation of accuracy */
static int16_t input_array[N_SAMPLES];
static int16_t output_array[N_SAMPLES];
static int16_t expect_output_array[N_SAMPLES];

/* Function for generating of all possible inputs and outputs */
void gen_ok_test(void)
{
    int16_t x_input_i;
    float x_input_f;
    int16_t ref_ok_i;
    float ref_ok_f;

    for (int i = 0; i < N_SAMPLES; i++)
    {
        x_input_i = (int16_t)i;

        // Q3.12
        x_input_f = ((float)x_input_i) / MAX_INT_16_NUM_F;
        x_input_f *= 8.0;

        /* -1 -- 1 */
        ref_ok_f = qhmath_sigmoid_f(x_input_f);
        ref_ok_i = (int16_t)(ref_ok_f * MAX_INT_16_NUM_F);

        input_array[i] = x_input_i;
        expect_output_array[i] = ref_ok_i;
    }
}

int main(int argc, char const *argv[])
{
    float avg_error = 0.0;

    puts("Accuracy test of qhmath_sigmoid_h(x)");

    /* Generate input and output test array for calculating g accuracy */
    gen_ok_test();

    for (int i = 0; i < N_SAMPLES; i++)
    {
        output_array[i] = qhmath_sigmoid_h(input_array[i]);
    }

    /* Computation of error for each sample and calculates max_error and average error */
    int16_t max_error = 0;
    for (int i = 0; i < N_SAMPLES; i++)
    {
        int16_t error_i = abs(output_array[i] - expect_output_array[i]);
        avg_error += error_i;

        if (max_error < error_i)
            max_error = error_i;
    }
    avg_error /= N_SAMPLES;

    float max_error_f = (float)max_error / MAX_INT_16_NUM_F;
    float avg_error_f = avg_error / MAX_INT_16_NUM_F;

    printf("MAX error: %d [fixed-point] = %f [floating-point]\n", max_error, max_error_f);
    printf("AVG error: %f [fixed-point] = %f [floating-point]\n", avg_error, avg_error_f);

    return 0;
}
