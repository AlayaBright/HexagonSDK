/**=============================================================================
 @file
    test_cabs_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_cabs_f function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdint.h>
#include "qhcomplex.h"
#include "qhcomplex_test.h"

float complex test_inputs[] =
{
    {  3.0000000,  4.0000000 },
    { -3.0000000,  4.0000000 },
    {  4.0000000,  3.0000000 },
    {  4.0000000, -3.0000000 },
    { -3.0000000, -4.0000000 },
    { -8.0668488,  4.5356627 },
    {  4.3452396, -8.8879910 },
    { -8.3814335, -2.7636073 },
    { -6.5316734,  4.5675354 },
    {  INFINITY,   0.0000000 },
    { -INFINITY,   0.0000000 },
    {  INFINITY,   NAN       },
    {  INFINITY,  -NAN       },
    { -INFINITY,   NAN       },
    { -INFINITY,  -NAN       },
    {  NAN,        INFINITY  },
    {  NAN,       -INFINITY  },
    { -NAN,        INFINITY  },
    { -NAN,       -INFINITY  },
    { INFINITY,    INFINITY  },
    {-INFINITY,    INFINITY  },
    { INFINITY,   -INFINITY  },
    {-INFINITY,   -INFINITY  },
    {  NAN,        0.0000000 }, 
    {  0.0000000,  NAN       }
};

float test_expected_outputs[] =
{
    5.0000000,
    5.0000000,
    5.0000000,
    5.0000000,
    5.0000000,
    9.2545280,
    9.8933048,
    8.8253021,
    7.9702659,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    INFINITY,
    NAN,
    NAN
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_F1(qhcomplex_cabs_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}