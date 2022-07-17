/**=============================================================================
  @file
     test_abs_f.c

  @brief
     Accuracy test for qhmath_abs_f function

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
  =============================================================================**/

#include <stdio.h>
#include "qhmath.h"
#include "qhmath_test.h"

float test_inputs[] =
{
    -0.0,
    0.0,
    0.1,
    -0.01,
    0.7,
    -0.7,
    0.785398163397, // pi/4
    -0.0001,
    -190000.0,
    2000000.0,
    INFINITY,
    -INFINITY,
    NAN
};

float test_expected_outputs[] =
{
    0.0,
    0.0,
    0.1,
    0.01,
    0.7,
    0.7,
    0.785398163397, // pi/4
    0.0001,
    190000.0,
    2000000.0,
    INFINITY,
    INFINITY,
    NAN
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_FUN_F1(qhmath_abs_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
