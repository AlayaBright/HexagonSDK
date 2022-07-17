/**=============================================================================
@file
    test_cproj_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_cproj_f function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdint.h>
#include "qhcomplex.h"
#include "qhcomplex_test.h"

float complex test_inputs[] =
{
    [1]  = {  0.0f,      0.0f },
    [2]  = {  0.0f,     -0.0f },
    [3]  = { -0.0f,      0.0f },
    [4]  = { -0.0f,     -0.0f },
    [5]  = {  1.0f,      1.0f },
    [6]  = {  1.0f,     -1.0f },
    [7]  = { -1.0f,      1.0f },
    [8]  = { -1.0f,     -1.0f },
    [9]  = {  NAN,       NAN  },
    [10] = {  NAN,      -NAN  },
    [11] = { -NAN,       NAN  },
    [12] = { -NAN,      -NAN  },
    [13] = {  INFINITY,  1.0f },
    [14] = {  INFINITY, -1.0f },
    [15] = { -INFINITY,  1.0f },
    [16] = { -INFINITY, -1.0f },
    [17] = {  1.0f,  INFINITY },
    [18] = {  1.0f, -INFINITY },
    [19] = { -1.0f,  INFINITY },
    [20] = { -1.0f, -INFINITY }
};

float complex test_expected_outputs[] =
{
    [1]  = { 0.0f,      0.0f },
    [2]  = { 0.0f,     -0.0f },
    [3]  = { -0.0f,     0.0f },
    [4]  = { -0.0f,    -0.0f },
    [5]  = { 1.0f,      1.0f },
    [6]  = { 1.0f,     -1.0f },
    [7]  = { -1.0f,     1.0f },
    [8]  = { -1.0f,    -1.0f },
    [9]  = { NAN,       NAN  },
    [10] = { NAN,      -NAN  },
    [11] = { -NAN,      NAN  },
    [12] = { -NAN,     -NAN  },
    [13] = { INFINITY,  0.0f },
    [14] = { INFINITY, -0.0f },
    [15] = { INFINITY,  0.0f },
    [16] = { INFINITY, -0.0f },
    [17] = { INFINITY,  0.0f },
    [18] = { INFINITY, -0.0f },
    [19] = { INFINITY,  0.0f },
    [20] = { INFINITY, -0.0f }
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_cproj_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}
