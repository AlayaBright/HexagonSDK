/**=============================================================================
@file
    test_catan_f.c

@brief
    Accuracy test (includes handling of special cases) for qhcomplex_catan_f function.

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
    {INFINITY,-10.0f},
    {-INFINITY,-10.0f},
    {INFINITY,-0.0f},
    {-INFINITY,-0.0f},
    {INFINITY,0.0f},
    {-INFINITY,0.0f},
    {INFINITY,0.1f},
    {-INFINITY,0.1f},
    {0.0f, -INFINITY},
    {-0.0f, -INFINITY},
    {100.0f, -INFINITY},
    {-100.0f, -INFINITY},
    {0.0f, INFINITY},
    {-0.0f, INFINITY},
    {0.5f, INFINITY},
    {-0.5f, INFINITY},
    {NAN, 0.0f},
    {NAN, -0.0f},
    {NAN,INFINITY},
    {NAN,-INFINITY},
    {0.0, NAN},
    {-0.0, NAN},
    {INFINITY,NAN},
    {-INFINITY,NAN},
    {NAN,10.5f},
    {NAN,-10.5f},
    {0.75f,NAN},
    {-0.75f,NAN},
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
    {M_PI*(1.0/2.0),0.0},
    {M_PI*(1.0/2.0),-0.0},
    {-M_PI*(1.0/2.0),0.0},
    {-M_PI*(1.0/2.0),-0.0},
    {M_PI*(1.0/2.0),-0.0},
    {-M_PI*(1.0/2.0),-0.0},
    {M_PI*(1.0/2.0),-0.0},
    {-M_PI*(1.0/2.0),-0.0},
    {M_PI*(1.0/2.0),0.0},
    {-M_PI*(1.0/2.0),0.0},
    {M_PI*(1.0/2.0),0.0},
    {-M_PI*(1.0/2.0),0.0},
    {M_PI*(1.0/2.0),-0.0},
    {-M_PI*(1.0/2.0),-0.0},
    {M_PI*(1.0/2.0),-0.0},
    {-M_PI*(1.0/2.0),-0.0},
    {M_PI*(1.0/2.0),0.0},
    {-M_PI*(1.0/2.0),0.0},
    {M_PI*(1.0/2.0),0.0},
    {-M_PI*(1.0/2.0),0.0},
    {NAN, 0.0},
    {NAN, -0.0},
    {NAN, 0.0},
    {NAN, -0.0},
    {NAN, NAN},
    {NAN, NAN},
    {M_PI*(1.0/2.0),0.0},
    {-M_PI*(1.0/2.0),0.0},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {NAN, NAN},
    {1.0785744, 0.5770574f},
    {-1.4099210495965755225306193844604208, - 0.22907268296853876629588180294200276}
};

#define TEST_INPUTS_CNT (sizeof(test_inputs) / sizeof(*test_inputs))

int main(void)
{
    TEST_CMPLX_FUN_CF1(qhcomplex_catan_f, test_inputs, TEST_INPUTS_CNT, test_expected_outputs);

    return 0;
}
