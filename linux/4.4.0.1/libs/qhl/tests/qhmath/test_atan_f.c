/**=============================================================================
 @file
    test_atan_f.c

 @brief
    Accuracy test for qhmath_atan_f function

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
    1.0,
    -1.0,

};

float test_expected_outputs[] =
{
    -0.0,
    +0.0,
    -M_PI_2,
    M_PI_2,
    NAN,
    0.982793723247329067985710611014666014496877453631628556761,
    -0.982793723247329067985710611014666014496877453631628556761,
    1.564129758891028390082177704138146011476364458463650826792,
    -1.564129758891028390082177704138146011476364458463650826792,
    0.463647609000806116214256231461214402028537054286120263810,
    -0.463647609000806116214256231461214402028537054286120263810,
    0.24497866312686415417208248121127581091414409838118406712,
    -0.24497866312686415417208248121127581091414409838118406712,
    M_PI_4,
    -M_PI_4,


};


#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))


int main(void)
{
  TEST_FUN_F1(qhmath_atan_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
