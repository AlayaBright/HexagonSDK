/**=============================================================================
  @file
     test_div_f.c

  @brief
     Accuracy test for qhmath_div_f function

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
  =============================================================================**/

#include <stdio.h>
#include "qhmath.h"
#include "qhmath_test.h"

float test_inputs1[] =
{
    0.0,
    0.0,
    -0.0,
    -0.0,
    INFINITY,
    -INFINITY,
    INFINITY,
    -INFINITY,
    NAN,
    5.0,
    -5.0,
    2341.234214,
    459.2196849,
    1,


};

float test_inputs2[] =
{
    0.0,
    -0.0,
    0.0,
    -0.0,
    5.0,
    5.0,
    -5.0,
    -5.0,
    1.0,
    NAN,
    NAN,
    232.12342,
    5.248465,
    1.5,
    2.6,
    4,


};

float test_expected_outputs[] =
{
    NAN,
    NAN,
    NAN,
    NAN,
    INFINITY,
    -INFINITY,
    -INFINITY,
    INFINITY,
    NAN,
    NAN,
    NAN,
    10.0861607,
    87.4960022,
    0.6666667,
    0.25,

};

#define TEST_INPUTS_CNT (sizeof(test_inputs1) / sizeof(*test_inputs1))

int main(void)
{
    TEST_FUN_F2(qhmath_div_f, test_inputs1, test_inputs2, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
