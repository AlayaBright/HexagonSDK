/**=============================================================================
  @file
     test_copysign_f.c

  @brief
     Accuracy test for qhmath_copysign_f function

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
  =============================================================================**/

#include <stdio.h>
#include "qhmath.h"
#include "qhmath_test.h"

float test_inputs1[] =
{
    1.0,
    1.0,
    INFINITY,
    NAN,
    5.0,
    5.0,

};

float test_inputs2[] =
{
    2.0,
    -2.0,
    -2.0,
    -2.0,
    0.0,
    -0.0,
};

float test_expected_outputs[] =
{
    1.0,
    -1.0,
    -INFINITY,
    NAN,
    5.0,
    -5.0
};

#define TEST_INPUTS_CNT (sizeof(test_inputs1) / sizeof(*test_inputs1))

int main(void)
{
    TEST_FUN_F2(qhmath_copysign_f, test_inputs1, test_inputs2, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
