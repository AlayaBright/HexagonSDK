/**=============================================================================
@file
    test_catanh_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_catanh_f function.

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
    {INFINITY,INFINITY},
    {INFINITY,-INFINITY},
    {-INFINITY,INFINITY},
    {-INFINITY,-INFINITY},
    {-10.0f, INFINITY},
    {-10.0f,-INFINITY},
    {-0.0f, INFINITY},
    {-0.0f, -INFINITY},
    {0.0f, INFINITY},
    {0.0f,-INFINITY},
    {0.1f, INFINITY},
    {0.1f, -INFINITY},
    {-INFINITY,0.0f},
    {-INFINITY,-0.0f},
    {-INFINITY,100.0f},
    {-INFINITY,-100.0f},
    {INFINITY, 0.0f},
    {INFINITY, -0.0f},
    {INFINITY, 0.5f},
    {INFINITY, -0.5f},
    { 0.0f,NAN},
    { -0.0f,NAN},
    {INFINITY, NAN},
    {-INFINITY, NAN},
    {NAN,0.0},
    {NAN,-0.0},
    {NAN,INFINITY},
    {NAN,-INFINITY},
    {10.5f, NAN},
    {-10.5f, NAN},
    {NAN,0.75f},
    {NAN,-0.75f},
    {NAN, NAN},
    {0.7f, 1.2f},
    {-2.0f, -3.0f}
 };

 float complex test_expected_outputs[] =
 {
    {0.0f, 0.0f},
    {-0.0f, 0.0f},
    {0.0f, -0.0f},
    {-0.0f, -0.0f},
    {0.0f,M_PI*(1.0/2.0)},
    {0.0f,-M_PI*(1.0/2.0)},
    {-0.0f,M_PI*(1.0/2.0)},
    {-0.0f,-M_PI*(1.0/2.0)},
    {-0.0f,M_PI*(1.0/2.0)},
    {-0.0f,-M_PI*(1.0/2.0)},
    {-0.0f,M_PI*(1.0/2.0)},
    {-0.0f,-M_PI*(1.0/2.0)},
    {0.0f,M_PI*(1.0/2.0)},
    {0.0f,-M_PI*(1.0/2.0)},
    {0.0f,M_PI*(1.0/2.0)},
    {0.0f,-M_PI*(1.0/2.0)},
    {-0.0f,M_PI*(1.0/2.0)},
    {-0.0f,-M_PI*(1.0/2.0)},
    {-0.0f,M_PI*(1.0/2.0)},
    {-0.0f,-M_PI*(1.0/2.0)},
    {0.0f,M_PI*(1.0/2.0)},
    {0.0f,-M_PI*(1.0/2.0)},
    {0.0f,M_PI*(1.0/2.0)},
    {0.0f,-M_PI*(1.0/2.0)},
    {0.0f, NAN},
    {-0.0f, NAN},
    {0.0f, NAN},
    {-0.0f, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {0.0,M_PI*(1.0/2.0)},
    {0.0,-M_PI*(1.0/2.0)},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {0.2600749516525135959200648705635915,0.97024030779509898497385130162655963},
    {-0.14694666862487792968,-1.33897244930267333984}
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_catanh_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}
