/**=============================================================================
@file
    test_ctan_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_ctan_f function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdint.h>
#include "qhcomplex.h"
#include "qhcomplex_test.h"

float complex test_inputs[] =
{
    { 0.0f, 0.0f },
    { 0.0f, -0.0f },
    { -0.0f, 0.0f },
    { -0.0f, -0.0f },
    { INFINITY, -0.0f },
    { INFINITY, -2.0f },
    { NAN, -0.0f },
    { NAN, -5.0f },
    { 0.0f, -INFINITY },
    { -0.0f, -INFINITY },
    { INFINITY, -INFINITY },
    { -INFINITY, -INFINITY },
    { NAN, -INFINITY },
    { NAN, INFINITY },
    { 0.0f, NAN },
    { -0.0f, NAN },
    { 0.5f, NAN },
    { -4.5f, NAN },
    { NAN, NAN },
    { 0.7f, 1.2f },
    { -2.0f, -3.0f },
    { 0.001f, 2.0f },
    { 10.0f, 2.0f },
    { 95.0f, 2.0f },
    { 300.0f, 2.0f },
    { 80.0f, 80.0f },
    { -80.0f, -80.0f },
    { 120.0f, 200.0f },
    { -120.0f, -200.0f }
 };

float complex test_expected_outputs[] =
{
    { 0.0000000f, 0.0000000f },
    { 0.0000000f, -0.0000000f },
    { -0.0000000f,  0.0000000f },
    { -0.0000000f, -0.0000000f },
    { NAN, -0.0000000f },
    { NAN, NAN },
    { NAN, -0.0000000f },
    { NAN, NAN },
    { 0.0000000f, -1.0000000f },
    { -0.0000000f, -1.0000000f },
    { 0.0000000f, -1.0000000f },
    { -0.0000000f, -1.0000000f },
    { -0.0000000f, -1.0000000f },
    { -0.0000000f, 1.0000000f },
    { 0.0000000f, NAN },
    { -0.0000000f, NAN },
    { NAN, NAN },
    { NAN, NAN },
    { NAN, NAN },
    { 0.17207342386245727539,  0.95448070764541625976 },
    { 0.003764025401324033737182, -1.00323855876922607421 },
    { 0.000070650792622473090887, 0.96402764320373535156 },
    { 0.032938912510871887207031, 0.98461574316024780273 },
    { 0.036449901759624481201171, 0.9969087839126586914 },
    { 0.001679353066720068454742, 1.03727626800537109375 },
    { 0.0000000f, 1.0000000f },
    { -0.0000000f, -1.0000000f },
    { 0.0000000f, 1.0000000f },
    { -0.0000000f, -1.0000000f }
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_ctan_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}