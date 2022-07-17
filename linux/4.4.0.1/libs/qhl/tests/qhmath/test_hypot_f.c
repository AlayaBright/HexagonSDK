/**=============================================================================
   @file
      test_hypot_f.c

   @brief
      Accuracy test for qhmath_hypot_f function

   Copyright (c) 2019 Qualcomm Technologies Incorporated.
   All Rights Reserved. Qualcomm Proprietary and Confidential.
   =============================================================================**/


#include <stdio.h>
#include "qhmath.h"
#include "qhmath_test.h"

float test_inputs1[] =
{
    3.0000000,
    -3.0000000,
    4.0000000,
    4.0000000,
    -3.0000000,
    -8.0668488,
    4.3452396,
    -8.3814335,
    -6.5316734,
    INFINITY,
    -INFINITY,
    INFINITY,
    INFINITY,
    -INFINITY,
    -INFINITY,
    NAN,
    NAN,
    -NAN,
    -NAN,
    NAN,
    0.0000000,
};

float test_inputs2[] =
{
    4.0000000,
    4.0000000,
    3.0000000,
    -3.0000000,
    -4.0000000,
    4.5356627,
    -8.8879910,
    -2.7636073,
    4.5675354,
    0.0000000,
    0.0000000,
    NAN      ,
    -NAN      ,
    NAN      ,
    -NAN      ,
    INFINITY ,
    -INFINITY ,
    INFINITY ,
    -INFINITY ,
    0.0000000,
    NAN,
};

float test_expected_outputs[] =
{
    5.0000000,
    5.0000000,
    5.0000000,
    5.0000000,
    5.0000000,
    9.2545280,
    9.8933048,
    8.8253021,
    7.9702659,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    NAN,
    NAN,
};

#define TEST_INPUTS_CNT (sizeof(test_inputs1) / sizeof(*test_inputs1))

int main(void)
{
    TEST_FUN_F2(qhmath_hypot_f, test_inputs1, test_inputs2, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
