/**=============================================================================
 @file
     test_atan2_f.c

 @brief
     Test qhmath_atan2_f function

 Copyright (c) 2019 Qualcomm Technologies Incorporated.
 All Rights Reserved. Qualcomm Proprietary and Confidential.
 =============================================================================**/

#include <stdio.h>
#include "qhmath.h"
#include "qhmath_test.h"

float test_inputs1[] =
{
    -0x1.0223ap+3,
    0x1.161868p+2,
    -0x1.0c34b4p+3,
    -0x1.a206fp+2,
    0x1.288bbcp+3,
    -0x1.9ccd8cp+2,
    0x1.f6f80ep+2,
    -0x1.95882cp-1,
    0x1.3b3d62p-1,
    1.0,
    1.0,
    -1.0,
    -1.0,
    10.0,
    0.0,
    -0.0,
    0.0,
    -0.0,
    0.0,
    -0.0,
    0.0,
    -0.0,
    INFINITY,
    -INFINITY,
    INFINITY,
    -INFINITY,
    INFINITY,
    -INFINITY,
    -2.0,
    -2.0,
    3.0,
    3.0,
    3.0,
    -3.0,
    3.0,
    -3.0,
    NAN,
    NAN,
    5.0,
    -5.0,
    NAN,
};

float test_inputs2[] =
{
    0x1.22484cp+2,
    -0x1.1c6a6cp+3,
    -0x1.61bde2p+1,
    0x1.24528p+2,
    0x1.33eddap+2,
    0x1.52fb12p-1,
    0x1.ab3ff8p-5,
    0x1.eb4a2ep+2,
    0x1.018606p+1,
    1.0,
    -1.0,
    -1.0,
    1.0,
    20.0,
    -2.0,
    -2.0,
    -0.0,
    -0.0,
    2.0,
    2.0,
    +0.0,
    +0.0,
    3.0,
    3.0,
    -INFINITY,
    -INFINITY,
    +INFINITY,
    +INFINITY,
    +0.0,
    -0.0,
    +0.0,
    -0.0,
    -INFINITY,
    -INFINITY,
    INFINITY,
    INFINITY,
    5.0,
    -5.0,
    NAN,
    NAN,
    NAN,
};

float test_expected_outputs[] =
    {
    -0x1.0effbap+0,
    0x1.57eb78p+1,
    -0x1.e3a92cp+0,
    -0x1.ebccccp-1,
    0x1.178792p+0,
    -0x1.77f02cp+0,
    0x1.906cccp+0,
    -0x1.a52304p-4,
    0x1.301b76p-2,
    0.7853982,
    2.3561945,
    -2.3561945,
    -0.7853982,
    0.4636476,
    M_PI ,
    -M_PI ,
    M_PI ,
    -M_PI ,
    0.0 ,
    -0.0 ,
    0.0 ,
    -0.0 ,
    M_PI/2.0,
    -M_PI/2.0,
    3.0*M_PI/4.0,
    -3.0*M_PI/4.0,
    M_PI/4.0,
    -M_PI/4.0,
    -M_PI/2.0,
    -M_PI/2.0,
    M_PI/2.0,
    M_PI/2.0,
    M_PI,
    -M_PI,
    0.0,
    -0.0,
    NAN,
    NAN,
    NAN,
    NAN,
    NAN,
};


#define TEST_INPUTS_CNT (sizeof(test_inputs1) / sizeof(*test_inputs1))

int main(void)
{
    TEST_FUN_F2(qhmath_atan2_f, test_inputs1, test_inputs2, TEST_INPUTS_CNT, test_expected_outputs)

    return 0;
}
