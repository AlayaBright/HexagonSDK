/**=============================================================================
@file
    test_creal_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_creal_f function.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdint.h>
#include "qhcomplex.h"
#include "qhcomplex_test.h"

float complex test_inputs[] =
{
  {  1.0,       1.0 },
  {  1.0,      -1.0 },
  { -1.0,       1.0 },
  { -1.0,      -1.0 },
  {  0.0,       0.0 },
  {  0.0,      -0.0 },
  { -0.0,       0.0 },
  { -0.0,      -0.0 },
  {  INFINITY,  INFINITY },
  {  INFINITY, -INFINITY },
  { -INFINITY,  INFINITY },
  { -INFINITY, -INFINITY },
  {  NAN,       NAN },
  {  NAN,      -NAN },
  { -NAN,       NAN },
  { -NAN,      -NAN }
};

float test_expected_outputs[] =
{
    1.0,
    1.0,
    -1.0,
    -1.0,
    0.0,
    0.0,
    -0.0,
    -0.0,
    INFINITY,
    INFINITY,
    -INFINITY,
    -INFINITY,
    NAN,
    NAN,
    -NAN,
    -NAN
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_F1(qhcomplex_creal_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}