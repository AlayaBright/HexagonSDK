/**=============================================================================
 @file
    test_tan_f.c

 @brief
    Accuracy test for qhmath_tan_f function

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
    -22877334.0000000,
    22877334.0000000,
    1.0f,
    -1.0f,
    4.535204837744807481768710714272047041278985095265724523158,
    -0.93076732119821435948700661435382921946334223088708119004,

};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_tan_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
