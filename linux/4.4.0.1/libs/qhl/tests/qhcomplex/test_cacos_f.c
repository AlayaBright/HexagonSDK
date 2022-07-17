/**=============================================================================
@file
    test_cacos_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_cacos_f function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdint.h>
#include "qhcomplex.h"
#include "qhcomplex_test.h"

float complex test_inputs[] =
{
    {0.0f,0.0f},
    {-0.0f,0.0f},
    {-0.0f,-0.0f},
    {0.0f,-0.0f},
    {-INFINITY,-INFINITY},
    {INFINITY,-INFINITY},
    {10.5f, NAN},
    {-10.5f, NAN},
    {NAN,0.75f},
    {NAN,-0.75f},
    {NAN, NAN},
    {0.7f, 1.2f},
    {-2.0f, -3.0f},
    {5.0f, 2.0f}
};

float complex test_expected_outputs[] =
{
    {M_PI*(1.0/2.0),-0.0},
    {M_PI*(1.0/2.0),-0.0},
    {M_PI*(1.0/2.0),0.0},
    {M_PI*(1.0/2.0),0.0},
    {M_PI*(3.0/4.0),INFINITY},
    {M_PI*(1.0/4.0),INFINITY},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {1.1351827477151551088992008271819053,-1.0927647857577371459105272080819308},
    {2.1414491111159960199416055713254211,1.9833870299165354323470769028940395},
    {0.3865646f,-2.370549f}
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_cacos_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}