/**=============================================================================
 @file
    test_log2_f.c

 @brief
    Accuracy test for qhmath_log2_f function

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
    M_LOG2E,
    3.321928094887362347870319429489390175864831393024580612054,
    13.28771237954944939148127771795756070345932557209832244821,

};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_log2_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
