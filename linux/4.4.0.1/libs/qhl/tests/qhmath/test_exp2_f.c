/**=============================================================================
 @file
    test_exp2_f.c

 @brief
    Accuracy test for qhmath_exp2_f function

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
    1.0f,
    2.0f,
    3.0f,
    4.0f,
    5.0f,
    6.0f,
    7.0f,
    8.0f,
    9.0f,
    10.0f,
    0.5f,
    0.25f,
    0.1f,
    -0.1f,
    -0.5f,


};

float test_exp2ected_outputs[] =
{
    1.0f,
    1.0f,
    0.0f,
    INFINITY,
    NAN,
    2.0f,
    4.0f,
    8.0f,
    16.0f,
    32.0f,
    64.0f,
    128.0f,
    256.0f,
    512.0f,
    1024.0f,
    M_SQRT2,
    1.189207115002721066717499970560475915292972092463817413019,
    1.071773462536293164213006325023342022906384604977556783482,
    0.933032991536807415981343266149942167027229964351494038900,
    M_SQRT1_2,


};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_exp2_f, test_inputs, TEST_INPUTS_CNT, test_exp2ected_outputs);
  return 0;
}
