/**=============================================================================
@file
    test_ceil_f.c

@brief
    Test qhmath_ceil_f function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include "qhmath.h"
#include "qhmath_test.h"

float test_inputs[] =
{
     0.0000000f,
    -0.0000000f,
     INFINITY,
    -INFINITY,
     NAN,
    -512.0004880f,
     512.0004880f,
    -88.0449830f,
     88.0449830f,
    -0.9375000f,
     0.9375000f,
    -8.0668488f,
     4.3452396f,
    -8.3814335f,
    -6.5316734f,
     9.2670574f,
     0.6619859f,
    -0.4066039f,
     0.5617598f,
     0.7741523f,
    -0.6787637f,
     4176827.7500000f,
    -4176827.7500000f
};

float test_expected_outputs[] =
{
     0.0000000f,
    -0.0000000f,
     INFINITY,
    -INFINITY,
     NAN,
    -512.0000000f,
     513.0000000f,
    -88.0000000f,
     89.0000000f,
    -0.0000000f,
     1.0000000f,
    -8.0000000f,
     5.0000000f,
    -8.0000000f,
    -6.0000000f,
     10.0000000f,
     1.0000000f,
    -0.0000000f,
     1.0000000f,
     1.0000000f,
    -0.0000000f,
     4176828.0000000f,
    -4176827.0000000f
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_FUN_F1(qhmath_ceil_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}
