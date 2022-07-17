/**=============================================================================
 @file
    test_exp10_f.c

 @brief
    Accuracy test for qhmath_exp10_f function

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

float test_exp10ected_outputs[] =
{
    1.0f,
    1.0f,
    0.0f,
    INFINITY,
    NAN,
    10.0f,
    100.0f,
    1000.0f,
    10000.0f,
    100000.0f,
    1000000.0f,
    10000000.0f,
    100000000.0f,
    1000000000.0f,
    10000000000.0f,
    3.162277660168379331998893544432718533719555139325216826857,
    1.778279410038922801225421195192684844735790526402255358011,
    1.258925411794167210423954106395800606093617409466931069107,
    0.794328234724281502065918282836387932588960631755484332092,
    0.316227766016837933199889354443271853371955513932521682685,



};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_exp10_f, test_inputs, TEST_INPUTS_CNT, test_exp10ected_outputs);
  return 0;
}
