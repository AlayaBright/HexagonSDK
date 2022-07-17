/**=============================================================================
@file
    test_casin_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_casin_f function.

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
    {0.0f,-0.0f},
    {-0.0f,-0.0f},
    {INFINITY,-INFINITY},
    {-INFINITY,-INFINITY},
    {NAN, 10.5f},
    {NAN, -10.5f},
    {0.75f, NAN},
    {-0.75f, NAN},
    {NAN, NAN},
    {0.7f, 1.2f},
    {-2.0f, -3.0f},
    {5.0f, 2.0f}
};

float complex test_expected_outputs[] =
{

    {0.0f, 0.0f},
    {-0.0f, 0.0f},
    {0.0f, -0.0f},
    {-0.0f, -0.0f},
     {M_PI*(1.0/4.0),-INFINITY},
    {-M_PI*(1.0/4.0),-INFINITY},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {0.4356136f,1.0927648f},
    {-0.5706528f,- 1.9833870f},
    {1.184231f, 2.3705485f}
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_casin_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}
