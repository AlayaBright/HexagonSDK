/**=============================================================================
 @file
    test_asin_f.c

 @brief
    Accuracy test for qhmath_asin_f function

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
    1.5,
    -1.5,
    150.0,
    -150.0,
    0.5,
    -0.5,
    0.25,
    -0.25,

};

float test_expected_outputs[] =
{
    -0.0,
    +0.0,
    NAN,
    NAN,
    NAN,
    NAN,
    NAN,
    NAN,
    NAN,
    0.5235987755982988730771072305465838140328615665625176,
    -0.5235987755982988730771072305465838140328615665625176,
    0.252680255142078653485657436993710972252193733096838,
    -0.252680255142078653485657436993710972252193733096838


};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_asin_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
