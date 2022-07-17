/**=============================================================================
 @file
    test_sqrt_f.c

 @brief
    Accuracy test for qhmath_sqrt_f function

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
    -81.0,
    -1.0,
    1.0,
    2.0,
    9.0,
    81.0,
    0.25,
    0.01,
    0.000001,
    0.0000000001,
    1.0e+100,
};

float test_expected_outputs[] =
{
    NAN,
    -0.0,
    +0.0,
    +INFINITY,
    NAN,
    NAN,
    NAN,
    1.0,
    1.4142135623730950488016887242097,
    3.0,
    9.0,
    0.5,
    0.1,
    0.001,
    0.00001,
    INFINITY,
};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_sqrt_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
