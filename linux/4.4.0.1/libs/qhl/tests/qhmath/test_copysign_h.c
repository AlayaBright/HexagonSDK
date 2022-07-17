/**=============================================================================
@file
    test_copysign_h.c

@brief
    Benchmark for measuring performance of qhmath_copysign_h function.

    Benchmark measures number of cycles needed for executing qhmath_copysign_h function
    certain number of times on randomly generated values.
    Performance is shown in cycles per call (CPC) and it is calculated as

    CPC = elapsed_nr_of_cycles / nr_of_calls

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "qhmath.h"
#include "qhl_common.h"

#define N_SAMPLES       (1 << 16) // full

#ifndef T_START
#   define T_START      0
#endif
#ifndef T_END
#   define T_END        N_SAMPLES
#endif

int16_t ok_qhmath_copysign_h(int16_t x, int16_t k)
{
    int16_t result;
    int16_t x_abs = abs(x);
    int is_negative = k < 0;

    if (x == (int16_t)0x8000)
    {
        result = is_negative ? x : 0x7fff;
    }
    else
    {
        result = is_negative ? -x_abs : x_abs;
    }

    return result;
}

int main(int argc, char const *argv[])
{
    bool test_ok = true;

    for (int16_t k = -1; k <= 1; k++)
    {
        for (int x_i = T_START; x_i < T_END; x_i++)
        {
            int16_t x = x_i;
            int16_t res_ok = ok_qhmath_copysign_h(x, k);
            int16_t y = qhmath_copysign_h(x, k);

            if (res_ok != y)
            {
                printf("Error: x= %d k= %d y= %d | ok= %d\n", x, k, y, res_ok);

                test_ok = false;
            }
        }
    }

    printf("Accuracy test of qhmath_copysign_h(x,k) : %s\n",
           test_ok ? "PASSED" : "FAILED");

    return 0;
}
