/**=============================================================================
  @file
     test_tanh_f.c

  @brief
     Accuracy test for qhmath_tanh_f function

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
  =============================================================================**/

#include <stdio.h>
#include "qhmath.h"
#include "qhmath_test.h"

float test_inputs[] =
{
    0.0,
    -0.0,
    INFINITY,
    -INFINITY,
    NAN,
    0.7,
    -0.7,
    1.0,
    -1.0,
    0x8p-151,
    -0x1p-120,
    0x1p-130,
    15.0,
    0.3,
    0.6,
    2.0,
    5.0,
};

float test_expected_outputs[] =
{
    0.0,
    -0.0,
    1.0,
    -1.0,
    NAN,
    0.60436777711716349631,
    -0.60436777711716349631,
    0.7615941559557648881194582826047935904,
    -0.7615941559557648881194582826047935904,
    0x8p-151,
    -0x1p-120,
    0x1p-130,
    0.99999999999981284754062321402092320874693430031776848529,
    0.291312612451590905818221272823765928153596804917612174414,
    0.537049566998035285861825304926896705982841987719232134172,
    0.9640275800758168839464137241009231502550299762409347,
    0.999909204262595131210990447534473021089812615990547862736,

};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
  TEST_FUN_F1(qhmath_tanh_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
  return 0;
}
