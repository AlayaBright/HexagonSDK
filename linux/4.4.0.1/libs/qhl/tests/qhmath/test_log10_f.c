/**=============================================================================
 @file
    test_log10_f.c

 @brief
    Accuracy test for qhmath_log10_f function

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
    1024.0f,

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
    M_LOG10E,
    1.0f,
    4.0f,
    3.010299956639811952137388947244930267681898814621085413104,

};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_log10_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
