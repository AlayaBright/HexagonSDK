/**=============================================================================
 @file
    test_acos_f.c

 @brief
    Accuracy test for qhmath_acos_f function

 Copyright (c) 2019 Qualcomm Technologies Incorporated.
 All Rights Reserved. Qualcomm Proprietary and Confidential.
 =============================================================================**/

#include <stdio.h>
#include "qhmath.h"
#include "qhmath_test.h"

float test_inputs[] =
{
    +1.0,
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
    +0.0,
    NAN,
    NAN,
    NAN,
    NAN,
    NAN,
    NAN,
    NAN,
    1.047197551196597746154214461093167628065723133125035273658,
    2.094395102393195492308428922186335256131446266250070547316,
    1.318116071652817965745664254646040469846390966590714716853,
    1.823476581936975272716979128633462414350778432784391104121,


};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_acos_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
