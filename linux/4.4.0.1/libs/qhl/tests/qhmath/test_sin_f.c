/**=============================================================================
 @file
    test_sin_f.c

 @brief
    Accuracy test for qhmath_sin_f function

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
    -0.0,
    +0.0,
    NAN,
    NAN,
    NAN,
    0.0f,
    0.0f,
    1.0f,
    -1.0f,
    M_SQRT1_2,
    -M_SQRT1_2,
    -0.97654246865708292173786875668230727866586450127603282255,
    -0.68131376555550101290846750757333824131970428886485728258,

};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_sin_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
