/**=============================================================================
@file
    test_abs_h.c

@brief
    Test accuracy of qhmath_abs_h function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/
#include <stdio.h>
#include <stdbool.h>

#include <qhmath.h>

#define N_SAMPLES (1 << 16) // full

#ifndef T_START
#define T_START 0
#endif
#ifndef T_END
#define T_END N_SAMPLES
#endif

int16_t ok_qhmath_abs_h(int16_t x)
{
    int16_t result;

    if (x == (int16_t)0x8000)
    {
        result = 0x7fff;
    }
    else if (x < 0)
    {
        result = -x;
    }
    else
    {
        result = x;
    }

    return result;
}

int main(int argc, char const *argv[])
{
    bool test_ok = true;

    for (int x_i = T_START; x_i < T_END; x_i++)
    {
        int16_t x = x_i;
        int16_t res_ok = ok_qhmath_abs_h(x);
        int16_t y = qhmath_abs_h(x);

        if (res_ok != y)
        {
            printf("Error: x= %d -> y= %d | ok= %d\n", x, y, res_ok);

            test_ok = false;
        }
    }

    printf("Accuracy test of qhmath_abs_h(x) : %s\n",
           test_ok ? "PASSED" : "FAILED");

    return 0;
}
