/**=============================================================================
@file
    test_casinh_f.c

@brief
    Testing qhcomplex_casinh_f function for corner cases and accuracy.

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
    {-INFINITY,INFINITY},
    {-INFINITY,-INFINITY},
    {10.5f, NAN},
    {-10.5f, NAN},
    {NAN, 0.75f},
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
    {-INFINITY,M_PI*(1.0/4.0)},
    {-INFINITY,-M_PI*(1.0/4.0)},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    { 0.97865459559367387689317593222160964f,0.91135418953156011567903546856170941f},
    {-1.9686379257930962917886650952454982f, - 0.96465850440760279204541105949953237f},
    {2.3830308809f,0.3746708048f}
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_casinh_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}