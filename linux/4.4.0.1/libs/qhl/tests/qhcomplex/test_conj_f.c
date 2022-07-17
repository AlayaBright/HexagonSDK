/**=============================================================================
 @file
    test_conj_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_conj_f function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdint.h>
#include "qhcomplex.h"
#include "qhcomplex_test.h"

float complex test_inputs[] =
{
    {  0.0f,      0.0f },
    {  0.0f,     -0.0f },
    { -0.0f,      0.0f },
    { -0.0f,     -0.0f },
    {  1.0f,      1.0f },
    {  1.0f,     -1.0f },
    { -1.0f,      1.0f },
    { -1.0f,     -1.0f },
    {  NAN,       NAN },
    {  NAN,      -NAN },
    { -NAN,       NAN },
    { -NAN,      -NAN },
    {  INFINITY,  1.0f },
    {  INFINITY, -1.0f },
    { -INFINITY,  1.0f },
    { -INFINITY, -1.0f },
    {  1.0f,      INFINITY },
    {  1.0f,     -INFINITY },
    { -1.0f,      INFINITY },
    { -1.0f,     -INFINITY }
};

float complex test_expected_outputs[] =
{
    {  0.0f,     -0.0f      },
    {  0.0f,      0.0f      },
    {  -0.0f,     -0.0f     },
    {  -0.0f,      0.0f     },
    {  1.0f,     -1.0f      },
    {  1.0f,      1.0f      },
    {  -1.0f,     -1.0f     },
    {  -1.0f,      1.0f     },
    {   NAN,      -NAN      },
    {   NAN,       NAN      },
    {  -NAN,      -NAN      },
    {  -NAN,       NAN      },
    {  INFINITY, -1.0f      },
    {  INFINITY,  1.0f      },
    {  -INFINITY, -1.0f     },
    {  -INFINITY,  1.0f     },
    {  1.0f,     -INFINITY  },
    {  1.0f,      INFINITY  },
    {  -1.0f,     -INFINITY },
    {  -1.0f,      INFINITY }
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_conj_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}