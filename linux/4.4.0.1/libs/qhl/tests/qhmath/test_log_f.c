/**=============================================================================
 @file
    test_log_f.c

 @brief
    Accuracy test for qhmath_log_f function

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
    1.0f,
    -5.0f,
    +INFINITY,
    NAN,
    M_E,
    10.0f,
    10000.0f,

};

float test_expected_outputs[] =
{
    NAN,
    -INFINITY,
    -INFINITY,
    0.0f,
    NAN,
    +INFINITY,
    NAN,
    1.0,
    2.302585092994045684017991454684364207601101488628772976033,
    9.210340371976182736071965818737456830404405954515091904133,

};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_log_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
