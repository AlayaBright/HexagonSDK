/**=============================================================================
  @file
     test_sinh_f.c

  @brief
     Accuracy test for qhmath_sinh_f function

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
  =============================================================================**/

#include <stdio.h>
#include "qhmath.h"
#include "qhmath_test.h"

float test_inputs[] =
{
    0.0f,
    -0.0f,
    0.0001f,
    -0.0001f,
    0.9f,
    -0.9f,
    17.0f,
    -17.0f,
    90.1234f,
    -90.1234f,
    190000.0f,
    2000000.0f,
    INFINITY,
    -INFINITY,
    NAN,
};

float test_expected_outputs[] =
{
    0.0f,
    -0.0f,
    0.0001f,
    -0.0001f,
    1.0265167f,
    -1.0265167f,
    12077476.0000000f,
    -12077476.0000000f,
    INFINITY,
    -INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    -INFINITY,
    NAN,
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_FUN_F1(qhmath_sinh_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
