/**=============================================================================
@file
    test_cpow_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_cpow_f function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdint.h>
#include "qhcomplex.h"
#include "qhcomplex_test.h"

float complex test_inputs1[] =
{
    {1.0, 0.0 },
    {2.0, 0.0},
    {M_E, 0.0},
    //{2.0, 3.0},
    {-0.5,10.0},
    //{-1.5,-7.0},
    {NAN, NAN}
};

float complex test_inputs2[] =
{
    { 0.0, 0.0},
    { 10.0, 0.0},
    { 0.0, 2*M_PI},
    //{ 4.0, 0.0},
    {-0.5,1.0},
    //{-2.25,-3.0},
    { NAN, NAN}
};

float complex test_expected_outputs[] =
{
    {1.0, 0.0},
    {1024.0, 0.0},
    {1.0, 0.0},
    //{-119.0, -120.0},
    {0.00482854, 0.0623080},
    //{-0.0000181653,0.0000539069},
    {NAN, NAN}
};

#define TEST_INPUTS_CNT (sizeof(test_inputs1) / sizeof(*test_inputs1))

int main(void)
{
    TEST_CMPLX_FUN_CF2(qhcomplex_cpow_f, test_inputs1, test_inputs2, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}