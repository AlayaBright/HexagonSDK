/**=============================================================================
 @file
     test_rsqrt_f.c

 @brief
     Accuracy test for qhmath_rsqrt_f function

 Copyright (c) 2019 Qualcomm Technologies Incorporated.
 All Rights Reserved. Qualcomm Proprietary and Confidential.
 =============================================================================**/

#include <stdio.h>
#include "qhmath.h"
#include "qhmath_test.h"

float test_inputs[] =
{
    -INFINITY,
    -0.0,
    +0.0,
    +INFINITY,
    NAN,
    -1.0,
    1.0,
    0.25,
    0.01,
    0.0000000001,
    1.0e+100,
    2.700999498367309400000000000000e-026,
    3.900998353958130100000000000000e-023,
    1.001000046730041500000000000000e+000,
};

float test_expected_outputs[] =
{
    NAN,
    NAN,
    INFINITY,
    +0.0,
    NAN,
    NAN,
    1.0,
    2.0,
    10.0,
    100000.0,
    0.0,
    6.084680071340762700000000000000e+012,
    1.601076622807124900000000000000e+011,
    9.995003513577569400000000000000e-001,
};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_rsqrt_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
