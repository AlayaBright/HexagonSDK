/**=============================================================================
@file
    test_csinh_f.c

@brief
    Testing qhcomplex_csinh_f function for corner cases and accuracy.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include <stdio.h>
#include <stdint.h>
#include "qhcomplex.h"
#include "qhcomplex_test.h"

float complex test_inputs[] =
{
    {-2.0f, -3.0f},
    {NAN, NAN},
    {NAN, INFINITY},
    {NAN, INFINITY},
    {NAN, 10.0f},
    {NAN, 10.0f},
    {NAN, 0.0f},
    {NAN, -0.0f},
    {9.0f,NAN},
    {-9.0f,NAN},
    {INFINITY,NAN},
    {-INFINITY,NAN},
    {0.0f,NAN},
    {-0.0f,NAN},
    {6.75f,INFINITY},
    {-6.75f,INFINITY},
    {6.75f,-INFINITY},
    {-6.75f,-INFINITY},
    {INFINITY, 4.625f},
    {-INFINITY, 4.625f},
    {INFINITY, -4.625f},
    {-INFINITY, -4.625f},
    {INFINITY,INFINITY},
    {-INFINITY,INFINITY},
    {INFINITY,-INFINITY},
    {-INFINITY,-INFINITY},
    {INFINITY,0.0f},
    {-INFINITY,0.0f},
    {INFINITY,-0.0f},
    {-INFINITY,-0.0f},
    {0.0f,INFINITY},
    {-0.0f,INFINITY},
    {0.0f,-INFINITY},
    {-0.0f,-INFINITY},
    {0.0f,0.0f},
    {-0.0f,0.0f},
    {0.0f,-0.0f},
    {-0.0f,-0.0f}
 };

float complex test_expected_outputs[] =
{
    { 3.5905645 , -0.5309211},
    { NAN, NAN},
    { NAN, NAN},
    { NAN, NAN},
    { NAN, NAN},
    { NAN, NAN},
    { NAN, 0.0000000},
    { NAN, -0.0000000},
    { NAN, NAN},
    { NAN, NAN},
    { INFINITY, NAN},
    { INFINITY, NAN},
    { -0.0000000, NAN},
    { -0.0000000, NAN},
    { NAN, NAN},
    { NAN, NAN},
    { NAN, NAN},
    { NAN, NAN},
    { -INFINITY, -INFINITY},
    { INFINITY, -INFINITY},
    { -INFINITY, INFINITY},
    { INFINITY, INFINITY},
    { INFINITY, NAN},
    { INFINITY, NAN},
    { INFINITY, NAN},
    { INFINITY, NAN},
    { INFINITY, 0.0000000},
    { -INFINITY, 0.0000000},
    { INFINITY, -0.0000000},
    { -INFINITY, -0.0000000},
    { -0.0000000, NAN},
    { -0.0000000, NAN},
    { -0.0000000 , NAN},
    { -0.0000000 , NAN},
    { 0.0000000 , 0.0000000},
    { -0.0000000 , 0.0000000},
    { 0.0000000 , -0.0000000},
    { -0.0000000 , -0.0000000}
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_csinh_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}