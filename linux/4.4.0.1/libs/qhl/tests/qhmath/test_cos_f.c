/**=============================================================================
 @file
    test_cos_f.c

 @brief
    Accuracy test for qhmath_cos_f function

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
    +INFINITY,
    NAN,
    M_PI,
    -M_PI,
    M_PI_2,
    -M_PI_2,
    M_PI_4,
    -M_PI_4,
    5000000.0f,
    18.1f,

};

float test_expected_outputs[] =
{
    1.0,
    1.0,
    NAN,
    NAN,
    NAN,
    -1.0f,
    -1.0f,
    0.0f,
    -0.0f,
    M_SQRT1_2,
    M_SQRT1_2,
    -0.2153249,
    0.7319918,

};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_cos_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
