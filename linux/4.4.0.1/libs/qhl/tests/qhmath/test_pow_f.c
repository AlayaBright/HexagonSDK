/**=============================================================================
@file
    test_pow_f.c

@brief
    Accuracy test for qhmath_pow_f function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include "qhmath.h"
#include "qhmath_test.h"

float test_inputs1[] =
{
   0.0f,
  -0.0f,
   0.0f,
  -0.0f,
   0.0f,
  -0.0f,
   0.0f,
  -0.0f,
   0.0f,
  -0.0f,
  -1.0f,
  -1.0f,
   1.0f,
   1.0f,
   1.0f,
   1.0f,
   NAN,
   1.0f,
   NAN,
  -1.0f,
  -1.0f,
   0.5f,
   1.5f,
   0.5f,
   1.5f,
  -INFINITY,
  -INFINITY,
  -INFINITY,
  -INFINITY,
   INFINITY,
   INFINITY,
   NAN,
   5.0f,
   2.0f,
   2.0f,
  -2.0f,
  -2.0f,
   4.3f,
   4.3f
};

float test_inputs2[] =
{
  -1.0f,
  -1.0f,
  -2.0f,
  -2.0f,
  -INFINITY,
  -INFINITY,
   1.0f,
   1.0f,
   2.0f,
   2.0f,
  -INFINITY,
   INFINITY,
  -0.0f,
   10.0f,
   NAN,
   0.0f,
   0.0f,
  -0.0f,
  -0.0f,
   1.2f,
  -1.2f,
  -INFINITY,
  -INFINITY,
   INFINITY,
   INFINITY,
  -1.0f,
  -1.1f,
   1.0f,
   1.1f,
  -5.0f,
   5.0f,
   5.0f,
   NAN,
   4.0f,
  -4.0f,
   4.0f,
  -4.0f,
   6.4f,
  -6.4f
};

float test_expected_outputs[] =
{
   INFINITY,
  -INFINITY,
   INFINITY,
   INFINITY,
   INFINITY,
   INFINITY,
   0.0f,
  -0.0f,
   0.0f,
   0.0f,
   1.0f,
   1.0f,
   1.0f,
   1.0f,
   1.0f,
   1.0f,
   1.0f,
   1.0f,
   1.0f,
   NAN,
   NAN,
   INFINITY,
   0.0f,
   0.0f,
   INFINITY,
  -0.0f,
   0.0f,
  -INFINITY,
   INFINITY,
   0.0f,
   INFINITY,
   NAN,
   NAN,
   16.0000000f,
   0.0625000f,
   16.0000000f,
   0.0625000f,
   11329.1757813f,
   0.0000883f
};

#define TEST_INPUTS_CNT (sizeof(test_inputs1) / sizeof(*test_inputs1))

int main(void)
{
  TEST_FUN_F2(qhmath_pow_f, test_inputs1, test_inputs2, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
