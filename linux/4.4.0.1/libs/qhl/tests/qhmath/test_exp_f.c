/**=============================================================================
 @file
    test_exp_f.c

 @brief
    Accuracy test for qhmath_exp_f function

 Copyright (c) 2019 Qualcomm Technologies Incorporated.
 All Rights Reserved. Qualcomm Proprietary and Confidential.
 =============================================================================**/

#include <stdio.h>
#include "qhmath.h"
#include "qhmath_test.h"

float test_inputs[] =
{
    -0.0,
    +0.0,
    -INFINITY,
    INFINITY,
    NAN,
    M_LN2,
    M_LN10,
    1.0f,
    10.0f,
    -10.0f,


};

float test_expected_outputs[] =
{
    1.0f,
    1.0f,
    0.0f,
    INFINITY,
    NAN,
    2.0f,
    10.0f,
    M_E,
    22026.465794806703,
    0.000045399929762484875,


};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_exp_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
