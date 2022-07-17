/**=============================================================================
  @file
     test_cot_f.c

  @brief
     Test qhmath_cot_f function and compare with functions from Quallcomm libm

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
  =============================================================================**/

 #include <stdio.h>
 #include "qhmath.h"
 #include "qhmath_test.h"

float test_inputs[] =
{
    NAN,
    0.0,
    -0.0,
    0.7,
    -0.7,
    M_PI_4, // pi/4
    0.0001,
    190000.0,
    2000000.0,
    1.6101720e+0,
    6.5081270e-1,
    9.6070570e-1,
    -1.4619480e+0,
    1.0555760e+0,
    6.5191730e-1,
    1.3674480e+0,
    -2.1763320e+0,
    1.9753620e-1,
    7.0250490e-1,
    -9.3572910e+1,
    -5.8672100e+1,
     1.8013090e+2,
     2.0801900e+1,
    -1.4519180e+2,
     4.1483930e+1,
     7.5153370e+1,
     8.8144220e+1,
     1.8425780e+1,
     4.1441680e+1,
    -9.3973240e+1,
     1.2997560e+1,
    -1.7563500e+1,
    -5.0899510e+1,
     2.7882770e+1,
     4.4369480e+1,
    -1.3889530e+2,
     2.5465690e+1,
    -1.6252680e+0,
    -9.5459260e+1,


};

float test_expected_outputs[] =
{
    NAN,
    INFINITY,
    -INFINITY,
    1.1872419,
    -1.1872419,
    0.9999999,
    10000.0,
    -2.4885175,
    -1.1514299,
    -0.0393961,
    1.3132191,
    0.6990537,
    -0.1092802,
    0.5662326,
    1.3102139,
    0.2061984,
    0.6922944,
    4.996346,
    1.1812241,
    1.2496613,
    0.6167379,
    0.5602419,
    -0.4012095,
    -1.2403086,
    1.3341029,
    -4.0021892,
    5.5071769,
    -2.2167544,
    1.4586378,
    3.5505357,
    2.1736257,
    0.2926931,
    -1.3599879,
    -2.4219902,
    2.4523888,
    -1.2746986,
    2.8916476,
    0.0545256,
    -0.3756232,
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(float))

int main(int argc, char *argv[])
{
    TEST_FUN_F1(qhmath_cot_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);
    return 0;
}
